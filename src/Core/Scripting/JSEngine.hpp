/*

JSEngine.hpp

ooscript facade: a thin C++23 layer between Oolite and its embedded JS
engine. Sized to the call-site histogram in docs/architecture.md §4 R1 —
3,828 JS_* call sites across 102 files, concentrated in JS_NewNumberValue
(145), JS_ValueToNumber (101), JS_ValueToBoolean (63), the five stub hooks
JS_PropertyStub/JS_ResolveStub/JS_ConvertStub/JS_EnumerateStub (60+32+32+29),
and JS_InitClass (29) — not against SpiderMonkey's or QuickJS-ng's own API
surface. See ADR-0002, ADR-0011, docs/phases/1-js-engine.md work item 1.

Phase 1 step 2 retargets every call site onto this facade with SpiderMonkey
1.8.5 still behind it; behaviour must stay byte-identical. This header is
therefore a renaming and grouping of existing jsapi.h primitives, not a
redesign: every method here is a direct, inline, zero-overhead call to the
jsapi.h function it replaces. When the QuickJS-ng backend (Phase 1 step 3)
lands, this file gets a second implementation behind the same names; call
sites do not change again.

What this header deliberately does NOT touch, and why:

  - JSClass hook signatures (JSBool(JSContext*, JSObject*, jsid, jsval*),
    JSNative, etc.) are SpiderMonkey's C ABI for JSClass/JSFunctionSpec
    function-pointer members, not a Oolite convention. The facade cannot
    change them without changing what JS_InitClass expects to store, so
    hook bodies keep the jsapi.h signature and use façade types internally.

  - OOJS_NATIVE_ENTER/OOJS_NATIVE_EXIT and OOJS_PROFILE_ENTER/EXIT
    (OOJSEngineNativeWrappers.h) are untouched. They braid an Objective-C
    @try/@catch/@finally around a C-stack-allocated profiler frame; wrapping
    that in a C++ RAII guard is exactly the kind of redesign this migration
    forbids mid-translation (CLAUDE.md: "no redesign during conversion").
    Façade code is written to be used *inside* those macro-bracketed
    bodies unchanged. Re-expressing them in C++ terms is Phase 1 step 3's
    problem, once the QuickJS-ng backend removes the Objective-C dependency
    from the hot call path entirely.

  - Native objects attach their backing struct via JS_SetPrivate/GetPrivate
    — "a plain pointer, which ports to C++ unchanged" (architecture.md §4
    R1). setPrivate/getPrivate below stay untemplated for the same reason
    jsapi.h's versions are: the facade does not know the pointee's type.

A binding file that adopts this facade needs C++ types in scope and so
moves from Objective-C (.m) to Objective-C++ (.mm); see OOJSVector.mm for
the exemplar.

JavaScript support for Oolite
Copyright (C) 2007-2013 David Taylor and Jens Ayton.

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

#include <jsapi.h>


namespace ooscript {


// ---------------------------------------------------------------------
// Value — one JS value slot (jsval). Implicitly convertible both ways so
// every existing call site that takes/returns jsval keeps compiling as
// call sites are converted one at a time.
// ---------------------------------------------------------------------
class Value
{
public:
	Value() noexcept : _v(JSVAL_VOID) {}
	Value(jsval v) noexcept : _v(v) {}

	operator jsval() const noexcept { return _v; }
	jsval raw() const noexcept { return _v; }

	bool isObject() const noexcept { return JSVAL_IS_OBJECT(_v); }
	bool isNumber() const noexcept { return JSVAL_IS_NUMBER(_v); }
	bool isVoid()   const noexcept { return JSVAL_IS_VOID(_v); }
	bool isNull()   const noexcept { return JSVAL_IS_NULL(_v); }

	JSObject *toObjectOrNull() const noexcept
	{
		return isObject() ? JSVAL_TO_OBJECT(_v) : nullptr;
	}

	static Value FromObject(JSObject *o) noexcept { return Value(OBJECT_TO_JSVAL(o)); }
	static Value Void() noexcept { return Value(JSVAL_VOID); }
	static Value Null() noexcept { return Value(JSVAL_NULL); }

private:
	jsval _v;
};


// ---------------------------------------------------------------------
// PropertyId — a property/argument identifier (jsid). Oolite's classes use
// the "shared getter/setter, dispatch by integer id" pattern (VectorGetProperty
// et al.) almost exclusively, so this wraps exactly that, not a per-property
// callback shape.
// ---------------------------------------------------------------------
class PropertyId
{
public:
	PropertyId(jsid id) noexcept : _id(id) {}
	bool isInt() const noexcept { return JSID_IS_INT(_id); }
	int toInt() const noexcept { return JSID_TO_INT(_id); }
	jsid raw() const noexcept { return _id; }

private:
	jsid _id;
};


// ---------------------------------------------------------------------
// ClassDef / PropertySpec / FunctionSpec — renamed, not redesigned.
// ---------------------------------------------------------------------
using ClassDef     = JSClass;
using PropertySpec = JSPropertySpec;
using FunctionSpec = JSFunctionSpec;
using Native       = JSNative;

// The five stub hooks alone cover ~40% of the 3,828 call sites
// (architecture.md §4 R1) and are the scripted clang-refactor target
// named in the phase doc's executor split. Named here so a class's hook
// table reads in facade vocabulary; each is still exactly jsapi.h's stub.
inline constexpr JSPropertyOp AddPropertyStub    = JS_PropertyStub;
inline constexpr JSPropertyOp DeletePropertyStub = JS_PropertyStub;
inline constexpr JSResolveOp  ResolveStub        = JS_ResolveStub;
inline constexpr JSConvertOp  ConvertStub        = JS_ConvertStub;
inline constexpr JSEnumerateOp EnumerateStub     = JS_EnumerateStub;


// ---------------------------------------------------------------------
// Rooted handles.
//
// SpiderMonkey 1.8.5 predates the Rooted<T>/Handle<T> template family; GC
// safety here means "the value lives in a slot the collector already
// scans" — an argv/rval slot, a native's private data, or something
// explicitly registered with JS_AddRoot. RootedValue/RootedObject give that
// last case an RAII name that survives the QuickJS-ng swap (where rooting
// becomes explicit for every local), implemented today as
// JS_AddNamedValueRoot/JS_AddNamedObjectRoot paired with the matching removal.
//
// Not used by the OOJSVector retarget: every local there is already rooted
// via an argv slot, the return-value slot, or the object's own private
// data, so adding an explicit root would be new behaviour, not a rename.
// ---------------------------------------------------------------------
class RootedObject
{
public:
	RootedObject(JSContext *cx, JSObject *initial = nullptr) noexcept
		: _cx(cx), _obj(initial)
	{
		JS_AddNamedObjectRoot(_cx, &_obj, "ooscript::RootedObject");
	}
	~RootedObject() { JS_RemoveObjectRoot(_cx, &_obj); }

	RootedObject(const RootedObject &) = delete;
	RootedObject &operator=(const RootedObject &) = delete;

	JSObject *get() const noexcept { return _obj; }
	void set(JSObject *o) noexcept { _obj = o; }
	operator JSObject *() const noexcept { return _obj; }
	JSObject **address() noexcept { return &_obj; }

private:
	JSContext *_cx;
	JSObject *_obj;
};

class RootedValue
{
public:
	RootedValue(JSContext *cx, jsval initial = JSVAL_VOID) noexcept
		: _cx(cx), _v(initial)
	{
		JS_AddNamedValueRoot(_cx, &_v, "ooscript::RootedValue");
	}
	~RootedValue() { JS_RemoveValueRoot(_cx, &_v); }

	RootedValue(const RootedValue &) = delete;
	RootedValue &operator=(const RootedValue &) = delete;

	Value get() const noexcept { return Value(_v); }
	void set(jsval v) noexcept { _v = v; }
	operator jsval() const noexcept { return _v; }
	jsval *address() noexcept { return &_v; }

private:
	JSContext *_cx;
	jsval _v;
};


// ---------------------------------------------------------------------
// CallArgs — the (JSContext *context, uintN argc, jsval *vp) triple every
// JSNative receives, plus the JS_THIS_OBJECT/JS_ARGV/JS_SET_RVAL macros
// that currently all depend on a local literally named `context`. Wrapping
// the triple removes that naming convention; call sites still read
// almost identically (OOJS_ARGV → args.argv(), OOJS_THIS → args.thisObject()).
// ---------------------------------------------------------------------
class CallArgs
{
public:
	CallArgs(JSContext *cx, uintN argc, jsval *vp) noexcept
		: _cx(cx), _argc(argc), _vp(vp) {}

	JSContext *context() const noexcept { return _cx; }
	uintN argc() const noexcept { return _argc; }
	JSObject *thisObject() const noexcept { return JS_THIS_OBJECT(_cx, _vp); }
	jsval *argv() const noexcept { return JS_ARGV(_cx, _vp); }

	Value arg(uintN i) const noexcept
	{
		return i < _argc ? Value(argv()[i]) : Value::Void();
	}

	void setReturn(Value v) const noexcept { JS_SET_RVAL(_cx, _vp, v.raw()); }

private:
	JSContext *_cx;
	uintN _argc;
	jsval *_vp;
};


// ---------------------------------------------------------------------
// Context — the concentrated numeric-conversion and object-lifecycle call
// sites: JS_NewNumberValue/JS_ValueToNumber/JS_ValueToBoolean are ~8% of
// all 3,828 sites on their own (architecture.md §4 R1); JS_SetPrivate/
// JS_GetPrivate/JS_GetInstancePrivate are how every native object attaches
// its backing pointer; JS_InitClass and JS_NewObject construct classes and
// instances. Wrapping these as methods means the QuickJS-ng backend
// supplies one implementation apiece instead of hundreds of call-site
// edits at that stage.
// ---------------------------------------------------------------------
class Context
{
public:
	Context(JSContext *cx) noexcept : _cx(cx) {}
	operator JSContext *() const noexcept { return _cx; }
	JSContext *raw() const noexcept { return _cx; }

	bool newNumberValue(double d, Value *outValue) const noexcept
	{
		jsval v;
		bool ok = JS_NewNumberValue(_cx, d, &v);
		*outValue = Value(v);
		return ok;
	}

	bool valueToNumber(Value v, double *outNumber) const noexcept
	{
		return JS_ValueToNumber(_cx, v.raw(), outNumber);
	}

	bool valueToBoolean(Value v, JSBool *outBool) const noexcept
	{
		return JS_ValueToBoolean(_cx, v.raw(), outBool);
	}

	bool setPrivate(JSObject *obj, void *data) const noexcept
	{
		return JS_SetPrivate(_cx, obj, data);
	}

	void *getPrivate(JSObject *obj) const noexcept
	{
		return JS_GetPrivate(_cx, obj);
	}

	void *getInstancePrivate(JSObject *obj, ClassDef *clasp) const noexcept
	{
		return JS_GetInstancePrivate(_cx, obj, clasp, nullptr);
	}

	bool instanceOf(JSObject *obj, ClassDef *clasp) const noexcept
	{
		return JS_InstanceOf(_cx, obj, clasp, nullptr);
	}

	JSObject *newObject(ClassDef *clasp, JSObject *proto = nullptr, JSObject *parent = nullptr) const noexcept
	{
		return JS_NewObject(_cx, clasp, proto, parent);
	}

	JSObject *initClass(JSObject *global, JSObject *parentProto, ClassDef *clasp,
						 Native constructor, uintN nargs,
						 PropertySpec *properties, FunctionSpec *methods,
						 PropertySpec *staticProperties, FunctionSpec *staticMethods) const noexcept
	{
		return JS_InitClass(_cx, global, parentProto, clasp, constructor, nargs,
							 properties, methods, staticProperties, staticMethods);
	}

private:
	JSContext *_cx;
};


} // namespace ooscript
