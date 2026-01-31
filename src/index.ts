import { XPlaneArduinoBridge } from './bridge.js'
import { initializeMappings as initFF757 } from './mappings/ff_757.js'
import { initializeMappings as initZibo737 } from './mappings/zibo_737.js'

import path from 'path'
import { fileURLToPath } from 'url'

const __filename = fileURLToPath(import.meta.url)
const __dirname = path.dirname(__filename)

console.log(`Loading env file from ${__dirname}`)
process.loadEnvFile(path.join(__dirname, '..', '.env'))

const bridge = new XPlaneArduinoBridge()

switch (process.env.ACTIVE_PLANE) {
  case 'ff_757':
    console.log('[🏗️] 🚀 Initializing mappings for FF 757')
    initFF757(bridge)
    break
  case 'zibo_737':
  default:
    console.log('[🏗️] 🚀 Initializing mappings for Zibo 737')
    initZibo737(bridge)
    break
}

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
