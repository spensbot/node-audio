#pragma once

#include "miniaudio.h"
#include "io_manager.h"

#include <string>
#include <vector>
#include <optional>
#include <iostream>

struct ConnectionState {
  std::vector<DeviceInfo> available;
  std::optional<DeviceInfo> connected;
};

struct SessionState {
  float bpm;
  float beats;
};

class AudioEngine: public IoListener {
  public: 
    AudioEngine(): _ioManager(IoManager::New(*this)) {}

    SessionState getSessionState() {
      return SessionState {
        0.0,
        0.0
      };
    }

    ConnectionState getConnectionState() {
      const auto available_inputs = _ioManager ? _ioManager->list_inputs() : std::vector<DeviceInfo> {};
      return ConnectionState {
        available_inputs,
        std::nullopt,
      };
    }

    void connect(std::string audioPortId) {
      if (_ioManager) _ioManager->connect_input();
    }

    void audio_callback(float* input, float* output, uint32_t frame_count) override {
      std::cout << "audio_callback() !!!" << std::endl;
    }
  
  private:
    std::optional<IoManager> _ioManager;
    // Where the good stuff'll go
};