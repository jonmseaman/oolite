/*

OOMeshToOctreeConverter.h

Class to manage the construction of octrees from triangle soups.


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
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "Foundation.h"
#include "Maths.h"
#include "Octree.h"
#include "oofnd/Object.hpp"

class ShipEntity;

namespace oo {

inline constexpr uint32_t kMeshToOctreeConverterSmallDataCapacity = 16;

struct MeshToOctreeConverterInternalData {
  Triangle* triangles;
  uint32_t count;
  uint32_t capacity;
  uint32_t pendingCapacity;
  Triangle smallData[kMeshToOctreeConverterSmallDataCapacity];
  std::vector<Triangle> heap;  // the storage once smallData is outgrown
};

class MeshToOctreeConverter : public oo::Object {
 public:
  explicit MeshToOctreeConverter(std::size_t capacity);  // -initWithCapacity:
  static std::shared_ptr<MeshToOctreeConverter> converterWithCapacity(
      std::size_t capacity);
  ~MeshToOctreeConverter() override;

  void addTriangle(Triangle tri);

  std::shared_ptr<Octree> findOctreeToDepth(std::size_t depth);

  // What "%@" prints between the braces of <OOMeshToOctreeConverter 0x...>{...}
  // (OODescription.h).
  std::optional<std::string> descriptionComponents() const;

 private:
  struct MeshToOctreeConverterInternalData _data = {};
};

}  // namespace oo
