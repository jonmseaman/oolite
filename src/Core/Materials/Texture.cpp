/*

        OOTexture.m

        Copyright (C) 2007-2013 Jens Ayton and contributors

        Permission is hereby granted, free of charge, to any person obtaining a
   copy of this software and associated documentation files (the "Software"), to
   deal in the Software without restriction, including without limitation the
   rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
   sell copies of the Software, and to permit persons to whom the Software is
        furnished to do so, subject to the following conditions:

        The above copyright notice and this permission notice shall be included
   in all copies or substantial portions of the Software.

        THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
   OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
        FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
   THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
        LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
   IN THE SOFTWARE.

*/

#include "Texture.h"

#include "Headless.h"
// Before OOConcreteTexture.h, in a block of its own: OOTextureInternal.h tests
// OOTEXTURE_RELOADABLE, which OOConcreteTexture.h defines.
#include <cstddef>
#include <cstdint>
#include <format>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <typeinfo>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "Cache.h"
#include "ConcreteTexture.h"
#include "Demangle.h"
#include "Description.h"
#include "MacroOpenGL.h"
#include "NullTexture.h"
#include "OpenGLExtensionManager.h"
#include "PixMap.h"
#include "ResourceManager.h"
#include "TextureGenerator.h"
#include "TextureInternal.h"
#include "TextureLoader.h"
#include "Universe.h"
#include "oofnd/AutoreleaseScope.hpp"
#include "oofnd/Defaults.hpp"
#include "oofnd/PList.hpp"
#include "oofnd/PListGet.hpp"
#include "oofnd/String.hpp"
#include "oofnd/objc/Assert.h"

namespace {

// Used only by "internal" specifiers from OOMakeTextureSpecifier.
constexpr std::string_view kTextureSpecifierFlagValueInternalKey =
    "_oo_internal_flags";

}  // namespace

/*	Texture caching:
        two and a half parallel caching mechanisms are used. sLiveTextureCache
        tracks all live texture objects with cache keys, without retaining them
        (a std::unordered_map of raw pointers keyed by the cache key's UTF-8).

        sAllLiveTextures tracks all textures, including ones without cache keys,
        so that they can be notified of graphics resets. This also holds raw
        pointers to avoid retaining the textures.

        RecentTextures() tracks up to kRecentTexturesCount textures which
        have been used recently, and retains them.

        This means that the number of live texture objects will never fall below
        80% of kRecentTexturesCount (80% comes from the behaviour of OOCache),
   but old textures will eventually be released. If the number of active
   textures exceeds kRecentTexturesCount, all of them will be reusable through
        sLiveTextureCache, but only a most-recently-fetched subset will be kept
        around by the cache when the number drops.

        Note the textures in RecentTextures() are a superset of the textures in
        sLiveTextureCache, and the textures in sLiveTextureCache are a superset
        of RecentTextures().
*/
constexpr unsigned kRecentTexturesCount = 50;

namespace {

// Allocated on first use and never freed, so a texture deallocated during exit
// never finds them destroyed. Were a Foundation mutable dictionary / set of
// boxed pointers. They hold the C++ textures.
std::unordered_map<std::string, oo::Texture*>* sLiveTextureCache;
std::unordered_set<oo::Texture*>* sAllLiveTextures;

// The textures, retained, as Object nodes (OOTextureObjectNode()); null until
// the first texture is cached. The holder is never deleted, so exit(0) frees no
// texture after the GL context and the logger have gone.
std::shared_ptr<oo::Cache>& RecentTextures() {
  static std::shared_ptr<oo::Cache>& recent = *new std::shared_ptr<oo::Cache>();
  return recent;
}

}  // namespace

static bool sCheckedExtensions;
oo::TextureInfo gOOTextureInfo;

// Nothing reads a trace context, so the markers are no-ops in every build.
#define SET_TRACE_CONTEXT(str) \
  do {                         \
  } while (0)
#define CLEAR_TRACE_CONTEXT() SET_TRACE_CONTEXT(nullptr)

oo::Texture::NameFactory oo::Texture::nameFactory =
    &oo::Texture::loadTextureWithName;
oo::Texture::ConfigurationFactory oo::Texture::configurationFactory =
    &oo::Texture::loadTextureWithConfiguration;

