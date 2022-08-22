#pragma once

#include "io/io_manager.h"

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
      const auto connected = _ioManager ? _ioManager->connected() : std::nullopt;
      return ConnectionState {
        available_inputs,
        connected,
      };
    }

    void connect(std::string audioPortId) {
      auto state = getConnectionState();
      if (state.available.size() > 0 && !state.connected) {
        if (_ioManager) _ioManager->connect_input(state.available[0]);  
      }
    }

    // AUDIO THREAD. Realtime-safe code only
    void audio_callback(float* input, float* output, uint32_t frame_count) override {
      std::cout << "audio_callback()" << std::endl;
    }
  
  private:
    // Where the good stuff'll go
    std::unique_ptr<IoManager> _ioManager;
};