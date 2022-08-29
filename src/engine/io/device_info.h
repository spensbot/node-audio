#pragma once 

#include <miniaudio.h>
#include <string>

struct DeviceInfo {
  DeviceInfo(ma_device_info& info) {
    isDefault = info.isDefault;
    id = std::string(info.name);
    name = std::string(info.name);
  }

  std::string id;
  bool isDefault;
  std::string name;

private:
  std::string ma_id_to_string(ma_device_id ma_id) {
    const char* data = reinterpret_cast<const char*>(&ma_id);
    const size_t bytes = sizeof(ma_id);
    return std::string(data, bytes);
  }
};
