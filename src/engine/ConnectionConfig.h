#pragma once

#include <miniaudio.h>

struct ConnectionConfig {
  ConnectionConfig() = default;

  ConnectionConfig(ma_device& device)
  : sampleRate(device.sampleRate)
  , frameCount(device.capture.internalPeriodSizeInFrames)
  , channelCount(device.capture.channels)
  {}

  uint32_t sampleRate, frameCount, channelCount;
};
