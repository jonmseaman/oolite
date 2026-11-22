/*

OODebugTCPConsoleClient.h


Oolite Debug Support

Copyright (C) 2007-2013 Jens Ayton

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

#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <utility>

#include "DebuggerInterface.h"
#include "Foundation.h"
#include "TCPStreamDecoderAbstractionLayer.h"
#include "oofnd/PList.hpp"

#if OOLITE_WINDOWS
struct fd_set;  // winsock2.h's, which the .cpp includes
#else
#include <sys/select.h>
#endif

namespace oo {

struct TCPStreamDecoder;

enum class TCPClientConnectionStatus : int {
  kNotConnected,
  kStartedConnectionStage1,
  kStartedConnectionStage2,
  kConnected,
  kConnectionRefused,
  kDisconnected,
};

}  // namespace oo

static_assert(std::to_underlying(oo::TCPClientConnectionStatus::kNotConnected) == 0);
static_assert(std::to_underlying(oo::TCPClientConnectionStatus::kStartedConnectionStage1) == 1);
static_assert(std::to_underlying(oo::TCPClientConnectionStatus::kStartedConnectionStage2) == 2);
static_assert(std::to_underlying(oo::TCPClientConnectionStatus::kConnected) == 3);
static_assert(std::to_underlying(oo::TCPClientConnectionStatus::kConnectionRefused) == 4);
static_assert(std::to_underlying(oo::TCPClientConnectionStatus::kDisconnected) == 5);

/*	The frame loop's side of the console socket: what the run loop did for
   the console's streams. OODebugTCPConsoleIsWaitingForInput() is true while a
   console socket can deliver something (input, end of stream or an error);
   OODebugTCPConsoleServiceInput() waits up to timeout seconds (< 0: no limit)
   for that, then handles what arrived, as one run-loop wait did.
*/
namespace oo {

bool DebugTCPConsoleIsWaitingForInput();
void DebugTCPConsoleServiceInput(double timeout);

class DebugMonitor;

/*	The connection is one non-blocking TCP socket, where it was a
        Foundation host lookup and an input/output stream pair on the run loop.
   _inStatus/_outStatus keep the two streams' status numbers (Foundation's
   stream-status values), which the connection-failure messages print;
   _pendingErrorCode is an error event the run loop would still have delivered
   to the stream delegate.

        The debug monitor reaches the client through the C++
   OODebuggerInterface, which the client implements.
*/
class DebugTCPConsoleClient : public DebuggerInterface {
 public:
  // [[OODebugTCPConsoleClient alloc] initWithAddress:port:]; null when the
  // connection fails.
  static std::shared_ptr<DebugTCPConsoleClient> clientWithAddress(
      const std::optional<std::string>& address,  // Pass nullopt for localhost
      uint16_t port);                             // Pass 0 for default port

  ~DebugTCPConsoleClient() override;

  // The debugger interface (OODebuggerInterface).
  bool connectDebugMonitor(DebugMonitor* debugMonitor,
                           std::optional<std::string>* message) override;
  void disconnectDebugMonitor(DebugMonitor* debugMonitor,
                              const std::optional<std::string>& message) override;
  void debugMonitor(DebugMonitor* debugMonitor, const std::string& output,
                    const std::optional<std::string>& colorKey, NSRange emphasisRange) override;
  void debugMonitorClearConsole(DebugMonitor* debugMonitor) override;
  void debugMonitorShowConsole(DebugMonitor* debugMonitor) override;
  void debugMonitor(DebugMonitor* debugMonitor, const oo::PList& configuration) override;
  void debugMonitor(DebugMonitor* debugMonitor, const oo::PList& newValue,
                    const std::string& key) override;
  std::string description() const override;  // <OODebugTCPConsoleClient 0x...>

 private:
  // The frame loop's functions below read the clients' sockets.
  friend bool ::oo::DebugTCPConsoleIsWaitingForInput();
  friend void ::oo::DebugTCPConsoleServiceInput(double timeout);

  DebugTCPConsoleClient() = default;
  bool initWithAddress(const std::optional<std::string>& address, uint16_t port);

  // The decoder's callbacks; its string and dictionary handles are
  // OOALObjectRef.
  void DecoderPacket(ALObjectRef packetType, ALObjectRef packet);
  void DecoderError(ALObjectRef errorDesc);

  void closeConnection();

  bool sendBytes(std::span<const std::byte> bytes);
  void sendDictionary(const oo::PList& dictionary);

  // Packet type and parameter names are OODebugTCPConsoleProtocol.h's constants
  // (ProtocolName()).
  void sendPacket(const std::string& packetType,
                  const oo::PList& parameters);  // a dictionary; null: the type alone

  void sendPacket(const std::string& packetType, const oo::PList& value,
                  const std::string& paramKey);  // a null value or empty key: the type alone

  bool openSocketToHost(const std::string& address, uint16_t port);
  std::ptrdiff_t receive(std::span<uint8_t> buffer);
  bool isWaitingForInput();
  bool hasPendingErrorEvent();
  uintptr_t socket();
  void addToWaitSets(fd_set* readSet, fd_set* writeSet, fd_set* exceptSet);
  void handleWaitResultRead(bool readable, bool writable, bool exceptional);
  void handleErrorEvent();
  void serviceSocketFor(double seconds);

  void readData();
  void dispatchPacket(const oo::PList& packet, const std::string& packetType);

  // Packets are property-list dictionaries (anything else answers no values).
  void handleApproveConnectionPacket(const oo::PList& packet);
  void handleRejectConnectionPacket(const oo::PList& packet);
  void handleCloseConnectionPacket(const oo::PList& packet);
  void handleNoteConfigurationChangePacket(const oo::PList& packet);
  void handlePerformCommandPacket(const oo::PList& packet);
  void handleRequestConfigurationValuePacket(const oo::PList& packet);
  void handlePingPacket(const oo::PList& packet);
  void handlePongPacket(const oo::PList& packet);

  void disconnectFromServerWithMessage(
      const std::optional<std::string>& message);  // nullopt: a close packet without a message
  void breakConnectionWithMessage(const std::string& message);
  void breakConnectionWithStreamError(int error);

  std::optional<std::string> _hostName;  // was the host object; nullopt when closed
  uintptr_t _socket = {};                // SOCKET / file descriptor; all ones when closed
  int _inStatus = {}, _outStatus = {};
  int _inError = {}, _outError = {};
  int _pendingErrorCode = {};
  bool _errorEventPending = {};
  TCPClientConnectionStatus _status = {};
  DebugMonitor* _monitor = {};
  ::oo::TCPStreamDecoder* _decoder = {};
};

}  // namespace oo
