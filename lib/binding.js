"use strict";
const addon = require("../build/Release/node-audio-native");
class NodeAudio {
    constructor() {
        this._addonInstance = new addon.NodeAudio();
    }
    connect(audioPortId) {
        return this._addonInstance.connect(audioPortId);
    }
    getConnectionState() {
        return this._addonInstance.getConnectionState();
    }
    getSessionState() {
        return this._addonInstance.getSessionState();
    }
}
module.exports = NodeAudio;
