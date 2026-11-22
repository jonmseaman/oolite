/*	test_OOJoystickProfile.mm
        Unit tests for the joystick axis profiles
   (src/Core/OOJoystickProfile.h).

        OOJoystickAxisProfile maps a raw axis position through a dead zone and a
   response curve: OOJoystickStandardAxisProfile's power curve, or
   OOJoystickSplineAxisProfile's spline through control points (made of private
   OOJoystickSplineSegment pieces). The expectations cover: the defaults and
   clamps, the dead zone, both curves at pinned sample points, the spline's
   control-point editing rules, and what a copy keeps (a copy does NOT keep the
   dead zone; the spline copy keeps the points). The cases use the C++ API
   (oo::makeShared, copy(), dynamic_cast/typeid for class checks). Run: bash
   tests/run.sh unit
*/

#include <cmath>
#include <typeinfo>
#include <vector>

#include <gtest/gtest.h>

#include "JoystickManager.h"

namespace {

bool Near(double a, double b) { return std::fabs(a - b) < 1e-9; }

bool SamePoint(NSPoint p, double x, double y) {
  return Near(p.x, x) && Near(p.y, y);
}

// Whether profile is exactly a T (typeid of a plain pointer's object, not of a
// Ref's overloaded operator*, which clang counts as a side effect).
template <class T>
bool IsExactly(oo::JoystickAxisProfile* profile) {
  return profile != nullptr && typeid(*profile) == typeid(T);
}

}  // namespace

TEST(OOJoystickProfile, axisProfileDeadzone) {
  {
    std::shared_ptr<oo::JoystickAxisProfile> profile =
        oo::makeShared<oo::JoystickAxisProfile>();
    EXPECT_TRUE(Near(profile->deadzone(), STICK_DEADZONE));
    EXPECT_TRUE(Near(profile->rawValue(0.3), 0.3));  // identity curve
    EXPECT_TRUE(profile->value(STICK_DEADZONE / 2) == 0.0);
    EXPECT_TRUE(profile->value(-STICK_DEADZONE / 2) == 0.0);
    EXPECT_TRUE(Near(profile->value(1.0), 1.0));
    EXPECT_TRUE(Near(profile->value(-1.0), -1.0));
    EXPECT_TRUE(Near(profile->value(0.5),
                  (0.5 - STICK_DEADZONE) / (1.0 - STICK_DEADZONE)));
    EXPECT_TRUE(Near(profile->value(-0.5),
                  -(0.5 - STICK_DEADZONE) / (1.0 - STICK_DEADZONE)));

    profile->setDeadzone(1.0);
    EXPECT_TRUE(Near(profile->deadzone(), STICK_MAX_DEADZONE));
    profile->setDeadzone(-1.0);
    EXPECT_TRUE(profile->deadzone() == 0.0);
    profile->setDeadzone(STICK_DEADZONE * 1.5);
    EXPECT_TRUE(Near(profile->deadzone(), STICK_DEADZONE * 1.5));

    std::shared_ptr<oo::JoystickAxisProfile> copy = profile->copy();
    EXPECT_TRUE(copy != profile && IsExactly<oo::JoystickAxisProfile>(copy.get()));
    EXPECT_TRUE(
        Near(copy->deadzone(), STICK_DEADZONE));  // the dead zone is not copied
  }
}

TEST(OOJoystickProfile, standardProfile) {
  {
    std::shared_ptr<oo::JoystickStandardAxisProfile> profile =
        oo::makeShared<oo::JoystickStandardAxisProfile>();
    EXPECT_TRUE(dynamic_cast<oo::JoystickAxisProfile*>(profile.get()) != nullptr);
    EXPECT_TRUE(profile->power() == 1.0 && profile->parameter() == 1.0);
    EXPECT_TRUE(Near(profile->deadzone(), STICK_DEADZONE));
    EXPECT_TRUE(Near(profile->rawValue(0.4), 0.4));

    profile->setPower(0.5);
    EXPECT_TRUE(profile->power() == 1.0);
    profile->setPower(25.0);
    EXPECT_TRUE(profile->power() == STICKPROFILE_MAX_POWER);
    profile->setPower(3.0);
    EXPECT_TRUE(profile->power() == 3.0);
    profile->setParameter(2.0);
    EXPECT_TRUE(profile->parameter() == 1.0);
    profile->setParameter(-2.0);
    EXPECT_TRUE(profile->parameter() == 0.0);
    profile->setParameter(0.5);
    EXPECT_TRUE(profile->parameter() == 0.5);

    // parameter * x^power - (parameter - 1) * x, clamped to [0, 1], odd.
    EXPECT_TRUE(Near(profile->rawValue(0.5), 0.5 * 0.125 + 0.5 * 0.5));
    EXPECT_TRUE(Near(profile->rawValue(-0.5), -(0.5 * 0.125 + 0.5 * 0.5)));
    EXPECT_TRUE(Near(profile->rawValue(1.0), 1.0));
    EXPECT_TRUE(Near(profile->value(0.5),
                  profile->rawValue(
                      (0.5 - STICK_DEADZONE) /
                      (1.0 - STICK_DEADZONE))));  // value: reaches the override

    profile->setDeadzone(0.0);
    std::shared_ptr<oo::JoystickStandardAxisProfile> copy(oo::share(
        dynamic_cast<oo::JoystickStandardAxisProfile*>(profile->copy().get())));
    EXPECT_TRUE(copy != nullptr &&
             IsExactly<oo::JoystickStandardAxisProfile>(copy.get()));
    EXPECT_TRUE(copy->power() == 3.0 && copy->parameter() == 0.5);
    EXPECT_TRUE(Near(copy->deadzone(), STICK_DEADZONE));  // not copied
  }
}

