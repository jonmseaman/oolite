/*	test_OOCache.mm
        Unit tests for OOCache (src/Core/OOCache.h).

        It pins what the cache computes: an empty cache's
   settings, storing, replacing and removing values (a null value is not
   stored), dirtiness, least-recently-used order (a lookup promotes), pruning
   (the threshold's floor, auto-pruning to 80%, manual pruning to the threshold,
   no pruning while loading), the property-list round trip, a live object held
        in an Object node, the name and the description. The calls here are the
   C++ class's. Run: tests/run.sh unit
*/

#include "Cache.h"
#include "oofnd/AutoreleaseScope.hpp"
#include "oofnd/PList.hpp"
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>
#include "oo_test.hpp"

/*	The game's OOStringParsing.mm reaches the JavaScript engine, so it is
   not linked. The one function of it that the cache names (in its
   DEBUG_GRAPHVIZ dump, which no test calls) is defined here instead.
*/
std::string EscapedGraphVizString(const std::string& string) {
  return string;
}

namespace {

oo::PList Str(const std::string& string) { return oo::PList(string); }

std::vector<std::string> Strings(const std::vector<oo::PList>& values) {
  std::vector<std::string> result;
  for (const oo::PList& value : values)
    result.push_back(value.isString() ? *value.getIf<std::string>()
                                      : std::string("?"));
  return result;
}

oo::PList Entry(const std::string& key, const oo::PList& value) {
  oo::PList::Dict entry;
  entry.emplace("key", Str(key));
  entry.emplace("value", value);
  return oo::PList(std::move(entry));
}

}  // namespace

OO_TEST(emptyCache) {
  {
    std::shared_ptr<oo::Cache> cache = oo::Cache::cacheWithPList(oo::PList());
    OO_CHECK(cache != nullptr);
    OO_CHECK(cache->pruneThreshold() == oo::kCacheDefaultPruneThreshold);
    OO_CHECK(cache->autoPrune());
    OO_CHECK(!cache->dirty());
    OO_CHECK(!cache->name().has_value());
    OO_CHECK(cache->pListRepresentation().isNull());
    OO_CHECK(cache->pListsByAge().empty());
    OO_CHECK(cache->pListForKey("absent").isNull());

    OO_CHECK(
        cache->descriptionComponents() ==
        std::optional<std::string>("\"(null)\", 0 elements, prune "
                                   "threshold=200, auto-prune=yes dirty=no"));
    cache->setName(std::string("Text encoding"));
    OO_CHECK(cache->name() == std::optional<std::string>("Text encoding"));
    OO_CHECK(
        cache->descriptionComponents() ==
        std::optional<std::string>("\"Text encoding\", 0 elements, prune "
                                   "threshold=200, auto-prune=yes dirty=no"));
    cache->setName(std::nullopt);
    OO_CHECK(!cache->name().has_value());
  }
}

OO_TEST(storeReplaceRemove) {
  {
    std::shared_ptr<oo::Cache> cache = oo::Cache::cacheWithPList(oo::PList());
    cache->setPList(oo::PList(), "null");  // a null value is not stored
    OO_CHECK(!cache->dirty() && cache->pListsByAge().empty());

    cache->setPList(Str("one"), "a");
    OO_CHECK(cache->dirty());
    OO_CHECK(cache->pListForKey("a") == Str("one"));
    cache->markClean();
    OO_CHECK(!cache->dirty());

    cache->setPList(Str("uno"), "a");  // replacing marks it dirty too
    OO_CHECK(cache->dirty() && cache->pListForKey("a") == Str("uno"));
    cache->markClean();

    cache->removePListForKey("b");  // absent: not dirty
    OO_CHECK(!cache->dirty());
    cache->removePListForKey("a");
    OO_CHECK(cache->dirty() && cache->pListForKey("a").isNull());
    OO_CHECK(
        cache->descriptionComponents() ==
        std::optional<std::string>("\"(null)\", 0 elements, prune "
                                   "threshold=200, auto-prune=yes dirty=yes"));
  }
}

