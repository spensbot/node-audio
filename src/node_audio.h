#pragma once

#include "./engine/audio_engine.h"

#include <napi.h>

class NodeAudio : public Napi::ObjectWrap<NodeAudio>
{
public:
    NodeAudio(const Napi::CallbackInfo&);

    static Napi::Function GetClass(Napi::Env);

    void connect(const Napi::CallbackInfo&);
    Napi::Value getConnectionState(const Napi::CallbackInfo&);
    Napi::Value getSessionState(const Napi::CallbackInfo&);

private:
    AudioEngine _engine;
};
