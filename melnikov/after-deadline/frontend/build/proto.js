// Build protobuf
var protobuf = require('protobufjs')
var fs = require('fs')

protobuf.load('../tables.proto', function(e, p) {
  var compiledProto = JSON.stringify(p.toJSON())
  fs.writeFile('src/assets/proto.json', compiledProto, function(err) {
    if (err) return console.log(err)
    console.log('Compiled proto!')
  })
})

