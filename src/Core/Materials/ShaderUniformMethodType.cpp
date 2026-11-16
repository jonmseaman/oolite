/*

OOShaderUniformMethodType.m


Copyright (C) 2007-2013 Jens Ayton

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

/*
        The entities' shader hooks. A binding's uniform type comes from its
   row's value type, oo::ShaderUniformTypeOf (OOShaderUniformMethodType.h).
*/

#include "ShaderUniformMethodType.h"

#if OO_SHADERS || defined(OO_DEBUG_MONITOR_ENABLED)

// The entities' hooks: none until the entities register theirs.
namespace {
oo::ShaderEntityHooks sEntityHooks = {};
bool sEntityHooksSet = false;
}  // namespace

namespace oo {

void SetShaderEntityHooks(const ShaderEntityHooks& hooks) noexcept {
  sEntityHooks = hooks;
  sEntityHooksSet = true;
}

const ShaderEntityHooks* ShaderEntityHooksIfSet() noexcept {
  return sEntityHooksSet ? &sEntityHooks : nullptr;
}

}  // namespace oo

#endif  // OO_SHADERS
