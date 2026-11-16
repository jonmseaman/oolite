/*

OOCache.cpp
By Jens Ayton

Oolite
Copyright (C) 2004-2013 Giles C Williams and contributors

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
MA 02110-1301, USA.

*/

/*	IMPLEMENTATION NOTES
        A cache does three things: retrieves an element by key, sets the
   element for a key, and, when it holds too many, deletes the least recently
   used elements.

        The entries live in an AGE LIST (std::list), ordered from oldest to
   youngest, and a hash map from key to list position finds them. Whenever an
   element is retrieved or inserted, it is moved to the youngest end of the age
   list. Pruning proceeds from the oldest end.

        If autoPrune is on, pruning is batched: it brings the cache down to 80%
   of the prune threshold once the threshold is passed. Pruning is also done
   before serialization (which happens, if the cache has changed, whenever the
   user docks), so the number of items in the cache on disk never exceeds 80%
   of the prune threshold. Otherwise, pruning is done by calling prune().
*/

#include "Cache.h"

#include <algorithm>
#include <format>
#include <iterator>
#include <list>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "Logging.h"
#include "oofnd/PList.hpp"
#if DEBUG_GRAPHVIZ
#include "StringParsing.h"
#include "oofnd/FileSystem.hpp"
#include "oofnd/String.hpp"
#endif

namespace oo {

struct CacheImpl {
  struct Entry {
    std::string key;
    oo::PList value;
  };

  // Oldest first.
  std::list<Entry> byAge;
  std::unordered_map<std::string, std::list<Entry>::iterator> index;
  std::optional<std::string> name;  // nullopt: no name (nil)

  // The entry for <key>, made the youngest; null where there is none.
  Entry* retrieve(const std::string& key) {
    const auto found = index.find(key);
    if (found == index.end()) return nullptr;
    byAge.splice(byAge.end(), byAge, found->second);
    return &*found->second;
  }

  /*	A value may hold the last reference to an object whose destructor uses
     the cache again (a texture removes itself from the recent-textures
     cache), so a replaced or removed value is destroyed only once the cache is
     consistent again.
  */

  // A null value is not stored. True where the cache changed.
  bool insert(const std::string& key, const oo::PList& value) {
    if (value.isNull()) return false;
    if (Entry* entry = retrieve(key)) {
      const oo::PList replaced = std::exchange(entry->value, value);
    } else {
      byAge.push_back(Entry{key, value});
      index.emplace(key, std::prev(byAge.end()));
    }
    return true;
  }

  bool remove(const std::string& key) {
    const auto found = index.find(key);
    if (found == index.end()) return false;
    const oo::PList removed = std::move(found->second->value);
    byAge.erase(found->second);
    index.erase(found);
    return true;
  }

  void removeOldest(const std::string& logKey) {
    if (byAge.empty()) return;
    OO_LOG(logKey, "Pruning cache \"{}\": removing {}", name.value_or("(null)"),
           byAge.front().key);
    const std::string key = byAge.front().key;  // a copy: the entry goes
    remove(key);
  }
};

}  // namespace oo

namespace {

constexpr const char* kSerializedEntryKeyKey = "key";
constexpr const char* kSerializedEntryKeyValue = "value";

}  // namespace

namespace oo {

Cache::~Cache() = default;

}  // namespace oo

// oo::DescriptionOf (OODescription.h) wraps this as "<OOCache 0x...>{...}",
// which is what this class's own -description printed.
std::optional<std::string> oo::Cache::descriptionComponents() const {
  return std::format(
      "\"{}\", {} elements, prune threshold={}, auto-prune={} dirty={}",
      cache->name.value_or("(null)"), cache->byAge.size(), _pruneThreshold,
      _autoPrune ? "yes" : "no", _dirty ? "yes" : "no");
}

std::shared_ptr<oo::Cache> oo::Cache::cacheWithPList(const oo::PList& pList) {
  std::shared_ptr<oo::Cache> result = oo::makeShared<oo::Cache>();
  if (!result->initWithPList(pList)) return nullptr;
  return result;
}

bool oo::Cache::initWithPList(const oo::PList& pList) {
  cache = std::make_unique<oo::CacheImpl>();

  if (!pList.isNull()) {
    if (!pList.isArray()) return false;
    loadFromArray(pList);
  }
  _pruneThreshold = oo::kCacheDefaultPruneThreshold;
  _autoPrune = true;
  return true;
}

// Property-list data: [{key, value}, ...] from oldest to youngest; null for an
// empty cache (nil).
oo::PList oo::Cache::pListRepresentation() {
  if (cache->byAge.empty()) return oo::PList();

  oo::PList::Array result;
  result.reserve(cache->byAge.size());
  for (const oo::CacheImpl::Entry& entry : cache->byAge) {
    oo::PList::Dict serialized;
    serialized.emplace(kSerializedEntryKeyKey, oo::PList(entry.key));
    serialized.emplace(kSerializedEntryKeyValue, entry.value);
    result.push_back(oo::PList(std::move(serialized)));
  }
  return oo::PList(std::move(result));
}