OO_TEST(leastRecentlyUsedOrder) {
  {
    std::shared_ptr<oo::Cache> cache = oo::Cache::cacheWithPList(oo::PList());
    for (const char* key : {"d", "b", "a", "c"})
      cache->setPList(Str(std::string("value ") + key), key);
    OO_CHECK(Strings(cache->pListsByAge()) ==
             std::vector<std::string>({"value c", "value a", "value b",
                                       "value d"}));  // youngest first

    (void)cache->pListForKey("d");   // a lookup promotes
    (void)cache->pListForKey("zz");  // a miss does not change the order
    cache->markClean();
    OO_CHECK(
        Strings(cache->pListsByAge()) ==
        std::vector<std::string>({"value d", "value c", "value a", "value b"}));
    OO_CHECK(!cache->dirty());  // reordering does not dirty it

    // The property list: {key, value} entries, oldest first.
    oo::PList::Array expected = {
        Entry("b", Str("value b")), Entry("a", Str("value a")),
        Entry("c", Str("value c")), Entry("d", Str("value d"))};
    OO_CHECK(cache->pListRepresentation() == oo::PList(expected));
  }
}

OO_TEST(pruning) {
  {
    std::shared_ptr<oo::Cache> cache = oo::Cache::cacheWithPList(oo::PList());
    cache->setPruneThreshold(3);
    OO_CHECK(cache->pruneThreshold() ==
             oo::kCacheMinimumPruneThreshold);  // the floor

    // Auto-pruning: past the threshold, down to 80% of it, oldest first.
    for (int i = 0; i < 25; i++)
      cache->setPList(Str(std::to_string(i)), std::to_string(100 + i));
    OO_CHECK(cache->pListsByAge().size() == 25);
    cache->setPList(Str("25"), "125");
    OO_CHECK(cache->pListsByAge().size() == 20);
    OO_CHECK(cache->pListForKey("105").isNull() &&
             cache->pListForKey("106") == Str("6"));

    // Manual pruning: nothing until -prune, then down to the threshold itself.
    cache->setAutoPrune(false);
    OO_CHECK(!cache->autoPrune());
    for (int i = 26; i < 40; i++)
      cache->setPList(Str(std::to_string(i)), std::to_string(100 + i));
    OO_CHECK(cache->pListsByAge().size() == 34);
    cache->prune();
    OO_CHECK(cache->pListsByAge().size() == 25);

    // Raising the threshold prunes nothing; lowering it with auto-prune on
    // prunes at once.
    cache->setPruneThreshold(30);
    OO_CHECK(cache->pListsByAge().size() == 25);
    cache->setAutoPrune(
        true);  // turning it on prunes (under the threshold: nothing)
    OO_CHECK(cache->pListsByAge().size() == 25);
    cache->setPruneThreshold(oo::kCacheMinimumPruneThreshold);
    OO_CHECK(cache->pListsByAge().size() == 25);  // not over the threshold
    cache->setPList(Str("x"), "x");
    OO_CHECK(cache->pListsByAge().size() == 20);

    cache->setPruneThreshold(oo::kCacheNoPrune);
    for (int i = 0; i < 40; i++)
      cache->setPList(Str("y"), "y" + std::to_string(i));
    cache->prune();
    OO_CHECK(cache->pListsByAge().size() == 60);
  }
}

OO_TEST(propertyListRoundTrip) {
  {
    oo::PList::Array entries;
    for (int i = 0; i < 250; i++)
      entries.push_back(
          Entry("k" + std::to_string(i), oo::PList(std::int64_t(i))));
    entries.push_back(Str("not an entry"));
    entries.push_back(
        oo::PList(oo::PList::Dict({{"key", oo::PList(std::int64_t(7))},
                                   {"value", Str("key not a string")}})));
    entries.push_back(oo::PList(oo::PList::Dict({{"key", Str("no value")}})));

    std::shared_ptr<oo::Cache> loaded = oo::Cache::cacheWithPList(oo::PList(entries));
    OO_CHECK(loaded != nullptr);
    OO_CHECK(loaded->pListsByAge().size() ==
             250);  // loading does not prune, even past the threshold
    OO_CHECK(loaded->dirty());
    OO_CHECK(loaded->pruneThreshold() == oo::kCacheDefaultPruneThreshold &&
             loaded->autoPrune());
    OO_CHECK(loaded->pListForKey("k0") == oo::PList(std::int64_t(0)));
    OO_CHECK(loaded->pListForKey("no value").isNull());

    std::shared_ptr<oo::Cache> again =
        oo::Cache::cacheWithPList(loaded->pListRepresentation());
    OO_CHECK(again->pListRepresentation() == loaded->pListRepresentation());

    OO_CHECK(oo::Cache::cacheWithPList(oo::PList())->pListsByAge().empty());
    OO_CHECK(oo::Cache::cacheWithPList(Str("not an array")) == nullptr);
    OO_CHECK(oo::Cache::cacheWithPList(oo::PList(oo::PList::Dict())) == nullptr);
  }
}

