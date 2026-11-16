/*

OOFullScreenController.m


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

#include "FullScreenController.h"

#include "Logging.h"

oo::FullScreenController::FullScreenController(MyOpenGLView* view)
    : _gameView(oo::share(view)) {}

MyOpenGLView* oo::FullScreenController::gameView() { return _gameView.get(); }

bool oo::FullScreenController::inFullScreenMode() {
  OOLogGenericSubclassResponsibility();
  return false;
}

void oo::FullScreenController::setFullScreenMode(bool /*value*/) {
  OOLogGenericSubclassResponsibility();
}

oo::PList oo::FullScreenController::displayModes() {
  OOLogGenericSubclassResponsibility();
  return oo::PList();
}

oo::PList oo::FullScreenController::currentDisplayMode() {
  const oo::PList modes = displayModes();
  const oo::PList::Array* arr = modes.getIf<oo::PList::Array>();
  if (arr == nullptr) return oo::PList();
  const std::size_t idx = indexOfCurrentDisplayMode();
  if (idx >= arr->size()) return oo::PList();
  return (*arr)[idx];
}

std::size_t oo::FullScreenController::indexOfCurrentDisplayMode() {
  OOLogGenericSubclassResponsibility();
  return oo::kNotFound;
}

bool oo::FullScreenController::setDisplayWidth(std::size_t /*width*/,
                                               std::size_t /*height*/,
                                               std::size_t /*refresh*/) {
  OOLogGenericSubclassResponsibility();
  return false;
}

oo::PList oo::FullScreenController::findDisplayModeForWidth(
    std::size_t /*width*/, std::size_t /*height*/, std::size_t /*d_refresh*/) {
  OOLogGenericSubclassResponsibility();
  return oo::PList();
}

void oo::FullScreenController::noteMouseInteractionModeChangedFrom(
    oo::MouseInteractionMode /*oldMode*/,
    oo::MouseInteractionMode /*newMode*/) {}
