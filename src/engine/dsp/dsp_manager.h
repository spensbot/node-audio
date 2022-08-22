#pragma once

#include "session_state.h"

#include <aubio.h>
#include <memory>

class DspManager {
  struct Data {
    aubio_tempo_t* tempo;
  };
public:
  DspManager() {
    _data = std::make_unique<Data>();
    _data->tempo = new_aubio_tempo("default", 1024, 128, 48000);
  }
  ~DspManager() {
    del_aubio_tempo(_data->tempo);
    aubio_cleanup();
  }

  // AUDIO THREAD! Realtime safe only
  void update(float* input, unsigned int count) {
    auto in = fvec_t {count, input};
    auto out = new_fvec (count);
    aubio_tempo_do(_data->tempo, &in, out);
  }

  SessionState sessionState() {
    return SessionState {
      aubio_tempo_get_bpm(_data->tempo),
      aubio_tempo_get_confidence(_data->tempo)
    };
  }
  
private:
  std::unique_ptr<Data> _data;
};