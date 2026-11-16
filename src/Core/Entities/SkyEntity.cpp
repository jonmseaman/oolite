/*

SkyEntity.m

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

#include "SkyEntity.h"

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <utility>

#include "Color.h"
#include "Description.h"
#include "Maths.h"
#include "MyOpenGLView.h"
#include "PlayerEntity.h"
#include "SkyDrawable.h"
#include "Universe.h"
#include "oofnd/Log.hpp"
#include "oofnd/PList.hpp"
#include "oofnd/String.hpp"

constexpr int SKY_BASIS_STARS = 4800;
constexpr int SKY_BASIS_BLOBS = 1280;
#define SKY_clusterChance 0.80
#define SKY_alpha 0.10
#define SKY_scale 10.0

namespace {

// -objectForKey: as plist data (a null PList when absent), for
// +colorWithDescription:.
oo::PList ValueForKey(const oo::PList& dict, std::string_view key) {
  const oo::PList* value = dict.find(key);
  return value != nullptr ? *value : oo::PList();
}

// get<std::string> where the Foundation code read nil: std::nullopt when the
// key is absent or its value is neither a string nor a number.
std::optional<std::string> OptionalStringForKey(const oo::PList& dict,
                                                std::string_view key) {
  const oo::PList* value = dict.find(key);
  if (value == nullptr || !(value->isString() || value->isNumber()))
    return std::nullopt;
  return dict.get<std::string>(key);
}

// [[OOColor colorWithDescription:description] premultipliedColor]: null
// where the colour was nil.
std::shared_ptr<oo::Color> PremultipliedColorWithDescription(
    const oo::PList& description) {
  const std::shared_ptr<oo::Color> color =
      oo::Color::colorWithDescription(description);
  return color != nullptr ? color->premultipliedColor() : nullptr;
}

}  // namespace

void SkyEntity::initWithColors(oo::Color* col1In, oo::Color* col2In,
                               const oo::PList& systemInfo) {
  std::shared_ptr<oo::SkyDrawable> skyDrawable;
  float clusterChance, alpha, scale, starCountMultiplier, nebulaCountMultiplier;
  signed starCount,  // Need to be able to hold -1...
      nebulaCount;

  // self = [super init]: the constructor ran Entity's -init body.

  // The colours the body replaces are owned here (they were autoreleased).
  std::shared_ptr<oo::Color> col1 = oo::share(col1In);
  std::shared_ptr<oo::Color> col2 = oo::share(col2In);
  std::shared_ptr<oo::Color> col3 =
      oo::Color::colorWithDescription(oo::ColorObjectNode(
          col1.get()));  // a copy, as the id form made of a colour
  std::shared_ptr<oo::Color> col4 =
      oo::Color::colorWithDescription(oo::ColorObjectNode(col2.get()));

  // Load colours
  bool nebulaColorSet = readColor1(&col1, &col2, &col3, &col4, systemInfo);

  skyColor =
      oo::Color::colorWithDescription(ValueForKey(systemInfo, "sun_color"));
  if (skyColor == nullptr) {
    // A message to a nil colour answered nil.
    skyColor = (col2 != nullptr)
                   ? col2->blendedColorWithFraction(0.5, col1.get())
                   : nullptr;
  }

  // Load distribution values
  clusterChance = systemInfo.get<float>("sky_blur_cluster_chance",
                                        static_cast<float>(SKY_clusterChance));
  alpha =
      systemInfo.get<float>("sky_blur_alpha", static_cast<float>(SKY_alpha));
  scale = systemInfo.get<float>("sky_blur_scale", SKY_scale);

  // Load star count
  starCount = static_cast<int>(systemInfo.get<float>("sky_n_stars", -1));
  starCountMultiplier = systemInfo.get<float>("star_count_multiplier", 1.0f);
  if (starCountMultiplier < 0.0f) starCountMultiplier *= -1.0f;
  if (0 <= starCount) {
    // changed for 1.82, default set to 1
    // lets OXPers modify the broad number without stopping variation
    starCount =
        static_cast<int>(static_cast<float>(starCount) * (starCountMultiplier));
  } else {
    starCount = static_cast<int>(starCountMultiplier * SKY_BASIS_STARS *
                                 (0.5 + randf()));
  }

  // ...and nebula count. (Note: simplifying this would change the appearance of
  // stars/blobs.)
  nebulaCount = static_cast<int>(systemInfo.get<float>("sky_n_blurs", -1));
  nebulaCountMultiplier =
      systemInfo.get<float>("nebula_count_multiplier", 1.0f);
  if (nebulaCountMultiplier < 0.0f) nebulaCountMultiplier *= -1.0f;
  if (0 <= nebulaCount) {
    // changed for 1.82, default set to 1
    // lets OXPers modify the broad number without stopping variation
    nebulaCount = static_cast<int>(static_cast<float>(nebulaCount) *
                                   (nebulaCountMultiplier));
  } else {
    nebulaCount = static_cast<int>(nebulaCountMultiplier * SKY_BASIS_BLOBS *
                                   (0.5 + randf()));
  }

  if ((UNIVERSE != nullptr ? UNIVERSE->reducedDetail() : false)) {
    // limit stars and blobs to basis levels, and halve stars again
    if (starCount > SKY_BASIS_STARS) {
      starCount = SKY_BASIS_STARS;
    }
    starCount /= 2;
    if (nebulaCount > SKY_BASIS_BLOBS) {
      nebulaCount = SKY_BASIS_BLOBS;
    }
  }

  skyDrawable = oo::makeShared<oo::SkyDrawable>(
      col1.get(), col2.get(), col3.get(), col4.get(),
      static_cast<unsigned>(starCount), static_cast<unsigned>(nebulaCount),
      nebulaColorSet, clusterChance, alpha, scale);
  setDrawable(skyDrawable.get());

  setStatus(oo::EntityStatus::kEffect);
}

oo::Color* SkyEntity::getSkyColor() { return skyColor.get(); }

bool SkyEntity::changeProperty(const std::string& key, const oo::PList& dict) {
  // TODO: properties requiring reInit?
  if (key == "sun_color") {
    std::shared_ptr<oo::Color> col =
        oo::Color::colorWithDescription(ValueForKey(dict, key));
    if (col != nullptr) {
      skyColor = col;  // [col copy]: a copy of an immutable colour is the
                       // colour itself
      if (UNIVERSE != nullptr) UNIVERSE->setLighting();
    }
  } else {
    OO_LOG_WARN("script.warning",
                "Change to property '{}' not applied, will apply only on "
                "leaving and re-entering this system.",
                key);
    return false;
  }
  return true;
}

void SkyEntity::update(oo::TimeDelta /*delta_t*/) {
  ::PlayerEntity* player = PLAYER;
  zero_distance = static_cast<GLfloat>(MAX_CLEAR_DEPTH * MAX_CLEAR_DEPTH);
  cam_zero_distance = zero_distance;
  if (player != nullptr) {
    position = (player != nullptr ? player->viewpointPosition() : HPVector{});
  } else {
    OO_LOG("sky.warning", "{}", "PLAYER is nil");
  }
}

