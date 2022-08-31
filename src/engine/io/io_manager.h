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
  struct Ptrs {
    ma_context context;
  };
public:
  IoManager(IoListener& ioListener, std::unique_ptr<Ptrs> ptrs)
  : _ioListener(ioListener)
  , _ptrs(std::move(ptrs)) 
  {}

  static std::unique_ptr<IoManager> New(IoListener& ioListener) {
    auto ptrs = std::make_unique<Ptrs>();
    if (ma_context_init(NULL, 0, NULL, &ptrs->context) != MA_SUCCESS) {
      std::cout << "ma_context_init() failed" << std::endl;
      return nullptr;
    }
    return std::make_unique<IoManager>(ioListener, std::move(ptrs));
  }

  std::vector<DeviceInfo> list_inputs() {
    ma_device_info* pPlaybackDeviceInfos;
    ma_uint32 playbackDeviceCount;
    ma_device_info* pCaptureDeviceInfos;
    ma_uint32 captureDeviceCount;

    auto res = ma_context_get_devices(&_ptrs->context, &pPlaybackDeviceInfos, &playbackDeviceCount, &pCaptureDeviceInfos, &captureDeviceCount);
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
      return _input_connection->info().deviceInfo;
    } else {
      return std::nullopt;
    }
  }

  std::optional<ConnectionInfo> connect_input(std::optional<std::string> device_id) {
    if (device_id) {
      for (const auto& input: list_inputs()) {
        if (input.id == device_id) {
          _input_connection = Connection::New(input, _ioListener);
          if (_input_connection) return _input_connection->info();
        }
      }
    } else {
      for (const auto& input: list_inputs()) {
        if (input.is_default) {
          _input_connection = Connection::New(input, _ioListener);
          if (_input_connection) return _input_connection->info();
        }
      }
    }
    return std::nullopt;
  }

  void start_input() {
    if (_input_connection) {
      if (!_input_connection->start()) {
        std::cout << "io_manager.start_input() failed" << std::endl;
      }
    }
  }

private:
  IoListener& _ioListener;
  std::unique_ptr<Ptrs> _ptrs;
  std::unique_ptr<Connection> _input_connection;
};