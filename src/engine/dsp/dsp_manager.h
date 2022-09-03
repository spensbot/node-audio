#pragma once

#include "dsp/rolling_average.h"
#include "dsp/beat_tracker.h"
#include "dsp/FftBuffer.h"
#include "ConnectionConfig.h"

#include <aubio.h>
#include <memory>
#include <atomic>
#include <vector>


class DspManager {
  constexpr static float DEFAULT_BPM = 120.0;
  constexpr static float CONFIDENCE_THRESHOLD = 0.00;
  struct Ptrs {
    Ptrs(ConnectionConfig config): beats_out(new_fvec(1)) {
      tempo = new_aubio_tempo("default", 1024, 256, config.sampleRate);
    }
    ~Ptrs() {
      del_fvec(beats_out);
      del_aubio_tempo(tempo);
    }
    aubio_tempo_t* tempo;
    fvec_t* beats_out;
  };
public:
  DspManager(): _beatTracker(DEFAULT_BPM, CONFIDENCE_THRESHOLD) {}
  ~DspManager() {
    aubio_cleanup();
  }

  void reset(ConnectionConfig config) {
    _ptrs = std::make_unique<Ptrs>(config);
    _rms.setSustainSeconds(config.sampleRate, 0.05, 0.2);
    _fftBuffer.reset(config);
    _config = config;
  }

  // AUDIO THREAD! Realtime safe only
  void update(float* frames) {
    _fftBuffer.write(frames);
    while(auto hop = _fftBuffer.read()) {
      aubio_tempo_do(_ptrs->tempo, &*hop, _ptrs->beats_out);
    }
    
    auto bpm = aubio_tempo_get_bpm(_ptrs->tempo);
    auto seconds_since_last_beat = aubio_tempo_get_last_s(_ptrs->tempo);
    auto confidence = aubio_tempo_get_confidence(_ptrs->tempo);
    auto dt_s = calc_seconds(_config.frameCount, _config.sampleRate);
    _beatTracker.update(bpm, seconds_since_last_beat, confidence, dt_s);

    _outBpm.store(_beatTracker.bpm());
    _outBeats.store(_beatTracker.beats());
    _outConfidence.store(confidence);

    for (const auto sample : _fftBuffer.reader().readAvg()) {
      _rms.push(sample);
    }

    _outRms.store(_rms.get());
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
      _outBpm.load(),
      _outBeats.load(),
      _outRms.load(),
      _outConfidence.load(),
      CONFIDENCE_THRESHOLD,
    };
  }

private:
  ConnectionConfig _config;
  std::unique_ptr<Ptrs> _ptrs;
  RollingRMSBiased _rms;
  BeatTracker _beatTracker;
  FftBuffer _fftBuffer;

  std::atomic<float> _outBpm;
  std::atomic<float> _outBeats;
  std::atomic<float> _outRms;
  std::atomic<float> _outConfidence;

  static float calc_seconds(uint32_t samples, uint32_t sampleRate) {
    return static_cast<float>(samples) / static_cast<float>(sampleRate);
  }
};