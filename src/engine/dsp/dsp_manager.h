#pragma once

#include "dsp/rolling_average.h"
#include "dsp/beat_tracker.h"

#include <aubio.h>
#include <memory>
#include <atomic>

class DspManager {
  constexpr static float DEFAULT_BPM = 120.0;
  constexpr static float CONFIDENCE_THRESHOLD = 0.5;
  struct Ptrs {
    Ptrs(float sample_rate): beats_out(new_fvec(1)) {
      tempo = new_aubio_tempo("default", 1024, 256, sample_rate);
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

  void reset(uint32_t sample_rate) {
    _ptrs = std::make_unique<Ptrs>(sample_rate);
    _rms.setSustainSeconds(sample_rate, 0.05, 0.2);
    _sample_rate = sample_rate;
  }

  // AUDIO THREAD! Realtime safe only
  void update(float* samples, uint32_t sample_count) {
    auto in = fvec_t {sample_count, samples};
    aubio_tempo_do(_ptrs->tempo, &in, _ptrs->beats_out);

    auto bpm = aubio_tempo_get_bpm(_ptrs->tempo);
    auto seconds_since_last_beat = aubio_tempo_get_last_s(_ptrs->tempo);
    auto confidence = aubio_tempo_get_confidence(_ptrs->tempo);
    auto dt_s = calc_seconds(sample_count, _sample_rate);
    _beat_tracker.update(bpm, seconds_since_last_beat, confidence, dt_s);

    _out_bpm.store(_beat_tracker.bpm());
    _out_beats.store(_beat_tracker.beats());
    _out_confidence.store(confidence);

    for (uint32_t i=0 ; i<sample_count ; i++) {
      _rms.push(samples[i]);
    }

    _out_rms.store(_rms.get());
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
      _out_bpm.load(),
      _out_beats.load(),
      _out_rms.load(),
      _out_confidence.load(),
      CONFIDENCE_THRESHOLD,
    };
  }
private:
  float _sample_rate;
  std::unique_ptr<Ptrs> _ptrs;
  RollingRMSBiased _rms;
  BeatTracker _beat_tracker;

  std::atomic<float> _out_bpm;
  std::atomic<float> _out_beats;
  std::atomic<float> _out_rms;
  std::atomic<float> _out_confidence;

  static float calc_seconds(uint32_t samples, uint32_t sampleRate) {
    return static_cast<float>(samples) / static_cast<float>(sampleRate);
  }
};