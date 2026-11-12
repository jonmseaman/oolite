/*

OODisplayGeometry.h

The space view's projection worked out from the player's screen and seat:
the physical field of view of a flat or curved panel, the chosen wide-angle
look, and the four mappings between window positions, directions from the
eye and directions in the scene. The renderer's frustum, the final warp, the
HUD's markers over objects and mouse picking all use one OODisplayGeometry,
so they cannot disagree. Pure maths: no OpenGL, no SDL, no game state.

Spec: specs/008-display-geometry-fov (contracts/geometry.md).


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

#include <cstdint>
#include <optional>
#include <vector>

enum class OOFovMode : std::uint8_t { kPhysical, kCustom };
enum class OOWideLook : std::uint8_t { kPanini, kEvenSpacing, kClassicFlat };
enum class OOScreenSizeSource : std::uint8_t { kDefault, kDetected, kEntered };

struct OOScreenDescription {
  double diagonalMm = 609.6;  // 24"
  double aspect = 16.0 / 9.0;
  std::optional<double> curvatureMm;  // empty = flat
  OOScreenSizeSource source = OOScreenSizeSource::kDefault;
};

struct OOFieldOfViewChoice {
  OOFovMode mode = OOFovMode::kPhysical;
  double customHorizontalDeg = 90.0;
  OOWideLook look = OOWideLook::kPanini;
};

// Unit vector; +z forward, +x right, +y up (eye or scene space).
struct OODirection {
  double x, y, z;
};

// Window pixels, origin top-left, y down.
struct OOWindowPoint {
  double x, y;
};

// Extents at distance 1, as OOGLFrustum takes them.
struct OOFlatFrustum {
  double left, right, bottom, top;
};

struct OOWarpVertex {
  float windowX, windowY;  // normalised 0..1, y down
  float u, v;  // flat-render texture coordinate (slices side by side)
};

// Window pixels, origin top-left.
struct OOPixelRect {
  int x, y, width, height;
};

class OODisplayGeometry {
 public:
  // A placeholder until the view has a size: a 90-degree square perspective.
  // Header-only, so a type that holds one needs nothing else linked.
  OODisplayGeometry() = default;

  // Screen-matching off: today's projection from the view's field-of-view
  // tangent (MyOpenGLView::fov(true)).
  static OODisplayGeometry legacy(double fovTangent, int windowWidthPx,
                                  int windowHeightPx);

  // Screen-matching on. Inputs are clamped to the data-model limits.
  OODisplayGeometry(const OOScreenDescription& screen, double viewerDistanceMm,
                    const OOFieldOfViewChoice& choice, int windowWidthPx,
                    int windowHeightPx, double windowFractionX = 1.0,
                    double windowFractionY = 1.0, bool shadersAvailable = true,
                    int maxTextureSizePx = 16384);

  bool screenMatching() const { return _screenMatching; }

  // The inputs after clamping (FR-011).
  const OOScreenDescription& clampedScreen() const { return _screen; }
  double clampedViewerDistanceMm() const { return _distanceMm; }
  const OOFieldOfViewChoice& clampedChoice() const { return _choice; }

  double physicalHorizontalDeg() const;
  double physicalVerticalDeg() const;
  double horizontalDeg() const;  // in use
  double verticalDeg() const;    // in use, always derived
  OOWideLook effectiveLook() const { return _effectiveLook; }
  bool needsWarp() const { return _warp; }

  // Flat render (research R3/R6).
  OOFlatFrustum flatFrustum(int slice = 0) const;
  double sliceYawDeg(int slice) const;
  int sliceCount() const { return static_cast<int>(_slices.size()); }
  int flatRenderWidthPx() const { return _flatWidthPx; }  // per slice
  int flatRenderHeightPx() const { return _flatHeightPx; }

  // The four mappings (FR-006, FR-012). Window points outside the window are
  // allowed.
  OODirection windowToEye(OOWindowPoint p) const;
  std::optional<OOWindowPoint> eyeToWindow(OODirection d) const;
  OODirection eyeToScene(OODirection d) const;
  std::optional<OODirection> sceneToEye(OODirection d) const;

  // Convenience for HUD markers and picking.
  std::optional<OOWindowPoint> sceneToWindow(OODirection d) const;
  OODirection windowToScene(OOWindowPoint p) const;
  double localPixelsPerRadian(OOWindowPoint p) const;

  // Warp mesh (research R2): two triangles per cell, six vertices each, so a
  // cell never interpolates across a slice seam; seams fall on cell edges.
  std::vector<OOWarpVertex> warpMesh(int cellsX = 512, int cellsY = 128) const;

  // The script value (FR-013): horizontal degrees in use.
  double scriptFovValue() const { return horizontalDeg(); }

 private:
  struct Slice {
    double yaw;  // radians, about +y
    OOFlatFrustum frustum;
  };

  // Panel model, millimetres, eye at the origin.
  void panelPoint(double s, double yp, double out[3]) const;
  std::optional<OOWindowPoint> eyeToPanelWindow(OODirection d) const;
  double windowXToArc(double px) const;
  double windowYToHeight(double py) const;

  // Scene <-> panel coordinates for the Panini look.
  void paniniSceneToPanel(double azimuth, double elevation, double* s,
                          double* yp) const;

  double sceneAzimuthAtWindowX(double px) const;
  void buildFlatRender(int maxTextureSizePx);

  bool _screenMatching = false;
  bool _warp = false;
  OOWideLook _effectiveLook = OOWideLook::kClassicFlat;

  OOScreenDescription _screen;
  double _distanceMm = 600.0;
  OOFieldOfViewChoice _choice;

  int _windowWidth = 1;
  int _windowHeight = 1;

  // Panel (warp mode): half extents of the part the window covers.
  double _halfArcMm = 0.0;     // along the surface
  double _halfHeightMm = 0.0;  // vertical
  double _radiusMm = 0.0;      // 0 = flat

  double _physicalHalfH = 0.0;  // radians, eye azimuth of the window edge
  double _physicalHalfV = 0.0;  // radians, eye elevation of the window top
  double _halfH = 0.0;          // radians, scene horizontal half-angle in use

  // Look parameters (warp mode).
  double _evenK = 1.0;     // eye angle = k * scene angle
  double _classicC = 1.0;  // eye gnomonic = c * scene gnomonic
  double _paniniK = 1.0;   // panel mm per Panini unit
  double _paniniXe = 1.0;  // Panini x at the window edge

  // Linear mode (no warp): window <-> scene gnomonic is linear.
  OOFlatFrustum _linear{-1.0, 1.0, -1.0, 1.0};

  std::vector<Slice> _slices{Slice{0.0, {-1.0, 1.0, -1.0, 1.0}}};
  std::vector<double> _seamWindowX;  // window x of slice boundaries
  int _flatWidthPx = 1;
  int _flatHeightPx = 1;
};

// The HUD area (contracts/hud-area.md): the window if w / h <= 16/9, else the
// centred full-height 16:9 rectangle.
OOPixelRect OOHudAreaForWindow(int windowWidthPx, int windowHeightPx);

// Detection plausibility (research R7): diagonal 10"-120" and aspect within 5%
// of the pixel aspect.
bool OOPlausibleDisplaySize(double widthMm, double heightMm, int pixelWidth,
                            int pixelHeight);
