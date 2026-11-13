/*	oofnd/PListFile.hpp
        Where a property list file's data comes from: wherever the game loads
   <name>.plist, a <name>.json5 in the same place (directory or OXZ) stands in
   for it. When both exist the JSON5 one is read and the plist ignored, and the
   result says so for the caller to log.

            oo::PListFileLoad r = oo::loadPListFile(path, read);

   then r.plistData holds a plist's bytes to parse as before, or r.isJSON5 says
   r.plist (or r.error) came from the .json5. OOPropertyListFromFile
   (src/Core/OOPListParsing.cpp) is the game's one caller.

        The reader is a callable std::optional<oo::Data>(const std::string&):
   nullopt means "no such file" (the game passes OODataFromOXZFile, which also
   reads inside .oxz archives). A path ending in ".json5" is read as JSON5. Any
   other path not ending in ".plist" (any case) is read as it is: saved games
   and other files keep their own formats.

        Code that has to find a file before it loads it asks

            std::optional<std::string> p = oo::existingPListPath(path, exists);

   which answers the .json5 when there is one, so that the path it hands on is
   a file that exists; loadPListFile reads either.

        Code that reads a property list file itself, without the game's logging
   (the built-in files read before the log is set up), asks

            auto r = oo::parsePListFile(path, read);  // std::expected

   for the tree whichever format the file is in.
*/

#pragma once

#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include <utility>

#include "oofnd/Data.hpp"
#include "oofnd/PList.hpp"
#include "oofnd/PListJSON5.hpp"
#include "oofnd/PListParsing.hpp"

namespace oo {

namespace plist_file_detail {

// `path` without `extension` (lower case, with its dot) when it ends in it, in
// any case, after a name; nullopt otherwise.
inline std::optional<std::string_view> stemOf(std::string_view path,
                                              std::string_view extension) {
  if (path.size() <= extension.size()) return std::nullopt;
  const std::string_view tail = path.substr(path.size() - extension.size());
  for (std::size_t i = 0; i < extension.size(); ++i) {
    char c = tail[i];
    if (c >= 'A' && c <= 'Z') c = static_cast<char>(c - 'A' + 'a');
    if (c != extension[i]) return std::nullopt;
  }
  const std::string_view stem = path.substr(0, path.size() - extension.size());
  if (stem.empty() || stem.back() == '/' || stem.back() == '\\')
    return std::nullopt;  // ".plist" alone is a hidden file, not a name
  return stem;
}

}  // namespace plist_file_detail

// "dir/name.plist" -> "dir/name.json5" (the extension in any case); nullopt
// for a path with any other extension.
inline std::optional<std::string> json5PathFor(std::string_view path) {
  const std::optional<std::string_view> stem =
      plist_file_detail::stemOf(path, ".plist");
  if (!stem) return std::nullopt;
  return std::string(*stem) + ".json5";
}

// Whether `path` names a JSON5 file: "dir/name.json5", the extension in any
// case.
inline bool isJSON5Path(std::string_view path) {
  return plist_file_detail::stemOf(path, ".json5").has_value();
}

// Where the property list `path` names is: the .json5 beside it when there is
// one, else `path` when it exists, else nullopt. `exists` is a callable
// bool(const std::string&).
template <class Exists>
std::optional<std::string> existingPListPath(const std::string& path,
                                             Exists&& exists) {
  if (std::optional<std::string> json5 = json5PathFor(path)) {
    if (exists(*json5)) return json5;
  }
  if (exists(path)) return path;
  return std::nullopt;
}

struct PListFileLoad {
  // The file that was read, or empty when neither exists.
  std::string source;
  // A .json5 file was read: `plist` holds it, or `error` says why it failed.
  bool isJSON5 = false;
  PList plist;
  std::optional<PListError> error;
  // A .plist beside the .json5 that was read, and ignored.
  bool shadowedPlist = false;
  // No .json5: the .plist's bytes, for the caller's plist parser.
  std::optional<Data> plistData;
};

namespace plist_file_detail {

// `result` as the reading of the JSON5 file `source`, whose bytes are `data`.
inline void readJSON5(PListFileLoad& result, std::string source,
                      const Data& data) {
  result.source = std::move(source);
  result.isJSON5 = true;
  const std::string_view bytes(reinterpret_cast<const char*>(data.bytes()),
                               data.length());
  std::expected<PList, PListError> parsed = parseJSON5PList(bytes);
  if (parsed)
    result.plist = std::move(*parsed);
  else
    result.error = std::move(parsed.error());
}

}  // namespace plist_file_detail

template <class Read>
PListFileLoad loadPListFile(const std::string& path, Read&& read) {
  PListFileLoad result;
  if (std::optional<std::string> json5 = json5PathFor(path)) {
    if (std::optional<Data> data = read(*json5)) {
      plist_file_detail::readJSON5(result, std::move(*json5), *data);
      result.shadowedPlist = read(path).has_value();
      return result;
    }
  } else if (isJSON5Path(path)) {
    if (std::optional<Data> data = read(path))
      plist_file_detail::readJSON5(result, path, *data);
    return result;
  }
  if (std::optional<Data> data = read(path)) {
    result.source = path;
    result.plistData = std::move(data);
  }
  return result;
}

// The tree of the property list file `path` names, whichever format it is in:
// loadPListFile, then the plist parser for a .plist's bytes. A null PList when
// neither file exists; an error when the one read does not parse.
template <class Read>
std::expected<PList, PListError> parsePListFile(const std::string& path,
                                                Read&& read) {
  PListFileLoad load = loadPListFile(path, std::forward<Read>(read));
  if (load.isJSON5) {
    if (load.error.has_value()) return std::unexpected(std::move(*load.error));
    return std::move(load.plist);
  }
  if (!load.plistData.has_value()) return PList();
  return parsePropertyListData(
      std::string_view(reinterpret_cast<const char*>(load.plistData->bytes()),
                       load.plistData->length()));
}

}  // namespace oo