bool SkyEntity::isSky() { return true; }

bool SkyEntity::isVisible() { return true; }

bool SkyEntity::canCollide() { return false; }

GLfloat SkyEntity::cameraRangeFront() { return MAX_CLEAR_DEPTH; }

GLfloat SkyEntity::cameraRangeBack() { return MAX_CLEAR_DEPTH; }

void SkyEntity::drawImmediate(bool immediate, bool translucent) {
  if ((UNIVERSE != nullptr ? UNIVERSE->breakPatternHide() : false)) return;

  oo::EntityWithDrawable::drawImmediate(immediate, translucent);

  oo::CheckOpenGLErrors([&]() -> std::string {
    return "SkyEntity after drawing " + oo::EntityDescription(this);
  });
}

#ifdef OO_DEBUG_MONITOR_ENABLED
std::optional<std::string> SkyEntity::descriptionForObjDump() {
  // Don't include range and visibility flag as they're irrelevant.
  return descriptionForObjDumpBasic();
}
#endif

bool SkyEntity::readColor1(std::shared_ptr<oo::Color>* ioColor1,
                           std::shared_ptr<oo::Color>* ioColor2,
                           std::shared_ptr<oo::Color>* ioColor3,
                           std::shared_ptr<oo::Color>* ioColor4,
                           const oo::PList& dictionary) {
  oo::PList colorDesc;
  std::shared_ptr<oo::Color> color;
  bool nebulaSet = false;

  assert(ioColor1 != nullptr && ioColor2 != nullptr);

  const std::optional<std::string> string =
      OptionalStringForKey(dictionary, "sky_rgb_colors");
  if (string.has_value()) {
    oo::PList::Array tokenList;
    for (std::string& token : oo::str::tokens(*string))
      tokenList.emplace_back(std::move(token));
    const oo::PList tokens(std::move(tokenList));

    if (tokens.count() == 6) {
      float r1 = oo::Clamp_0_1_f(tokens.at<float>(0));
      float g1 = oo::Clamp_0_1_f(tokens.at<float>(1));
      float b1 = oo::Clamp_0_1_f(tokens.at<float>(2));
      float r2 = oo::Clamp_0_1_f(tokens.at<float>(3));
      float g2 = oo::Clamp_0_1_f(tokens.at<float>(4));
      float b2 = oo::Clamp_0_1_f(tokens.at<float>(5));
      *ioColor1 = oo::Color::colorWithRed(r1, g1, b1, 1.0);
      *ioColor2 = oo::Color::colorWithRed(r2, g2, b2, 1.0);
    } else {
      OO_LOG_WARN("sky.fromDict",
                  "could not interpret \"{}\" as two RGB colours (must be six "
                  "numbers).",
                  *string);
    }
  }
  colorDesc = ValueForKey(dictionary, "sky_color_1");
  if (!colorDesc.isNull()) {
    color = PremultipliedColorWithDescription(colorDesc);
    if (color != nullptr)
      *ioColor1 = color;
    else
      OO_LOG_WARN("sky.fromDict", "could not interpret \"{}\" as a colour.",
                  oo::DescriptionOf(colorDesc));
  }
  colorDesc = ValueForKey(dictionary, "sky_color_2");
  if (!colorDesc.isNull()) {
    color = PremultipliedColorWithDescription(colorDesc);
    if (color != nullptr)
      *ioColor2 = color;
    else
      OO_LOG_WARN("sky.fromDict", "could not interpret \"{}\" as a colour.",
                  oo::DescriptionOf(colorDesc));
  }

  colorDesc = ValueForKey(dictionary, "nebula_color_1");
  if (!colorDesc.isNull()) {
    color = PremultipliedColorWithDescription(colorDesc);
    if (color != nullptr) {
      *ioColor3 = color;
      nebulaSet = true;
    } else {
      OO_LOG_WARN("sky.fromDict", "could not interpret \"{}\" as a colour.",
                  oo::DescriptionOf(colorDesc));
    }
  } else {
    colorDesc = ValueForKey(dictionary, "sky_color_1");
    if (!colorDesc.isNull()) {
      color = PremultipliedColorWithDescription(colorDesc);
      if (color != nullptr)
        *ioColor3 = color;
      else
        OO_LOG_WARN("sky.fromDict", "could not interpret \"{}\" as a colour.",
                    oo::DescriptionOf(colorDesc));
    }
  }

  colorDesc = ValueForKey(dictionary, "nebula_color_2");
  if (!colorDesc.isNull()) {
    color = PremultipliedColorWithDescription(colorDesc);
    if (color != nullptr) {
      *ioColor4 = color;
      nebulaSet = true;
    } else {
      OO_LOG_WARN("sky.fromDict", "could not interpret \"{}\" as a colour.",
                  oo::DescriptionOf(colorDesc));
    }
  } else {
    colorDesc = ValueForKey(dictionary, "sky_color_2");
    if (!colorDesc.isNull()) {
      color = PremultipliedColorWithDescription(colorDesc);
      if (color != nullptr)
        *ioColor4 = color;
      else
        OO_LOG_WARN("sky.fromDict", "could not interpret \"{}\" as a colour.",
                    oo::DescriptionOf(colorDesc));
    }
  }
  return nebulaSet;
}
