#define MINIAUDIO_IMPLEMENTATION
#include "node_audio.h"

using namespace Napi;

namespace {
    void assert_num_args(const Napi::CallbackInfo& info, const Napi::Env& env, size_t num) {
        if (info.Length() < num) {
            Napi::TypeError::New(env, "Wrong number of arguments")
                .ThrowAsJavaScriptException();
        }
    }

    Napi::Value to_js(const Napi::Env& env, const std::string& string) {
        return Napi::String::New(env, string.c_str());
    }
    std::string string_from_js(const Napi::Env& env, const Napi::Value& val) {
        if (!val.IsString()) {
            Napi::TypeError::New(env, "Argument must be a string")
                .ThrowAsJavaScriptException();
        }
        Napi::String string = val.As<Napi::String>();
        return string.Utf8Value();
    }

    Napi::Value to_js(const Napi::Env& env, const DeviceInfo& deviceInfo) {
        auto obj = Napi::Object::New(env);
        obj.Set("is_default", deviceInfo.is_default);
        obj.Set("name", to_js(env, deviceInfo.name));
        obj.Set("id", to_js(env, deviceInfo.id));
        return obj;
    }

    Napi::Value to_js(const Napi::Env& env, const ConnectionState& state) {
        auto len = state.available.size();
        Napi::Object obj = Napi::Object::New(env);
        Napi::Array available = Napi::Array::New(env, len);
        for (size_t i=0 ; i<len; i++) {
            available[i] = to_js(env, state.available[i]);
        }
        obj.Set("available", available);

        if (state.connected) {
            obj.Set("connected", to_js(env, *state.connected));
        } else {
            obj.Set("connected", env.Null());
        }
        return obj;
    }

    Napi::Value to_js(const Napi::Env& env, std::vector<float> floatVector) {
        Napi::Array out = Napi::Array::New(env, floatVector.size());
        for (size_t i=0 ; i<floatVector.size() ; i++) {
            out[i] = floatVector[i];
        }
        return out;
    }
} // namespace 

NodeAudio::NodeAudio(const Napi::CallbackInfo& info) : ObjectWrap(info) {
}

void NodeAudio::connect(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    assert_num_args(info, env, 1);

    if (info[0].IsNull()) {
        _engine.connect(std::nullopt);
    } else {
        _engine.connect(string_from_js(env, info[0]));
    }
}

Napi::Value NodeAudio::getConnectionState(const Napi::CallbackInfo& info) {
    auto env = info.Env();
    return to_js(env, _engine.getConnectionState());
}

Napi::Value NodeAudio::getSessionState(const Napi::CallbackInfo& info) {
    const auto sessionState = _engine.getSessionState();

    auto env = info.Env();
    Napi::Object obj = Napi::Object::New(env);
    obj.Set("bpm", sessionState.bpm);
    obj.Set("beats", sessionState.beats);
    obj.Set("rms", sessionState.rms);
    obj.Set("lrBalance", sessionState.lrBalance);
    obj.Set("rms_l", sessionState.rms_l);
    obj.Set("rms_r", sessionState.rms_r);
    obj.Set("phaseVocoder", to_js(env, sessionState.phaseVocoder));
    obj.Set("pitch", sessionState.pitch);
    obj.Set("pitchConfidence", sessionState.pitchConfidence);
    obj.Set("bpmConfidence", sessionState.bpmConfidence);
    obj.Set("bpmConfidenceThreshold", sessionState.bpmConfidenceThreshold);
    obj.Set("bpmUnconfident", sessionState.bpmUnconfident);

    return obj;
}

Napi::Function NodeAudio::GetClass(Napi::Env env) {
    return DefineClass(env, "NodeAudio", {
        NodeAudio::InstanceMethod("connect", &NodeAudio::connect),
        NodeAudio::InstanceMethod("getConnectionState", &NodeAudio::getConnectionState),
        NodeAudio::InstanceMethod("getSessionState", &NodeAudio::getSessionState),
    });
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    Napi::String name = Napi::String::New(env, "NodeAudio");
    exports.Set(name, NodeAudio::GetClass(env));
    return exports;
}

NODE_API_MODULE(addon, Init)