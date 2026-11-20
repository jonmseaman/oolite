/*	OOTCPStreamDecoderAbstractionLayer.h

The handle types and functions OOTCPStreamDecoder works through: strings, data
buffers and property lists, implemented over oofnd.
*/

#pragma once

#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>

#include <string>

/*	The handles are opaque in C and C++ alike: a struct OOALObject, defined
   in OOTCPStreamDecoderAbstractionLayer.cpp, holding a string, a data buffer or
   a property list (an oo::PList). Handles made by a Create/FromData function
   are owned (+1) and freed by OOALRelease(); OOALDictionaryGetValue(),
   OOTypeDescription() and OOALSTR() answer borrowed handles that live as long
   as their parent (or for ever, for OOALSTR). A handle from
   OOALPropertyListFromData() belongs to the innermost
        OOALCreateAutoreleasePool() and is freed when it is destroyed.
*/
namespace oo {

struct ALObject;
using ALObjectRef = ALObject*;

using ALStringRef = ALObject*;
using ALDataRef = ALObject*;
using ALMutableDataRef = ALObject*;
using ALDictionaryRef = ALObject*;
struct ALAutoreleasePool;
using ALAutoreleasePoolRef = ALAutoreleasePool*;

ALStringRef ALGetConstantString(const char* string);

}  // namespace oo
   // Should only be used with string literals!

namespace oo {

void ALRelease(ALObjectRef object);
ALStringRef TypeDescription(ALObjectRef object);

bool ALIsString(ALObjectRef object);
ALStringRef ALStringCreateWithFormatAndArguments(ALStringRef format,
                                                 va_list args);

bool ALIsDictionary(ALObjectRef object);
ALObjectRef ALDictionaryGetValue(ALDictionaryRef dictionary, ALObjectRef key);

bool ALIsData(ALObjectRef object);
ALMutableDataRef ALDataCreateMutable(size_t capacity);
void ALMutableDataAppendBytes(ALMutableDataRef data, const void* bytes,
                              size_t length);
const void* ALDataGetBytePtr(ALDataRef data);
size_t ALDataGetLength(ALDataRef data);

ALAutoreleasePoolRef ALCreateAutoreleasePool();
void ALDestroyAutoreleasePool(ALAutoreleasePoolRef pool);

ALObjectRef ALPropertyListFromData(ALMutableDataRef data, ALStringRef* errStr);

}  // namespace oo

#include "oofnd/PList.hpp"

// The value a handle holds, as a property list (a string or data handle as a
// string or data node).
namespace oo {

const oo::PList& ALObjectPList(ALObjectRef object);

}  // namespace oo
