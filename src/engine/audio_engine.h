#pragma once

#include "io/connection_state.h"
#include "io/io_manager.h"
#include "dsp/dsp_manager.h"

#include <string>
#include <vector>
#include <optional>
#include <iostream>

class AudioEngine: public IoListener {
  public: 
    AudioEngine()
    : _ioManager(IoManager::New(*this)) // <-- This may fail, in which case _ioManager will be null
    , _dspManager(std::make_unique<DspManager>()) 
    {}

    DspManager::SessionState getSessionState() {
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

    void connect(std::optional<std::string> device_id) {
      if (_ioManager) {
        const auto connection_info = _ioManager->connect_input(device_id);
        if (connection_info) {
          _dspManager->reset(connection_info->sampleRate);
          _ioManager->start_input();
        }
      }
    }

    // AUDIO THREAD. Realtime-safe code only
    void audio_callback(float* input, float* output, uint32_t frame_count) override {
      _dspManager->update(input, frame_count);
    }
  
  private:
    std::unique_ptr<IoManager> _ioManager;
    std::unique_ptr<DspManager> _dspManager;
};