/*	test_OOVector.mm
        Unit tests for OONativeVector (src/Core/OOVector.h), the box that stores
   a Vector in a property list. The box is a PList::Object node's foreign
   object itself, and the cases below ask the C++ box. The rest of OOVector.mm
   is plain C and stays verbatim; the free functions the box shares a file with
   are pinned here too.

        The tests pin that the box hands back the vector it was made with, is
   an OONativeVector, and survives an Object node (as OOJSShip.mm and
   OOPListGameTypes.mm use it). Run: tests/run.sh unit
*/

#include <gtest/gtest.h>

#include "Maths.h"
#include "oofnd/PList.hpp"
#include <memory>

namespace {

bool Same(Vector a, Vector b) { return a.x == b.x && a.y == b.y && a.z == b.z; }

}  // namespace

TEST(OOVector, boxHoldsItsVector) {
  {
    Vector v = make_vector(1.5f, -2.0f, 3.25f);
    std::shared_ptr<oo::NativeVector> box = oo::makeShared<oo::NativeVector>(v);
    EXPECT_TRUE(box != nullptr);
    EXPECT_TRUE(box->className() == "OONativeVector");
    EXPECT_TRUE(Same(box->getVector(), v));

    std::shared_ptr<oo::NativeVector> zero = oo::makeShared<oo::NativeVector>(kZeroVector);
    EXPECT_TRUE(Same(zero->getVector(), kZeroVector));
    EXPECT_TRUE(zero != box);
  }
}

TEST(OOVector, boxSurvivesAnObjectNode) {
  {
    Vector v = make_vector(4.0f, 5.0f, 6.0f);
    std::shared_ptr<oo::NativeVector> box = oo::makeShared<oo::NativeVector>(v);
    oo::PList node = oo::PList(oo::PList::Object(box));
    const oo::PList::Object* object = node.getIf<oo::PList::Object>();
    EXPECT_TRUE(object != nullptr && object->get() == box.get());
    oo::NativeVector* held = dynamic_cast<oo::NativeVector*>(object->get());
    EXPECT_TRUE(held != nullptr);
    EXPECT_TRUE(Same(held->getVector(), v));
  }
}

TEST(OOVector, freeFunctions) {
  EXPECT_TRUE(VectorDescription(make_vector(1.0f, 0.5f, -2.0f)) == "(1, 0.5, -2)");
  EXPECT_TRUE(Same(kBasisXVector, make_vector(1, 0, 0)) &&
           Same(kBasisYVector, make_vector(0, 1, 0)) &&
           Same(kBasisZVector, make_vector(0, 0, 1)));
  EXPECT_TRUE(
      Same(oo::RandomPositionInBoundingBox((BoundingBox){{2, 3, 4}, {2, 3, 4}}),
           make_vector(2, 3, 4)));
  ranrot_srand(12345);  // unseeded, randf() is always 0 and
                        // OORandomUnitVector() never returns
  for (int i = 0; i < 100; i++) {
    EXPECT_TRUE(fabs(magnitude(oo::RandomUnitVector()) - 1.0f) < 1e-5f);
    EXPECT_TRUE(magnitude(oo::VectorRandomSpatial(2.0f)) <= 2.0f + 1e-5f);
    EXPECT_TRUE(magnitude(oo::VectorRandomRadial(3.0f)) <= 3.0f + 1e-5f);
  }
}

TEST(OOVector, cxxBox) {
  Vector v = make_vector(-1.0f, 0.25f, 8.0f);
  std::shared_ptr<oo::NativeVector> box = oo::makeShared<oo::NativeVector>(v);
  EXPECT_TRUE(Same(box->getVector(), v));
}
