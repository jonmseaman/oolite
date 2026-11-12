/*	test_OODisplayGeometry.cpp
        Unit tests for OODisplayGeometry (src/Core/OODisplayGeometry.h): the
   screen-matching projection of spec 008. Each OO_TEST is one row of the
   table in specs/008-display-geometry-fov/contracts/geometry.md (G1-G15),
   with the numbers and tolerances the spec's stories give.

        Shape and size are measured from the local Jacobian of the scene ->
   eye mapping at the horizontal edge; line bend as the deviation of a straight
   scene line from a great circle, seen from the eye (research R8).
   Run: tests/run.sh unit
*/

#include <cmath>
#include <numbers>

#include "OODisplayGeometry.h"
#include "oo_test.hpp"

namespace {

constexpr double kPi = std::numbers::pi;
constexpr double kInch = 25.4;

double Rad(double deg) { return deg * kPi / 180.0; }
double Deg(double rad) { return rad * 180.0 / kPi; }

bool Near(double a, double b, double tolerance) {
  return std::fabs(a - b) <= tolerance;
}

OODirection FromAzEl(double azimuth, double elevation) {
  return {std::cos(elevation) * std::sin(azimuth), std::sin(elevation),
          std::cos(elevation) * std::cos(azimuth)};
}

double Azimuth(OODirection d) { return std::atan2(d.x, d.z); }
double Elevation(OODirection d) {
  return std::atan2(d.y, std::hypot(d.x, d.z));
}

OOScreenDescription Screen(double diagonalInches, double aspect,
                           std::optional<double> curvatureMm) {
  OOScreenDescription s;
  s.diagonalMm = diagonalInches * kInch;
  s.aspect = aspect;
  s.curvatureMm = curvatureMm;
  s.source = OOScreenSizeSource::kEntered;
  return s;
}

const OOScreenDescription kFlat27 = Screen(27, 16.0 / 9.0, std::nullopt);
const OOScreenDescription kCurved1800 = Screen(49, 32.0 / 9.0, 1800.0);
const OOScreenDescription kFlat49 = Screen(49, 32.0 / 9.0, std::nullopt);
const OOScreenDescription kG9 = Screen(49, 32.0 / 9.0, 1000.0);

OOFieldOfViewChoice Physical() { return {}; }

OOFieldOfViewChoice Custom(double horizontalDeg, OOWideLook look) {
  OOFieldOfViewChoice c;
  c.mode = OOFovMode::kCustom;
  c.customHorizontalDeg = horizontalDeg;
  c.look = look;
  return c;
}

OODisplayGeometry G9At(double distanceMm, OOFieldOfViewChoice choice,
                       int maxTexture = 16384) {
  return OODisplayGeometry(kG9, distanceMm, choice, 5120, 1440, 1.0, 1.0, true,
                           maxTexture);
}

// Horizontal scale (eye radians per scene radian, along the horizon) and
// vertical scale at scene azimuth t on the horizon.
void Scales(const OODisplayGeometry& g, double t, double* horizontal,
            double* vertical) {
  const double e = 1e-5;
  OODirection p = *g.sceneToEye(FromAzEl(t + e, 0));
  OODirection m = *g.sceneToEye(FromAzEl(t - e, 0));
  OODirection c = *g.sceneToEye(FromAzEl(t, 0));
  *horizontal = (Azimuth(p) - Azimuth(m)) / (2 * e) * std::cos(Elevation(c));
  OODirection u = *g.sceneToEye(FromAzEl(t, e));
  OODirection d = *g.sceneToEye(FromAzEl(t, -e));
  *vertical = (Elevation(u) - Elevation(d)) / (2 * e);
}

// Width-to-height ratio at the horizontal edge, relative to the centre.
double EdgeShape(const OODisplayGeometry& g) {
  double h0, v0, h1, v1;
  Scales(g, 0, &h0, &v0);
  Scales(g, Rad(g.horizontalDeg() / 2) * 0.999, &h1, &v1);
  return (h1 / v1) / (h0 / v0);
}

// Size at the horizontal edge, relative to the centre.
double EdgeSize(const OODisplayGeometry& g) {
  double h0, v0, h1, v1;
  Scales(g, 0, &h0, &v0);
  Scales(g, Rad(g.horizontalDeg() / 2) * 0.999, &h1, &v1);
  return h1 / h0;
}

// A straight scene line through the top of the view at the centre, seen from
// the eye: degrees between where it is at the centre and at the edge, measured
// on the eye's own gnomonic plane (zero when the line looks straight).
double TopLineBendDeg(const OODisplayGeometry& g) {
  const double p0 = Rad(g.verticalDeg() / 2);
  const double edge = Rad(g.horizontalDeg() / 2) * 0.999;
  auto gnomonicY = [&](double t) {
    double p = std::atan(std::tan(p0) * std::cos(t));
    OODirection d = *g.sceneToEye(FromAzEl(t, p));
    return d.y / d.z;
  };
  return std::fabs(Deg(std::atan(gnomonicY(0))) -
                   Deg(std::atan(gnomonicY(edge))));
}

// atan2 of the cross and dot products: exact for tiny angles, where acos of a
// dot product near 1 cannot resolve below about 1e-8 rad.
double AngleBetween(OODirection a, OODirection b) {
  double cx = a.y * b.z - a.z * b.y;
  double cy = a.z * b.x - a.x * b.z;
  double cz = a.x * b.y - a.y * b.x;
  double dot = a.x * b.x + a.y * b.y + a.z * b.z;
  return std::atan2(std::sqrt(cx * cx + cy * cy + cz * cz), dot);
}

}  // namespace

