#pragma once

#include "dsp/rolling_average.h"
#include "dsp/beat_tracker.h"
#include "dsp/FftBuffer.h"
#include "ConnectionConfig.h"

#include <aubio.h>
#include <memory>
#include <atomic>
#include <vector>
#include <array>


class DspManager {
  struct Ptrs {
    Ptrs(ConnectionConfig config): beats_out(new_fvec(1)) {
      tempo = new_aubio_tempo("default", FftBuffer::WINDOW_SIZE, FftBuffer::HOP_SIZE, config.sampleRate);
      beats_out = new_fvec(1);
      phaseVocoder = new_aubio_pvoc(FftBuffer::WINDOW_SIZE, FftBuffer::HOP_SIZE);
      phaseVocoder_out = new_cvec(FftBuffer::HOP_SIZE);
      fft = new_aubio_fft(FftBuffer::WINDOW_SIZE);
      fft_out = new_cvec(FftBuffer::WINDOW_SIZE);
      pitch = new_aubio_pitch("default", FftBuffer::WINDOW_SIZE, FftBuffer::HOP_SIZE, config.sampleRate);
      pitch_out = new_fvec(1);
    }
    ~Ptrs() {
      del_aubio_tempo(tempo);
      del_fvec(beats_out);
      del_aubio_pvoc(phaseVocoder);
      del_cvec(phaseVocoder_out);
      del_aubio_fft(fft);
      del_cvec(fft_out);
      del_aubio_pitch(pitch);
      del_fvec(pitch_out);
    }
    aubio_tempo_t* tempo;
    fvec_t* beats_out;
    aubio_pvoc_t* phaseVocoder;
    cvec_t* phaseVocoder_out;
    aubio_fft_t* fft;
    cvec_t* fft_out;
    aubio_pitch_t* pitch;
    fvec_t* pitch_out;
  };
public:
  DspManager() {}
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
      aubio_pvoc_do(_ptrs->phaseVocoder, &*hop, _ptrs->phaseVocoder_out);
      aubio_fft_do(_ptrs->fft, &*hop, _ptrs->fft_out);
      aubio_pitch_do(_ptrs->pitch, &*hop, _ptrs->pitch_out);
    }
    
    auto bpm = aubio_tempo_get_bpm(_ptrs->tempo);
    uint64_t runningFrameCount = _fftBuffer.runningFrameCount();
    uint64_t lastBeatFrameCount = aubio_tempo_get_last(_ptrs->tempo);
    uint32_t framesSinceLastBeat = runningFrameCount - lastBeatFrameCount;
    _outFramesSinceLastBeat.store(framesSinceLastBeat);
    _outDelayFrameCount.store(aubio_tempo_get_delay(_ptrs->tempo));
    auto secondsSinceLastBeat = calc_seconds(framesSinceLastBeat, _config.sampleRate);
    auto bpmConfidence = aubio_tempo_get_confidence(_ptrs->tempo);
    auto dt_s = calc_seconds(_config.frameCount, _config.sampleRate);
    _beatTracker.update(bpm, secondsSinceLastBeat, bpmConfidence, dt_s);

    _outBpm.store(_beatTracker.bpm());   
    _outBeats.store(_beatTracker.beats());
    _outBpmConfidence.store(bpmConfidence);
    _outBpmUnconfident.store(bpm);
    _outPitch.store(_ptrs->pitch_out->data[0]);
    _outPitchConfidence.store(aubio_pitch_get_confidence(_ptrs->pitch));

    _outSecondsSinceLastBeat.store(secondsSinceLastBeat);
    _outRunningFrameCount.store(runningFrameCount);
    _outLastBeatFrameCount.store(lastBeatFrameCount);

    for (const auto sample : _fftBuffer.reader().readAvg()) {
      _rms.push(sample);
    }

    _outRms.store(_rms.get());
  }
  
  struct SessionState {
    float bpm;
    double beats;
    float rms;
    float lrBalance;
    float rms_l;
    float rms_r;
    float pitch;
    float pitchConfidence;
    float bpmConfidence;
    float bpmConfidenceThreshold;
    float bpmUnconfident;
    std::vector<float> phaseVocoder;
  };

  SessionState sessionState() {
    std::cout 
    << _outRunningFrameCount.load() << " | " 
    << _outLastBeatFrameCount.load() << " | " 
    << _outDelayFrameCount.load() << " | " 
    << _outFramesSinceLastBeat.load() << " | " 
    << _outSecondsSinceLastBeat.load() << std::endl;
    return SessionState {
      _outBpm.load(),
      _outBeats.load(),
      _outRms.load(),
      _outLrBalance.load(),
      _outRms_l.load(),
      _outRms_r.load(),
      _outPitch.load(),
      _outPitchConfidence.load(),
      _outBpmConfidence.load(),
      BeatTracker::CONFIDENCE_THRESHOLD,
      _outBpmUnconfident.load(),
      std::vector<float> {}
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
  std::atomic<float> _outLrBalance;
  std::atomic<float> _outRms_l;
  std::atomic<float> _outRms_r;
  std::atomic<float> _outPitch;
  std::atomic<float> _outPitchConfidence;
  std::atomic<float> _outBpmConfidence;
  std::atomic<float> _outBpmUnconfident;
  std::vector<std::atomic<float>> _outPhaseVocoder;

  std::atomic<float> _outSecondsSinceLastBeat;
  std::atomic<uint64_t> _outRunningFrameCount;
  std::atomic<uint64_t> _outLastBeatFrameCount;
  std::atomic<uint32_t> _outFramesSinceLastBeat;
  std::atomic<uint64_t> _outDelayFrameCount;
};