std::shared_ptr<oo::Texture> oo::Texture::textureWithName(
    const std::optional<std::string>& name,
    const std::optional<std::string>& directory, oo::TextureFlags options,
    GLfloat anisotropy, GLfloat lodBias) {
  return nameFactory(name, directory, options, anisotropy, lodBias);
}

std::shared_ptr<oo::Texture> oo::Texture::loadTextureWithName(
    const std::optional<std::string>& name,
    const std::optional<std::string>& directory, oo::TextureFlags options,
    GLfloat anisotropy, GLfloat lodBias) {
  std::string key;
  std::shared_ptr<oo::Texture> result;
  std::optional<std::string> path;
  bool noFNF;

  if (!name.has_value()) [[unlikely]]
    return nullptr;
  if (!oo::GraphicsAvailable()) {
    // Headless: the file is still looked for, so a missing one is still
    // reported, but nothing is decoded or uploaded; the null texture stands
    // in, with zero size.
    if (!ResourceManager::pathForFileNamed(*name, directory).has_value()) {
      if (!oo::Any(options & oo::TextureFlags::kNoFNFMessage))
        OO_LOG_WARN(oo::kLogFileNotFound, "Could not find texture file \"{}\".",
                    *name);
      return nullptr;
    }
    return nullTexture();
  }
  if (!sCheckedExtensions) [[unlikely]]
    checkExtensions();

  if (!gOOTextureInfo.anisotropyAvailable ||
      (options & oo::TextureFlags::kMinFilterMask) !=
          oo::TextureFlags::kMinFilterMipMap) {
    anisotropy = 0.0f;
  }
  if (!gOOTextureInfo.textureLODBiasAvailable ||
      (options & oo::TextureFlags::kMinFilterMask) !=
          oo::TextureFlags::kMinFilterMipMap) {
    lodBias = 0.0f;
  }

  noFNF = oo::Any(options & oo::TextureFlags::kNoFNFMessage);
  options = oo::ApplyTextureOptionDefaults(options &
                                           ~oo::TextureFlags::kNoFNFMessage);

  // Look for existing texture
  key = oo::GenerateTextureCacheKey(directory, *name, options, anisotropy,
                                    lodBias);
  result = oo::share(existingTextureForKey(key));
  if (result == nullptr) {
    path = ResourceManager::pathForFileNamed(*name, directory);
    if (!path.has_value()) {
      if (!noFNF)
        OO_LOG_WARN(oo::kLogFileNotFound, "Could not find texture file \"{}\".",
                    *name);
      return nullptr;
    }

    // No existing texture, load texture.
    result = ::oo::ConcreteTexture::initWithPath(*path, key, options,
                                                 anisotropy, lodBias);
  }

  return result;
}

std::shared_ptr<oo::Texture> oo::Texture::textureWithName(
    const std::optional<std::string>& name,
    const std::optional<std::string>& directory) {
  return loadTextureWithName(name, directory, oo::TextureFlags::kDefaultOptions,
                             kOOTextureDefaultAnisotropy,
                             kOOTextureDefaultLODBias);
}

std::shared_ptr<oo::Texture> oo::Texture::textureWithConfiguration(
    const oo::PList& configuration) {
  return configurationFactory(configuration);
}

std::shared_ptr<oo::Texture> oo::Texture::loadTextureWithConfiguration(
    const oo::PList& configuration) {
  return textureWithConfiguration(configuration, oo::TextureFlags::kNone);
}

std::shared_ptr<oo::Texture> oo::Texture::textureWithConfiguration(
    const oo::PList& configuration, oo::TextureFlags extraOptions) {
  const std::optional<oo::TextureSpecifier> spec =
      oo::InterpretTextureSpecifier(configuration, false);
  if (!spec.has_value()) return nullptr;

  return loadTextureWithName(spec->name, "Textures",
                             spec->options | extraOptions, spec->anisotropy,
                             spec->lodBias);
}

std::shared_ptr<oo::Texture> oo::Texture::nullTexture() {
  // The shared null texture, which OONullTexture keeps for the life of the
  // process.
  return oo::share(::oo::NullTexture::sharedNullTexture());
}

std::shared_ptr<oo::Texture> oo::Texture::textureWithGenerator(
    ::oo::TextureGenerator* generator) {
  return textureWithGenerator(generator, false);
}

