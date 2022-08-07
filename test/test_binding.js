const NodeAudio = require("../dist/binding.js")
const assert = require("assert")

assert(NodeAudio, "The expected module is undefined")

function testBasic() {
  const instance = new NodeAudio()
  instance.connect("this_port_does_not_exist")
  console.log(instance.getConnectionState())
  console.log(instance.getSessionState())
  setTimeout(testBasic, 1000)
}

testBasic()
