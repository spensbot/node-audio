#include "dsp/rolling_average.h"

#include <math.h>

// TODO: Evaluate the idea of turning _last_confident_bpm into a rolling_avg that moves more quickly 

// Takes bpm values & confidence from aubio and turns them into a consistent beat-time
class BeatTracker {
public:
  constexpr static float MAX_CORRECTION_PER_BEAT = 0.5;
  constexpr static float INIT_BPM = 120.0;
  constexpr static float CONFIDENCE_THRESHOLD = 0.11;
  BeatTracker()
  : _last_confident_bpm(INIT_BPM)
  , _beats(0.0)
  , _confidence_threshold(CONFIDENCE_THRESHOLD) 
  {}

  void update(float detected_bpm, float detected_s_since_last_beat, float confidence, float dt_s) {
    if (confidence > _confidence_threshold) _last_confident_bpm = detected_bpm;
    const float dt_beats = s_to_beats(dt_s, _last_confident_bpm);
    _beats += dt_beats;

    if (confidence > _confidence_threshold) {
      _last_confident_bpm = detected_bpm;
      const float _detected_intra_beat = s_to_beats(detected_s_since_last_beat, detected_bpm);
      correct_beats(
        _detected_intra_beat,
        dt_beats
      );
    }
  }

  float bpm() const {
    return _last_confident_bpm;
  }

  float beats() const {
    return _beats;
  }

private:
  float _last_confident_bpm;
  double _beats;
  float _confidence_threshold;

  static float s_to_beats(float seconds, float bpm) {
    const float minutes = seconds / 60.0;
    return bpm * minutes;
  }

  static float intra_beat(double beats) {
    return fmod(beats, 1.0);
  }

  void correct_beats(float target_intra_beat, float dt_beats) {
    const float current_intra_beat = intra_beat(_beats);
    float dif = target_intra_beat - current_intra_beat;
    const float max_correction = MAX_CORRECTION_PER_BEAT * dt_beats;
    if (dif > 0.5) {
      dif -= 1.0;
    } else if (dif < -0.5) {
      dif += 1.0;
    }
    _beats += fmin(dif, max_correction);
  }
};