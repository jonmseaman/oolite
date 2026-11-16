/*

OOManifestProperties.h

The property keys used in manifest.plist entries

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

#include <string_view>

namespace oo {

inline constexpr std::string_view kManifestIdentifier = "identifier";
inline constexpr std::string_view kManifestVersion = "version";
inline constexpr std::string_view kManifestRequiredOoliteVersion =
    "required_oolite_version";
inline constexpr std::string_view kManifestMaximumOoliteVersion =
    "maximum_oolite_version";
inline constexpr std::string_view kManifestTitle = "title";
inline constexpr std::string_view kManifestRequiresOXPs = "requires_oxps";
inline constexpr std::string_view kManifestConflictOXPs = "conflict_oxps";
inline constexpr std::string_view kManifestDescription = "description";
inline constexpr std::string_view kManifestCategory = "category";
inline constexpr std::string_view kManifestDownloadURL = "download_url";
inline constexpr std::string_view kManifestFileSize = "file_size";
inline constexpr std::string_view kManifestInformationURL = "information_url";
inline constexpr std::string_view kManifestAuthor = "author";
inline constexpr std::string_view kManifestLicense = "license";
inline constexpr std::string_view kManifestTags = "tags";
/* these properties are not contained in the manifest.plist (and would be
   overwritten if they were...) but are calculated by Oolite */
inline constexpr std::string_view kManifestFilePath = "file_path";
inline constexpr std::string_view kManifestRequiredBy = "required_by";
inline constexpr std::string_view kManifestAvailableVersion =
    "available_version";
/* these properties are not contained in the manifest.plist but are
 * provided by in the manifest*s* list by the API */
inline constexpr std::string_view kManifestUploadDate = "upload_date";

}  // namespace oo

// following manifest.plist properties not (yet?) used by Oolite
// but may be used by other manifest reading applications
#if 0
namespace oo {

inline constexpr std::string_view kManifestOptionalOXPs			= "optional_oxps";

}  // namespace oo

#endif

// properties for within requires/optional/conflicts entries
namespace oo {

inline constexpr std::string_view kManifestRelationIdentifier = "identifier";
inline constexpr std::string_view kManifestRelationVersion = "version";
inline constexpr std::string_view kManifestRelationMaxVersion =
    "maximum_version";
inline constexpr std::string_view kManifestRelationDescription = "description";

// 'magic' value for a tag to exclude an OXP from loading except when
// required by a scenario
inline constexpr std::string_view kManifestTagScenarioOnly =
    "oolite-scenario-only";

}  // namespace oo
