/*

OONullTexture.m


Copyright (C) 2008-2013 Jens Ayton

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

#include "NullTexture.h"

#include <memory>
#include <string>  // IWYU pragma: keep: used only without NDEBUG

#include "Foundation.h"

namespace {

oo::NullTexture* sSingleton = nullptr;

}  // namespace

oo::NullTexture* oo::NullTexture::sharedNullTexture() {
  // NOTE: assumes single-threaded access.
  if (sSingleton == nullptr) {
    // never deleted
    static std::shared_ptr<oo::NullTexture>* sSingletonHolder = nullptr;
    sSingletonHolder =
        new std::shared_ptr<oo::NullTexture>(oo::makeShared<oo::NullTexture>());
    sSingleton = sSingletonHolder->get();
  }

  return sSingleton;
}

void oo::NullTexture::apply() { oo::Texture::applyNone(); }

NSSize oo::NullTexture::dimensions() { return NSZeroSize; }

bool oo::NullTexture::isMipMapped() { return false; }

void oo::NullTexture::forceRebind() {}

#ifndef NDEBUG
std::optional<std::string> oo::NullTexture::name() {
  return std::string("<null texture>");
}
#endif

/*	The (Singleton) category's canonical singleton boilerplate
   (+allocWithZone: answering nil after the first, -copyWithZone: answering
   self, and -retain/-release/-autorelease doing nothing) is not translated:
   nothing but sharedNullTexture() makes the object, and the one reference it
        keeps is never released.
*/
