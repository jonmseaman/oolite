/*

OODisplaySizeWindows.cpp

OOQueryDisplayPhysicalSize on Windows: the monitor's own EDID, read from its
device registry key (spec 008 research R7). The SDL display is matched to its
monitor through the monitor's device interface path, which
EnumDisplayDevices(EDD_GET_DEVICE_INTERFACE_NAME) and SetupAPI both report.

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

#include "OODisplaySize.h"

// clang-format off
#include <windows.h>
#include <setupapi.h>
// clang-format on

#include <SDL3/SDL.h>

#include <cstdint>
#include <string>
#include <vector>

namespace {

// GUID_DEVINTERFACE_MONITOR (ntddvdeo.h), spelled out so no INITGUID dance is
// needed in this one file.
const GUID kMonitorInterface = {
    0xe6f07b5f,
    0xee97,
    0x4a90,
    {0xb0, 0x76, 0x33, 0xf5, 0x7b, 0xf4, 0xea, 0xa7}};

// The EDID's image size: the first detailed timing descriptor in millimetres,
// else the basic display parameters in centimetres.
std::optional<std::pair<double, double>> SizeFromEdid(
    const std::vector<std::uint8_t>& edid) {
  if (edid.size() < 128) return std::nullopt;
  if (edid[54] != 0 || edid[55] != 0) {  // a timing, not a display descriptor
    const int width = edid[66] | ((edid[68] & 0xF0) << 4);
    const int height = edid[67] | ((edid[68] & 0x0F) << 8);
    if (width > 0 && height > 0) {
      return std::make_pair(static_cast<double>(width),
                            static_cast<double>(height));
    }
  }
  if (edid[21] > 0 && edid[22] > 0) {
    return std::make_pair(edid[21] * 10.0, edid[22] * 10.0);
  }
  return std::nullopt;
}

// The device interface path of the monitor showing the SDL display.
std::optional<std::wstring> MonitorInterfacePath(int sdlDisplayIndex) {
  SDL_Rect bounds;
  if (!SDL_GetDisplayBounds(static_cast<SDL_DisplayID>(sdlDisplayIndex),
                            &bounds)) {
    return std::nullopt;
  }
  const POINT centre = {bounds.x + bounds.w / 2, bounds.y + bounds.h / 2};
  HMONITOR monitor = MonitorFromPoint(centre, MONITOR_DEFAULTTONULL);
  if (monitor == nullptr) return std::nullopt;
  MONITORINFOEXW info = {};
  info.cbSize = sizeof info;
  if (!GetMonitorInfoW(monitor, &info)) return std::nullopt;
  DISPLAY_DEVICEW device = {};
  device.cb = sizeof device;
  if (!EnumDisplayDevicesW(info.szDevice, 0, &device,
                           EDD_GET_DEVICE_INTERFACE_NAME)) {
    return std::nullopt;
  }
  return std::wstring(device.DeviceID);
}

std::optional<std::vector<std::uint8_t>> EdidForInterface(
    const std::wstring& path) {
  HDEVINFO set = SetupDiGetClassDevsW(&kMonitorInterface, nullptr, nullptr,
                                      DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);
  if (set == INVALID_HANDLE_VALUE) return std::nullopt;
  std::optional<std::vector<std::uint8_t>> result;
  SP_DEVICE_INTERFACE_DATA interfaceData = {};
  interfaceData.cbSize = sizeof interfaceData;
  for (DWORD i = 0;
       !result && SetupDiEnumDeviceInterfaces(set, nullptr, &kMonitorInterface,
                                              i, &interfaceData);
       i++) {
    DWORD needed = 0;
    SetupDiGetDeviceInterfaceDetailW(set, &interfaceData, nullptr, 0, &needed,
                                     nullptr);
    if (needed < sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_W)) continue;
    std::vector<std::uint8_t> buffer(needed);
    auto* detail =
        reinterpret_cast<SP_DEVICE_INTERFACE_DETAIL_DATA_W*>(buffer.data());
    detail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_W);
    SP_DEVINFO_DATA deviceData = {};
    deviceData.cbSize = sizeof deviceData;
    if (!SetupDiGetDeviceInterfaceDetailW(set, &interfaceData, detail, needed,
                                          nullptr, &deviceData)) {
      continue;
    }
    if (_wcsicmp(detail->DevicePath, path.c_str()) != 0) continue;

    HKEY key = SetupDiOpenDevRegKey(set, &deviceData, DICS_FLAG_GLOBAL, 0,
                                    DIREG_DEV, KEY_READ);
    if (key == INVALID_HANDLE_VALUE) continue;
    DWORD size = 0;
    if (RegQueryValueExW(key, L"EDID", nullptr, nullptr, nullptr, &size) ==
            ERROR_SUCCESS &&
        size > 0) {
      std::vector<std::uint8_t> edid(size);
      if (RegQueryValueExW(key, L"EDID", nullptr, nullptr, edid.data(),
                           &size) == ERROR_SUCCESS) {
        edid.resize(size);
        result = std::move(edid);
      }
    }
    RegCloseKey(key);
  }
  SetupDiDestroyDeviceInfoList(set);
  return result;
}

}  // namespace

std::optional<std::pair<double, double>> OOQueryDisplayPhysicalSize(
    int sdlDisplayIndex) {
  const std::optional<std::wstring> path =
      MonitorInterfacePath(sdlDisplayIndex);
  if (!path) return std::nullopt;
  const std::optional<std::vector<std::uint8_t>> edid = EdidForInterface(*path);
  if (!edid) return std::nullopt;
  return SizeFromEdid(*edid);
}