std::shared_ptr<oo::Texture> oo::Texture::textureWithGenerator(
    ::oo::TextureGenerator* generator, bool enqueue) {
  if (generator == nullptr) return nullptr;
  // Headless: no generator is queued, so no loader thread runs.
  if (!oo::GraphicsAvailable()) return nullTexture();
  ::oo::TextureGenerator* cxxGenerator = generator;

#ifndef OOTEXTURE_NO_CACHE
  oo::Texture* existing = existingTextureForKey(cxxGenerator->cacheKey());
  if (existing != nullptr && !enqueue) return oo::share(existing);
#endif

  if (!cxxGenerator->enqueue()) {
    OO_LOG_ERR("texture.generator.queue.failed", "Failed to queue generator {}",
               generator->description());
    return nullptr;
  }
  OO_LOG("texture.generator.queue", "Queued texture generator {}",
         generator->description());

  std::shared_ptr<oo::Texture> result = ::oo::ConcreteTexture::initWithLoader(
      generator, cxxGenerator->cacheKey(),
      oo::ApplyTextureOptionDefaults(cxxGenerator->textureOptions()),
      cxxGenerator->anisotropy(), cxxGenerator->lodBias());

  return result;
}

oo::Texture::Texture() {
  if (sAllLiveTextures == nullptr) [[unlikely]]
    sAllLiveTextures = new std::unordered_set<oo::Texture*>;
  sAllLiveTextures->insert(this);
}

namespace oo {

Texture::~Texture() {
  if (sAllLiveTextures != nullptr) sAllLiveTextures->erase(this);
}

}  // namespace oo

void oo::Texture::apply() { OOLogGenericSubclassResponsibility(); }

void oo::Texture::applyNone() {
  OO_ENTER_OPENGL();
  OOGL(glBindTexture(GL_TEXTURE_2D, 0));
#if OO_TEXTURE_CUBE_MAP
  if (oo::CubeMapsAvailable()) OOGL(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));
#endif

#if GL_EXT_texture_lod_bias
  if (gOOTextureInfo.textureLODBiasAvailable)
    OOGL(glTexEnvf(GL_TEXTURE_FILTER_CONTROL_EXT, GL_TEXTURE_LOD_BIAS_EXT, 0));
#endif
}

void oo::Texture::ensureFinishedLoading() {}

bool oo::Texture::isFinishedLoading() { return true; }

std::optional<std::string> oo::Texture::cacheKey() { return std::nullopt; }

NSSize oo::Texture::dimensions() {
  OOLogGenericSubclassResponsibility();
  return NSZeroSize;
}

NSSize oo::Texture::originalDimensions() { return dimensions(); }

bool oo::Texture::isMipMapped() {
  OOLogGenericSubclassResponsibility();
  return false;
}

oo::PixMap oo::Texture::copyPixMapRepresentation() { return oo::kNullPixMap; }

bool oo::Texture::isRectangleTexture() { return false; }

bool oo::Texture::isCubeMap() { return false; }

NSSize oo::Texture::texCoordsScale() { return NSMakeSize(1.0, 1.0); }

GLint oo::Texture::glTextureName() {
  OOLogGenericSubclassResponsibility();
  return 0;
}

void oo::Texture::clearCache() {
  /*	Does not clear sAllLiveTextures - that really must refer to all
          live texture objects.
  */
  SET_TRACE_CONTEXT("clearing sLiveTextureCache");
  if (sLiveTextureCache != nullptr) sLiveTextureCache->clear();

  SET_TRACE_CONTEXT("clearing sRecentTextures");
  // [sRecentTextures autorelease]: released at the drain of the nearest
  // oo::AutoreleaseScope, as upstream's pool did; the only caller,
  // Universe::reinitAndShowDemo(), runs in the frame loop's or the start-up's
  // scope.
  oo::autorelease(std::move(RecentTextures()));
  CLEAR_TRACE_CONTEXT();
}

void oo::Texture::rebindAllTextures() {
  // Keeping around unused, cached textures is unhelpful at this point.
  // DESTROY(): cleared before the release
  const std::shared_ptr<oo::Cache> recentTextures = std::move(RecentTextures());

  if (sAllLiveTextures == nullptr) return;
  // A copy: the set is unordered (as the Foundation set was) and must not
  // change under the loop.
  const std::vector<oo::Texture*> textures(sAllLiveTextures->begin(),
                                           sAllLiveTextures->end());
  for (oo::Texture* texture : textures) {
    texture->forceRebind();
  }
}