TEST(OOJoystickProfile, splineProfileEditing) {
  {
    std::shared_ptr<oo::JoystickSplineAxisProfile> profile =
        oo::makeShared<oo::JoystickSplineAxisProfile>();
    EXPECT_TRUE(dynamic_cast<oo::JoystickAxisProfile*>(profile.get()) != nullptr);
    EXPECT_TRUE(profile->countPoints() == 0 && profile->controlPoints().empty());
    EXPECT_TRUE(Near(profile->rawValue(0.3), 0.3) &&
             Near(profile->rawValue(-0.3), -0.3));  // the straight line
    EXPECT_TRUE(Near(profile->gradient(0.3), 1.0));
    EXPECT_TRUE(SamePoint(profile->pointAtIndex(-1), 0, 0) &&
             SamePoint(profile->pointAtIndex(0), 1, 1));

    EXPECT_TRUE(profile->addControl(NSMakePoint(0.01, 0.5)) ==
             -1);  // too near the ends
    EXPECT_TRUE(profile->addControl(NSMakePoint(0.99, 0.5)) == -1);
    EXPECT_TRUE(profile->addControl(NSMakePoint(0.5, 0.25)) == 0);
    EXPECT_TRUE(profile->addControl(NSMakePoint(0.25, 0.1)) ==
             0);  // inserted in x order
    EXPECT_TRUE(profile->addControl(NSMakePoint(0.75, 0.6)) == 2);
    EXPECT_TRUE(profile->addControl(NSMakePoint(0.26, 0.2)) ==
             0);  // within the spacing: replaces
    EXPECT_TRUE(profile->countPoints() == 3);
    EXPECT_TRUE(SamePoint(profile->pointAtIndex(0), 0.26, 0.2));
    EXPECT_TRUE(SamePoint(profile->pointAtIndex(1), 0.5, 0.25));
    EXPECT_TRUE(SamePoint(profile->pointAtIndex(2), 0.75, 0.6));
    EXPECT_TRUE(SamePoint(profile->pointAtIndex(3), 1, 1));
    std::vector<NSPoint> points = profile->controlPoints();
    EXPECT_TRUE(points.size() == 3 && SamePoint(points[1], 0.5, 0.25));

    profile->moveControl(
        1, NSMakePoint(0.1, 0.3));  // past its left neighbour: kept right of it
    EXPECT_TRUE(SamePoint(profile->pointAtIndex(1), 0.26 + 0.02, 0.3));
    profile->moveControl(1, NSMakePoint(0.5, 1.5));  // y clamped
    EXPECT_TRUE(SamePoint(profile->pointAtIndex(1), 0.5, 1.0));
    profile->moveControl(5, NSMakePoint(0.5, 0.5));  // out of range: nothing
    EXPECT_TRUE(profile->countPoints() == 3);

    profile->removeControl(1);
    EXPECT_TRUE(profile->countPoints() == 2 &&
             SamePoint(profile->pointAtIndex(1), 0.75, 0.6));
    profile->removeControl(7);
    EXPECT_TRUE(profile->countPoints() == 2);
    profile->clearControlPoints();
    EXPECT_TRUE(profile->countPoints() == 0 && Near(profile->rawValue(0.6), 0.6));
  }
}

