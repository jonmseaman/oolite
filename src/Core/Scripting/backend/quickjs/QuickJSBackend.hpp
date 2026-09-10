/*

QuickJSBackend.hpp

Skeleton QuickJS-ng backend for the ooscript facade (JSEngine.hpp). Produces
the `Context`/`Value`/`ClassDef` types and the exotic-method mapping named
in docs/phases/1-js-engine.md's backend seam row, against QuickJS-ng
v0.16.2 (vendored at upstream/quickjs-ng, added as part of this work item —
it was not previously vendored anywhere in this repo).

Status: SKELETON, not wired to anything. No call site targets this backend
yet — that is differential-test territory (work item 4) once the sweep
(work item 2) has landed and a `--backend=quickjs` build exists to diff
against SpiderMonkey. This header exists to work out the shape of the
mapping and surface the places it does not fall out mechanically.

Two decisions below are flagged rather than made, because they change how
every one of the 3,828 retargeted call sites behaves and are exactly the
kind of call this project reserves for Jon (CLAUDE.md: "You may propose;
only Jon adjudicates"):

  1. VALUE OWNERSHIP. ooscript::Value (JSEngine.hpp, the SpiderMonkey side
     of the facade) wraps a SpiderMonkey jsval, which is non-owning: the
     engine's conservative-stack GC scans argv/rval slots and private
     data, and a jsval can be copied and discarded freely with no
     JS_FreeValue/JS_DupValue bookkeeping. QuickJS-ng's JSValue is
     reference-counted and *owning*: "a function returning a JSValue
     transfers ownership to caller; caller must call JS_FreeValue"
     (quickjs.h, JS_CHECK_JSVALUE comment block). A jsval-shaped call site
     retargeted in work item 2 -- `Value v = someCall(); return v;` with
     no free, `Value a = b;` copied into two places -- is correct against
     SpiderMonkey and a leak or a use-after-free against QuickJS-ng.
     Reconciling this needs one of: (a) an owning/borrowed split in the
     facade itself (a breaking change to JSEngine.hpp, revisiting work
     item 1's already-landed exemplar), (b) an owning wrapper local to
     this backend that a differential-test harness can leak-check without
     changing the facade's public shape, or (c) accepting that the
     QuickJS-ng backend cannot be a drop-in JSEngine.hpp implementation
     and instead sits behind a second, explicitly-owning API. This
     skeleton takes (b) provisionally -- see `Value` below -- because it
     is the only option that does not reopen work item 1, but it needs
     sign-off before work item 3 (full implementation) proceeds.

  2. EXOTIC-METHOD MAPPING IS NOT 1:1. The five SpiderMonkey stub hooks
     (JSClass::addProperty/delProperty/getProperty/setProperty/enumerate/
     resolve/convert) and QuickJS-ng's JSClassExoticMethods
     (get_own_property/get_own_property_names/delete_property/
     define_own_property/has_property/get_property/set_property) overlap
     but are not the same shape:
       - enumerate  -> get_own_property_names                  (close)
       - resolve    -> get_own_property / has_property          (close)
       - getProperty/setProperty -> get_property/set_property   (close,
         but QuickJS's default -- omitting exotic entirely and using
         ordinary class properties via JS_SetPropertyFunctionList --
         covers the *dispatch-by-id* pattern OOJSVector.mm and its
         siblings use, in which case no exotic methods are needed at all
         and PropertySpec below is the real mapping target for that 60%
         of classes)
       - convert    -> NO ANALOG. JSConvertOp existed for SpiderMonkey's
         JSVAL_TO_* coercions; QuickJS-ng uses the standard ES ToPrimitive
         protocol (Symbol.toPrimitive / valueOf / toString) uniformly and
         has no per-class hook to intercept it. Classes that relied on
         JS_ConvertStub (i.e. all of them, per architecture.md §4 R1 --
         it is a stub everywhere in this codebase) need nothing here;
         classes that ever implemented a *non-stub* convert hook (none
         found in a survey of this file's exemplar, OOJSVector.mm) would
         need a toString/valueOf method instead. Flagged as a difference
         to watch during work item 4's differential testing, not a gap
         that needs filling now.

See ADR-0002, ADR-0011, docs/phases/1-js-engine.md work items 1 and 3.

*/

#pragma once

// Deliberately does not include ../../JSEngine.hpp: that header pulls in
// <jsapi.h> (SpiderMonkey), and one backend should not need the other
// engine's headers to compile. The two are siblings under `ooscript`, not
// a dependency of each other; JSEngine.hpp is referenced only in prose
// above.
#include <quickjs.h>


namespace ooscript::backend::quickjs {


// ---------------------------------------------------------------------
// Value — owning, move-only wrapper around a QuickJS-ng JSValue.
//
// Provisional answer to design decision 1 above: rather than pretend
// QuickJS's owning JSValue is interchangeable with SpiderMonkey's
// non-owning jsval (ooscript::Value), this type makes ownership explicit
// in the C++ type system. A retargeted call site written against
// ooscript::Value will not compile against this type unchanged, which is
// the point -- it forces every site touched in the differential-test pass
// to make an explicit ownership call instead of inheriting a silent bug.
// ---------------------------------------------------------------------
class Value
{
public:
	Value() noexcept : _ctx(nullptr), _v(JS_UNDEFINED) {}

	// Takes ownership of v (the common case: v is a fresh return value).
	Value(JSContext *ctx, JSValue v) noexcept : _ctx(ctx), _v(v) {}

	~Value() { reset(); }

	Value(const Value &other) noexcept : _ctx(other._ctx), _v(JS_DupValue(other._ctx, other._v)) {}
	Value &operator=(const Value &other) noexcept
	{
		if (this != &other)
		{
			JSValue dup = JS_DupValue(other._ctx, other._v);
			reset();
			_ctx = other._ctx;
			_v = dup;
		}
		return *this;
	}

