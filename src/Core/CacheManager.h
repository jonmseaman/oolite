/*

OOCacheManager.h
By Jens Ayton

Singleton class responsible for handling Oolite's data cache.
The cache manager stores arbitrary property lists in separate namespaces
(referred to simply as caches). The cache is emptied if it was created with a
different verison of Oolite, or if it was created on a system with a different
byte sex.

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

#pragma once

#include <cstddef>
#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <string>

#include "AsyncWorkManager.h"
#include "oofnd/Object.hpp"
#include "oofnd/PList.hpp"

/*	Cache names and keys are UTF-8
        std::strings. The cache directory is std::nullopt where it was nil.
   Every cached value is property-list data, held as an oo::PList and written to
   the cache file as it is; a null PList is "no value".
*/
/*	The cache manager is a singleton. The write task (OOAsyncCacheWriter) is
   a C++ task (OOAsyncWorkTask).
*/
namespace oo {

class AsyncCacheWriter;

class CacheManager : public oo::Object {
 public:
  // the one instance, made on first use and never released
  static CacheManager* sharedCache();
  ~CacheManager() override;

  oo::PList pListForKey(const std::string& key,
                        const std::string& cache);  // null PList: absent
  void setPList(const oo::PList& value, const std::string& key,
                const std::string& cache);  // value: not null

  void removeObjectForKey(const std::string& inKey,
                          const std::string& inCacheKey);
  void clearCache(const std::string& inCacheKey);
  void clearAllCaches();
  void reloadAllCaches();

  void setAllowCacheWrites(bool flag);

  std::optional<std::string> cacheDirectoryPathCreatingIfNecessary(bool create);

  void flush();
  // Wait for flush to complete. Does nothing if async flushing is disabled.
  void finishOngoingFlush();

  // What "%@" printed between the braces of <OOCacheManager 0x...>{...}
  // (OODescription.h).
  std::optional<std::string> descriptionComponents() const;

 private:
  friend class AsyncCacheWriter;  // calls writeDict() on a work thread

  bool writeDict(const oo::PList& inDict);

  void
  init();  // -init's body, run after the instance is recorded as the singleton

  void loadCache();
  void write();
  void clear();
  bool dirty();
  void markClean();

  oo::PList loadDict();  // null: no cache

  void buildCachesFromDictionary(const oo::PList* inDict);  // nullptr: none
  oo::PList dictionaryOfCaches();

  bool directoryExists(const std::string& inPath, bool inCreate);

  std::optional<std::string> cachePathCreatingIfNecessary(bool create);

  // cache name -> key -> cached value; std::nullopt before loading, as nil was.
  std::optional<std::map<
      std::string, std::map<std::string, oo::PList, std::less<>>, std::less<>>>
      _caches = {};
  // the pending write task
  std::shared_ptr<AsyncCacheWriter> _scheduledWrite = {};
  bool _permitWrites = {};
  bool _dirty = {};
};

// The write task write() schedules.
class AsyncCacheWriter : public AsyncWorkTask {
 public:
  // Null for a null contents (-initWithCacheContents: answered nil).
  static std::shared_ptr<AsyncCacheWriter> writerWithCacheContents(
      const oo::PList& cacheContents);

  void performAsyncTask() override;

  bool hasCompleteAsyncTask() const override { return true; }

  void completeAsyncTask() override;

 private:
  explicit AsyncCacheWriter(const oo::PList& cacheContents)
      : _cacheContents(cacheContents) {}

  oo::PList _cacheContents;
};

}  // namespace oo
