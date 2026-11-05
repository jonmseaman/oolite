/*

OOLogStub.cpp (QuickJS-ng proof of concept)

Logging for the harness: every class is shown, with its name, and each line
goes to stdout and is recorded, so the tests can check what was logged. The
game sets up oo::log from logcontrol.plist (OOLoggingInit()); the harness has
none of that.

Oolite
Copyright (C) 2004-2013 Giles C Williams and contributors

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

#include "OOLogStub.h"

#include <cstdio>

#include "oofnd/Log.hpp"

namespace {
std::vector<std::string> sRecordedMessages;

void RecordingSink(std::string_view line) {
  sRecordedMessages.emplace_back(line);
  std::fwrite(line.data(), 1, line.size(), stdout);
  std::fputc('\n', stdout);
  std::fflush(stdout);
}
}  // namespace

void OOPocInstallLogging() {
  oo::log::Logger& logger = oo::log::logger();
  oo::log::Options options;
  options.showClass = true;  // "[class]: message", as the tests expect
  logger.setOptions(options);
  logger.setSink(RecordingSink);
  logger.setInitialized(true);
}

const std::vector<std::string>& OOPocRecordedLogMessages() {
  return sRecordedMessages;
}

void OOPocClearRecordedLogMessages() { sRecordedMessages.clear(); }
