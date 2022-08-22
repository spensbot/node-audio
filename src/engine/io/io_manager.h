#pragma once

#include "io/device_info.h"
#include "io/connection.h"

#include <miniaudio.h>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>
#include <iostream>

class IoManager {
private:
  struct Data {
    ma_context context;
  };
public:
  IoManager(std::unique_ptr<Data> data): _data(std::move(data)) {};
  
  static std::unique_ptr<IoManager> New() {
    auto data = std::make_unique<Data>();
    if (ma_context_init(NULL, 0, NULL, &data->context) != MA_SUCCESS) {
      std::cout << "ma_context_init() failed" << std::endl;
      return nullptr;
    }
    return std::make_unique<IoManager>(std::move(data));
  }

  std::vector<DeviceInfo> list_inputs() {
    ma_device_info* pPlaybackDeviceInfos;
    ma_uint32 playbackDeviceCount;
    ma_device_info* pCaptureDeviceInfos;
    ma_uint32 captureDeviceCount;

    auto res = ma_context_get_devices(&_data->context, &pPlaybackDeviceInfos, &playbackDeviceCount, &pCaptureDeviceInfos, &captureDeviceCount);
    if (res != MA_SUCCESS) {
      std::cout << "ma_context_get_devices() failed" << std::endl;
      return {};
    }

    std::vector<DeviceInfo> inputs;
    inputs.reserve(captureDeviceCount);
    for (uint32_t i=0; i<captureDeviceCount; ++i) {
      inputs.push_back(DeviceInfo(pCaptureDeviceInfos[i]));
    }
    return inputs;
  }

  std::optional<DeviceInfo> connected() {
    if (_input_connection) {
      return _input_connection->info();
    } else {
      return std::nullopt;
    }
  }

  void connect_input(DeviceInfo info) {
    _input_connection = Connection::New(info);
  }

private:
  std::unique_ptr<Data> _data;
  std::unique_ptr<Connection> _input_connection;
};