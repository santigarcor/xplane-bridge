import { XPlaneArduinoBridge } from './bridge.js'
import { initializeMappings as initFF757 } from './mappings/ff_757.js'
import { initializeMappings as initZibo737 } from './mappings/zibo_737.js'

import path from 'node:path'
import { fileURLToPath } from 'node:url'
import readline from 'readline'
import { select } from '@inquirer/prompts'

const __filename = fileURLToPath(import.meta.url)
const __dirname = path.dirname(__filename)

console.log(`Loading env file from ${__dirname}`)
process.loadEnvFile(path.join(__dirname, '..', '.env'))

const planeOptions = [
  { id: 'ff_757', label: 'FF 757' },
  { id: 'zibo_737', label: 'Zibo 737' },
]

const selectActivePlane = async (): Promise<string> =>
  select({
    message: 'Select active plane',
    choices: planeOptions.map((plane) => ({
      name: plane.label,
      value: plane.id,
    })),
  })

const main = async (): Promise<void> => {
  const bridge = new XPlaneArduinoBridge()
  const activePlane =
    process.env.APP_ENV != 'development'
      ? await selectActivePlane()
      : process.env.ACTIVE_PLANE || 'zibo_737'

  switch (activePlane) {
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

  process.on('SIGINT', function () {
    console.log('\n[🏗️] 🛑 Shutting down gracefully...')
    bridge.close()
    console.log('\n[🏗️] ⛔ Bridge closed.')
    process.exit()
  })
}

if (process.platform === 'win32') {
  const rl = readline.createInterface({
    input: process.stdin,
    output: process.stdout,
  })

  rl.on('SIGINT', function () {
    process.emit('SIGINT')
  })
}

main().catch((error) => {
  console.error('[🏗️] ❌ Failed to start bridge:', error)
  process.exit(1)
})
