/*	test_OOCrosshairs.mm
        Unit tests for OOCrosshairs (src/Core/OOCrosshairs.h), a C++ class.

        OOCrosshairs turns a crosshair definition (a property-list array of
   6-number arrays: alpha, x, y of each end of a line segment) into one vertex
   buffer of 2 coordinates and 4 colour components per endpoint, and render()
   hands that buffer to OpenGL. The test pins the buffer: the scale, the colour
   (green when there is none), the per-point alpha clamped and scaled by the
   overall alpha, the red marker a bad entry becomes, and that nothing is built
   when the overall alpha or the colour's alpha is zero. The test reads the
   members through the class's test friend, OOCrosshairsTestAccess.

        render() needs a GL context and the game (UNIVERSE, the GL state and
   matrix managers), which a unit test does not have; the test never calls it.
   The functions it references are defined below as link stubs that abort if
   called, so OOCrosshairs links with OOColor alone. Run:
   tests/run.sh unit
*/

#include <cmath>
#include <cstdlib>
#include <vector>

#include <gtest/gtest.h>

#include "MyOpenGLView.h"
#include "Color.h"
#include "Crosshairs.h"
#include "OpenGL.h"
#include "OpenGLMatrixManager.h"

// --- Link stubs: what render() references. The test never renders; a call is a
// test bug. -------

class Universe;
Universe* gSharedUniverse = nullptr;
GLfloat MyOpenGLView::getDisplay_z() { std::abort(); }

namespace oo {

void SetOpenGLState_(OpenGLStateID, const char*, unsigned) { std::abort(); }
void VerifyOpenGLState_(const char*, unsigned) { std::abort(); }
void GLPushModelView(void) { std::abort(); }
Matrix GLPopModelView(void) { std::abort(); }
void GLTranslateModelView(Vector) { std::abort(); }
bool CheckOpenGLErrors(const char*, ...) { std::abort(); }

}  // namespace oo

#if OO_CHECK_GL_HEAVY && OO_GL_STATE_VERIFICATION
void OOGLNoteCurrentFunction(const char*, unsigned) {
  std::abort();
}  // OOGL() in the debug flavour (OOOpenGLCheck.h)
#endif

// The vertex buffer: 12 floats (x, y, r, g, b, a of each endpoint) per line
// segment.
struct OOCrosshairsTestAccess {
  static std::vector<float> Buffer(oo::Crosshairs* crosshairs) {
    if (crosshairs->_data.empty()) return {};
    return std::vector<float>(crosshairs->_data.begin(),
                              crosshairs->_data.begin() + 12 * crosshairs->_count);
  }
};

namespace {

std::vector<float> Build(const oo::PList& points, float scale, oo::Color* color,
                         float alpha) {
  return OOCrosshairsTestAccess::Buffer(
      oo::makeShared<oo::Crosshairs>(points, scale, color, alpha).get());
}

oo::PList Segment(double a1, double x1, double y1, double a2, double x2,
                  double y2) {
  return oo::PList(oo::PList::Array{a1, x1, y1, a2, x2, y2});
}

bool Same(const std::vector<float>& actual,
          const std::vector<float>& expected) {
  if (actual.size() != expected.size()) return false;
  for (size_t i = 0; i < actual.size(); i++) {
    if (std::fabs(actual[i] - expected[i]) > 1e-6f) return false;
  }
  return true;
}

const std::vector<float> kRedMarker = {-0.01f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
                                       0.01f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f};

}  // namespace

TEST(OOCrosshairs, greenWithoutAColour) {
  oo::PList points(oo::PList::Array{Segment(1.0, 1.0, 2.0, 0.5, 3.0, 4.0)});
  EXPECT_TRUE(Same(Build(points, 2.0f, nullptr, 1.0f),
                {2, 4, 0, 1, 0, 1, 6, 8, 0, 1, 0, 0.5f}));
}

TEST(OOCrosshairs, colourAndAlpha) {
  std::shared_ptr<oo::Color> color = oo::Color::colorWithRed(0.25f, 0.5f, 0.75f, 0.5f);
  oo::PList points(oo::PList::Array{Segment(1.0, -1.0, 0.0, 0.5, 1.0, 0.0),
                                    Segment(0.0, 0.0, -2.0, 1.0, 0.0, 2.0)});

  // Point alpha * colour alpha, clamped, then * overall alpha.
  EXPECT_TRUE(Same(Build(points, 0.5f, color.get(), 0.5f),
                {-0.5f, 0,    0.25f, 0.5f,   0.75f, 0.25f, 0.5f,  0,
                 0.25f, 0.5f, 0.75f, 0.125f, 0,     -1,    0.25f, 0.5f,
                 0.75f, 0.0f, 0,     1,      0.25f, 0.5f,  0.75f, 0.25f}));
}

TEST(OOCrosshairs, alphaIsClamped) {
  oo::PList points(oo::PList::Array{Segment(4.0, 0.0, 0.0, -1.0, 1.0, 1.0)});
  // 4 * 1 clamps to 1 before the overall alpha, and a negative alpha to 0.
  EXPECT_TRUE(Same(Build(points, 1.0f, nullptr, 0.75f),
                {0, 0, 0, 1, 0, 0.75f, 1, 1, 0, 1, 0, 0}));
}

TEST(OOCrosshairs, badEntriesAreRedMarkers) {
  oo::PList shortEntry(oo::PList::Array{1.0, 2.0, 3.0, 4.0, 5.0});
  oo::PList points(oo::PList::Array{oo::PList("not an array"), shortEntry,
                                    Segment(1.0, 1.0, 1.0, 1.0, 2.0, 2.0)});

  std::vector<float> expected = kRedMarker;
  expected.insert(expected.end(), kRedMarker.begin(), kRedMarker.end());
  std::vector<float> good = {1, 1, 0, 1, 0, 1, 2, 2, 0, 1, 0, 1};
  expected.insert(expected.end(), good.begin(), good.end());
  EXPECT_TRUE(Same(Build(points, 1.0f, nullptr, 1.0f), expected));
}

TEST(OOCrosshairs, nothingIsBuilt) {
  oo::PList points(oo::PList::Array{Segment(1.0, 1.0, 2.0, 0.5, 3.0, 4.0)});
  EXPECT_TRUE(Build(points, 1.0f, nullptr, 0.0f).empty());  // no overall alpha
  EXPECT_TRUE(Build(points, 1.0f, nullptr, -1.0f).empty());
  EXPECT_TRUE(Build(points, 1.0f, oo::Color::clearColor().get(), 1.0f)
               .empty());  // a transparent colour
  EXPECT_TRUE(Build(oo::PList(oo::PList::Array{}), 1.0f, nullptr, 1.0f)
               .empty());                                     // no points
  EXPECT_TRUE(Build(oo::PList(), 1.0f, nullptr, 1.0f).empty());  // not an array
}
