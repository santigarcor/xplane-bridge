import path from 'node:path'
import { fileURLToPath } from 'node:url'
import readline from 'readline'

const __dirname = path.dirname(fileURLToPath(import.meta.url))

import { XPlaneBridge } from './bridge/index.js'
import { initializer } from './mappings/index.js'

console.log(`Loading env file from ${__dirname}`)
process.loadEnvFile(path.join(__dirname, '..', '.env'))

const main = async (): Promise<void> => {
  const bridge = new XPlaneBridge(__dirname, initializer)

  console.log('[🏗️] 🚀 Starting bridge')
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
