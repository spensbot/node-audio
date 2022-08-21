#pragma once

#include "miniaudio.h"
#include <aubio.h>
// #include <portaudio.h>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>
#include <iostream>

class IoListener {
public:
  virtual void audio_callback(float* input, float* output, uint32_t frame_count) = 0;
};

// I don't like this, but I'm not sure how else to have data_callback reference something
static IoListener* active_listener;

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
  std::cout << "data_callback()" << std::endl;
  // active_listener->audio_callback((float*)pInput, (float*)pOutput, frameCount);
}

class Connection {
public:
  static std::optional<Connection> New() {
    std::cout << "Connection::New()" << std::endl;
    ma_device_config config  = ma_device_config_init(ma_device_type_capture);
    config.capture.format    = ma_format_f32;   // Set to ma_format_unknown to use the device's native format.
    config.capture.channels  = 0;               // Set to 0 to use the device's native channel count.
    config.sampleRate        = 0;               // Set to 0 to use the device's native sample rate.
    config.dataCallback      = data_callback;   // This function will be called when miniaudio needs more data.
    // config.capture.pDeviceID = &info.og_id;
    ma_device device;

    if (ma_device_init(NULL, &config, &device) == MA_SUCCESS) {
      std::cout << "ma_device_init()" << std::endl;
      if (ma_device_start(&device) == MA_SUCCESS) {
        std::cout << "ma_device_start()" << std::endl;
        return Connection(device);
      } else {
        std::cout << "ma_device_start() failed" << std::endl;
      }
    } else {
      std::cout << "ma_device_init() failed" << std::endl;
    }
    
    return std::nullopt;
  }

  ~Connection() {
    std::cout << "~Connection()" << std::endl;
    // ma_device_uninit(&_device);
  }
private:
  Connection(ma_device device): _device(device) {}

  ma_device _device;
};

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

class IoManager {
public:
  static std::optional<IoManager> New(IoListener& listener) {
    std::cout << "IoManager::New()" << std::endl;
    active_listener = &listener;
    ma_context context;
    if (ma_context_init(NULL, 0, NULL, &context) != MA_SUCCESS) {
      std::cout << "ma_context_init() failed" << std::endl;
      return std::nullopt;
    }
    return IoManager(context);
  }

  std::vector<DeviceInfo> list_inputs() {
    ma_device_info* pPlaybackDeviceInfos;
    ma_uint32 playbackDeviceCount;
    ma_device_info* pCaptureDeviceInfos;
    ma_uint32 captureDeviceCount;

    auto res = ma_context_get_devices(&_context, &pPlaybackDeviceInfos, &playbackDeviceCount, &pCaptureDeviceInfos, &captureDeviceCount);
    if (res != MA_SUCCESS) {
      std::cout << "ma_context_get_devices() failed" << std::endl;
      return {};
    }

    std::cout << "captureDeviceCount: " << captureDeviceCount << std::endl;
    std::vector<DeviceInfo> inputs;
    inputs.reserve(captureDeviceCount);
    for (uint32_t i=0; i<captureDeviceCount; ++i) {
      inputs.push_back(DeviceInfo(pCaptureDeviceInfos[i]));
    }
    return inputs;
  }

  void connect_input() {
    _input_connection = Connection::New();
  }

private:
  IoManager(ma_context context): _context(context) {}
  ma_context _context;
  std::optional<Connection> _input_connection;
};