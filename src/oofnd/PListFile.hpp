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
   reads inside .oxz archives). A path not ending in ".plist" (any case) is read
   as it is: saved games and other files keep their own formats.
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

namespace oo {

// "dir/name.plist" -> "dir/name.json5" (the extension in any case); nullopt
// for a path with any other extension.
inline std::optional<std::string> json5PathFor(std::string_view path) {
  constexpr std::string_view kExtension = ".plist";
  if (path.size() <= kExtension.size()) return std::nullopt;
  const std::string_view tail = path.substr(path.size() - kExtension.size());
  for (std::size_t i = 0; i < kExtension.size(); ++i) {
    char c = tail[i];
    if (c >= 'A' && c <= 'Z') c = static_cast<char>(c - 'A' + 'a');
    if (c != kExtension[i]) return std::nullopt;
  }
  const std::string_view stem = path.substr(0, path.size() - kExtension.size());
  if (stem.empty() || stem.back() == '/' || stem.back() == '\\')
    return std::nullopt;  // ".plist" alone is a hidden file, not a name
  return std::string(stem) + ".json5";
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

template <class Read>
PListFileLoad loadPListFile(const std::string& path, Read&& read) {
  PListFileLoad result;
  if (std::optional<std::string> json5 = json5PathFor(path)) {
    if (std::optional<Data> data = read(*json5)) {
      result.source = std::move(*json5);
      result.isJSON5 = true;
      result.shadowedPlist = read(path).has_value();
      const std::string_view bytes(reinterpret_cast<const char*>(data->bytes()),
                                   data->length());
      std::expected<PList, PListError> parsed = parseJSON5PList(bytes);
      if (parsed)
        result.plist = std::move(*parsed);
      else
        result.error = std::move(parsed.error());
      return result;
    }
  }
  if (std::optional<Data> data = read(path)) {
    result.source = path;
    result.plistData = std::move(data);
  }
  return result;
}

}  // namespace oo
