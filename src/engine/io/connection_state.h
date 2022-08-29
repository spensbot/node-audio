#pragma once

#include "io/device_info.h"

#include <vector>
#include <optional>

struct ConnectionState {
  std::vector<DeviceInfo> available;
  std::optional<DeviceInfo> connected;
};
