#pragma once 

#include <miniaudio.h>
#include <string>

struct DeviceInfo {
  DeviceInfo(ma_device_info& info) {
    is_default = info.isDefault;
    id = std::string(info.name);
    name = std::string(info.name);
    og_id = info.id;
  }

  std::string id;
  bool is_default;
  std::string name;
  ma_device_id og_id;
};