	Value(Value &&other) noexcept : _ctx(other._ctx), _v(other._v)
	{
		other._ctx = nullptr;
		other._v = JS_UNDEFINED;
	}
	Value &operator=(Value &&other) noexcept
	{
		if (this != &other)
		{
			reset();
			_ctx = other._ctx;
			_v = other._v;
			other._ctx = nullptr;
			other._v = JS_UNDEFINED;
		}
		return *this;
	}

	// Borrow without taking ownership -- for passing to a QuickJS API that
	// takes JSValueConst (a documentation-only alias of JSValue; see
	// quickjs.h's JS_CHECK_JSVALUE comment). Does not dup.
	JSValue borrow() const noexcept { return _v; }

	// Releases ownership to the caller, who becomes responsible for
	// JS_FreeValue. For handing a Value to a QuickJS API that consumes it
	// (a JSValue parameter, which "takes ownership; caller must not call
	// JS_FreeValue").
	JSValue release() noexcept
	{
		JSValue v = _v;
		_ctx = nullptr;
		_v = JS_UNDEFINED;
		return v;
	}

	bool isObject() const noexcept { return JS_IsObject(_v); }
	bool isException() const noexcept { return JS_IsException(_v); }
	bool isUndefined() const noexcept { return JS_IsUndefined(_v); }
	bool isNull() const noexcept { return JS_IsNull(_v); }

private:
	void reset() noexcept
	{
		if (_ctx != nullptr)  JS_FreeValue(_ctx, _v);
		_ctx = nullptr;
		_v = JS_UNDEFINED;
	}

	JSContext *_ctx;
	JSValue _v;
};


// ---------------------------------------------------------------------
// ClassDef — a JSClassID plus the JSClassDef QuickJS-ng registers it
// with. Unlike SpiderMonkey's JSClass (a single struct literal a call
// site can declare `static`), QuickJS-ng classes are registered at
// runtime via JS_NewClassID + JS_NewClass against a specific JSRuntime,
// so this type owns that two-step registration instead of being a plain
// data aggregate.
// ---------------------------------------------------------------------
class ClassDef
{
public:
	ClassDef(const char *name, JSClassFinalizer *finalizer, JSClassExoticMethods *exotic = nullptr) noexcept
		: _id(JS_INVALID_CLASS_ID)
	{
		_def.class_name = name;
		_def.finalizer = finalizer;
		_def.gc_mark = nullptr;
		_def.call = nullptr;
		_def.exotic = exotic;
	}

	// Must be called once per JSRuntime before any object of this class
	// is created against that runtime. Idempotent per-runtime registration
	// (tracking whether a given JSRuntime has already registered this
	// class) is deliberately not handled here -- ownership of "one
	// JSRuntime per process, registered once at startup" versus
	// "possibly-multiple runtimes" is an Oolite-lifecycle question, not a
	// façade question, and belongs with work item 3's full implementation.
	bool registerWith(JSRuntime *rt) noexcept
	{
		if (JS_NewClassID(rt, &_id) == JS_INVALID_CLASS_ID)  return false;
		return JS_NewClass(rt, _id, &_def) == 0;
	}

	JSClassID id() const noexcept { return _id; }

private:
	JSClassID _id;
	JSClassDef _def;
};


// ---------------------------------------------------------------------
// Context — mirrors ooscript::Context's method set (JSEngine.hpp) so the
// two backends read the same at a call site, modulo the Value ownership
// difference above. Every method here is a direct, thin call to the
// quickjs.h primitive it wraps.
// ---------------------------------------------------------------------
class Context
{
public:
	explicit Context(JSContext *cx) noexcept : _cx(cx) {}
	operator JSContext *() const noexcept { return _cx; }
	JSContext *raw() const noexcept { return _cx; }

	Value newNumberValue(double d) const noexcept
	{
		return Value(_cx, JS_NewFloat64(_cx, d));
	}

	// Returns false (and leaves *outNumber unmodified) on exception, same
	// contract as JS_ValueToNumber's JSBool return in the SpiderMonkey
	// facade -- mirrors ooscript::Context::valueToNumber's signature
	// rather than surfacing QuickJS's exception value directly, so a
	// differential test can compare the two backends' control flow
	// without special-casing this call.
	bool valueToNumber(const Value &v, double *outNumber) const noexcept
	{
		return JS_ToFloat64(_cx, outNumber, v.borrow()) == 0;
	}

	bool valueToBoolean(const Value &v, bool *outBool) const noexcept
	{
		int b = JS_ToBool(_cx, v.borrow());
		if (b < 0)  return false;	// exception
		*outBool = (b != 0);
		return true;
	}

	// setPrivate/getPrivate: JS_SetOpaque/JS_GetOpaque, the QuickJS-ng
	// equivalent of JS_SetPrivate/JS_GetPrivate -- "a plain pointer, which
	// ports to C++ unchanged" (architecture.md §4 R1) holds for this
	// backend too; only the accessor names differ.
	bool setPrivate(const Value &obj, void *data) const noexcept
	{
		return JS_SetOpaque(obj.borrow(), data) == 0;
	}

	void *getPrivate(const Value &obj, const ClassDef &clasp) const noexcept
	{
		return JS_GetOpaque2(_cx, obj.borrow(), clasp.id());
	}

	Value newObject(const ClassDef &clasp) const noexcept
	{
		return Value(_cx, JS_NewObjectClass(_cx, clasp.id()));
	}

private:
	JSContext *_cx;
};


} // namespace ooscript::backend::quickjs
