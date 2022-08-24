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

// I don't like this, but I'm not sure how else to have data_callback reference something
static IoListener* _active_listener;

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
  // std::cout << "channels: " << pDevice->capture.channels
  // << " | format: " << pDevice->capture.format
  // << " | id: " << (char*)(&pDevice->capture.id)
  // << " | name: " << pDevice->capture.name
  // << " | bytes_per_frame: " << ma_get_bytes_per_frame(pDevice->capture.format, pDevice->capture.channels)
  // << " | " << pInput << std::endl;

  // float* fInput = (float*)pInput;
  // for (uint32_t i=0 ; i<frameCount ; i++) {
  //   std::cout << ", " << fInput[i];
  // }
  // std::cout << "Sample Rate: " << pDevice->sampleRate << " | buffer: " << frameCount;
  _active_listener->audio_callback((float*)pInput, (float*)pOutput, frameCount);
}

class Connection {
private:
  struct Data {
    Data(DeviceInfo info): info(info) {
      config = ma_device_config_init(ma_device_type_capture);
      config.capture.format = ma_format_f32;
      // config.capture.channels = 1;
      // config.capture.pDeviceID = &info.og_id;
      config.capture.shareMode = ma_share_mode_shared;
      config.dataCallback      = data_callback;
    }
    DeviceInfo info;
    ma_device_config config;
    ma_device device;
  };
    
public:
  Connection(std::unique_ptr<Data> data): _data(std::move(data)) {
    std::cout << "Connection()" << std::endl;
  }
  ~Connection() { 
    ma_device_uninit(&_data->device);
    std::cout << "~Connection()" << std::endl; 
  }

  static std::unique_ptr<Connection> New(DeviceInfo info) {
    auto data = std::make_unique<Data>(info);

    std::cout << "Sample Rate: " << data->device.sampleRate << std::endl;

    if (ma_device_init(NULL, &data->config, &data->device) == MA_SUCCESS) {
      std::cout << "Sample Rate 2: " << data->device.sampleRate << std::endl;
      std::cout << "ma_device_init()" << std::endl;
      if (ma_device_start(&data->device) == MA_SUCCESS) {
        std::cout << "ma_device_start()" << std::endl;
        return std::make_unique<Connection>(std::move(data));
      } else {
        std::cout << "ma_device_start() failed" << std::endl;
      }
    } else {
      std::cout << "ma_device_init() failed" << std::endl;
    }
    
    return nullptr;
  }

  DeviceInfo info() {
    return _data->info;
  }
  
private:

  std::unique_ptr<Data> _data;
};
