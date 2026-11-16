/*

OOPixMapTextureLoader.h

Load a texture from a pixmap. The loader takes ownership of the pixmap.


Copyright (C) 2010-2013 Jens Ayton

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

#pragma once

#include <memory>

#include "PixMap.h"
#include "Texture.h"
#include "TextureGenerator.h"

/*	A leaf of the texture generators, over OOTextureGenerator. Its callers
   (the planets) make it with loaderWithPixMap and hand the texture the C++
   loader.
*/
namespace oo {

class PixMapTextureLoader : public TextureGenerator {
 public:
  ~PixMapTextureLoader() override;  // was -dealloc

  /*	Was [[OOPixMapTextureLoader alloc]
     initWithPixMap:textureOptions:freeWhenDone:]: null where that answered nil.
  */
  static std::shared_ptr<PixMapTextureLoader> loaderWithPixMap(
      PixMap pixMap, TextureFlags options, bool freeWhenDone);

  // Was -initWithPixMap:textureOptions:freeWhenDone:; false where it answered
  // nil.
  bool initWithPixMap(PixMap pixMap, TextureFlags options, bool freeWhenDone);

  void loadTexture() override;
  TextureFlags textureOptions() override;

 private:
  PixMap _pixMap = {};
  TextureFlags _texOptions = {};
};

}  // namespace oo
