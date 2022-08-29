const NodeAudio = require("../dist/binding.js")
const assert = require("assert")

assert(NodeAudio, "The expected module is undefined")

const nodeAudio = new NodeAudio()
console.log("new NodeAudio()")
nodeAudio.connect(null)
console.log(`nodeAudio.connect("Nothing")`)

function testBasic() {
  let connectionState = nodeAudio.getConnectionState()
  console.log(connectionState)

  let sessionState = nodeAudio.getSessionState()
  console.log(sessionState)

  setTimeout(testBasic, 1000)
}

testBasic()