// G1: screen-matching off reproduces Universe::drawUniverse's frustum.
OO_TEST(legacyFrustumIsTodays) {
  const int sizes[][2] = {{640, 480}, {1920, 1080}, {5120, 1440}, {1080, 1920}};
  for (double deg : {57.2, 30.0, 80.0}) {
    double f = std::tan(Rad(deg) / 2);
    for (auto& s : sizes) {
      OODisplayGeometry g = OODisplayGeometry::legacy(f, s[0], s[1]);
      OO_CHECK(!g.screenMatching());
      OO_CHECK(!g.needsWarp());
      OO_CHECK(g.sliceCount() == 1);
      double aspect = static_cast<double>(s[1]) / s[0];
      OOFlatFrustum fr = g.flatFrustum();
      if (4 * aspect <= 3) {
        OO_CHECK(Near(fr.right, f, 1e-12) && Near(fr.left, -f, 1e-12));
        OO_CHECK(Near(fr.top, aspect * f, 1e-12));
        OO_CHECK(Near(fr.bottom, -aspect * f, 1e-12));
      } else {
        OO_CHECK(Near(fr.right, 3 * f / aspect / 4, 1e-12));
        OO_CHECK(Near(fr.top, 3 * f / 4, 1e-12));
      }
      OO_CHECK(Near(g.scriptFovValue(), 2 * Deg(std::atan(fr.right)), 1e-9));
      OO_CHECK(g.flatRenderWidthPx() == s[0] && g.flatRenderHeightPx() == s[1]);
    }
  }
}

// G2: Story 1, a 27" 16:9 flat screen at 60 cm.
OO_TEST(flat27At60) {
  OODisplayGeometry g(kFlat27, 600, Physical(), 2560, 1440);
  OO_CHECK(Near(g.physicalHorizontalDeg(), 53.0, 0.5));
  OO_CHECK(Near(g.physicalVerticalDeg(), 31.3, 0.5));
  OO_CHECK(Near(g.horizontalDeg(), g.physicalHorizontalDeg(), 1e-9));
  OO_CHECK(Near(g.verticalDeg(), g.physicalVerticalDeg(), 1e-9));
  OO_CHECK(!g.needsWarp());  // a flat screen at the physical angle is a perspective
}

// G3: Story 2, 49" 32:9 1800R at 80 cm, and the same panel flat.
OO_TEST(curved1800At80) {
  OODisplayGeometry g(kCurved1800, 800, Physical(), 5120, 1440);
  OO_CHECK(Near(g.physicalHorizontalDeg(), 80.0, 0.5));
  OO_CHECK(Near(g.physicalVerticalDeg(), 23.8, 0.5));
  OO_CHECK(g.needsWarp());
  OODisplayGeometry flat(kFlat49, 800, Physical(), 5120, 1440);
  OO_CHECK(Near(flat.physicalHorizontalDeg(), 73.7, 0.5));
}

