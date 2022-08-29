#pragma once 

#include <miniaudio.h>
#include <string>
#include <optional>

struct DeviceInfo {
  DeviceInfo(ma_device_info& info) {
    id = ma_id_to_string(info.id);
    name = std::string(info.name);
    is_default = info.isDefault;
    ma_id = info.id;
  }

  std::string id;
  std::string name;
  bool is_default;
  ma_device_id ma_id;

private:
  std::string ma_id_to_string(ma_device_id ma_id) {
    const char* data = reinterpret_cast<const char*>(&ma_id);
    const size_t bytes = sizeof(ma_id);
    return std::string(data, bytes);
  }
};
