/*

OOJSFunction.h

Object encapsulating a runnable JavaScript function.


JavaScript support for Oolite
Copyright (C) 2007-2013 David Taylor and Jens Ayton.

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

#include <optional>
#include <string>
#include <vector>

#include "OOFoundation.h"
#include "oofnd/Ref.hpp"
#include "oofnd/StdLib.hpp"
#include "ooscript/JSEngine.hpp"

class OOJSFunction : public oo::RefCounted {
 public:
  // The initialisers' factories: null where the initialiser answered nil (a
  // NULL function; no code, missing argument names, or a compile error).
  static oo::Ref<OOJSFunction> initWithFunction(ooscript::Function function,
                                                ooscript::Context context);
  static oo::Ref<OOJSFunction> initWithName(
      const std::optional<std::string>& name,
      ooscript::Object
          scope,  // may be NULL, in which case global object is used.
      const std::optional<std::string>&
          code,  // full JS code for function, including function declaration.
      NSUInteger argCount, const char** argNames,
      const std::optional<std::string>& fileName, NSUInteger lineNumber,
      ooscript::Context
          context);  // may be NULL. If not null, must be in a request.

  ~OOJSFunction() override;

  std::optional<std::string> descriptionComponents() const;

  std::optional<std::string> name();  // nullopt: anonymous
  ooscript::Function function();
  ooscript::Value functionValue();

  // Raw evaluation. Context may not be NULL and must be in a request.
  bool evaluateWithContext(ooscript::Context context, ooscript::Object jsThis,
                           unsigned argc, ooscript::Value* argv,
                           ooscript::Value* result);

 private:
  // The part of -initWithFunction:context: that cannot fail.
  OOJSFunction(ooscript::Function function, ooscript::Context context);

  void deleteJSValue();

  ooscript::Function _function = {};
  std::optional<std::string>
      _name;  // nullopt for an anonymous function (was nil)
};
