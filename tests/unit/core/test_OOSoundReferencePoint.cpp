/*	test_OOSoundReferencePoint.mm
        Unit tests for OOSoundReferencePoint
   (src/Core/OOBasicSoundReferencePoint.h).

        The class is the no-op reference point of the OpenAL sound code
   (OOSound.h: positional sound is not implemented). A point can be made, takes
   a position, a velocity and an orientation without effect, and is freed by
   its last release. Run: tests/run.sh unit
   tests/run.sh unit
*/

#include <gtest/gtest.h>

#include "BasicSoundReferencePoint.h"

TEST(OOSoundReferencePoint, settersAreNoOps) {
  std::shared_ptr<oo::SoundReferencePoint> point = oo::makeShared<oo::SoundReferencePoint>();
  EXPECT_TRUE(point != nullptr);
  point->setPosition(make_vector(1.0f, 2.0f, 3.0f));
  point->setVelocity(make_vector(-1.0f, 0.0f, 0.5f));
  point->setOrientation(make_vector(0.0f, 0.0f, 1.0f));
  EXPECT_TRUE(point.use_count() == 1);
}

TEST(OOSoundReferencePoint, lastReleaseFrees) {
  std::shared_ptr<oo::SoundReferencePoint> point =
      oo::makeShared<oo::SoundReferencePoint>();  // the one owner, as +alloc's +1 was
  std::weak_ptr<oo::SoundReferencePoint> weak = point;
  EXPECT_TRUE(weak.lock().get() == point.get());
  point.reset();  // the last release
  EXPECT_TRUE(weak.lock().get() == nullptr);
}
