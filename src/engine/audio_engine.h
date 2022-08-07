#pragma once

#include <string>
#include <vector>
#include <optional>

struct AudioPort {
  std::string id;
  std::string name;
};

struct ConnectionState {
  std::vector<AudioPort> available;
  std::optional<AudioPort> connected;
};

struct SessionState {
  float bpm;
  float beats;
};

class AudioEngine {
  public: 
    SessionState getSessionState() {
      return SessionState {
        0.0,
        0.0
      };
    }

    ConnectionState getConnectionState() {
      return ConnectionState {
        std::vector<AudioPort> {},
        std::nullopt,
      };
    }

    void connect(std::string audioPortId) {}
  
  private:
    // Where the good stuff'll go
};