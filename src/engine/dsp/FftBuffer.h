#pragma once

#include "ConnectionConfig.h"

#include <aubio.h>
#include <vector>
#include <algorithm>
#include <optional>

fvec_t fvec(uint32_t count, float* data) {
  return fvec_t {
    count,
    data
  };
}

// Reads channel-interspersed frames into nicer formats
class FrameReader {
public:
  FrameReader() = default;

  void reset(ConnectionConfig config) {
    _config = config;
    _avg.resize(config.frameCount);
  }

  void write(float* frames) {
    for(uint32_t f=0 ; f<_config.frameCount ; f++) {
      float total = 0.0;
      for(uint32_t c=0; c<_config.channelCount ; c++) {
        const float sample = frames[f * _config.channelCount + c];
        total += sample;
      }
      _avg[f] = total / static_cast<float>(_config.channelCount);
    }
  }

  const std::vector<float>& readAvg() const {
    return _avg;
  }
private:
  ConnectionConfig _config;
  std::vector<float> _avg;
};

class CircularBuffer {
public:
  CircularBuffer() = default;

  void reset(uint32_t size) {
    _data.resize(size + 1);
  }

  void write(float val) {
    increment();
    _data[_pos] = val;
  }

  // read(0) gives the latest written value
  float read(uint32_t offset) const {
    if (offset <= _pos) {
      return _data[_pos - offset];
    } else {
      return _data[_data.size() - (offset - _pos)];
    }
  }

private:
  uint32_t _pos;
  std::vector<float> _data;

  void increment() {
    _pos += 1;
    _pos %= _data.size();
  }
};

// Maintains buffers that contain enough samples
// This allows a full hop of samples to be sent to the fft algorithm even between frames
class FftBuffer {
public:
  static constexpr uint32_t WINDOW_SIZE = 1024;
  static constexpr uint32_t HOP_SIZE = 256;

  FftBuffer() = default;

  // You must call reset before doing anything
  void reset(ConnectionConfig config) {
    _config = config;
    _reader.reset(config);
    _buffer.reset(std::max(_config.frameCount, HOP_SIZE) * 2);
    _hop.resize(HOP_SIZE);
  }

  // Write all frames
  void write(float* frames) {
    _reader.write(frames);
    for (const float sample: _reader.readAvg()) {
      _buffer.write(sample);
    }
    _count += _config.frameCount;
  }

  // Be sure to read all hops after each write
  std::optional<fvec_t> read() {
    if (_count >= HOP_SIZE) {
      for (uint32_t i=0 ; i<HOP_SIZE ; i++) {
        const auto offset = HOP_SIZE - 1 - i;
        _hop[i] = _buffer.read(offset);
      }
      _count -= HOP_SIZE;
      return fvec(HOP_SIZE, _hop.data());
    } else {
      return std::nullopt;
    }
  }

  const FrameReader& reader() const {
    return _reader;
  }

private:
  ConnectionConfig _config;
  FrameReader _reader;
  CircularBuffer _buffer;
  std::vector<float> _hop;
  uint32_t _count;
};
