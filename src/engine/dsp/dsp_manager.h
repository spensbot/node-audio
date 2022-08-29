#pragma once

#include "dsp/rolling_average.h"
#include "dsp/beat_tracker.h"

#include <aubio.h>
#include <memory>

class DspManager {
  constexpr static float DEFAULT_BPM = 120.0;
  constexpr static float CONFIDENCE_THRESHOLD = 0.5;
  struct Ptrs {
    Ptrs(float sampleRate): beats_out(new_fvec(1)) {
      tempo = new_aubio_tempo("default", 1024, 256, sampleRate);
    }
    ~Ptrs() {
      del_fvec(beats_out);
      del_aubio_tempo(tempo);
    }
    aubio_tempo_t* tempo;
    fvec_t* beats_out;
  };
public:
  DspManager()
  : _ptrs(std::make_unique<Ptrs>(44100))
  , _beat_tracker(DEFAULT_BPM, CONFIDENCE_THRESHOLD) {}
  ~DspManager() {
    aubio_cleanup();
  }

  void reset(uint32_t sampleRate) {
    _ptrs = std::make_unique<Ptrs>(sampleRate);
  }

  // AUDIO THREAD! Realtime safe only
  void update(float* samples, uint32_t sampleCount) {
    for (uint32_t i=0 ; i<sampleCount ; i++) {
      _rms.push(samples[i]);
    }

    auto in = fvec_t {sampleCount, samples};
    aubio_tempo_do(_ptrs->tempo, &in, _ptrs->beats_out);
  }
  
  struct SessionState {
    float bpm;
    float beats;
    float rms;
    float confidence;
    float confidence_threshold;
  };

  SessionState sessionState() {
    return SessionState {
      _beat_tracker.bpm(),
      _beat_tracker.beats(),
      _rms.get(),
      aubio_tempo_get_confidence(_ptrs->tempo),
      CONFIDENCE_THRESHOLD,
    };
  }
private:
  std::unique_ptr<Ptrs> _ptrs;
  RollingRMSBiased _rms;
  BeatTracker _beat_tracker;
};