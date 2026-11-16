/*	OOTCPStreamDecoderAbstractionLayer.cpp

        The oofnd implementation of OOTCPStreamDecoderAbstractionLayer.h: each
   handle is a struct OOALObject holding its value as an oo::PList (a string, a
   data buffer or the parsed packet).
*/

#ifdef OO_DEBUG_MONITOR_ENABLED

#include "TCPStreamDecoderAbstractionLayer.h"

#include <cstddef>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "Foundation.h"
#include "TCPStreamDecoderFormat.hpp"
#include "oofnd/AutoreleaseScope.hpp"
#include "oofnd/PListParsing.hpp"

namespace oo {

struct ALObject {
  oo::PList value;
  // Borrowed handles given out for this object: its dictionary values and its
  // type description.
  std::map<std::string, std::unique_ptr<ALObject>, std::less<>> children;
  std::unique_ptr<ALObject> typeDescription;
};

struct ALAutoreleasePool {
  std::vector<std::unique_ptr<ALObject>> objects;
  // Declared last so it drains first, before the objects are freed.
  oo::AutoreleaseScope deferred;
};

}  // namespace oo

namespace {

// The innermost pools, innermost last (per thread, as autorelease pools are).
thread_local std::vector<oo::ALAutoreleasePool*> sPools;

oo::ALObject* NewObject(oo::PList value) {
  auto* object = new oo::ALObject;
  object->value = std::move(value);
  return object;
}

// "%@" of a handle: what GNUstep printed for the object it stood for.
std::string DescriptionOf(oo::ALObjectRef object) {
  if (object == nullptr) return "(null)";
  if (const std::string* string = object->value.getIf<std::string>())
    return *string;
  return oo::DescriptionOf(object->value);
}

}  // namespace

// Simulate literal CF/NS strings. Each literal string that is used becomes a
// single object. Since it uses pointers as keys, it should only be used with
// literals. Called from OOTCPStreamDecoder.c through OOALSTR(), so it keeps C
// linkage.
namespace oo {

ALStringRef ALGetConstantString(const char* string);

ALStringRef ALGetConstantString(const char* string) {
  // Never destroyed (Google style: no non-trivial static destructors).
  static auto& sStrings = *new std::map<const char*, std::unique_ptr<ALObject>>;

  std::unique_ptr<ALObject>& value = sStrings[string];
  if (value == nullptr) {
    // Note: non-ASCII strings are not permitted, but we don't bother to detect
    // them.
    value.reset(NewObject(oo::PList(std::string(string))));
  }

  return value.get();
}

void ALRelease(ALObjectRef object) { delete object; }

ALStringRef TypeDescription(ALObjectRef object) {
  // What the handle holds: "(null)" for nothing, else the property-list type's
  // name ("string", "array", ...).
  auto* mutableObject = const_cast<ALObject*>(object);
  if (mutableObject->typeDescription == nullptr) {
    const std::string name =
        object->value.isNull()
            ? std::string("(null)")
            : std::string(oo::typeName(object->value.type()));
    mutableObject->typeDescription.reset(NewObject(oo::PList(name)));
  }
  return mutableObject->typeDescription.get();
}

bool ALIsString(ALObjectRef object) { return object->value.isString(); }

ALStringRef ALStringCreateWithFormatAndArguments(ALStringRef format,
                                                 va_list args) {
  return NewObject(
      oo::PList(OOTCPStreamDecoderFormat::FormatWithArguments<ALObjectRef>(
          ::DescriptionOf(format), args, ::DescriptionOf)));
}

bool ALIsDictionary(ALObjectRef object) { return object->value.isDict(); }

ALObjectRef ALDictionaryGetValue(ALDictionaryRef dictionary, ALObjectRef key) {
  const std::string* keyString = key->value.getIf<std::string>();
  if (keyString == nullptr) return nullptr;
  const oo::PList* value = dictionary->value.find(*keyString);
  if (value == nullptr) return nullptr;

  auto* parent = const_cast<ALObject*>(dictionary);
  std::unique_ptr<ALObject>& child = parent->children[*keyString];
  if (child == nullptr) child.reset(NewObject(*value));
  return child.get();
}

bool ALIsData(ALObjectRef object) { return object->value.isData(); }

ALMutableDataRef ALDataCreateMutable(size_t capacity) {
  (void)capacity;
  return NewObject(oo::PList(oo::Data()));
}

void ALMutableDataAppendBytes(ALMutableDataRef data, const void* bytes,
                              size_t length) {
  data->value.getIf<oo::PList::Data>()->append(bytes, length);
}

const void* ALDataGetBytePtr(ALDataRef data) {
  return data->value.getIf<oo::PList::Data>()->bytes();
}

size_t ALDataGetLength(ALDataRef data) {
  return data->value.getIf<oo::PList::Data>()->length();
}

ALAutoreleasePoolRef ALCreateAutoreleasePool() {
  auto* pool = new ALAutoreleasePool;
  sPools.push_back(pool);
  return pool;
}

void ALDestroyAutoreleasePool(ALAutoreleasePoolRef pool) {
  if (!sPools.empty() && sPools.back() == pool) sPools.pop_back();
  delete pool;
}

ALObjectRef ALPropertyListFromData(ALMutableDataRef data, ALStringRef* errStr) {
  const oo::Data& bytes = *data->value.getIf<oo::PList::Data>();
  std::expected<oo::PList, oo::PListError> parsed =
      oo::parsePropertyList(std::string_view(
          reinterpret_cast<const char*>(bytes.bytes()), bytes.length()));
  if (!parsed || parsed->isNull()) {
    // The error string is owned by the caller (+1); no error text reads as nil
    // did.
    if (errStr != nullptr)
      *errStr =
          parsed ? nullptr : NewObject(oo::PList(parsed.error().description()));
    return nullptr;
  }

  ALObject* result = NewObject(std::move(*parsed));
  // Belongs to the innermost pool (the decoder never releases a packet).
  if (!sPools.empty()) sPools.back()->objects.emplace_back(result);
  return result;
}

const oo::PList& ALObjectPList(ALObjectRef object) { return object->value; }

}  // namespace oo

#endif /* OO_DEBUG_MONITOR_ENABLED */
