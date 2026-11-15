/*

LogIndentScope.h

Indents this thread's log lines for the lifetime of a scope: one level on
construction, back on destruction, however the scope is left (a return, a
break or an exception). It replaces a matched OOLogIndent()/OOLogOutdent()
pair around a block.

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

#include "oofnd/Log.hpp"

namespace oo {

class LogIndentScope {
 public:
  LogIndentScope() { log::indent(); }
  ~LogIndentScope() { log::outdent(); }

  LogIndentScope(const LogIndentScope&) = delete;
  LogIndentScope& operator=(const LogIndentScope&) = delete;
};

}  // namespace oo