// G4: Story 2, the Odyssey G9 (49" 32:9 1000R) at 80 cm and 100 cm.
OO_TEST(g9At80And100) {
  OODisplayGeometry g = G9At(800, Physical());
  OO_CHECK(Near(g.physicalHorizontalDeg(), 84.0, 0.5));
  OO_CHECK(Near(g.physicalVerticalDeg(), 23.8, 0.5));
  OODisplayGeometry c = G9At(1000, Physical());
  OO_CHECK(Near(c.physicalHorizontalDeg(), 68.6, 0.5));
  OO_CHECK(Near(c.physicalVerticalDeg(), 19.1, 0.5));
  // Eye at the centre of curvature: equal distances along the screen are equal
  // angles, within 1%.
  const double half = Rad(c.physicalHorizontalDeg() / 2);
  bool uniform = true;
  for (int i = 0; i <= 20; i++) {
    double x = 5120.0 * i / 20;
    double expected = (x / 5120.0 - 0.5) * 2 * half;
    double actual = Azimuth(c.windowToEye({x, 720}));
    if (!Near(actual, expected, 0.01 * half)) uniform = false;
  }
  OO_CHECK(uniform);
}

// G5: at the physical angle, shape and size hold from centre to edge.
OO_TEST(physicalShapeAndSize) {
  OODisplayGeometry setups[] = {
      OODisplayGeometry(kFlat27, 600, Physical(), 2560, 1440),
      OODisplayGeometry(kCurved1800, 800, Physical(), 5120, 1440),
      G9At(800, Physical()), G9At(1000, Physical())};
  for (const OODisplayGeometry& g : setups) {
    OO_CHECK(Near(EdgeShape(g), 1.0, 0.02));
    OO_CHECK(Near(EdgeSize(g), 1.0, 0.02));
  }
}

// G6: Story 3, the G9 at 80 cm with a custom 120 degrees.
OO_TEST(g9Custom120Looks) {
  OODisplayGeometry panini = G9At(800, Custom(120, OOWideLook::kPanini));
  OO_CHECK(panini.effectiveLook() == OOWideLook::kPanini);
  OO_CHECK(Near(panini.horizontalDeg(), 120.0, 1e-9));
  OO_CHECK(Near(EdgeShape(panini), 1.0, 0.02));
  OO_CHECK(Near(EdgeSize(panini), 1.25, 0.03));
  OO_CHECK(Near(panini.verticalDeg(), 36.0, 0.5));
  OO_CHECK(TopLineBendDeg(panini) <= 2.0);

  OODisplayGeometry even = G9At(800, Custom(120, OOWideLook::kEvenSpacing));
  OO_CHECK(Near(EdgeShape(even), 1.0, 0.02));
  OO_CHECK(Near(EdgeSize(even), 1.0, 0.02));
  OO_CHECK(Near(even.verticalDeg(), 34.0, 0.5));

  OODisplayGeometry classic = G9At(800, Custom(120, OOWideLook::kClassicFlat));
  OO_CHECK(TopLineBendDeg(classic) <= 0.2);
  OO_CHECK(Near(EdgeShape(classic), 1.49, 0.03));
  OO_CHECK(Near(classic.verticalDeg(), 44.1, 0.5));
}

// G7: the warp mesh's linear interpolation stays under 0.1 texel.
OO_TEST(warpMeshInterpolation) {
  for (double deg : {84.0, 120.0, 150.0}) {
    for (OOWideLook look : {OOWideLook::kPanini, OOWideLook::kEvenSpacing,
                            OOWideLook::kClassicFlat}) {
      OODisplayGeometry g = G9At(800, Custom(deg, look));
      std::vector<OOWarpVertex> mesh = g.warpMesh();
      OO_CHECK(!mesh.empty() && mesh.size() % 6 == 0);
      const double texW = g.flatRenderWidthPx() * g.sliceCount();
      const double texH = g.flatRenderHeightPx();
      double worst = 0;
      // The exact texture coordinate of a window point, in the slice the
      // triangle uses: recover it from the triangle's own vertices' slice.
      for (size_t i = 0; i < mesh.size(); i += 3) {
        const OOWarpVertex* t = &mesh[i];
        // Sample the triangle's centroid.
        double wx = (t[0].windowX + t[1].windowX + t[2].windowX) / 3;
        double wy = (t[0].windowY + t[1].windowY + t[2].windowY) / 3;
        double u = (t[0].u + t[1].u + t[2].u) / 3;
        double v = (t[0].v + t[1].v + t[2].v) / 3;
        int slice = static_cast<int>(
            std::floor(((t[0].u + t[1].u + t[2].u) / 3) * g.sliceCount()));
        if (slice >= g.sliceCount()) slice = g.sliceCount() - 1;
        OODirection d = g.windowToScene({wx * 5120.0, wy * 1440.0});
        double yaw = Rad(g.sliceYawDeg(slice));
        double x = d.x * std::cos(-yaw) + d.z * std::sin(-yaw);
        double z = -d.x * std::sin(-yaw) + d.z * std::cos(-yaw);
        OOFlatFrustum f = g.flatFrustum(slice);
        double eu = (slice + (x / z - f.left) / (f.right - f.left)) / g.sliceCount();
        double ev = (d.y / z - f.bottom) / (f.top - f.bottom);
        worst = std::fmax(worst, std::fabs(eu - u) * texW);
        worst = std::fmax(worst, std::fabs(ev - v) * texH);
      }
      OO_CHECK(worst < 0.1);
    }
  }
}

