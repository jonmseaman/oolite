/*

OOColor.h

An RGBA colour in device colour space.

A colour is its own PList::Object node payload (oo::PListForeign), as
OONativeVector is.


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
#include <string>
#include <vector>

#include "oofnd/Object.hpp"
#include "oofnd/PList.hpp"

namespace oo {

struct RGBAComponents {
  float r, g, b, a;
};

struct HSBAComponents {
  float h, s, b, a;
};

class Color : public oo::PListForeign {
 public:
  static std::shared_ptr<Color> colorWithHue(float hue, float saturation,
                                             float brightness,
                                             float alpha);  // Note: hue in 0..1
  static std::shared_ptr<Color> colorWithRed(float red, float green, float blue,
                                             float alpha);
  static std::shared_ptr<Color> colorWithWhite(float white, float alpha);
  static std::shared_ptr<Color> colorWithRGBAComponents(
      RGBAComponents components);
  // Note: hue in 0..360
  static std::shared_ptr<Color> colorWithHSBAComponents(
      HSBAComponents components);

  /*	Flexible color creator: <description> is
     plist data, one of
          - a string: the name of one of the named colours below ("redColor" &
     co.), or components (colorFromString());
          - an array of components (their descriptions, joined with spaces, as a
     string);
          - a dictionary of hue/saturation/brightness (or value)/alpha (or
     opacity) keys, hue in 0..360, or of red/green/blue/alpha (or opacity) keys;
          - an Object node holding an OOColor (the colour is the node's foreign
     object), which is returned as is. Anything else, or a null PList, gives
     null.
  */
  static std::shared_ptr<Color> colorWithDescription(
      const oo::PList& description);

  /*	Like colorWithDescription(), but multiplies saturation by provided
     factor. If the colour is an HSV dictionary, it may specify a saturation
     greater than 1.0 to override the scaling.
  */
  static std::shared_ptr<Color> colorWithDescription(
      const oo::PList& description, float factor);

  // Like colorWithDescription(), but forces brightness of at least 0.5.
  static std::shared_ptr<Color> brightColorWithDescription(
      const oo::PList& description);

  // Creates a colour given a string with components.
  static std::shared_ptr<Color> colorFromString(
      const std::string& colorFloatString);

  static std::shared_ptr<Color> blackColor();      // 0.0 white
  static std::shared_ptr<Color> darkGrayColor();   // 0.333 white
  static std::shared_ptr<Color> lightGrayColor();  // 0.667 white
  static std::shared_ptr<Color> whiteColor();      // 1.0 white
  static std::shared_ptr<Color> grayColor();       // 0.5 white
  static std::shared_ptr<Color> redColor();        // 1.0, 0.0, 0.0 RGB
  static std::shared_ptr<Color> greenColor();      // 0.0, 1.0, 0.0 RGB
  static std::shared_ptr<Color> blueColor();       // 0.0, 0.0, 1.0 RGB
  static std::shared_ptr<Color> cyanColor();       // 0.0, 1.0, 1.0 RGB
  static std::shared_ptr<Color> yellowColor();     // 1.0, 1.0, 0.0 RGB
  static std::shared_ptr<Color> magentaColor();    // 1.0, 0.0, 1.0 RGB
  static std::shared_ptr<Color> orangeColor();     // 1.0, 0.5, 0.0 RGB
  static std::shared_ptr<Color> purpleColor();     // 0.5, 0.0, 0.5 RGB
  static std::shared_ptr<Color> brownColor();      // 0.6, 0.4, 0.2 RGB
  static std::shared_ptr<Color> clearColor();      // 0.0 white, 0.0 alpha

  //	Linear blend in working colour space (no attempt at gamma correction).
  std::shared_ptr<Color> blendedColorWithFraction(float fraction, Color* color);

  //	Get the red, green, or blue components.
  float redComponent();
  float greenComponent();
  float blueComponent();
  void getRed(float* red, float* green, float* blue, float* alpha);

  RGBAComponents rgbaComponents();

  bool isBlack();
  bool isWhite();

  /*	Get the components as hue, saturation, or brightness.

          IMPORTANT: for reasons of bugwards compatibility, these return hue
     values in the range [0, 360], but colorWithHue() expects values in the
          range [0, 1].
  */
  float hueComponent();
  float saturationComponent();
  float brightnessComponent();
  void getHue(float* hue, float* saturation, float* brightness, float* alpha);

  HSBAComponents hsbaComponents();

  // Get the alpha component.
  float alphaComponent();

  /*	Returns the colour, premultiplied by its alpha channel, and with an
     alpha of 1.0. If the reciever's alpha is 1.0, it will return itself.
  */
  std::shared_ptr<Color> premultipliedColor();

  // Multiply r, g and b components of a colour by specified factor, clamped to
  // [0..1].
  std::shared_ptr<Color> colorWithBrightnessFactor(float factor);

  // r,g,b,a array in 0..1 range.
  std::vector<float> normalizedArray();

  std::optional<std::string> rgbaDescription();
  std::optional<std::string> hsbaDescription();

  // What "%@" prints between the braces of <OOColor 0x...>{...}
  // (OODescription.h).
  std::optional<std::string> descriptionComponents() const;

  // oo::PListForeign: the class name, and "%@" as <OOColor 0x...>{...}.
  std::string className() const override;
  std::string description() const override;

 private:
  // Set methods are internal, because OOColor is immutable (as seen from
  // outside).
  void setRed(float r, float g, float b, float a);
  void setHue(float h, float s, float b, float a);

  float rgba[4] = {};
};

/*	A colour carried through plist data as a PList::Object node: the colour
   is the node's foreign object (oo::PListObject() and oo::ObjectIn()).
*/
inline oo::PList ColorObjectNode(Color* color) {
  // null colour -> null PList
  if (color == nullptr) return oo::PList();
  return oo::PList(oo::PList::Object(oo::share(color)));
}

inline Color* ColorInObjectNode(const oo::PList& plist) {
  // null for any other node
  const oo::PList::Object* node = plist.getIf<oo::PList::Object>();
  return (node != nullptr) ? dynamic_cast<Color*>(node->get()) : nullptr;
}

std::string RGBAComponentsDescription(RGBAComponents components);
std::string HSBAComponentsDescription(HSBAComponents components);

}  // namespace oo
