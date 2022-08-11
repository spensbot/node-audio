const NodeAudio = require("../dist/binding.js")
const assert = require("assert")

assert(NodeAudio, "The expected module is undefined")

const nodeAudio = new NodeAudio()
// nodeAudio.connect("this_port_does_not_exist")

function testBasic() {
  console.log(nodeAudio.getConnectionState())
  console.log(nodeAudio.getSessionState())
  console.log("timeout")
  setTimeout(testBasic, 1000)
}

testBasic()
