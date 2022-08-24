#pragma once

#include "session_state.h"
#include "dsp/rolling_average.h"

#include <aubio.h>
#include <memory>

class DspManager {
  struct Data {
    aubio_tempo_t* tempo;
  };
public:
  DspManager() {
    _data = std::make_unique<Data>();
    // https://aubio.org/doc/latest/tempo_2test-tempo_8c-example.html#a4
    _data->tempo = new_aubio_tempo("default", 1024, 256, 44100);
    _rms.setSustainSeconds(44100, 1.0);
  }
  ~DspManager() {
    del_aubio_tempo(_data->tempo);
    aubio_cleanup();
  }

  // AUDIO THREAD! Realtime safe only
  void update(float* input, unsigned int count) {
    for (auto i=0 ; i<count ; i++) {
      _rms.push(input[i]);
    }

    auto in = fvec_t {count, input};
    auto out = new_fvec (count);
    aubio_tempo_do(_data->tempo, &in, out);
  }

  SessionState sessionState() {
    return SessionState {
      aubio_tempo_get_bpm(_data->tempo),
      // _rms.get(),
      aubio_tempo_get_confidence(_data->tempo)
    };
  }
  
private:
  std::unique_ptr<Data> _data;
  RollingRMS _rms;
};