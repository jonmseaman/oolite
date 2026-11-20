/*	test_OOGraphicsResetManager.mm
        Unit tests for OOGraphicsResetManager
   (src/Core/OOGraphicsResetManager.h).

        The tests pin: one shared manager; a reset re-reads the extension
   manager (its ResourceManager stub counts the re-read), rebinds the textures
   (this file's OOTexture stub counts it), then tells every registered client
   once; nil is never registered, a client can be registered twice but is told
   once, unregistered clients are not told, a client unregistered by an earlier
   client during the reset is skipped, and a client that raises is logged and
   does not stop the rest. Clients are not retained. The GL context is a hidden
   window's (oo_gl_test_context.hpp). The test's client is a C++ client. Run:
   tests/run.sh unit
*/

#include <string>
#include <vector>

#include "GraphicsResetManager.h"
#include "OpenGLExtensionManager.h"
#include "RegExpMatcher.h"
#include "oo_gl_test_context.hpp"
#include "oo_test.hpp"
#include "oofnd/PList.hpp"
#include "oofnd/Exception.hpp"

// --- Stubs
// --------------------------------------------------------------------------------------

static unsigned sPathsCalls = 0;
static unsigned sRebinds = 0;
static std::vector<int>
    sOrder;  // client tags, in the order they were told, texture rebinds as 0

#include "Texture.h"
#include "ResourceManager.h"

// C++ definitions of the ResourceManager statics the code under test calls,
// with the stand-in's answers.
std::vector<std::string> ResourceManager::paths() {
  sPathsCalls++;
  return {};
}
oo::PList ResourceManager::dictionaryFromFilesNamed(
    const std::string& /*fileName*/,
    const std::optional<std::string>& /*folderName*/, bool /*mergeFiles*/) {
  return oo::PList(oo::PList::Dict{});
}

// OORegExpMatcher: the stub answers no match.
std::shared_ptr<oo::RegExpMatcher> oo::RegExpMatcher::regExpMatcher() {
  return oo::makeShared<oo::RegExpMatcher>();
}
bool oo::RegExpMatcher::string(const std::string&, const std::string&) {
  return false;
}
namespace oo {

RegExpMatcher::~RegExpMatcher() {}

// OOLogging.mm's (it would bring the resource manager into the link); in the
// debug flavour OOOpenGL.mm's OOGL error check and state dump call them.
void LogIndent(void) {}
void LogOutdent(void) {}

ShaderSetting ShaderSettingFromString(const std::string& string) {
  return string == "SHADERS_FULL" ? ShaderSetting::kFull : ShaderSetting::kNotSupported;
}

// OOLogging.mm's (it would bring the resource manager into the link).
extern const char* const kLogException;
const char* const kLogException = "exception";

}  // namespace oo


// The C++ OOTexture's static member.
void oo::Texture::rebindAllTextures() {
  sRebinds++;
  sOrder.push_back(0);
}

// A client: counts its resets; may unregister another client, or raise, when
// told. Reference counted, so the test can see the manager does not retain it.
struct OOTestResetClient : oo::Object, oo::GraphicsResetClient {
  int tag = 0;
  unsigned resets = 0;
  OOTestResetClient* victim = nullptr;
  bool raises = false;

  void resetGraphicsState() override {
    resets++;
    sOrder.push_back(tag);
    if (victim != nullptr)
      oo::GraphicsResetManager::sharedManager()->unregisterCxxClient(victim);
    if (raises) oo::RaiseException("OOTestException", "client %d raised", tag);
  }
};

namespace {

// Kept until the test's process ends, as the autoreleased clients outlived each
// case's use.
std::vector<std::shared_ptr<OOTestResetClient>> sClients;

OOTestResetClient* Client(int tag) {
  sClients.push_back(oo::makeShared<OOTestResetClient>());
  sClients.back()->tag = tag;
  return sClients.back().get();
}

}  // namespace

// --- Tests
// --------------------------------------------------------------------------------------

