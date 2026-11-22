/*	test_ref.cpp
        Unit tests for the game's shared ownership: oo::Object, oo::makeShared,
   oo::share and the weak-reference helpers (oofnd/Object.hpp) over
   std::shared_ptr / std::weak_ptr, and oo::AutoreleaseScope, against the
   Objective-C retain/release/autorelease contract they replace and the
   OOWeakReference semantics.

        Every test object records its destruction in a log, so the tests assert
   not just counts but WHEN and in WHAT ORDER objects die. Running
   this file under AddressSanitizer also helps: it turns any over-release or
   use of a dead object into a failure.
*/

#include <atomic>
#include <memory>
#include <set>
#include <string>
#include <thread>
#include <type_traits>
#include <unordered_set>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

#include "oofnd/AutoreleaseScope.hpp"
#include "oofnd/Object.hpp"

namespace {

std::vector<std::string> gDeathLog;
int gLive = 0;

class Thing : public oo::Object {
 public:
  explicit Thing(std::string name) : name_(std::move(name)) { ++gLive; }
  Thing(const Thing& other) : oo::Object(other), name_(other.name_ + "-copy") {
    ++gLive;
  }
  Thing& operator=(const Thing&) = default;
  ~Thing() override {
    --gLive;
    gDeathLog.push_back(name_);
  }

  const std::string& name() const { return name_; }
  int frob() const { return static_cast<int>(name_.size()); }

  std::shared_ptr<Thing> strong;  // a retained ivar
  std::weak_ptr<Thing> weak;      // a weakRetain'd ivar

 private:
  std::string name_;
};

class Derived : public Thing {
 public:
  explicit Derived(std::string name) : Thing(std::move(name)) {}
  int extra = 7;
};

// A -dealloc that does work: it autoreleases a new object and observes weak
// refs to itself.
class Dealloc : public oo::Object {
 public:
  explicit Dealloc(std::weak_ptr<Dealloc>* watcher, bool* sawSelfNull)
      : watcher_(watcher), sawSelfNull_(sawSelfNull) {
    ++gLive;
  }
  ~Dealloc() override {
    if (watcher_ != nullptr && sawSelfNull_ != nullptr)
      *sawSelfNull_ = watcher_->lock() == nullptr && watcher_->expired();
    oo::autorelease(oo::makeShared<Thing>("born-in-dealloc"));
    --gLive;
    gDeathLog.push_back("dealloc");
  }

 private:
  std::weak_ptr<Dealloc>* watcher_;
  bool* sawSelfNull_;
};

// Records what the shared_ptr that held it contains while it is being
// destroyed.
class Watched : public oo::Object {
 public:
  static inline std::shared_ptr<Watched>* holder = nullptr;
  static inline std::string seenDuringDealloc;

  explicit Watched(std::string n) : name(std::move(n)) {}
  ~Watched() override {
    if (holder != nullptr)
      seenDuringDealloc = *holder ? (*holder)->name : std::string("null");
  }

