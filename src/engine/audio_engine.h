#pragma once

#include "io/io_manager.h"
#include "session_state.h"
#include "dsp/dsp_manager.h"

#include <string>
#include <vector>
#include <optional>
#include <iostream>

struct ConnectionState {
  std::vector<DeviceInfo> available;
  std::optional<DeviceInfo> connected;
};

class AudioEngine: public IoListener {
  public: 
    AudioEngine()
    : _ioManager(IoManager::New(*this))
    , _dspManager(std::make_unique<DspManager>()) 
    {}

    SessionState getSessionState() {
      return _dspManager->sessionState();
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
      if (state.available.size() > 1 && !state.connected) {
        if (_ioManager) _ioManager->connect_input(state.available[1]);  
      }
    }

    // AUDIO THREAD. Realtime-safe code only
    void audio_callback(float* input, float* output, uint32_t frame_count) override {
      // std::cout << "frames: " << frame_count << " | ";
      // for (auto i=0 ; i<5 ; i++) {
      //   float val = input[i];
      //   std::cout << val << ", ";
      // }
      // std::cout << std::endl;
      _dspManager->update(input, frame_count);
    }
  
  private:
    std::unique_ptr<IoManager> _ioManager;
    std::unique_ptr<DspManager> _dspManager;
};