OO_TEST(resetTellsEachClientOnce) {
  OO_CHECK(OOTestGLContext());
  {
    oo::GraphicsResetManager* manager = oo::GraphicsResetManager::sharedManager();
    OO_CHECK(manager != nullptr);
    OO_CHECK(oo::GraphicsResetManager::sharedManager() == manager);

    (void)oo::OpenGLExtensionManager::sharedManager();
    const unsigned pathsBefore = sPathsCalls;

    OOTestResetClient *a = Client(1), *b = Client(2), *c = Client(3);
    manager->registerCxxClient(a);
    manager->registerCxxClient(b);
    manager->registerCxxClient(b);  // a set: told once
    manager->registerCxxClient(c);
    manager->registerCxxClient(nullptr);
    manager->unregisterCxxClient(c);
    manager->unregisterCxxClient(c);  // not registered: nothing
    manager->unregisterCxxClient(nullptr);

    sOrder.clear();
    manager->resetGraphicsState();
    OO_CHECK(sPathsCalls ==
             pathsBefore + 1);  // the extension manager was reset
    OO_CHECK(sRebinds == 1);
    OO_CHECK(a->resets == 1 && b->resets == 1 && c->resets == 0);
    OO_CHECK(sOrder.size() == 3 &&
             sOrder[0] == 0);  // textures first, then the clients

    manager->unregisterCxxClient(a);
    manager->unregisterCxxClient(b);
    manager->resetGraphicsState();
    OO_CHECK(sRebinds == 2);
    OO_CHECK(a->resets == 1 && b->resets == 1);
  }
}

OO_TEST(clientsMayUnregisterAndRaise) {
  OO_CHECK(OOTestGLContext());
  {
    oo::GraphicsResetManager* manager = oo::GraphicsResetManager::sharedManager();

    // Two clients that each unregister the other: whichever is told first is
    // the only one told.
    OOTestResetClient *x = Client(10), *y = Client(11);
    x->victim = y;
    y->victim = x;
    manager->registerCxxClient(x);
    manager->registerCxxClient(y);
    manager->resetGraphicsState();
    OO_CHECK(x->resets + y->resets == 1);
    manager->unregisterCxxClient(x);
    manager->unregisterCxxClient(y);

    // A client that raises is logged and ignored; the others are still told.
    OOTestResetClient *thrower = Client(20), *p = Client(21), *q = Client(22);
    thrower->raises = true;
    manager->registerCxxClient(p);
    manager->registerCxxClient(thrower);
    manager->registerCxxClient(q);
    manager->resetGraphicsState();
    OO_CHECK(thrower->resets == 1 && p->resets == 1 && q->resets == 1);
    manager->unregisterCxxClient(thrower);
    manager->unregisterCxxClient(p);
    manager->unregisterCxxClient(q);
  }
}

OO_TEST(clientsAreNotRetained) {
  OO_CHECK(OOTestGLContext());
  {
    oo::GraphicsResetManager* manager = oo::GraphicsResetManager::sharedManager();
    std::shared_ptr<OOTestResetClient> client = oo::makeShared<OOTestResetClient>();
    const long before = client->weak_from_this().use_count();
    manager->registerCxxClient(client.get());
    OO_CHECK(client->weak_from_this().use_count() == before);
    manager->unregisterCxxClient(client.get());
  }
}

// A C++ client: told once per reset, after the textures, like the other client;
// null is never registered; unregistered, it is not told.
namespace {

struct TestCxxClient : oo::GraphicsResetClient {
  unsigned resets = 0;
  void resetGraphicsState() override {
    resets++;
    sOrder.push_back(100);
  }
};

}  // namespace

OO_TEST(cxxClients) {
  OO_CHECK(OOTestGLContext());
  {
    oo::GraphicsResetManager* manager = oo::GraphicsResetManager::sharedManager();
    TestCxxClient c1, c2;
    OOTestResetClient* objc = Client(40);
    manager->registerCxxClient(&c1);
    manager->registerCxxClient(&c1);  // a set: told once
    manager->registerCxxClient(&c2);
    manager->registerCxxClient(nullptr);
    manager->registerCxxClient(objc);
    manager->unregisterCxxClient(&c2);
    manager->unregisterCxxClient(nullptr);

    sOrder.clear();
    const unsigned rebinds = sRebinds;
    manager->resetGraphicsState();
    OO_CHECK(sRebinds == rebinds + 1);
    OO_CHECK(c1.resets == 1 && c2.resets == 0 && objc->resets == 1);
    OO_CHECK(sOrder.size() == 3 && sOrder[0] == 0);  // textures first

    manager->unregisterCxxClient(&c1);
    manager->unregisterCxxClient(objc);
    manager->resetGraphicsState();
    OO_CHECK(c1.resets == 1 && objc->resets == 1);
  }
}

OO_TEST_MAIN()