// A live object in an Object node: a C++ one.
namespace {

class TestForeign final : public oo::PListForeign {
 public:
  std::string className() const override { return "TestForeign"; }
  std::string description() const override { return "<TestForeign>"; }
};

}  // namespace

OO_TEST(holdsALiveObject) {
  std::shared_ptr<TestForeign> object = oo::makeShared<TestForeign>();
  std::shared_ptr<oo::Cache> cache = oo::Cache::cacheWithPList(oo::PList());
  {
    oo::AutoreleaseScope pool;
    cache->setPList(oo::PList(oo::PList::Object(object)), "texture");
    OO_CHECK(object->weak_from_this().use_count() == 2);  // the cache retains it
    const oo::PList held = cache->pListForKey("texture");
    const oo::PList::Object* node = held.getIf<oo::PList::Object>();
    OO_CHECK(node != nullptr && node->get() == object.get());
  }
  cache->removePListForKey("texture");
  OO_CHECK(object->weak_from_this().use_count() == 1);
  cache = nullptr;
}

// An object whose destructor looks itself up in the cache, as a texture does
// (OOTexture::removeFromCaches()): the cache holds its last reference. Once
// the cache itself is going, the lookup finds no cache, as a texture finds
// RecentTextures() empty.
namespace {

class SelfLookingUpForeign final : public oo::PListForeign {
 public:
  SelfLookingUpForeign(std::weak_ptr<oo::Cache> cache, std::string key,
                      int* destroyed)
      : _cache(std::move(cache)), _key(std::move(key)), _destroyed(destroyed) {}

  ~SelfLookingUpForeign() override {
    ++*_destroyed;
    // The lookup moves the key's entry to the young end, as the texture's
    // does; the value found is never this object.
    if (const std::shared_ptr<oo::Cache> cache = _cache.lock())
      (void)cache->pListForKey(_key);
  }

  std::string className() const override { return "SelfLookingUpForeign"; }
  std::string description() const override { return "<SelfLookingUpForeign>"; }

 private:
  std::weak_ptr<oo::Cache> _cache;
  std::string _key;
  int* _destroyed;
};

}  // namespace

OO_TEST(aValueMayUseTheCacheAsItGoes) {
  std::shared_ptr<oo::Cache> cache = oo::Cache::cacheWithPList(oo::PList());
  int destroyed = 0;
  auto Add = [&](const std::string& key) {
    cache->setPList(oo::PList(oo::PList::Object(
                        oo::makeShared<SelfLookingUpForeign>(cache, key,
                                                            &destroyed))),
                    key);
  };

  Add("a");
  Add("b");
  cache->removePListForKey("a");  // its value was the last reference
  OO_CHECK(destroyed == 1);
  OO_CHECK(cache->pListForKey("a").isNull());

  Add("b");  // replaces b's value, the last reference to the old one
  OO_CHECK(destroyed == 2);
  OO_CHECK(!cache->pListForKey("b").isNull());

  cache->setPruneThreshold(oo::kCacheMinimumPruneThreshold);
  for (int i = 0; i < 30; ++i) Add("p" + std::to_string(i));
  OO_CHECK(destroyed > 2);  // pruning dropped the oldest values
  OO_CHECK(cache->pListsByAge().size() <= oo::kCacheMinimumPruneThreshold);
}

OO_TEST_MAIN()
