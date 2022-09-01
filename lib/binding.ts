const addon = require('../build/Release/node-audio-native')

export interface AudioPort {
  id: string
  name: string
}

export interface AudioConnectionState {
  available: AudioPort[]
  connected: AudioPort | null
}

export interface SessionState {
  bpm: number
  beats: number
  rms: number
  confidence: number
  confidenceThreshold: number
}

interface INodeAudioNative {
  connect(audioPortId: string | null): void
  getConnectionState(): AudioConnectionState
  getSessionState(): SessionState
}

class NodeAudio {
  constructor() {
    this._addonInstance = new addon.NodeAudio()
  }

  connect(audioPortId: string | null) {
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

export default NodeAudio
