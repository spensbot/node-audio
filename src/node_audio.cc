#include "node_audio.h"

using namespace Napi;

NodeAudio::NodeAudio(const Napi::CallbackInfo& info) : ObjectWrap(info) {
}

void NodeAudio::connect(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() < 1) {
        Napi::TypeError::New(env, "Wrong number of arguments")
          .ThrowAsJavaScriptException();
    }

    if (!info[0].IsString()) {
        Napi::TypeError::New(env, "First argument must be a string")
          .ThrowAsJavaScriptException();
    }

    Napi::String audio_port_id = info[0].As<Napi::String>();

    _engine.connect(audio_port_id.Utf8Value());
}

Napi::Value NodeAudio::getConnectionState(const Napi::CallbackInfo& info) {
    auto env = info.Env();
    Napi::Object obj = Napi::Object::New(env);
    obj.Set("available", Napi::Array::New(env));
    obj.Set("connected", env.Null());

    return obj;
}

Napi::Value NodeAudio::getSessionState(const Napi::CallbackInfo& info) {
    const auto sessionState = _engine.getSessionState();

    auto env = info.Env();
    Napi::Object obj = Napi::Object::New(env);
    obj.Set("bpm", sessionState.bpm);
    obj.Set("beats", sessionState.beats);

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


// Napi::Value NodeAudio::Greet(const Napi::CallbackInfo& info) {
//     Napi::Env env = info.Env();

//     if (info.Length() < 1) {
//         Napi::TypeError::New(env, "Wrong number of arguments")
//           .ThrowAsJavaScriptException();
//         return env.Null();
//     }

//     if (!info[0].IsString()) {
//         Napi::TypeError::New(env, "You need to introduce yourself to greet")
//           .ThrowAsJavaScriptException();
//         return env.Null();
//     }

//     Napi::String name = info[0].As<Napi::String>();

//     printf("Hello %s\n", name.Utf8Value().c_str());
//     printf("I am %s\n", this->_greeterName.c_str());

//     return Napi::String::New(env, this->_greeterName);
// }