TEST(OOJoystickProfile, splineProfileCurve) {
  {
    std::shared_ptr<oo::JoystickSplineAxisProfile> profile =
        oo::makeShared<oo::JoystickSplineAxisProfile>();
    profile->addControl(NSMakePoint(0.5, 0.25));
    profile->addControl(NSMakePoint(0.8, 0.5));

    // Passes through its control points and the ends; odd; clamped to [0, 1].
    EXPECT_TRUE(Near(profile->rawValue(0.0), 0.0));
    EXPECT_TRUE(Near(profile->rawValue(0.5), 0.25));
    EXPECT_TRUE(Near(profile->rawValue(0.8), 0.5));
    EXPECT_TRUE(Near(profile->rawValue(1.0), 1.0));
    EXPECT_TRUE(Near(profile->rawValue(-0.5), -0.25));
    // Pinned: the linear-quadratic-quadratic curve
    // and its slope.
    EXPECT_TRUE(Near(profile->rawValue(0.05), 0.019375) &&
             Near(profile->gradient(0.05), 0.4));
    EXPECT_TRUE(Near(profile->rawValue(0.2), 0.085) &&
             Near(profile->gradient(0.2), 0.475));
    EXPECT_TRUE(Near(profile->rawValue(0.35), 0.161875) &&
             Near(profile->gradient(0.35), 0.55));
    EXPECT_TRUE(Near(profile->rawValue(0.65), 0.3421875) &&
             Near(profile->gradient(0.65), 0.71875));
    EXPECT_TRUE(Near(profile->rawValue(0.95), 0.8375) &&
             Near(profile->gradient(0.95), 3.0));
    EXPECT_TRUE(Near(profile->rawValue(-0.65), -0.3421875));

    std::shared_ptr<oo::JoystickSplineAxisProfile> copy(oo::share(
        dynamic_cast<oo::JoystickSplineAxisProfile*>(profile->copy().get())));
    EXPECT_TRUE(copy != nullptr &&
             IsExactly<oo::JoystickSplineAxisProfile>(copy.get()));
    EXPECT_TRUE(copy->countPoints() == 2 &&
             Near(copy->rawValue(0.3), profile->rawValue(0.3)));
    copy->addControl(NSMakePoint(0.3, 0.3));
    EXPECT_TRUE(profile->countPoints() == 2);  // the original is untouched
    EXPECT_TRUE(Near(copy->rawValue(0.3), 0.3));
  }
}

TEST(OOJoystickProfile, cxxProfiles) {
  std::shared_ptr<oo::JoystickStandardAxisProfile> standard =
      oo::makeShared<oo::JoystickStandardAxisProfile>();
  standard->setPower(3.0);
  standard->setParameter(0.5);
  EXPECT_TRUE(Near(standard->rawValue(0.5), 0.5 * 0.125 + 0.5 * 0.5));
  oo::JoystickAxisProfile* base = standard.get();
  EXPECT_TRUE(Near(base->value(1.0), 1.0) &&
           Near(base->rawValue(-0.5), -(0.5 * 0.125 + 0.5 * 0.5)));  // virtual

  standard->setDeadzone(0.0);
  std::shared_ptr<oo::JoystickAxisProfile> copy = standard->copy();
  oo::JoystickStandardAxisProfile* standardCopy =
      dynamic_cast<oo::JoystickStandardAxisProfile*>(copy.get());
  EXPECT_TRUE(standardCopy != nullptr && standardCopy->power() == 3.0 &&
           standardCopy->parameter() == 0.5);
  EXPECT_TRUE(Near(copy->deadzone(), STICK_DEADZONE));  // not copied

  std::shared_ptr<oo::JoystickSplineAxisProfile> spline =
      oo::makeShared<oo::JoystickSplineAxisProfile>();
  EXPECT_TRUE(spline->addControl(NSMakePoint(0.5, 0.25)) == 0 &&
           spline->addControl(NSMakePoint(0.8, 0.5)) == 1);
  EXPECT_TRUE(Near(spline->rawValue(0.65), 0.3421875) &&
           Near(spline->gradient(0.95), 3.0));
  std::shared_ptr<oo::JoystickAxisProfile> splineCopy = spline->copy();
  EXPECT_TRUE(dynamic_cast<oo::JoystickSplineAxisProfile*>(splineCopy.get()) !=
           nullptr);
  EXPECT_TRUE(Near(splineCopy->rawValue(0.35), 0.161875));
  EXPECT_TRUE(dynamic_cast<oo::JoystickStandardAxisProfile*>(
               oo::makeShared<oo::JoystickAxisProfile>()->copy().get()) == nullptr);
}