  std::string name;
};

void reset() { gDeathLog.clear(); }

// --- compile-time contract
// --------------------------------------------------------------------

static_assert(sizeof(std::shared_ptr<Thing>) == sizeof(std::shared_ptr<Thing>),
              "an owning reference is a std::shared_ptr<T> and nothing more");
static_assert(std::is_nothrow_move_constructible_v<std::shared_ptr<Thing>>);
static_assert(std::is_nothrow_move_assignable_v<std::shared_ptr<Thing>>);
static_assert(!std::is_convertible_v<Thing*, std::shared_ptr<Thing>>,
              "raw -> owner takes ownership, so it is explicit (oo::share)");
static_assert(
    std::is_convertible_v<std::shared_ptr<Derived>, std::shared_ptr<Thing>>);
static_assert(
    !std::is_convertible_v<std::shared_ptr<Thing>, std::shared_ptr<Derived>>);
static_assert(
    std::is_convertible_v<std::shared_ptr<Thing>, std::weak_ptr<Thing>>,
    "thing = [aThing weakRetain] stays one line");
static_assert(
    std::is_convertible_v<std::weak_ptr<Derived>, std::weak_ptr<Thing>>);
static_assert(!std::is_copy_constructible_v<oo::AutoreleaseScope>);

// An owning reference to an incomplete type can be declared as a member, like
// an ivar of a @class.
class Incomplete;
struct HoldsIncomplete {
  std::shared_ptr<Incomplete>* p = nullptr;
};

// --- ownership counts
// ---------------------------------------------------

TEST(ref, countStartsAtOneAndDeletesAtZero) {
  reset();
  std::shared_ptr<Thing> t = oo::makeShared<Thing>("a");  // [[Thing alloc] init]
  EXPECT_EQ(t.use_count(), 1);
  std::shared_ptr<Thing> another = t;
  EXPECT_EQ(t.use_count(), 2);
  another.reset();
  EXPECT_EQ(t.use_count(), 1);
  EXPECT_EQ(gLive, 1);
  t.reset();  // dealloc
  EXPECT_EQ(gLive, 0);
  EXPECT_TRUE(gDeathLog == std::vector<std::string>{"a"});
}

TEST(ref, nullSafeFreeFunctions) {
  // [nil autorelease] is nil.
  EXPECT_TRUE(oo::autorelease(std::shared_ptr<Thing>()) == nullptr);
  EXPECT_TRUE(oo::share(static_cast<Thing*>(nullptr)) == nullptr);
}

TEST(ref, copyingAnObjectGivesAFreshCount) {
  std::shared_ptr<Thing> a = oo::makeShared<Thing>("orig");
  std::shared_ptr<Thing> alias = a;
  EXPECT_EQ(a.use_count(), 2);
  std::weak_ptr<Thing> weakA = a;
  std::shared_ptr<Thing> b = oo::makeShared<Thing>(*a);  // -copyWithZone:
  EXPECT_EQ(b.use_count(), 1);
  EXPECT_TRUE(b != a);
  std::weak_ptr<Thing> weakB = b;
  EXPECT_TRUE(!oo::sameObject(weakA, weakB));  // the copy has its own identity
  *b = *a;                                  // assignment leaves counts alone
  EXPECT_EQ(a.use_count(), 2);
  EXPECT_EQ(b.use_count(), 1);
  b.reset();
  EXPECT_TRUE(weakB.expired());
  EXPECT_TRUE(!weakA.expired());
}

// --- std::shared_ptr<T>: copy / move / share
// ----------------------------------------------------------------

TEST(ref, refRetainsAndReleases) {
  reset();
  std::shared_ptr<Thing> owner = oo::makeShared<Thing>("r");
  Thing* raw = owner.get();
  {
    std::shared_ptr<Thing> a = oo::share(raw);  // retains: 2
    EXPECT_EQ(owner.use_count(), 2);
    {
      std::shared_ptr<Thing> b = a;  // copy retains: 3
      EXPECT_EQ(owner.use_count(), 3);
      EXPECT_TRUE(b == a);
      EXPECT_TRUE(b.get() == raw);
    }
    EXPECT_EQ(owner.use_count(), 2);
  }
  EXPECT_EQ(owner.use_count(), 1);  // the alloc's +1 is still ours
  EXPECT_EQ(gLive, 1);
  owner.reset();
  EXPECT_EQ(gLive, 0);
}

TEST(ref, adoptAndMakeRefTakeTheAllocPlusOne) {
  {
    std::shared_ptr<Thing> a = oo::makeShared<Thing>("adopted");
    EXPECT_EQ(a.use_count(), 1);
    std::shared_ptr<Thing> b = oo::makeShared<Thing>("made");
    EXPECT_EQ(b.use_count(), 1);
    EXPECT_EQ(gLive, 2);
  }
  EXPECT_EQ(gLive, 0);
}

TEST(ref, moveStealsWithoutTouchingTheCount) {
  std::shared_ptr<Thing> a = oo::makeShared<Thing>("m");
  Thing* raw = a.get();
  std::shared_ptr<Thing> b = std::move(a);
  EXPECT_TRUE(a == nullptr);
  EXPECT_TRUE(!a);
  EXPECT_TRUE(b.get() == raw);
  EXPECT_EQ(b.use_count(), 1);

  std::shared_ptr<Thing> c;
  c = std::move(b);
  EXPECT_TRUE(b == nullptr);
  EXPECT_EQ(c.use_count(), 1);

  std::shared_ptr<Thing>& self = c;
  c = std::move(self);  // self-move leaves it intact
  EXPECT_TRUE(c.get() == raw);
  c = self;  // self-copy too
  EXPECT_EQ(c.use_count(), 1);
  EXPECT_EQ(gLive, 1);
  c = nullptr;
  EXPECT_EQ(gLive, 0);
}

TEST(ref, assignmentInstallsNewBeforeReleasingOld) {
  // The old object's destructor must observe the holder's NEW value, like a
  // retaining setter.
  Watched::holder = nullptr;
  std::shared_ptr<Watched> slot;
  Watched::holder = &slot;
  slot = oo::makeShared<Watched>("old");
  slot = oo::makeShared<Watched>("new");
  EXPECT_EQ(Watched::seenDuringDealloc, std::string("new"));
  slot.reset();
  EXPECT_EQ(Watched::seenDuringDealloc, std::string("null"));
  Watched::holder = nullptr;
}

TEST(ref, convertingAndComparing) {
  std::shared_ptr<Derived> d = oo::makeShared<Derived>("d");
  std::shared_ptr<Thing> t = d;  // upcast copy retains
  EXPECT_EQ(d.use_count(), 2);
  EXPECT_TRUE(t == d);
  EXPECT_TRUE(t.get() == d.get());
  std::shared_ptr<Thing> moved = std::move(d);  // upcast move steals
  EXPECT_TRUE(d == nullptr);
  EXPECT_EQ(moved.use_count(), 2);
  EXPECT_TRUE(t != nullptr);
  EXPECT_EQ(static_cast<Derived&>(*t).extra, 7);

  std::shared_ptr<Thing> x = oo::makeShared<Thing>("x");
  EXPECT_TRUE((t < x) != (x < t));  // a strict order, for std::set / std::map
  std::set<std::shared_ptr<Thing>> ordered{t, x, moved};
  EXPECT_EQ(ordered.size(), 2u);
  std::unordered_set<std::shared_ptr<Thing>> hashed{t, x, moved};
  EXPECT_EQ(hashed.size(), 2u);
  EXPECT_EQ(x.use_count(), 3);  // x, ordered, hashed
}

TEST(ref, resetAndSwap) {
  std::shared_ptr<Thing> a = oo::makeShared<Thing>("a");
  std::shared_ptr<Thing> b = oo::makeShared<Thing>("b");
  Thing* ra = a.get();
  swap(a, b);
  EXPECT_TRUE(b.get() == ra);
  std::shared_ptr<Thing> owner = oo::makeShared<Thing>("c");
  Thing* raw = owner.get();
  a = oo::share(raw);  // retains, like the constructor
  EXPECT_EQ(owner.use_count(), 2);
  owner.reset();
  a.reset();
  b.reset();
  EXPECT_EQ(gLive, 0);
}

// --- cycles
// ---------------------------------------------------------------------------------

TEST(ref, strongCyclesAreNotCollected) {
  reset();
  std::weak_ptr<Thing> watchA, watchB;
  {
    std::shared_ptr<Thing> a = oo::makeShared<Thing>("cycle-a");
    std::shared_ptr<Thing> b = oo::makeShared<Thing>("cycle-b");
    a->strong = b;
    b->strong = a;
    watchA = a;
    watchB = b;
  }
  // Both external references are gone, and both objects are still alive:
  // exactly what two objects that retain each other do. Nothing collects
  // them.
  EXPECT_EQ(gLive, 2);
  EXPECT_TRUE(gDeathLog.empty());
  EXPECT_TRUE(!watchA.expired() && !watchB.expired());
  EXPECT_EQ(watchA.use_count(), 1);
  EXPECT_EQ(watchB.use_count(), 1);

  // Only breaking an edge by hand frees them (and keeps the rest of this file
  // leak-free).
  watchA.lock()->strong.reset();
  EXPECT_TRUE(watchA.expired() && watchB.expired());
  EXPECT_EQ(gLive, 0);
}

TEST(ref, breakingACycleByHandFreesIt) {
  reset();
  std::weak_ptr<Thing> watchA, watchB;
  {
    std::shared_ptr<Thing> a = oo::makeShared<Thing>("a");
    std::shared_ptr<Thing> b = oo::makeShared<Thing>("b");
    a->strong = b;
    b->strong = a;
    watchA = a;
    watchB = b;
    a->strong.reset();  // break it, as -dealloc-time teardown code does
  }
  EXPECT_TRUE(watchA.expired());
  EXPECT_TRUE(watchB.expired());
  EXPECT_TRUE(
      gDeathLog ==
      (std::vector<std::string>{
          "b",
          "a"}));  // locals die in reverse order; b's destructor releases a
}

TEST(ref, aWeakBackEdgeMeansNoCycle) {
  reset();
  int before = gLive;
  {
    std::shared_ptr<Thing> parent = oo::makeShared<Thing>("parent");
    std::shared_ptr<Thing> child = oo::makeShared<Thing>("child");
    parent->strong = child;  // owner -> owned: strong
    child->weak = parent;    // owned -> owner: weak, as in the entity graph
    EXPECT_TRUE(child->weak.lock() == parent);
  }
  EXPECT_EQ(gLive, before);
  EXPECT_TRUE(gDeathLog == (std::vector<std::string>{"parent", "child"}));
}

// --- std::weak_ptr<T>: OOWeakReference semantics
// ----------------------------------------------------

TEST(ref, weakRefNullsOnDealloc) {
  std::weak_ptr<Thing> w;
  EXPECT_TRUE(w.expired());
  EXPECT_TRUE(w.lock() == nullptr);
  {
    std::shared_ptr<Thing> t = oo::makeShared<Thing>("w");
    w = t;  // thing = [aThing weakRetain]
    EXPECT_TRUE(!w.expired());
    EXPECT_TRUE(w.lock() == t);         // weakRefUnderlyingObject
    EXPECT_EQ(t.use_count(), 1);   // a weak reference does not retain
    std::shared_ptr<Thing> locked = w.lock();
    EXPECT_TRUE(locked == t);
    EXPECT_EQ(t.use_count(), 2);
  }
  EXPECT_TRUE(w.expired());
  EXPECT_TRUE(w.lock() == nullptr);  // messages to it now act like messages to nil
  EXPECT_EQ(gLive, 0);
}

TEST(ref, weakRefsAreZeroedBeforeTheDestructorRuns) {
  reset();
  std::weak_ptr<Dealloc> watcher;
  bool sawSelfNull = false;
  {
    oo::AutoreleaseScope scope;
    std::shared_ptr<Dealloc> d = oo::makeShared<Dealloc>(&watcher, &sawSelfNull);
    watcher = d;
    d.reset();
    EXPECT_TRUE(sawSelfNull);  // inside ~Dealloc, the weak ref already read null
    EXPECT_EQ(scope.pendingCount(), 1u);  // what the destructor autoreleased
  }
  EXPECT_TRUE(gDeathLog ==
           (std::vector<std::string>{"dealloc", "born-in-dealloc"}));
  EXPECT_EQ(gLive, 0);
}

TEST(ref, manyWeakRefsOneControl) {
  std::shared_ptr<Thing> t = oo::makeShared<Thing>("shared");
  std::vector<std::weak_ptr<Thing>> refs(50, std::weak_ptr<Thing>(t));
  std::weak_ptr<Thing> another = t;
  for (const auto& r : refs)
    EXPECT_TRUE(oo::sameObject(r, another));  // one identity, like the one
                                           // weakSelf proxy
  EXPECT_EQ(t.use_count(), 1);
  t.reset();
  bool allExpired = true;
  for (const auto& r : refs) allExpired = allExpired && r.expired();
  EXPECT_TRUE(allExpired);
  refs.clear();  // the control block goes with the last weak_ptr (ASan)
  EXPECT_TRUE(another.expired());
}

TEST(ref, weakRefCopyMoveAndConversion) {
  std::shared_ptr<Derived> d = oo::makeShared<Derived>("d");
  std::weak_ptr<Derived> wd = d;
  std::weak_ptr<Thing> wt = wd;             // upcast copy
  std::weak_ptr<Thing> wm = std::move(wd);  // upcast move
  EXPECT_TRUE(wd.expired());
  EXPECT_TRUE(wd.lock() == nullptr);
  EXPECT_TRUE(wt.lock() == d);
  EXPECT_TRUE(wm.lock() == d);
  EXPECT_TRUE(oo::sameObject(wt, wm));

  std::weak_ptr<Thing> copy = wt;
  std::weak_ptr<Thing> moved;
  moved = std::move(copy);
  EXPECT_TRUE(copy.expired());
  EXPECT_TRUE(oo::sameObject(moved, wt));
  std::weak_ptr<Thing>& self = moved;
  moved = self;
  moved = std::move(self);
  EXPECT_TRUE(oo::sameObject(moved, wt));
  moved.reset();
  EXPECT_TRUE(moved.expired());
  d.reset();
  EXPECT_TRUE(wt.expired() && wm.expired());
}

TEST(ref, weakIdentitySurvivesDeathAndAddressReuse) {
  std::shared_ptr<Thing> a = oo::makeShared<Thing>("a");
  std::weak_ptr<Thing> w = a;
  const std::weak_ptr<Thing> before = w;
  EXPECT_TRUE(oo::sameObject(w, std::weak_ptr<Thing>(a)));
  a.reset();
  EXPECT_TRUE(oo::sameObject(w, before));
  EXPECT_TRUE(oo::sameObject(w, w));
  // However the allocator reuses the address, a new object is never the dead
  // referent.
  for (int i = 0; i < 16; ++i) {
    std::shared_ptr<Thing> n = oo::makeShared<Thing>("a");
    std::weak_ptr<Thing> wn = n;
    EXPECT_TRUE(!oo::sameObject(w, wn));
  }
  EXPECT_TRUE(!oo::isUnset(w));
}

TEST(ref, weakRefOnlyComparisonCreatesNoControl) {
  std::shared_ptr<Thing> a = oo::makeShared<Thing>("a");
  std::shared_ptr<Thing> b = oo::makeShared<Thing>("b");
  std::weak_ptr<Thing> w = a;
  EXPECT_TRUE(!oo::sameObject(w, b->weak_from_this()));
  EXPECT_EQ(b.use_count(), 1);  // comparing took no ownership of b
}

// willDispose(): the last owner's release runs it once, with the object whole
// and its weak references already empty, before the destructor runs.
class Disposing : public Thing {
 public:
  explicit Disposing(std::string name, std::vector<std::string>* log)
      : Thing(std::move(name)), _log(log) {}
  ~Disposing() override { _log->push_back("destructor"); }
  std::weak_ptr<Thing> watcher;