#ifdef OO_DEBUG_MONITOR_ENABLED
std::vector<std::shared_ptr<oo::Texture>> oo::Texture::cachedTexturesByAge() {
  std::vector<std::shared_ptr<oo::Texture>> result;
  for (const oo::PList& texture : (RecentTextures() != nullptr)
                                      ? RecentTextures()->pListsByAge()
                                      : std::vector<oo::PList>()) {
    result.emplace_back(oo::share(oo::TextureInObjectNode(texture)));
  }
  return result;
}

std::vector<std::shared_ptr<oo::Texture>> oo::Texture::allTextures() {
  std::vector<std::shared_ptr<oo::Texture>> result;
  if (sAllLiveTextures != nullptr) {
    result.reserve(sAllLiveTextures->size());
    for (oo::Texture* texture : *sAllLiveTextures)
      result.emplace_back(oo::share(texture));
  }

  return result;
}

size_t oo::Texture::dataSize() {
  NSSize dimensions = this->dimensions();
  size_t size = static_cast<size_t>(dimensions.width * dimensions.height);
  if (isCubeMap()) size *= 6;
  if (isMipMapped()) size = size * 4 / 3;

  return size;
}

std::optional<std::string> oo::Texture::name() {
  OOLogGenericSubclassResponsibility();
  return std::nullopt;
}

void oo::Texture::setTrace(bool trace) {
  if (trace && !_trace) {
    OO_LOG("texture.allocTrace.begin",
           "Started tracing texture {} with retain count {}.",
           oo::str::pointerDescription(this), weak_from_this().use_count());
  }
  _trace = trace;
}
#endif

std::optional<std::string> oo::Texture::descriptionComponents() const {
  return std::nullopt;
}

std::optional<std::string> oo::Texture::shortDescriptionComponents() const {
  return std::nullopt;
}

namespace {

// The C++ class's name.
std::string ClassName(const oo::Texture& texture) {
  std::string result = oo::DescriptionClassName(typeid(texture).name());
  return result;
}

}  // namespace

std::string oo::Texture::description() const {
  std::string result = std::format("<{} {}>", ClassName(*this),
                                   oo::str::pointerDescription(this));
  if (const std::optional<std::string> components = descriptionComponents())
    result += "{" + *components + "}";
  return result;
}

std::string oo::Texture::shortDescription() const {
  std::string result =
      std::format("<OOTexture {}>", oo::str::pointerDescription(this));
  if (const std::optional<std::string> components =
          shortDescriptionComponents())
    result += "{" + *components + "}";
  return result;
}

std::string oo::Texture::className() const { return "OOTexture"; }

void oo::Texture::forceRebind() { OOLogGenericSubclassResponsibility(); }

void oo::Texture::addToCaches() {
#ifndef OOTEXTURE_NO_CACHE
  const std::optional<std::string> cacheKey = this->cacheKey();
  if (!cacheKey.has_value()) return;

  // Add self to in-use textures cache, as a raw pointer so the texture isn't
  // retained by the cache.
  if (sLiveTextureCache == nullptr) [[unlikely]]
    sLiveTextureCache = new std::unordered_map<std::string, oo::Texture*>;

  SET_TRACE_CONTEXT("in-use textures cache - SHOULD NOT RETAIN");
  (*sLiveTextureCache)[*cacheKey] = this;
  CLEAR_TRACE_CONTEXT();

  // Add self to recent textures cache.
  if (RecentTextures() == nullptr) [[unlikely]] {
    RecentTextures() = oo::Cache::cacheWithPList(oo::PList());
    RecentTextures()->setName(std::string("recent textures"));
    RecentTextures()->setAutoPrune(true);
    RecentTextures()->setPruneThreshold(kRecentTexturesCount);
  }

  SET_TRACE_CONTEXT("adding to recent textures cache");
  RecentTextures()->setPList(oo::TextureObjectNode(this), *cacheKey);
  CLEAR_TRACE_CONTEXT();
#endif
}