oo::PList oo::Cache::pListForKey(const std::string& key) {
  // Note: while reordering the age list technically makes the cache dirty, it's
  // not worth rewriting it just for that, so we don't flag it.
  if (const oo::CacheImpl::Entry* entry = cache->retrieve(key))
    return entry->value;
  return oo::PList();
}

void oo::Cache::setPList(const oo::PList& value, const std::string& key) {
  if (cache->insert(key, value)) {
    _dirty = true;
    if (_autoPrune) prune();
  }
}

void oo::Cache::removePListForKey(const std::string& key) {
  if (cache->remove(key)) _dirty = true;
}

void oo::Cache::setPruneThreshold(unsigned threshold) {
  threshold = std::max(threshold,
                       static_cast<unsigned>(oo::kCacheMinimumPruneThreshold));
  if (threshold != _pruneThreshold) {
    _pruneThreshold = threshold;
    if (_autoPrune) prune();
  }
}

unsigned oo::Cache::pruneThreshold() { return _pruneThreshold; }

void oo::Cache::setAutoPrune(bool flag) {
  bool prune = (flag != false);
  if (prune != _autoPrune) {
    _autoPrune = prune;
    this->prune();
  }
}

bool oo::Cache::autoPrune() { return _autoPrune; }

void oo::Cache::prune() {
  unsigned pruneCount;
  unsigned desiredCount;
  unsigned count;

  // Order of operations is to ensure rounding down.
  if (_autoPrune)
    desiredCount = (_pruneThreshold * 4) / 5;
  else
    desiredCount = _pruneThreshold;

  if (_pruneThreshold == oo::kCacheNoPrune) return;
  count = static_cast<unsigned>(cache->byAge.size());
  if (count <= _pruneThreshold) return;

  pruneCount = count - desiredCount;

  const std::string logKey =
      std::format("dataCache.prune.{}", cache->name.value_or("(null)"));
  OO_LOG(logKey, "Pruning cache \"{}\" - removing {} entries",
         cache->name.value_or("(null)"), pruneCount);
  oo::log::indentIf(logKey);

  while (pruneCount--) cache->removeOldest(logKey);

  oo::log::outdentIf(logKey);
}

bool oo::Cache::dirty() { return _dirty; }

void oo::Cache::markClean() { _dirty = false; }

std::optional<std::string> oo::Cache::name() { return cache->name; }

void oo::Cache::setName(const std::optional<std::string>& name) {
  cache->name = name;
}

// The values, youngest first; empty for an empty cache (the Foundation version
// returned nil).
std::vector<oo::PList> oo::Cache::pListsByAge() {
  std::vector<oo::PList> result;
  result.reserve(cache->byAge.size());
  for (auto entry = cache->byAge.rbegin(); entry != cache->byAge.rend();
       ++entry) {
    result.emplace_back(entry->value);
  }
  return result;
}

// Private

void oo::Cache::loadFromArray(const oo::PList& array) {
  const oo::PList::Array* entries = array.getIf<oo::PList::Array>();
  if (entries == nullptr) return;

  for (const oo::PList& entry : *entries) {
    if (entry.isDict()) {
      const oo::PList* key = entry.find(kSerializedEntryKeyKey);
      const oo::PList* value = entry.find(kSerializedEntryKeyValue);
      if (key != nullptr && key->isString() && value != nullptr) {
        setPList(*value, *key->getIf<std::string>());
      }
    }
  }
}

#if DEBUG_GRAPHVIZ

// The cache as a GraphViz chain of its entries, oldest first.
std::optional<std::string> oo::Cache::generateGraphVizBodyWithRootNamed(
    const std::string& rootName) {
  std::string result;

  // Root node representing cache
  result += std::format(
      "\t{} [label=\"Cache \\\"{}\\\"\" shape=box];\n\tnode "
      "[shape=record];\n\t\n",
      rootName,
      name().has_value() ? EscapedGraphVizString(*name())
                         : std::string("(null)"));

  std::string previous = rootName;
  for (const oo::CacheImpl::Entry& entry : cache->byAge) {
    const std::string node =
        std::format("n{}", oo::str::pointerDescription(&entry));
    result += std::format("\t{} [label=\"{}\"];\n\t{} -> {};\n", node,
                          EscapedGraphVizString(entry.key), previous, node);
    previous = node;
  }

  return result;
}

std::string oo::Cache::generateGraphViz() {
  std::string result;

  // Header
  result += std::format(
      "// OOCache dump\n\ndigraph cache\n{{\n\tgraph [charset=\"UTF-8\", "
      "label=\"OOCache \"{}\" debug dump\", labelloc=t, labeljust=l];\n\t\n",
      name().value_or("(null)"));

  result += generateGraphVizBodyWithRootNamed("cache").value_or("");

  result += "}\n";

  return result;
}

// (-writeGraphVizToURL: folded in: its only sender was this method.)
void oo::Cache::writeGraphVizToPath(const std::string& path) {
  const std::string graphViz = generateGraphViz();
  (void)oo::fs::writeFile(oo::fs::pathFromUTF8(path),
                          oo::Data(graphViz.data(), graphViz.size()),
                          oo::fs::WriteMode::kAtomic);
}
#endif
