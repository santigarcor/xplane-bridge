import { XPlaneArduinoBridge } from './bridge.js'
import { initializeMappings } from './mappings/ff_757.js'
// import { initializeMappings } from './mappings/zibo_737.js'

const bridge = new XPlaneArduinoBridge()

initializeMappings(bridge)

bridge.run()

if (process.platform === 'win32') {
  var rl = require('readline').createInterface({
    input: process.stdin,
    output: process.stdout,
  })

  rl.on('SIGINT', function () {
    process.emit('SIGINT')
  })
}

process.on('SIGINT', function () {
  console.log('\n[🏗️] 🛑 Shutting down gracefully...')
  bridge.close()
  process.exit()
})
