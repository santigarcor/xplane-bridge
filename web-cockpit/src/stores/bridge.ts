import type { IncomingMessage, OutgoingMessage, SupportedAircraft } from '@/types'
import { defineStore } from 'pinia'
import { computed, ref } from 'vue'
import { use757Store } from '@/planes/ff_757/stores/757'
import { use737Store } from '@/planes/zibo_737/stores/737'
import { parseFmcString } from '@/helpers'

let webSocket: WebSocket | null = null

export const useBridgeStore = defineStore('bridge', () => {
  const activePlaneMapping: Record<
    SupportedAircraft,
    ReturnType<typeof use757Store> | ReturnType<typeof use737Store>
  > = {
    ff_757: use757Store(),
    zibo_737: use737Store(),
  }
  const activePlane = ref<SupportedAircraft | ''>('')

  const activePlaneStore = computed(() => {
    if (activePlane.value === '') {
      return null
    }
    return activePlaneMapping[activePlane.value as SupportedAircraft]
  })

  function setActivePlane(plane: SupportedAircraft): void {
    activePlane.value = plane
  }

  function handleXPlaneUpdate(data: string): void {
    try {
      const message: IncomingMessage = JSON.parse(data)
      if (message.cmd === 'set_active_plane') {
        setActivePlane(message.value as SupportedAircraft)
        return
      }

      activePlaneStore.value?.handleBridgeCommand(message.cmd, message.value)
      console.log(`[📱 ⇨ 💻] Received data from Bridge: ${data}`)
    } catch (error) {
      console.error('[📱 ⇨ 💻] ❌ Error parsing message from Bridge:', error)
      return
    }
  }

  function initializeWebSocket(): void {
    if (webSocket !== null) {
      return
    }

    webSocket = new WebSocket(`ws://${window.location.host}/ws`)

    webSocket.addEventListener('open', async () => {
      console.log('[📱 ⇨ 💻] ✅ WebSocket connection with Bridge established')
    })

    webSocket.addEventListener('message', (event) => handleXPlaneUpdate(event.data))

    webSocket.addEventListener('close', () => {
      console.warn('[📱 ⇨ 💻] ⚠️ WebSocket connection closed. Reconnecting in 5 seconds...')
      setTimeout(() => initializeWebSocket(), 5000)
    })

    webSocket.addEventListener('error', (event) => {
      console.error(`[📱 ⇨ 💻] ❌ WebSocket error:`, event)
    })
  }

  function sendCommand(command: string): void {
    if (webSocket && webSocket.readyState === WebSocket.OPEN) {
      const message = JSON.stringify({ user_input: command } as OutgoingMessage)
      webSocket.send(message)
      console.log(`[📱 ⇨ 💻] Sent command to Bridge: ${message}`)
    } else {
      console.warn('[📱 ⇨ 💻] ⚠️ WebSocket is not open. Cannot send command.')
    }
  }

  return {
    initializeWebSocket,
    sendCommand,
    activePlane: computed(() => activePlane.value),
    setActivePlane,
    activePlaneStore,
  }
})
