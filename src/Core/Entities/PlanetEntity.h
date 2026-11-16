/*

OOPlanetEntity.h

Entity subclass representing a planet.

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

#include <memory>
#include <optional>
#include <set>
#include <string>

#include "Color.h"
#include "Entity.h"
#include "GraphicsResetManager.h"
#include "StellarBody.h"
#include "legacy_random.h"
#include "oofnd/Object.hpp"
#include "oofnd/PList.hpp"

/*	The planet configuration and material parameters are oo::PLists (mixed:
   colours are PList::Object nodes); texture and planet names are std::optional
   (nullopt where they were nil).
*/

namespace oo {

class Material;
class Texture;

}  // namespace oo

class ShipEntity;

namespace oo {

class PlanetDrawable;

}  // namespace oo
   // this class is its one caller

/*	The universe and the legacy scripts make it with
   oo::makeShared<OOPlanetEntity>() and hold it as
   the C++ class. Its OOStellarBody answers are members (OOStellarBodyRadius()),
   and the shader binding names are answered for a planet only
   (OOEntityCalledByName.cpp).
*/
namespace oo {

class PlanetEntity : public Entity, public ::oo::GraphicsResetClient {
 public:
  ~PlanetEntity() override;  // unregisters the graphics reset client

  /*	The initialisers' bodies after [self init] (the constructor ran
     Entity's), run on a new planet once its object is made.
  */
  void initAsMainPlanetForSystem(oo::SystemID s);
  void initFromDictionary(const oo::PList& dict, bool atmosphere,
                          Random_Seed seed, oo::SystemID systemID);

  std::shared_ptr<PlanetEntity> miniatureVersion();

  double rotationalVelocity();
  void setRotationalVelocity(double v);

  bool planetHasStation();
  void launchShuttle();
  void welcomeShuttle(::ShipEntity* shuttle);

  bool hasAtmosphere();

  // FIXME: need material model.
  std::optional<std::string> textureFileName();  // nullopt: none
  void setTextureFileName(const std::optional<std::string>& textureName);

  bool setUpPlanetFromTexture(
      const std::optional<std::string>& fileName);  // nullopt: none

  // The drawables' materials, as the drawables keep them.
  oo::Material* material();
  oo::Material* atmosphereMaterial();
  oo::Material* atmosphereShaderMaterial();

  bool isFinishedLoading();

  Vector airColorAsVector();  // visible to shader bindings
  oo::Color* airColor();
  void setAirColor(oo::Color* newColor);
  Vector illuminationColorAsVector();  // visible to shader bindings
  oo::Color* illuminationColor();
  void setIlluminationColor(oo::Color* newColor);
  float airColorMixRatio();  // visible to shader bindings
  void setAirColorMixRatio(float newRatio);
  float airDensity();  // visible to shafer bindings
  void setAirDensity(float newDensity);

  void setTerminatorThresholdVector(Vector newTerminatorThresholdVector);
  Vector terminatorThresholdVector();  // visible to shader bindings

  // OOStellarBody.
  double radius();
  StellarBodyType planetType();
  std::optional<std::string> name();
  void setName(const std::optional<std::string>& name);

  // OOGraphicsResetClient: the planet registers itself once set up.
  void resetGraphicsState() override;

  std::optional<std::string> descriptionComponents() const override;
  void setOrientation(Quaternion quat) override;
  void update(oo::TimeDelta delta_t) override;
  void drawImmediate(bool immediate, bool translucent) override;
  bool checkCloseCollisionWith(Entity* other) override;
  bool isPlanet() override;
  bool isVisible() override;

  // The binding's bodies (OOJSPlanet.cpp).
  JSClassID jsClassID() override;
  std::optional<std::string> jsClassName() override;
  bool isVisibleToScripts() override;

 private:
  bool initAsMiniatureVersionOfPlanet(PlanetEntity* planet);

  void setUpTerrainParametersWithSourceInfo(const oo::PList& sourceInfo,
                                            oo::PList& targetInfo);
  void setUpLandParametersWithSourceInfo(const oo::PList& sourceInfo,
                                         oo::PList& targetInfo);
  void setUpAtmosphereParametersWithSourceInfo(const oo::PList& sourceInfo,
                                               oo::PList& targetInfo);
  void setUpColorParametersWithSourceInfo(const oo::PList& sourceInfo,
                                          oo::PList& targetInfo,
                                          bool isAtmosphere);
  void setUpTypeParametersWithSourceInfo(const oo::PList& sourceInfo,
                                         oo::PList& targetInfo);

  std::shared_ptr<oo::PlanetDrawable> _planetDrawable;
  std::shared_ptr<oo::PlanetDrawable> _atmosphereDrawable;
  std::shared_ptr<oo::PlanetDrawable> _cloudsShaderDrawable;
  std::shared_ptr<oo::PlanetDrawable> _atmosphereShaderDrawable;

  bool _miniature = false;
  std::shared_ptr<oo::Color> _airColor;
  std::shared_ptr<oo::Color> _illuminationColor;
  float _airColorMixRatio = {};
  float _airDensity = {};
  double _mesopause2 = {};

  Vector _rotationAxis = {};
  float _rotationalVelocity = {};
  Quaternion _atmosphereOrientation = {};
  float _atmosphereRotationalVelocity = {};

  Vector _terminatorThresholdVector = {};

  unsigned _shuttlesOnGround = {};
  oo::TimeDelta _lastLaunchTime = {};
  oo::TimeDelta _shuttleLaunchInterval = {};

  oo::PList _materialParameters;  // null where it was nil
  std::optional<std::string> _textureName;
  std::optional<std::string> _normSpecMapName;

  std::optional<std::string> _name;

  // The texture generators' noise seed, handed to them directly; was a value
  // box under "noise_map_seed" in planetInfo / _materialParameters.
  RANROTSeed _noiseMapSeed = {};
};

}  // namespace oo