// G8: window -> scene -> window round trips.
OO_TEST(roundTrips) {
  OODisplayGeometry setups[] = {
      OODisplayGeometry(kFlat27, 600, Physical(), 2560, 1440),
      G9At(800, Physical()), G9At(800, Custom(120, OOWideLook::kPanini)),
      G9At(800, Custom(120, OOWideLook::kEvenSpacing)),
      G9At(800, Custom(120, OOWideLook::kClassicFlat)),
      OODisplayGeometry::legacy(std::tan(Rad(57.2) / 2), 1920, 1080)};
  for (const OODisplayGeometry& g : setups) {
    bool ok = true;
    for (int i = 0; i <= 10; i++) {
      for (int j = 0; j <= 10; j++) {
        OOWindowPoint p{5120.0 * i / 10, 1440.0 * j / 10};
        OODirection d = g.windowToScene(p);
        std::optional<OOWindowPoint> back = g.sceneToWindow(d);
        if (!back) {
          ok = false;
          continue;
        }
        OODirection again = g.windowToScene(*back);
        if (AngleBetween(d, again) > 1e-9) ok = false;
      }
    }
    OO_CHECK(ok);
  }
}

// G9: out-of-range inputs come back as the data-model limits.
OO_TEST(clamping) {
  OOScreenDescription s = Screen(1, 0.5, 500.0);
  OODisplayGeometry low(s, 50, Custom(5, OOWideLook::kPanini), 1920, 1080);
  OO_CHECK(Near(low.clampedScreen().diagonalMm, 254, 1e-9));
  OO_CHECK(Near(low.clampedScreen().aspect, 1.0, 1e-9));
  OO_CHECK(Near(*low.clampedScreen().curvatureMm, 800, 1e-9));
  OO_CHECK(Near(low.clampedViewerDistanceMm(), 200, 1e-9));
  OO_CHECK(Near(low.clampedChoice().customHorizontalDeg, 20, 1e-9));

  OOScreenDescription big = Screen(500, 9.0, 9000.0);
  OODisplayGeometry high(big, 9000, Custom(200, OOWideLook::kPanini), 1920, 1080);
  OO_CHECK(Near(high.clampedScreen().diagonalMm, 3048, 1e-9));
  OO_CHECK(Near(high.clampedScreen().aspect, 4.0, 1e-9));
  OO_CHECK(Near(*high.clampedScreen().curvatureMm, 4000, 1e-9));
  OO_CHECK(Near(high.clampedViewerDistanceMm(), 3000, 1e-9));
  OO_CHECK(Near(high.clampedChoice().customHorizontalDeg, 170, 1e-9));
}

// G10: the flat render keeps the centre's detail, and slices when one image
// would be too large.
OO_TEST(flatRenderSize) {
  for (double deg : {84.0, 120.0, 150.0}) {
    OODisplayGeometry g = G9At(800, Custom(deg, OOWideLook::kPanini));
    OOFlatFrustum f = g.flatFrustum(0);
    double texelsPerUnit = g.flatRenderWidthPx() / (f.right - f.left);
    // Window pixels per unit of scene gnomonic x at the centre.
    const double e = 1e-6;
    std::optional<OOWindowPoint> a = g.sceneToWindow({e, 0, 1});
    std::optional<OOWindowPoint> b = g.sceneToWindow({-e, 0, 1});
    OO_CHECK(a && b);
    double windowPerUnit = (a->x - b->x) / (2 * e);
    if (g.sliceCount() == 1) OO_CHECK(texelsPerUnit >= windowPerUnit * (1 - 1e-9));
    OO_CHECK(g.flatRenderWidthPx() <= 16384);
  }
  OODisplayGeometry small = G9At(800, Custom(120, OOWideLook::kPanini), 4096);
  OO_CHECK(small.sliceCount() > 1);
  OO_CHECK(small.flatRenderWidthPx() <= 4096);
  OODisplayGeometry wide = G9At(800, Custom(170, OOWideLook::kPanini));
  OO_CHECK(wide.sliceCount() > 1);
}