void oo::Texture::removeFromCaches() {
#ifndef OOTEXTURE_NO_CACHE
  const std::optional<std::string> cacheKey = this->cacheKey();
  if (!cacheKey.has_value()) return;

  if (sLiveTextureCache != nullptr) sLiveTextureCache->erase(*cacheKey);
  // Called from a texture's destructor: the cached texture is compared by
  // address.
  oo::Texture* cached = oo::TextureInObjectNode(
      (RecentTextures() != nullptr) ? RecentTextures()->pListForKey(*cacheKey)
                                    : oo::PList());
  if (cached != nullptr && cached == this) [[unlikely]] {
    /* Experimental for now: I think the recent crash problems may
     * be because if the last reference to a texture is in
     * sRecentTextures, and the texture is regenerated, it
     * replaces the texture, causing a release. Therefore, if this
     * texture *isn't* overretained in the texture cache, the 2009
     * crash avoider will delete its replacement from the cache
     * ... possibly before that texture has been fully added to
     * the cache itself. So, the texture is only removed from the
     * cache by key if it was in it with that key. The extra time
     * needed to generate a planet texture compared with loading a
     * standard one may be why this problem shows up.  - CIM 20140122
     */
    OOCAssert(0, "Texture retain count error for %s; cacheKey is %s.",
              cached->description().c_str(),
              cacheKey->c_str());  // miscount in autorelease
    // The following line is needed in order to avoid crashes when there's a
    // 'texture retain count error'. Please do not delete. -- Kaks 20091221
    RecentTextures()->removePListForKey(
        *cacheKey);  // make sure there's no reference left inside
                     // sRecentTexture ( was a show stopper for 1.73)
  }
#endif
}

oo::Texture* oo::Texture::existingTextureForKey(
    const std::optional<std::string>& key) {
#ifndef OOTEXTURE_NO_CACHE
  if (key.has_value()) {
    if (sLiveTextureCache == nullptr) return nullptr;
    auto it = sLiveTextureCache->find(*key);
    return (it != sLiveTextureCache->end()) ? it->second : nullptr;
  }
  return nullptr;
#else
  return nullptr;
#endif
}

void oo::Texture::checkExtensions() {
  OO_ENTER_OPENGL();

  sCheckedExtensions = true;
  if (!oo::GraphicsAvailable()) return;  // headless: nothing is supported

  oo::OpenGLExtensionManager* extMgr =
      oo::OpenGLExtensionManager::sharedManager();
  bool ver120 = extMgr->versionIsAtLeastMajor(1, 2);
  bool ver130 = extMgr->versionIsAtLeastMajor(1, 3);

#if GL_EXT_texture_filter_anisotropic
  gOOTextureInfo.anisotropyAvailable =
      extMgr->haveExtension("GL_EXT_texture_filter_anisotropic") ? 1 : 0;
  OOGL(glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT,
                   &gOOTextureInfo.anisotropyScale));
  {
    const oo::PList anisoScale =
        oo::Defaults::standard().object("texture-anisotropy-scale");
    gOOTextureInfo.anisotropyScale *= oo::Clamp_0_1_f(oo::PListGet<float>::from(
        anisoScale.isNull() ? nullptr : &anisoScale, 0.5f));
  }
#endif

#ifdef GL_CLAMP_TO_EDGE
  gOOTextureInfo.clampToEdgeAvailable =
      ver120 || extMgr->haveExtension("GL_SGIS_texture_edge_clamp");
#endif

#if OO_GL_CLIENT_STORAGE
  gOOTextureInfo.clientStorageAvailable =
      extMgr->haveExtension("GL_APPLE_client_storage") ? 1 : 0;
#endif

  gOOTextureInfo.textureMaxLevelAvailable =
      ver120 || extMgr->haveExtension("GL_SGIS_texture_lod");

#if GL_EXT_texture_lod_bias
  {
    const oo::PList lodBias =
        oo::Defaults::standard().object("use-texture-lod-bias");
    if (oo::PListGet<bool>::from(lodBias.isNull() ? nullptr : &lodBias, true)) {
      gOOTextureInfo.textureLODBiasAvailable =
          extMgr->haveExtension("GL_EXT_texture_lod_bias") ? 1 : 0;
    } else {
      gOOTextureInfo.textureLODBiasAvailable = false;
    }
  }
#endif

