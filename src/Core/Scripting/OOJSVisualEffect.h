/*

OOJSVisualEffect.h (QuickJS-ng)

JavaScript proxy for OOVisualEffectEntities.

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

#include <quickjs.h>

#include <string>
#include <vector>

#include "EntityHold.h"
#include "Foundation.h"

class Entity;

namespace oo {

class VisualEffectEntity;

}  // namespace oo

void InitOOJSVisualEffect(JSContext* context, JSValueConst global);

JSClassID JSVisualEffectClassID();

/*	The bodies of OOVisualEffectEntity (OOJavaScriptExtensions), which the
   engine reaches by selector: the C++ effect's
   jsClassID()/jsClassName()/isVisibleToScripts() overrides call them. Its JS
   class is JSVisualEffectClassID().
*/
std::optional<std::string> OOJSVisualEffectJSClassName();
bool OOJSVisualEffectIsVisibleToScripts();
std::vector<oo::EntityHold> OOJSVisualEffectSubEntitiesForScript(
    oo::VisualEffectEntity* effect);
