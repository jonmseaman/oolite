/*	test_notification.cpp
        Unit tests for oofnd/Notification.hpp:
   oo::NotificationCenter, the replacement for NSNotificationCenter, checked
   against the semantics the header documents: name and object filters (empty
   name / nullptr object as wildcards), registration-order synchronous delivery,
        removal by observer, by (observer, name, object) and by token, removal
   and addition during a post, nested posts, and posting from another thread.
*/


#include <gtest/gtest.h>

#include "oofnd/Notification.hpp"

#include <string>
#include <thread>
#include <vector>


namespace {

const char kWillReset[] = "org.aegidian.oolite OOJavaScriptEngine will reset";
const char kDidReset[] = "org.aegidian.oolite OOJavaScriptEngine did reset";

int gEngine = 0;  // stand-ins for sender objects: only their addresses matter
int gOtherEngine = 0;
int gA = 0, gB = 0, gC = 0;

TEST(notification, deliversByNameAndObjectInRegistrationOrder) {
  oo::NotificationCenter nc;
  std::vector<std::string> log;
  nc.addObserver(&gA, kWillReset, &gEngine,
                 [&](const oo::Notification&) { log.push_back("a"); });
  nc.addObserver(&gB, kWillReset, &gEngine,
                 [&](const oo::Notification&) { log.push_back("b"); });
  nc.addObserver(&gC, kWillReset, &gEngine,
                 [&](const oo::Notification&) { log.push_back("c"); });
  nc.addObserver(&gC, kDidReset, &gEngine,
                 [&](const oo::Notification&) { log.push_back("did"); });

  nc.post(kWillReset, &gEngine);
  EXPECT_TRUE((log == std::vector<std::string>{"a", "b", "c"}));

  log.clear();
  nc.post(kDidReset, &gEngine);
  EXPECT_TRUE((log == std::vector<std::string>{"did"}));

  log.clear();
  nc.post(kWillReset,
          &gOtherEngine);  // a different object: nobody listens to it
  nc.post("unrelated", &gEngine);
  EXPECT_TRUE(log.empty());
}

TEST(notification, namesCompareByValue) {
  oo::NotificationCenter nc;
  int calls = 0;
  nc.addObserver(&gA, std::string("will reset"), nullptr,
                 [&](const oo::Notification&) { ++calls; });
  const std::string sameTextElsewhere = std::string("will ") + "reset";
  nc.post(sameTextElsewhere, nullptr);
  EXPECT_EQ(calls, 1);
}

TEST(notification, wildcardsWhenAdding) {
  oo::NotificationCenter nc;
  std::vector<std::string> log;
  nc.addObserver(&gA, kWillReset, nullptr, [&](const oo::Notification& n) {
    log.push_back(std::string("anyObject:") + std::string(n.name));
    EXPECT_TRUE(n.object == &gOtherEngine);
  });
  nc.addObserver(&gB, "", &gOtherEngine, [&](const oo::Notification& n) {
    log.push_back(std::string("anyName:") + std::string(n.name));
  });
  nc.post(kWillReset, &gOtherEngine);
  EXPECT_TRUE(
      (log == std::vector<std::string>{std::string("anyObject:") + kWillReset,
                                       std::string("anyName:") + kWillReset}));
}

TEST(notification, removeByObserverNameAndObject) {
  oo::NotificationCenter nc;
  int a = 0, b = 0, did = 0;
  nc.addObserver(&gA, kWillReset, &gEngine,
                 [&](const oo::Notification&) { ++a; });
  nc.addObserver(&gA, kDidReset, &gEngine,
                 [&](const oo::Notification&) { ++did; });
  nc.addObserver(&gB, kWillReset, &gEngine,
                 [&](const oo::Notification&) { ++b; });

  nc.removeObserver(&gA, kWillReset, &gEngine);  // only that registration of a
  nc.post(kWillReset, &gEngine);
  nc.post(kDidReset, &gEngine);
  EXPECT_EQ(a, 0);
  EXPECT_EQ(b, 1);
  EXPECT_EQ(did, 1);

  nc.removeObserver(&gB, kWillReset,
                    &gOtherEngine);  // object does not match: stays
  nc.post(kWillReset, &gEngine);
  EXPECT_EQ(b, 2);

  nc.removeObserver(&gA);  // everything of a
  nc.post(kDidReset, &gEngine);
  EXPECT_EQ(did, 1);
  EXPECT_EQ(nc.observerCount(), 1u);

  nc.removeObserver(&gB, "", nullptr);  // wildcards when removing
  EXPECT_EQ(nc.observerCount(), 0u);
  nc.removeObserver(&gB);  // removing twice is harmless
  nc.removeObserver(nullptr);
}

TEST(notification, duplicateRegistrationsDeliverTwiceAndRemoveTogether) {
  oo::NotificationCenter nc;
  int calls = 0;
  nc.addObserver(&gA, kWillReset, &gEngine,
                 [&](const oo::Notification&) { ++calls; });
  nc.addObserver(&gA, kWillReset, &gEngine,
                 [&](const oo::Notification&) { ++calls; });
  nc.post(kWillReset, &gEngine);
  EXPECT_EQ(calls, 2);
  nc.removeObserver(&gA, kWillReset, &gEngine);
  nc.post(kWillReset, &gEngine);
  EXPECT_EQ(calls, 2);
}

TEST(notification, removeByToken) {
  oo::NotificationCenter nc;
  int first = 0, second = 0;
  const auto t1 = nc.addObserver(nullptr, kWillReset, nullptr,
                                 [&](const oo::Notification&) { ++first; });
  const auto t2 = nc.addObserver(nullptr, kWillReset, nullptr,
                                 [&](const oo::Notification&) { ++second; });
  EXPECT_TRUE(t1 != 0);
  EXPECT_TRUE(t1 != t2);
  nc.removeObserver(t1);
  nc.post(kWillReset, &gEngine);
  EXPECT_EQ(first, 0);
  EXPECT_EQ(second, 1);
  nc.removeObserver(t1);  // unknown token: no-op
  nc.removeObserver(
      nullptr);  // a null observer never matches token-only entries
  EXPECT_EQ(nc.observerCount(), 1u);
}

// The game's pattern: each JS-holding object removes itself from inside its
// will-reset callback.
TEST(notification, removalDuringPostIsSafe) {
  oo::NotificationCenter nc;
  std::vector<std::string> log;
  nc.addObserver(&gA, kWillReset, &gEngine, [&](const oo::Notification&) {
    log.push_back("a");
    nc.removeObserver(&gA, kWillReset, &gEngine);  // removes itself mid-post
    nc.removeObserver(&gC);  // and a later observer, which must not run
  });
  nc.addObserver(&gB, kWillReset, &gEngine,
                 [&](const oo::Notification&) { log.push_back("b"); });
  nc.addObserver(&gC, kWillReset, &gEngine,
                 [&](const oo::Notification&) { log.push_back("c"); });

  nc.post(kWillReset, &gEngine);
  EXPECT_TRUE((log == std::vector<std::string>{"a", "b"}));

  log.clear();
  nc.post(kWillReset, &gEngine);
  EXPECT_TRUE((log == std::vector<std::string>{"b"}));
}

TEST(notification, additionDuringPostWaitsForTheNextPost) {
  oo::NotificationCenter nc;
  int added = 0;
  bool once = false;
  nc.addObserver(&gA, kWillReset, nullptr, [&](const oo::Notification&) {
    if (once) return;
    once = true;
    nc.addObserver(&gB, kWillReset, nullptr,
                   [&](const oo::Notification&) { ++added; });
  });
  nc.post(kWillReset, nullptr);
  EXPECT_EQ(added, 0);
  nc.post(kWillReset, nullptr);
  EXPECT_EQ(added, 1);
}

TEST(notification, nestedPosts) {
  oo::NotificationCenter nc;
  std::vector<std::string> log;
  nc.addObserver(&gA, kWillReset, nullptr, [&](const oo::Notification&) {
    log.push_back("will");
    nc.post(kDidReset, nullptr);
    log.push_back("will-end");
  });
  nc.addObserver(&gB, kDidReset, nullptr,
                 [&](const oo::Notification&) { log.push_back("did"); });
  nc.post(kWillReset, nullptr);
  EXPECT_TRUE((log == std::vector<std::string>{"will", "did", "will-end"}));
}

TEST(notification, postsOnThePostingThread) {
  oo::NotificationCenter nc;
  std::thread::id deliveredOn;
  nc.addObserver(&gA, kWillReset, nullptr, [&](const oo::Notification&) {
    deliveredOn = std::this_thread::get_id();
  });
  std::thread::id postedOn;
  std::thread t([&] {
    postedOn = std::this_thread::get_id();
    nc.post(kWillReset, nullptr);
  });
  t.join();
  EXPECT_TRUE(deliveredOn == postedOn);
  EXPECT_TRUE(deliveredOn != std::this_thread::get_id());
}

TEST(notification, defaultCenterIsOneInstance) {
  oo::NotificationCenter& a = oo::NotificationCenter::defaultCenter();
  oo::NotificationCenter& b = oo::NotificationCenter::defaultCenter();
  EXPECT_TRUE(&a == &b);
  int calls = 0;
  const auto token = a.addObserver(&gA, "test_notification.default", nullptr,
                                   [&](const oo::Notification&) { ++calls; });
  b.post("test_notification.default", nullptr);
  EXPECT_EQ(calls, 1);
  a.removeObserver(token);
  EXPECT_EQ(a.observerCount(), 0u);
}

}  // namespace