#if GL_EXT_texture_rectangle
  gOOTextureInfo.rectangleTextureAvailable =
      extMgr->haveExtension("GL_EXT_texture_rectangle");
#endif

#if OO_TEXTURE_CUBE_MAP
  if (!oo::Defaults::standard().boolForKey("disable-cube-maps")) {
    gOOTextureInfo.cubeMapAvailable =
        ver130 || extMgr->haveExtension("GL_ARB_texture_cube_map");
  } else {
    gOOTextureInfo.cubeMapAvailable = false;
  }

#endif
}

namespace oo {

oo::PList TextureSpecFromObject(const oo::PList& object,
                                const std::optional<std::string>& defaultName) {
  oo::PList value = object;
  if (value.isNull()) {
    if (!defaultName.has_value()) return oo::PList();
    value = oo::PList(*defaultName);
  }
  if (const std::string* name = value.getIf<std::string>()) {
    if (name->empty()) return oo::PList();
    return oo::PList(oo::PList::Dict{{"name", value}});
  }
  if (!value.isDict()) return oo::PList();

  // If we're here, it's a dictionary. (A "name" as the old string extractor
  // read it: a string or a number.)
  const oo::PList* name = value.find("name");
  if (!defaultName.has_value() ||
      (name != nullptr && (name->isString() || name->isNumber())))
    return value;

  // If we get here, there's no "name" key and there is a default, so we fill it
  // in:
  oo::PList::Dict mutableResult = *value.getIf<oo::PList::Dict>();
  mutableResult["name"] = oo::PList(*defaultName);
  return oo::PList(std::move(mutableResult));
}

uint8_t TextureComponentsForFormat(TextureDataFormat format) {
  switch (format) {
    case kTextureDataRGBA:
      return 4;

    case kTextureDataGrayscale:
      return 1;

    case kTextureDataGrayscaleAlpha:
      return 2;

    case kTextureDataInvalid:
      break;
  }

  return 0;
}

bool CubeMapsAvailable() { return gOOTextureInfo.cubeMapAvailable; }

std::optional<TextureSpecifier> InterpretTextureSpecifier(
    const oo::PList& specifier, bool ignoreExtract) {
  std::string name;
  TextureFlags options = TextureFlags::kDefaultOptions;
  float anisotropy = kOOTextureDefaultAnisotropy;
  float lodBias = kOOTextureDefaultLODBias;

  if (const std::string* string = specifier.getIf<std::string>()) {
    name = *string;
  } else if (specifier.isDict()) {
    // The old string extractor gave nil unless the value was a string or a
    // number.
    const oo::PList* nameValue = specifier.find(kTextureSpecifierNameKey);
    if (nameValue == nullptr ||
        !(nameValue->isString() || nameValue->isNumber())) {
      OO_LOG(
          "texture.load.noName",
          "Invalid texture configuration dictionary (must specify name):\n{}",
          oo::DescriptionOf(specifier));
      return std::nullopt;
    }
    name = specifier.get<std::string>(kTextureSpecifierNameKey);

    int quickFlags =
        specifier.get<int>(kTextureSpecifierFlagValueInternalKey, -1);
    if (quickFlags != -1) {
      options = static_cast<TextureFlags>(quickFlags);
    } else {
      std::string filterString =
          specifier.get<std::string>(kTextureSpecifierMinFilterKey, "default");
      if (filterString == "nearest")
        options |= TextureFlags::kMinFilterNearest;
      else if (filterString == "linear")
        options |= TextureFlags::kMinFilterLinear;
      else if (filterString == "mipmap")
        options |= TextureFlags::kMinFilterMipMap;
      else
        options |= TextureFlags::kMinFilterDefault;  // Covers "default"

      filterString =
          specifier.get<std::string>(kTextureSpecifierMagFilterKey, "default");
      if (filterString == "nearest")
        options |= TextureFlags::kMagFilterNearest;
      else
        options |=
            TextureFlags::kMagFilterLinear;  // Covers "default" and "linear"

      if (specifier.get<bool>(kTextureSpecifierNoShrinkKey, false))
        options |= TextureFlags::kNoShrink;
      if (specifier.get<bool>(kTextureSpecifierExtraShrinkKey, false))
        options |= TextureFlags::kExtraShrink;
      if (specifier.get<bool>(kTextureSpecifierRepeatSKey, false))
        options |= TextureFlags::kRepeatS;
      if (specifier.get<bool>(kTextureSpecifierRepeatTKey, false))
        options |= TextureFlags::kRepeatT;
      if (specifier.get<bool>(kTextureSpecifierCubeMapKey, false))
        options |= TextureFlags::kAllowCubeMap;

      if (!ignoreExtract) {
        const oo::PList* extractValue = specifier.find("extract_channel");
        if (extractValue != nullptr &&
            (extractValue->isString() || extractValue->isNumber())) {
          const std::string extractChannel =
              specifier.get<std::string>("extract_channel");
          if (extractChannel == "r") {
            options |= TextureFlags::kExtractChannelR;
          } else if (extractChannel == "g") {
            options |= TextureFlags::kExtractChannelG;
          } else if (extractChannel == "b") {
            options |= TextureFlags::kExtractChannelB;
          } else if (extractChannel == "a") {
            options |= TextureFlags::kExtractChannelA;
          } else {
            OO_LOG_WARN("texture.load.extractChannel.invalid",
                        "Unknown value \"{}\" for extract_channel in specifier "
                        "\"{}\" (should be \"r\", \"g\", \"b\" or \"a\").",
                        extractChannel, oo::DescriptionOf(specifier));
          }
        }
      }
    }
    anisotropy =
        specifier.get<float>("anisotropy", kOOTextureDefaultAnisotropy);
    lodBias =
        specifier.get<float>("texture_LOD_bias", kOOTextureDefaultLODBias);
  } else {
    // Bad type
    // "got" names an object's class, or the property-list type; an Object node
    // is "(null)".
    if (!specifier.isNull())
      OO_LOG(oo::kLogParameterError,
             "{}: expected string or dictionary, got {}.", __PRETTY_FUNCTION__,
             (specifier.type() == oo::PList::Type::kObject)
                 ? std::string("(null)")
                 : std::string(oo::typeName(specifier.type())));
    return std::nullopt;
  }

  if (name.empty()) return std::nullopt;

  return TextureSpecifier{std::move(name), options, anisotropy, lodBias};
}

oo::PList MakeTextureSpecifier(const std::string& name, TextureFlags options,
                               float anisotropy, float lodBias, bool internal) {
  oo::PList::Dict result;

  result[kTextureSpecifierNameKey] = oo::PList(name);

  // -oo_setFloat:forKey: stored +numberWithFloat:, -oo_setUnsignedInteger:
  // +numberWithUnsignedInteger:.
  if (anisotropy != kOOTextureDefaultAnisotropy)
    result[kTextureSpecifierAnisotropyKey] = oo::PList::singleReal(anisotropy);
  if (lodBias != kOOTextureDefaultLODBias)
    result[kTextureSpecifierLODBiasKey] = oo::PList::singleReal(lodBias);

  if (internal) {
    result[std::string(kTextureSpecifierFlagValueInternalKey)] =
        oo::PList::unsignedInteger(oo::ToUnderlying(options));
  } else {
    std::optional<std::string_view> value;
    switch (options & TextureFlags::kMinFilterMask) {
      case TextureFlags::kMinFilterDefault:
        break;

      case TextureFlags::kMinFilterNearest:
        value = "nearest";
        break;

      case TextureFlags::kMinFilterLinear:
        value = "linear";
        break;

      case TextureFlags::kMinFilterMipMap:
        value = "mipmap";
        break;

      default:  // the other enumerators are outside the mask
        break;
    }
    if (value.has_value())
      result[kTextureSpecifierNoShrinkKey] = oo::PList(*value);

    value.reset();
    switch (options & TextureFlags::kMagFilterMask) {
      case TextureFlags::kMagFilterNearest:
        value = "nearest";
        break;

      case TextureFlags::kMagFilterLinear:
        break;

      default:  // the other enumerators are outside the mask
        break;
    }
    if (value.has_value())
      result[kTextureSpecifierMagFilterKey] = oo::PList(*value);

    value.reset();
    switch (options & TextureFlags::kExtractChannelMask) {
      case TextureFlags::kExtractChannelNone:
        break;

      case TextureFlags::kExtractChannelR:
        value = "r";
        break;

      case TextureFlags::kExtractChannelG:
        value = "g";
        break;

      case TextureFlags::kExtractChannelB:
        value = "b";
        break;

      case TextureFlags::kExtractChannelA:
        value = "a";
        break;

      default:  // the other enumerators are outside the mask
        break;
    }
    if (value.has_value())
      result[kTextureSpecifierSwizzleKey] = oo::PList(*value);

    if (oo::Any(options & TextureFlags::kNoShrink))
      result[kTextureSpecifierNoShrinkKey] = oo::PList(true);
    if (oo::Any(options & TextureFlags::kRepeatS))
      result[kTextureSpecifierRepeatSKey] = oo::PList(true);
    if (oo::Any(options & TextureFlags::kRepeatT))
      result[kTextureSpecifierRepeatTKey] = oo::PList(true);
    if (oo::Any(options & TextureFlags::kAllowCubeMap))
      result[kTextureSpecifierCubeMapKey] = oo::PList(true);
  }

  return oo::PList(std::move(result));
}

TextureFlags ApplyTextureOptionDefaults(TextureFlags options) {
  // Set default flags if needed
  if ((options & TextureFlags::kMinFilterMask) ==
      TextureFlags::kMinFilterDefault) {
    if ((UNIVERSE != nullptr ? UNIVERSE->reducedDetail() : false)) {
      options |= TextureFlags::kMinFilterLinear;
    } else {
      options |= TextureFlags::kMinFilterMipMap;
    }
  }

  if (!gOOTextureInfo.textureMaxLevelAvailable) {
    /*	In the unlikely case of an OpenGL system without GL_SGIS_texture_lod,
     disable mip-mapping completely. Strictly this is only needed for
     non-square textures, but extra logic for such a rare case isn't
     worth it.
     */
    if ((options & TextureFlags::kMinFilterMask) ==
        TextureFlags::kMinFilterMipMap) {
      options ^=
          TextureFlags::kMinFilterMipMap ^ TextureFlags::kMinFilterLinear;
    }
  }

  if (oo::Any(options & TextureFlags::kAllowRectTexture)) {
    // Apply rectangle texture restrictions (regardless of whether rectangle
    // textures are available, for consistency)
    options &= TextureFlags::kFlagsAllowedForRectangleTexture;
    if ((options & TextureFlags::kMinFilterMask) ==
        TextureFlags::kMinFilterMipMap) {
      options = (TextureFlags::kMinFilterMask & ~TextureFlags::kMinFilterMask) |
                TextureFlags::kMinFilterLinear;
    }

#if GL_EXT_texture_rectangle
    if (!gOOTextureInfo.rectangleTextureAvailable) {
      options &= ~TextureFlags::kAllowRectTexture;
    }
#else
    options &= ~TextureFlags::kAllowRectTexture;
#endif
  }

  options &= TextureFlags::kDefinedFlags;

  return options;
}

std::string GenerateTextureCacheKey(const std::optional<std::string>& directory,
                                    const std::string& name,
                                    TextureFlags options, float anisotropy,
                                    float lodBias) {
  if (!gOOTextureInfo.anisotropyAvailable ||
      (options & TextureFlags::kMinFilterMask) !=
          TextureFlags::kMinFilterMipMap) {
    anisotropy = 0.0f;
  }
  if (!gOOTextureInfo.textureLODBiasAvailable ||
      (options & TextureFlags::kMinFilterMask) !=
          TextureFlags::kMinFilterMipMap) {
    lodBias = 0.0f;
  }
  options = ApplyTextureOptionDefaults(options & ~TextureFlags::kNoFNFMessage);

  return std::format("{}{}{}:0x{:04X}/{:g}/{:g}",
                     directory.has_value() ? directory->c_str() : "",
                     directory.has_value() ? "/" : "", name,
                     oo::ToUnderlying(options), anisotropy, lodBias);
}

std::string TextureCacheKeyForSpecifier(const oo::PList& specifier) {
  // An empty name, no options and zeros when the specifier is not understood
  // (they were uninitialised).
  const TextureSpecifier spec =
      InterpretTextureSpecifier(specifier, false)
          .value_or(TextureSpecifier{"", TextureFlags::kNone, 0.0f, 0.0f});
  return GenerateTextureCacheKey("Textures", spec.name, spec.options,
                                 spec.anisotropy, spec.lodBias);
}

}  // namespace oo
