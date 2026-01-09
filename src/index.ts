import { XPlaneArduinoBridge } from './bridge.js'

const bridge = new XPlaneArduinoBridge()

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
