/*

OODisplayGeometry.cpp

The screen-matching projection: panel geometry, wide-angle looks, the flat
render's frustum and size, and the warp mesh. See OODisplayGeometry.h.


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

#include "OODisplayGeometry.h"

#include <algorithm>
#include <cmath>
#include <numbers>

namespace {

constexpr double kPi = std::numbers::pi;

// Data-model limits (specs/008-display-geometry-fov/data-model.md).
constexpr double kMinDiagonalMm = 254.0;
constexpr double kMaxDiagonalMm = 3048.0;
constexpr double kMinAspect = 1.0;
constexpr double kMaxAspect = 4.0;
constexpr double kMinCurvatureMm = 800.0;
constexpr double kMaxCurvatureMm = 4000.0;
constexpr double kMinDistanceMm = 200.0;
constexpr double kMaxDistanceMm = 3000.0;
constexpr double kMinCustomDeg = 20.0;
constexpr double kMaxCustomDeg = 170.0;

constexpr int kMaxSlices = 3;
constexpr double kMaxSliceHalfAngle = 80.0 * kPi / 180.0;
constexpr double kMaxPixelFactor = 3.0;  // flat render vs window pixels

double Rad(double deg) { return deg * kPi / 180.0; }
double Deg(double rad) { return rad * 180.0 / kPi; }

OODirection Normalised(double x, double y, double z) {
  double length = std::sqrt(x * x + y * y + z * z);
  return {x / length, y / length, z / length};
}

double Azimuth(OODirection d) { return std::atan2(d.x, d.z); }

double Elevation(OODirection d) {
  return std::atan2(d.y, std::hypot(d.x, d.z));
}

OODirection FromAzimuthElevation(double azimuth, double elevation) {
  return {std::cos(elevation) * std::sin(azimuth), std::sin(elevation),
          std::cos(elevation) * std::cos(azimuth)};
}

// Turn a direction by -yaw about +y, into a slice's own frame.
OODirection IntoSlice(OODirection d, double yaw) {
  return {d.x * std::cos(yaw) - d.z * std::sin(yaw), d.y,
          d.x * std::sin(yaw) + d.z * std::cos(yaw)};
}

}  // namespace

OODisplayGeometry OODisplayGeometry::legacy(double fovTangent,
                                            int windowWidthPx,
                                            int windowHeightPx) {
  OODisplayGeometry g;
  g._windowWidth = std::max(1, windowWidthPx);
  g._windowHeight = std::max(1, windowHeightPx);
  // Universe::drawUniverse: 4:3 or wider keeps the horizontal half-tangent;
  // narrower keeps the vertical one at what a 4:3 window would have.
  double aspect = static_cast<double>(g._windowHeight) / g._windowWidth;
  if (4 * aspect <= 3) {
    g._linear = {-fovTangent, fovTangent, -aspect * fovTangent,
                 aspect * fovTangent};
  } else {
    g._linear = {-3 * fovTangent / aspect / 4, 3 * fovTangent / aspect / 4,
                 -3 * fovTangent / 4, 3 * fovTangent / 4};
  }
  g._halfH = std::atan(g._linear.right);
  g._physicalHalfH = g._halfH;
  g._physicalHalfV = std::atan(g._linear.top);
  g.buildFlatRender(1 << 30);
  return g;
}

OODisplayGeometry::OODisplayGeometry(
    const OOScreenDescription& screen, double viewerDistanceMm,
    const OOFieldOfViewChoice& choice, int windowWidthPx, int windowHeightPx,
    double windowFractionX, double windowFractionY, bool shadersAvailable,
    int maxTextureSizePx)
    : _screenMatching(true), _screen(screen), _choice(choice) {
  _screen.diagonalMm =
      std::clamp(screen.diagonalMm, kMinDiagonalMm, kMaxDiagonalMm);
  _screen.aspect = std::clamp(screen.aspect, kMinAspect, kMaxAspect);
  if (screen.curvatureMm && *screen.curvatureMm > 0) {
    _screen.curvatureMm =
        std::clamp(*screen.curvatureMm, kMinCurvatureMm, kMaxCurvatureMm);
  } else {
    _screen.curvatureMm.reset();
  }
  _distanceMm = std::clamp(viewerDistanceMm, kMinDistanceMm, kMaxDistanceMm);
  _choice.customHorizontalDeg =
      std::clamp(choice.customHorizontalDeg, kMinCustomDeg, kMaxCustomDeg);

  _windowWidth = std::max(1, windowWidthPx);
  _windowHeight = std::max(1, windowHeightPx);
  double fx = std::clamp(windowFractionX, 1e-3, 1.0);
  double fy = std::clamp(windowFractionY, 1e-3, 1.0);

  // The diagonal is measured along the surface, as manufacturers do.
  double a = _screen.aspect;
  double widthMm = _screen.diagonalMm * a / std::sqrt(a * a + 1);
  double heightMm = widthMm / a;
  _halfArcMm = fx * widthMm / 2;
  _halfHeightMm = fy * heightMm / 2;
  _radiusMm = _screen.curvatureMm ? *_screen.curvatureMm : 0.0;

  double edge[3];
  panelPoint(_halfArcMm, 0, edge);
  _physicalHalfH = std::atan2(edge[0], edge[2]);
  _physicalHalfV = std::atan2(_halfHeightMm, _distanceMm);

  _halfH = _choice.mode == OOFovMode::kCustom
               ? Rad(_choice.customHorizontalDeg / 2)
               : _physicalHalfH;

  bool wider =
      _choice.mode == OOFovMode::kCustom && _halfH > _physicalHalfH + 1e-12;
  _effectiveLook =
      (shadersAvailable && wider) ? _choice.look : OOWideLook::kClassicFlat;
  bool curved = _radiusMm > 0;
  _warp = shadersAvailable &&
          (curved || _effectiveLook != OOWideLook::kClassicFlat);

  const double e = _physicalHalfH;
  const double h = _halfH;
  _evenK = e / h;
  _classicC = std::tan(e) / std::tan(h);
  _paniniXe = 2 * std::tan(h / 2);
  _paniniK = _halfArcMm / _paniniXe;

  if (!_warp) {
    // A flat perspective spread evenly over the window: exact on a flat
    // panel, the fallback on a curved one without shaders (FR-017).
    double right = std::tan(h);
    double top = right * _halfHeightMm / _halfArcMm;
    _linear = {-right, right, -top, top};
  }
  buildFlatRender(maxTextureSizePx);
}

double OODisplayGeometry::physicalHorizontalDeg() const {
  return 2 * Deg(_physicalHalfH);
}

double OODisplayGeometry::physicalVerticalDeg() const {
  return 2 * Deg(_physicalHalfV);
}

double OODisplayGeometry::horizontalDeg() const {
  if (!_warp) return 2 * Deg(std::atan(_linear.right));
  return 2 * Deg(_halfH);
}

double OODisplayGeometry::verticalDeg() const {
  if (!_warp) return 2 * Deg(std::atan(_linear.top));
  return 2 * Deg(Elevation(windowToScene({_windowWidth / 2.0, 0.0})));
}

OOFlatFrustum OODisplayGeometry::flatFrustum(int slice) const {
  return _slices[std::clamp(slice, 0, sliceCount() - 1)].frustum;
}

double OODisplayGeometry::sliceYawDeg(int slice) const {
  return Deg(_slices[std::clamp(slice, 0, sliceCount() - 1)].yaw);
}

// Panel model --------------------------------------------------------------

void OODisplayGeometry::panelPoint(double s, double yp, double out[3]) const {
  if (_radiusMm <= 0) {
    out[0] = s;
    out[1] = yp;
    out[2] = _distanceMm;
    return;
  }
  // A cylinder about a vertical axis at z = D - R; arc length s from the
  // centre of the panel.
  double alpha = s / _radiusMm;
  out[0] = _radiusMm * std::sin(alpha);
  out[1] = yp;
  out[2] = (_distanceMm - _radiusMm) + _radiusMm * std::cos(alpha);
}

double OODisplayGeometry::windowXToArc(double px) const {
  return (px / _windowWidth - 0.5) * 2 * _halfArcMm;
}

double OODisplayGeometry::windowYToHeight(double py) const {
  return (0.5 - py / _windowHeight) * 2 * _halfHeightMm;
}

// Where a direction from the eye meets the panel, as a window point.
std::optional<OOWindowPoint> OODisplayGeometry::eyeToPanelWindow(
    OODirection d) const {
  double s, yp;
  if (_radiusMm <= 0) {
    if (d.z <= 0) return std::nullopt;
    double t = _distanceMm / d.z;
    s = t * d.x;
    yp = t * d.y;
  } else {
    double c = _distanceMm - _radiusMm;  // axis of curvature
    double qa = d.x * d.x + d.z * d.z;
    if (qa <= 0) return std::nullopt;
    double qb = d.z * c;
    double qc = c * c - _radiusMm * _radiusMm;
    double disc = qb * qb - qa * qc;
    if (disc < 0) return std::nullopt;
    double t = (qb + std::sqrt(disc)) / qa;  // the far side: the panel
    if (t <= 0) return std::nullopt;
    s = _radiusMm * std::atan2(t * d.x, t * d.z - c);
    yp = t * d.y;
  }
  return OOWindowPoint{(s / (2 * _halfArcMm) + 0.5) * _windowWidth,
                       (0.5 - yp / (2 * _halfHeightMm)) * _windowHeight};
}

void OODisplayGeometry::paniniSceneToPanel(double azimuth, double elevation,
                                           double* s, double* yp) const {
  double half = std::cos(azimuth / 2);
  *s = _paniniK * 2 * std::tan(azimuth / 2);
  *yp = _paniniK * std::tan(elevation) / (half * half);
}

// The four mappings ---------------------------------------------------------

OODirection OODisplayGeometry::windowToEye(OOWindowPoint p) const {
  if (!_warp) return windowToScene(p);
  double point[3];
  panelPoint(windowXToArc(p.x), windowYToHeight(p.y), point);
  return Normalised(point[0], point[1], point[2]);
}

std::optional<OOWindowPoint> OODisplayGeometry::eyeToWindow(
    OODirection d) const {
  if (!_warp) return sceneToWindow(d);
  return eyeToPanelWindow(d);
}

OODirection OODisplayGeometry::eyeToScene(OODirection d) const {
  if (!_warp) return d;
  switch (_effectiveLook) {
    case OOWideLook::kClassicFlat:
      return Normalised(d.x / d.z / _classicC, d.y / d.z / _classicC, 1.0);
    case OOWideLook::kEvenSpacing:
      return FromAzimuthElevation(Azimuth(d) / _evenK, Elevation(d) / _evenK);
    case OOWideLook::kPanini: {
      std::optional<OOWindowPoint> w = eyeToPanelWindow(d);
      if (!w) return d;
      double s = windowXToArc(w->x);
      double yp = windowYToHeight(w->y);
      double azimuth = 2 * std::atan(s / (2 * _paniniK));
      double half = std::cos(azimuth / 2);
      double elevation = std::atan(yp * half * half / _paniniK);
      return FromAzimuthElevation(azimuth, elevation);
    }
  }
  return d;
}

std::optional<OODirection> OODisplayGeometry::sceneToEye(OODirection d) const {
  if (!_warp) return d;
  switch (_effectiveLook) {
    case OOWideLook::kClassicFlat:
      if (d.z <= 0) return std::nullopt;
      return Normalised(_classicC * d.x / d.z, _classicC * d.y / d.z, 1.0);
    case OOWideLook::kEvenSpacing:
      return FromAzimuthElevation(_evenK * Azimuth(d), _evenK * Elevation(d));
    case OOWideLook::kPanini: {
      double s, yp, point[3];
      paniniSceneToPanel(Azimuth(d), Elevation(d), &s, &yp);
      panelPoint(s, yp, point);
      return Normalised(point[0], point[1], point[2]);
    }
  }
  return d;
}

std::optional<OOWindowPoint> OODisplayGeometry::sceneToWindow(
    OODirection d) const {
  if (!_warp) {
    if (d.z <= 0) return std::nullopt;
    double x = d.x / d.z;
    double y = d.y / d.z;
    return OOWindowPoint{
        (x - _linear.left) / (_linear.right - _linear.left) * _windowWidth,
        (_linear.top - y) / (_linear.top - _linear.bottom) * _windowHeight};
  }
  std::optional<OODirection> eye = sceneToEye(d);
  if (!eye) return std::nullopt;
  return eyeToPanelWindow(*eye);
}

OODirection OODisplayGeometry::windowToScene(OOWindowPoint p) const {
  if (!_warp) {
    double x =
        _linear.left + p.x / _windowWidth * (_linear.right - _linear.left);
    double y =
        _linear.top - p.y / _windowHeight * (_linear.top - _linear.bottom);
    return Normalised(x, y, 1.0);
  }
  return eyeToScene(windowToEye(p));
}

double OODisplayGeometry::localPixelsPerRadian(OOWindowPoint p) const {
  const double e = 1e-4;
  OODirection d = windowToScene(p);
  double az = Azimuth(d);
  double el = Elevation(d);
  auto distance = [&](OODirection a, OODirection b) {
    std::optional<OOWindowPoint> pa = sceneToWindow(a);
    std::optional<OOWindowPoint> pb = sceneToWindow(b);
    if (!pa || !pb) return 0.0;
    return std::hypot(pa->x - pb->x, pa->y - pb->y);
  };
  double horizontal = distance(FromAzimuthElevation(az + e, el),
                               FromAzimuthElevation(az - e, el)) /
                      (2 * e * std::cos(el));
  double vertical = distance(FromAzimuthElevation(az, el + e),
                             FromAzimuthElevation(az, el - e)) /
                    (2 * e);
  return (horizontal + vertical) / 2;
}

// Flat render ----------------------------------------------------------------

double OODisplayGeometry::sceneAzimuthAtWindowX(double px) const {
  return Azimuth(windowToScene({px, _windowHeight / 2.0}));
}

void OODisplayGeometry::buildFlatRender(int maxTextureSizePx) {
  _slices.clear();
  _seamWindowX.clear();
  if (!_warp) {
    _slices.push_back({0.0, _linear});
    _flatWidthPx = _windowWidth;
    _flatHeightPx = _windowHeight;
    return;
  }

  // Scene azimuth depends only on the window column in every look, so slice
  // seams are vertical window lines.
  const double left = sceneAzimuthAtWindowX(0);
  const double right = sceneAzimuthAtWindowX(_windowWidth);
  const double windowPixels = static_cast<double>(_windowWidth) * _windowHeight;

  auto columnAt = [&](double azimuth) {
    double lo = 0, hi = _windowWidth;
    for (int i = 0; i < 60; i++) {
      double mid = (lo + hi) / 2;
      if (sceneAzimuthAtWindowX(mid) < azimuth) {
        lo = mid;
      } else {
        hi = mid;
      }
    }
    return (lo + hi) / 2;
  };

  for (int n = 1; n <= kMaxSlices; n++) {
    double span = (right - left) / n;
    if (span / 2 > kMaxSliceHalfAngle && n < kMaxSlices) continue;

    std::vector<Slice> slices;
    std::vector<double> seams;
    for (int i = 1; i < n; i++) seams.push_back(columnAt(left + i * span));

    int widthPx = 1, heightPx = 1;
    bool valid = true;
    for (int i = 0; i < n; i++) {
      double yaw = left + (i + 0.5) * span;
      double x0 = i == 0 ? 0.0 : seams[i - 1];
      double x1 = i == n - 1 ? static_cast<double>(_windowWidth) : seams[i];
      OOFlatFrustum f{1e30, -1e30, 1e30, -1e30};
      double perUnitX = 0, perUnitY = 0;
      const int columns = 64, rows = 32;
      for (int c = 0; c <= columns && valid; c++) {
        double px = x0 + (x1 - x0) * c / columns;
        for (int r = 0; r <= rows; r++) {
          double py = static_cast<double>(_windowHeight) * r / rows;
          OODirection d = IntoSlice(windowToScene({px, py}), yaw);
          if (d.z <= 1e-6) {
            valid = false;
            break;
          }
          double gx = d.x / d.z, gy = d.y / d.z;
          f.left = std::min(f.left, gx);
          f.right = std::max(f.right, gx);
          f.bottom = std::min(f.bottom, gy);
          f.top = std::max(f.top, gy);
          // Window pixels per unit of the slice's gnomonic coordinates.
          OODirection dx = IntoSlice(windowToScene({px + 0.5, py}), yaw);
          OODirection dy = IntoSlice(windowToScene({px, py + 0.5}), yaw);
          double ux = std::fabs(dx.x / dx.z - gx);
          double uy = std::fabs(dy.y / dy.z - gy);
          if (ux > 0) perUnitX = std::max(perUnitX, 0.5 / ux);
          if (uy > 0) perUnitY = std::max(perUnitY, 0.5 / uy);
        }
      }
      if (!valid) break;
      // A little margin so linear filtering never reads past the edge.
      double mx = (f.right - f.left) * 0.002, my = (f.top - f.bottom) * 0.002;
      f = {f.left - mx, f.right + mx, f.bottom - my, f.top + my};
      widthPx = std::max(
          widthPx, static_cast<int>(std::ceil(perUnitX * (f.right - f.left))));
      heightPx = std::max(
          heightPx, static_cast<int>(std::ceil(perUnitY * (f.top - f.bottom))));
      slices.push_back({yaw, f});
    }
    if (!valid) continue;

    bool fits = widthPx <= maxTextureSizePx && heightPx <= maxTextureSizePx &&
                n * static_cast<double>(widthPx) * heightPx <=
                    kMaxPixelFactor * windowPixels;
    if (fits || n == kMaxSlices) {
      _slices = std::move(slices);
      _seamWindowX = std::move(seams);
      _flatWidthPx = std::min(widthPx, maxTextureSizePx);
      _flatHeightPx = std::min(heightPx, maxTextureSizePx);
      return;
    }
  }
  // Unreachable inside the data-model limits; keep a usable single slice.
  _slices.push_back({0.0, {-1, 1, -1, 1}});
  _flatWidthPx = _windowWidth;
  _flatHeightPx = _windowHeight;
}

std::vector<OOWarpVertex> OODisplayGeometry::warpMesh(int cellsX,
                                                      int cellsY) const {
  std::vector<double> columns;
  for (int i = 0; i <= cellsX; i++) {
    columns.push_back(static_cast<double>(_windowWidth) * i / cellsX);
  }
  for (double seam : _seamWindowX) columns.push_back(seam);
  std::sort(columns.begin(), columns.end());
  columns.erase(
      std::unique(columns.begin(), columns.end(),
                  [](double a, double b) { return std::fabs(a - b) < 1e-6; }),
      columns.end());

  const int n = sliceCount();
  auto vertex = [&](double px, double py, int slice) {
    const Slice& s = _slices[slice];
    OODirection d = IntoSlice(windowToScene({px, py}), s.yaw);
    double gx = d.x / d.z, gy = d.y / d.z;
    const OOFlatFrustum& f = s.frustum;
    return OOWarpVertex{
        static_cast<float>(px / _windowWidth),
        static_cast<float>(py / _windowHeight),
        static_cast<float>((slice + (gx - f.left) / (f.right - f.left)) / n),
        static_cast<float>((gy - f.bottom) / (f.top - f.bottom))};
  };

  std::vector<OOWarpVertex> mesh;
  mesh.reserve((columns.size() - 1) * cellsY * 6);
  for (size_t c = 0; c + 1 < columns.size(); c++) {
    double x0 = columns[c], x1 = columns[c + 1];
    double centre = (x0 + x1) / 2;
    int slice = 0;
    for (double seam : _seamWindowX) {
      if (centre > seam) slice++;
    }
    for (int r = 0; r < cellsY; r++) {
      double y0 = static_cast<double>(_windowHeight) * r / cellsY;
      double y1 = static_cast<double>(_windowHeight) * (r + 1) / cellsY;
      OOWarpVertex a = vertex(x0, y0, slice), b = vertex(x1, y0, slice);
      OOWarpVertex cc = vertex(x1, y1, slice), d = vertex(x0, y1, slice);
      mesh.insert(mesh.end(), {a, b, cc, a, cc, d});
    }
  }
  return mesh;
}

// Free functions -------------------------------------------------------------

OOPixelRect OOHudAreaForWindow(int windowWidthPx, int windowHeightPx) {
  if (static_cast<long long>(windowWidthPx) * 9 <=
      static_cast<long long>(windowHeightPx) * 16) {
    return {0, 0, windowWidthPx, windowHeightPx};
  }
  int width = static_cast<int>(std::lround(windowHeightPx * 16.0 / 9.0));
  return {(windowWidthPx - width) / 2, 0, width, windowHeightPx};
}

bool OOPlausibleDisplaySize(double widthMm, double heightMm, int pixelWidth,
                            int pixelHeight) {
  if (widthMm <= 0 || heightMm <= 0 || pixelWidth <= 0 || pixelHeight <= 0) {
    return false;
  }
  double diagonal = std::hypot(widthMm, heightMm);
  if (diagonal < kMinDiagonalMm || diagonal > kMaxDiagonalMm) return false;
  double ratio =
      (widthMm / heightMm) / (static_cast<double>(pixelWidth) / pixelHeight);
  return std::fabs(ratio - 1.0) <= 0.05;
}
