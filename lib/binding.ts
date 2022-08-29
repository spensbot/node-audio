const addon = require("../build/Release/node-audio-native")

interface AudioPort {
  id: string
  name: string
}

interface ConnectionState {
  available: [AudioPort]
  connected: AudioPort | null
}

interface SessionState {
  bpm: number
  beats: number
  rms: number
  confidence: number
  confidence_threshold: number
}

interface INodeAudioNative {
  connect(audioPortId: string): void
  getConnectionState(): ConnectionState
  getSessionState(): SessionState
}

class NodeAudio {
  constructor() {
    this._addonInstance = new addon.NodeAudio()
  }

  connect(audioPortId: string) {
    return this._addonInstance.connect(audioPortId)
  }

  getConnectionState() {
    return this._addonInstance.getConnectionState()
  }

  getSessionState() {
    return this._addonInstance.getSessionState()
  }

  // private members
  private _addonInstance: INodeAudioNative
}

export = NodeAudio