 protected:
  void willDispose() noexcept override {
    _log->push_back(std::string("willDispose, weak ") +
                    (watcher.lock().get() == this ? "live" : "dead") +
                    ", name " + name());
  }

 private:
  std::vector<std::string>* _log;
};

TEST(ref, willDisposeRunsOnceWithTheObjectWhole) {
  std::vector<std::string> log;
  std::shared_ptr<Disposing> d = oo::makeShared<Disposing>("d", &log);
  d->watcher = d;
  std::weak_ptr<Thing> outside = d;
  {
    std::shared_ptr<Disposing> extra = d;  // a balanced retain and release
  }
  EXPECT_TRUE(log.empty());
  d.reset();
  EXPECT_TRUE((log == std::vector<std::string>{"willDispose, weak dead, name d",
                                            "destructor"}));
  EXPECT_TRUE(outside.expired());
}

// --- AutoreleaseScope: the autorelease pool
// ----------------------------------------------------

TEST(ref, autoreleaseReleasesAtScopeEnd) {
  reset();
  EXPECT_TRUE(oo::AutoreleaseScope::current() == nullptr);
  {
    oo::AutoreleaseScope scope;
    EXPECT_TRUE(oo::AutoreleaseScope::current() == &scope);
    Thing* t = oo::autorelease(
        oo::makeShared<Thing>("ar"));  // [[[Thing alloc] init] autorelease]
    EXPECT_EQ(scope.pendingCount(), 1u);
    std::shared_ptr<Thing> keep = oo::share(t);  // a retain that outlives
                                                 // the pool
    EXPECT_EQ(keep.use_count(), 2);            // the pool's and keep's
    EXPECT_EQ(gLive, 1);
  }
  EXPECT_EQ(gLive, 0);  // keep died first, then the pool's release
  EXPECT_TRUE(oo::AutoreleaseScope::current() == nullptr);

  std::shared_ptr<Thing> survivor;
  {
    oo::AutoreleaseScope scope;
    survivor = oo::share(oo::autorelease(oo::makeShared<Thing>("survivor")));
  }
  EXPECT_EQ(gLive, 1);  // retained past the pool: alive
  EXPECT_EQ(survivor.use_count(), 1);
  survivor.reset();
  EXPECT_EQ(gLive, 0);
}

TEST(ref, autoreleaseScopesNest) {
  reset();
  {
    oo::AutoreleaseScope outer;
    oo::autorelease(oo::makeShared<Thing>("outer-1"));
    {
      oo::AutoreleaseScope inner;
      EXPECT_TRUE(oo::AutoreleaseScope::current() == &inner);
      oo::autorelease(oo::makeShared<Thing>("inner-1"));
      oo::autorelease(oo::makeShared<Thing>("inner-2"));
      EXPECT_EQ(inner.pendingCount(), 2u);
      EXPECT_EQ(outer.pendingCount(),
                  1u);  // the innermost scope receives them
    }
    EXPECT_TRUE(oo::AutoreleaseScope::current() == &outer);
    EXPECT_TRUE(gDeathLog ==
             (std::vector<std::string>{"inner-2", "inner-1"}));  // LIFO
    EXPECT_EQ(gLive, 1);
    oo::autorelease(oo::makeShared<Thing>("outer-2"));
  }
  EXPECT_TRUE(gDeathLog == (std::vector<std::string>{"inner-2", "inner-1",
                                                  "outer-2", "outer-1"}));
  EXPECT_EQ(gLive, 0);
}

TEST(ref, drainReleasesInOrderAndKeepsTheScopeOpen) {
  reset();
  oo::AutoreleaseScope scope;
  for (int i = 0; i < 3; ++i)
    oo::autorelease(oo::makeShared<Thing>("t" + std::to_string(i)));
  scope.drain();  // [pool release]; pool = [[... alloc] init]
  EXPECT_TRUE(gDeathLog == (std::vector<std::string>{
                            "t2", "t1", "t0"}));  // the game's order: LIFO
  EXPECT_EQ(scope.pendingCount(), 0u);
  EXPECT_TRUE(oo::AutoreleaseScope::current() == &scope);
  oo::autorelease(oo::makeShared<Thing>("after-drain"));
  EXPECT_EQ(scope.pendingCount(), 1u);
  scope.drain();
  EXPECT_EQ(gLive, 0);
}

TEST(ref, drainingCascades) {
  // Releasing a pooled object whose destructor autoreleases more objects drains
  // those too, in the same drain, as libobjc2's emptyPool loops until the pool
  // is empty. LIFO: the newest (sibling) goes first, and
  // born-in-dealloc lands on top, so it goes right after.
  reset();
  {
    oo::AutoreleaseScope scope;
    oo::autorelease(oo::makeShared<Dealloc>(
        nullptr, nullptr));  // no watcher: it would die before the drain
    oo::autorelease(oo::makeShared<Thing>("sibling"));
  }
  EXPECT_TRUE(gDeathLog ==
           (std::vector<std::string>{"sibling", "dealloc", "born-in-dealloc"}));
  EXPECT_EQ(gLive, 0);
}

TEST(ref, autoreleaseAnOwnedRef) {
  reset();
  {
    oo::AutoreleaseScope scope;
    std::shared_ptr<Thing> r = oo::makeShared<Thing>("owned");
    Thing* t = oo::autorelease(std::move(r));  // return [[x retain] autorelease]
    EXPECT_TRUE(r == nullptr);
    EXPECT_EQ(oo::share(t).use_count(), 2);  // the pool's, and this one
    EXPECT_EQ(t->frob(), 5);
  }
  EXPECT_EQ(gLive, 0);
}

TEST(ref, multipleAutoreleasesOfOneObject) {
  reset();
  {
    oo::AutoreleaseScope scope;
    std::shared_ptr<Thing> t = oo::makeShared<Thing>("twice");
    oo::autorelease(t);
    oo::autorelease(std::move(t));  // two owners handed over, two releases
    EXPECT_EQ(scope.pendingCount(), 2u);
  }
  EXPECT_TRUE(gDeathLog == std::vector<std::string>{"twice"});
  EXPECT_EQ(gLive, 0);
}

TEST(ref, autoreleaseWithNoScopeLeaksAndIsCounted) {
  // Not a Thing: the object is kept forever, so it must stay out of gLive.
  class Orphan : public oo::Object {};
  std::size_t before = oo::AutoreleaseScope::leakedWithoutScope();
  std::shared_ptr<Orphan> t = oo::makeShared<Orphan>();
  std::weak_ptr<Orphan> watch = t;
  oo::autorelease(std::move(t));  // GNUstep: "autorelease called without pool"
  EXPECT_EQ(oo::AutoreleaseScope::leakedWithoutScope(), before + 1);
  EXPECT_TRUE(!watch.expired());  // nothing will ever release that owner
  EXPECT_EQ(watch.use_count(), 1);
}

TEST(ref, scopesArePerThread) {
  reset();
  oo::AutoreleaseScope mainScope;
  std::atomic<bool> threadSawNoScope{false};
  std::atomic<bool> threadDrained{false};
  std::thread worker([&] {
    threadSawNoScope = oo::AutoreleaseScope::current() == nullptr;
    {
      oo::AutoreleaseScope workerScope;
      oo::autorelease(oo::makeShared<Thing>("worker"));
    }
    threadDrained = true;
  });
  worker.join();
  EXPECT_TRUE(threadSawNoScope);
  EXPECT_TRUE(threadDrained);
  EXPECT_EQ(mainScope.pendingCount(), 0u);
  EXPECT_TRUE(oo::AutoreleaseScope::current() == &mainScope);
  EXPECT_TRUE(gDeathLog == std::vector<std::string>{"worker"});
}

// --- thread safety of the count
// ----------------------------------------------------

TEST(ref, retainReleaseAreAtomicAcrossThreads) {
  class Counter : public oo::Object {
   public:
    explicit Counter(std::atomic<int>* deaths) : deaths_(deaths) {}
    ~Counter() override { deaths_->fetch_add(1); }

   private:
    std::atomic<int>* deaths_;
  };
  std::atomic<int> deaths{0};
  std::shared_ptr<Counter> shared = oo::makeShared<Counter>(&deaths);
  constexpr int kThreads = 8;
  constexpr int kIterations = 20000;
  std::vector<std::thread> threads;
  for (int i = 0; i < kThreads; ++i) {
    threads.emplace_back([shared] {  // each thread copies (retains) the owner
      for (int j = 0; j < kIterations; ++j) {
        std::shared_ptr<Counter> local = shared;
        std::shared_ptr<Counter> moved = std::move(local);
      }
    });
  }
  for (std::thread& t : threads) t.join();
  EXPECT_EQ(shared.use_count(), 1);
  EXPECT_EQ(deaths.load(), 0);
  shared.reset();
  EXPECT_EQ(deaths.load(), 1);  // exactly one dealloc

  // The final release may happen on another thread; the destructor runs there,
  // once.
  std::shared_ptr<Counter> handedOff = oo::makeShared<Counter>(&deaths);
  std::thread last([r = std::move(handedOff)]() mutable { r.reset(); });
  last.join();
  EXPECT_EQ(deaths.load(), 2);
}

}  // namespace
