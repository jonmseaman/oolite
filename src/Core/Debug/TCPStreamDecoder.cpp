/*

OOTCPStreamDecoder.c


Copyright (C) 2007 Jens Ayton and contributors

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

#ifdef OO_DEBUG_MONITOR_ENABLED

#include "TCPStreamDecoder.h"

#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>

#include <cstdint>
#include <memory>
#include <new>
#include <utility>

#include "DebugTCPConsoleProtocol.h"

// The decoder spells the protocol's names (DebugTCPConsoleProtocol.h) as
// string handles; DebugTCPConsoleClient.cpp spells them as UTF-8 literals.
#define OOALSTR(string) oo::ALGetConstantString("" string "")

#ifdef OO_LOG_DEBUG_PROTOCOL_PACKETS
extern void LogOOTCPStreamDecoderPacket(oo::ALDictionaryRef packet);
#else
#define LogOOTCPStreamDecoderPacket(packet) \
  do {                                      \
  } while (0)
#endif

namespace oo {

struct TCPStreamDecoder {
  uint8_t header[4];
  uint32_t headerSpaceUsed;
  ALMutableDataRef nextPacketData;
  uint32_t nextSize;

  TCPStreamDecoderPacketCallback Packet;
  TCPStreamDecoderErrorCallback Error;
  TCPStreamDecoderFinalizeCallback Finalize;
};

}  // namespace oo

static void Error(oo::TCPStreamDecoderRef decoder, oo::ALStringRef format, ...);
static void PacketReady(oo::TCPStreamDecoderRef decoder);

namespace oo {

TCPStreamDecoderRef TCPStreamDecoderCreate(
    TCPStreamDecoderPacketCallback packetCB,
    TCPStreamDecoderErrorCallback errorCB,
    TCPStreamDecoderFinalizeCallback finalizeCB) {
  if (packetCB == nullptr) return nullptr;

  // The caller owns the handle until OOTCPStreamDecoderDestroy().
  TCPStreamDecoderRef decoder = new (std::nothrow) TCPStreamDecoder{};
  if (decoder == nullptr) return nullptr;

  decoder->headerSpaceUsed = 0;
  decoder->nextPacketData = nullptr;
  decoder->nextSize = 0;
  decoder->Packet = std::move(packetCB);
  decoder->Error = std::move(errorCB);
  decoder->Finalize = std::move(finalizeCB);

  return decoder;
}

void TCPStreamDecoderDestroy(TCPStreamDecoderRef decoder) {
  if (decoder == nullptr) return;
  const std::unique_ptr<TCPStreamDecoder> owned(decoder);  // deleted on return

  if (decoder->Finalize != nullptr) {
    decoder->Finalize();
  }

  if (decoder->nextPacketData != nullptr) {
    ALRelease(decoder->nextPacketData);
    decoder->nextPacketData = nullptr;
  }
}

void TCPStreamDecoderReceiveData(TCPStreamDecoderRef decoder, ALDataRef data) {
  if (decoder == nullptr || data == nullptr) return;

  TCPStreamDecoderReceiveBytes(decoder, ALDataGetBytePtr(data),
                               ALDataGetLength(data));
}

void TCPStreamDecoderReceiveBytes(TCPStreamDecoderRef decoder,
                                  const void* inBytes, size_t length) {
  const unsigned char* bytes = nullptr;
  size_t remaining;
  size_t bytesToAdd;
  ALAutoreleasePoolRef pool = nullptr;

  if (decoder == nullptr) return;

  bytes = static_cast<const unsigned char*>(inBytes);
  remaining = length;

  if (bytes == nullptr && remaining != 0) {
    Error(decoder, OOALSTR("Invalid data -- NULL bytes but %u byte count."),
          remaining);
    return;
  }

  while (remaining != 0) {
    if (decoder->nextPacketData != nullptr) {
      // More data expected
      bytesToAdd = remaining;
      if (decoder->nextSize < bytesToAdd) bytesToAdd = decoder->nextSize;

      ALMutableDataAppendBytes(decoder->nextPacketData, bytes, bytesToAdd);

      remaining -= bytesToAdd;
      decoder->nextSize -= bytesToAdd;
      bytes += bytesToAdd;

      if (decoder->nextSize == 0) {
        // Packet is ready.
        pool = ALCreateAutoreleasePool();
        PacketReady(decoder);
        ALDestroyAutoreleasePool(pool);
        pool = nullptr;

        ALRelease(decoder->nextPacketData);
        decoder->nextPacketData = nullptr;
      }
    } else if (decoder->headerSpaceUsed < 4) {
      // Read bytes for packet header
      remaining--;
      decoder->header[decoder->headerSpaceUsed++] = *bytes++;
    } else if (decoder->headerSpaceUsed == 4) {
      // We've read a header, start on a packet.
      decoder->nextSize = static_cast<uint32_t>(
          (decoder->header[0] << 24) | (decoder->header[1] << 16) |
          (decoder->header[2] << 8) | (decoder->header[3] << 0));

      decoder->headerSpaceUsed = 0;
      if (decoder->nextSize != 0) {
        decoder->nextPacketData = ALDataCreateMutable(decoder->nextSize);
      }
    } else {
      Error(decoder,
            OOALSTR(
                "OOTCPStreamDecoder internal error: reached unreachable state. "
                "nextSize = %zu, bufferUsed = %zu, nextPacketData = %@."),
            static_cast<std::uint64_t>(decoder->nextSize),
            static_cast<std::uint64_t>(decoder->headerSpaceUsed),
            decoder->nextPacketData);
    }
  }
}

}  // namespace oo

static void PacketReady(oo::TCPStreamDecoderRef decoder) {
  oo::ALDictionaryRef packet = nullptr;
  oo::ALStringRef errorString = nullptr;
  oo::ALStringRef packetType = nullptr;

  packet = oo::ALPropertyListFromData(decoder->nextPacketData, &errorString);

  // Ensure that it's a property list.
  if (packet == nullptr) {
    Error(decoder,
          OOALSTR("Protocol error: packet is not property list (property list "
                  "error: %@)."),
          errorString);
    oo::ALRelease(errorString);
    return;
  }

  // Ensure that it's a dictionary.
  if (!oo::ALIsDictionary(packet)) {
    Error(decoder, OOALSTR("Protocol error: packet is a %@, not a dictionary."),
          oo::TypeDescription(packet));
    return;
  }

  LogOOTCPStreamDecoderPacket(packet);

  // Get packet type (and ensure that there is one).
  packetType = oo::ALDictionaryGetValue(packet, kOOTCPPacketType);
  if (packetType == nullptr) {
    Error(decoder, OOALSTR("Protocol error: packet contains no packet type."));
    return;
  }

  if (!oo::ALIsString(packetType)) {
    Error(decoder,
          OOALSTR("Protocol error: packet type is a %@, not a string."),
          oo::TypeDescription(packetType));
    return;
  }

  decoder->Packet(packetType, packet);
}

static void Error(oo::TCPStreamDecoderRef decoder, oo::ALStringRef format,
                  ...) {
  va_list args;
  oo::ALStringRef string = nullptr;

  if (decoder == nullptr || decoder->Error == nullptr || format == nullptr)
    return;

  va_start(args, format);
  string = oo::ALStringCreateWithFormatAndArguments(format, args);
  va_end(args);

  if (string != nullptr) {
    decoder->Error(string);
    oo::ALRelease(string);
  }
}

#endif /* OO_DEBUG_MONITOR_ENABLED */