// G11: when the final warp is needed.
OO_TEST(needsWarp) {
  OO_CHECK(!OODisplayGeometry(kFlat27, 600, Physical(), 2560, 1440).needsWarp());
  OO_CHECK(!OODisplayGeometry(kFlat27, 600, Custom(80, OOWideLook::kClassicFlat),
                              2560, 1440)
                .needsWarp());
  OO_CHECK(!OODisplayGeometry(kFlat27, 600, Custom(40, OOWideLook::kPanini), 2560,
                              1440)
                .needsWarp());  // narrower than physical: a plain zoom
  OO_CHECK(OODisplayGeometry(kFlat27, 600, Custom(100, OOWideLook::kPanini), 2560,
                             1440)
               .needsWarp());
  OO_CHECK(G9At(800, Physical()).needsWarp());
  OODisplayGeometry noShaders(kG9, 800, Custom(120, OOWideLook::kPanini), 5120, 1440,
                              1.0, 1.0, false);
  OO_CHECK(!noShaders.needsWarp());
  OO_CHECK(noShaders.effectiveLook() == OOWideLook::kClassicFlat);
  OO_CHECK(Near(noShaders.horizontalDeg(), 120.0, 1e-9));
  OODisplayGeometry narrow = G9At(800, Custom(60, OOWideLook::kPanini));
  OO_CHECK(narrow.effectiveLook() == OOWideLook::kClassicFlat);
}

// G12: SC-006, a straight edge on the G9 at 80 cm at the physical angle.
OO_TEST(g9StraightLines) {
  OO_CHECK(TopLineBendDeg(G9At(800, Physical())) <= 0.2);
}

// G13: a window covering half the panel.
OO_TEST(windowedHalfPanel) {
  OODisplayGeometry g(kG9, 800, Physical(), 2560, 720, 0.5, 0.5);
  const double w = 49 * kInch * (32.0 / 9.0) / std::hypot(32.0 / 9.0, 1.0);
  const double h = w / (32.0 / 9.0);
  const double alpha = (0.5 * w / 2) / 1000.0;
  double expectedH = 2 * Deg(std::atan2(1000.0 * std::sin(alpha),
                                        1000.0 * std::cos(alpha) - (1000.0 - 800.0)));
  OO_CHECK(Near(g.physicalHorizontalDeg(), expectedH, 1e-6));
  OO_CHECK(Near(g.physicalVerticalDeg(), 2 * Deg(std::atan((0.5 * h / 2) / 800.0)),
                1e-6));
  OO_CHECK(Near(EdgeShape(g), 1.0, 0.02));
}

// G14: the eye behind the centre of curvature.
OO_TEST(eyeBehindCurve) {
  OODisplayGeometry g = G9At(1200, Physical());
  OO_CHECK(std::isfinite(g.physicalHorizontalDeg()) && g.physicalHorizontalDeg() > 0);
  OO_CHECK(std::isfinite(g.physicalVerticalDeg()) && g.physicalVerticalDeg() > 0);
  OO_CHECK(Near(EdgeShape(g), 1.0, 0.02));
}

// G15: the window centre is straight ahead in every look (the crosshair).
OO_TEST(centreIsStraightAhead) {
  OODisplayGeometry setups[] = {
      OODisplayGeometry(kFlat27, 600, Physical(), 2560, 1440),
      G9At(800, Physical()), G9At(800, Custom(120, OOWideLook::kPanini)),
      G9At(800, Custom(120, OOWideLook::kEvenSpacing)),
      G9At(800, Custom(150, OOWideLook::kClassicFlat)),
      OODisplayGeometry::legacy(std::tan(Rad(57.2) / 2), 1920, 1080)};
  int widths[] = {2560, 5120, 5120, 5120, 5120, 1920};
  int heights[] = {1440, 1440, 1440, 1440, 1440, 1080};
  for (int i = 0; i < 6; i++) {
    OODirection d = setups[i].windowToScene({widths[i] / 2.0, heights[i] / 2.0});
    OO_CHECK(AngleBetween(d, {0, 0, 1}) < 1e-9);
  }
}

OO_TEST_MAIN()
