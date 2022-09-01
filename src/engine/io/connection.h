#pragma once 

#include "io/device_info.h"

#include <iostream>
#include <optional>

class IoListener {
public:
  // CALLED BY AUDIO THREAD!!! DON'T YOU DARE BLOCK IN HERE. 
  //    NO MEMORY MANAGEMENT
  //    NO LOCKS
  //    NO IO
  virtual void audio_callback(float* input, float* output, uint32_t frame_count) = 0;
};

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
  IoListener* ioListener = reinterpret_cast<IoListener*>(pDevice->pUserData);
  ioListener->audio_callback((float*)pInput, (float*)pOutput, frameCount);
}

struct ConnectionInfo {
  ConnectionInfo(ma_device& device, DeviceInfo& deviceInfo)
  : channelCount(device.capture.channels)
  , sampleRate(device.sampleRate)
  , deviceInfo(deviceInfo) {}

  uint32_t channelCount, sampleRate;
  DeviceInfo deviceInfo;
};

class Connection {
private:
  struct Ptrs {
    Ptrs(DeviceInfo deviceInfo, IoListener& ioListener): deviceInfo(deviceInfo) {
      config = ma_device_config_init(ma_device_type_capture);
      config.capture.format = ma_format_f32;
      config.capture.shareMode = ma_share_mode_shared;
      config.dataCallback = data_callback;
      config.capture.pDeviceID = &deviceInfo.ma_id;
      config.pUserData = &ioListener; // Store listener ptr in pUserPtrs
    }
    DeviceInfo deviceInfo;
    ma_device_config config;
    ma_device device;
  };
    
public:
  Connection(std::unique_ptr<Ptrs> ptrs): _ptrs(std::move(ptrs)) {}
  ~Connection() { 
    ma_device_uninit(&_ptrs->device);
  }

  static std::unique_ptr<Connection> New(ma_context& ma_context, DeviceInfo deviceInfo, IoListener& ioListener) {
    auto ptrs = std::make_unique<Ptrs>(deviceInfo, ioListener);

    if (ma_device_init(&ma_context, &ptrs->config, &ptrs->device) == MA_SUCCESS) {
      return std::make_unique<Connection>(std::move(ptrs));
    } else {
      std::cout << "ma_device_init() failed" << std::endl;
    }
    
    return nullptr;
  }

  bool start() {
    return ma_device_start(&_ptrs->device) == MA_SUCCESS;
  }

  ConnectionInfo info() {
    return ConnectionInfo(_ptrs->device, _ptrs->deviceInfo);
  }
  
private:
  std::unique_ptr<Ptrs> _ptrs;
};
