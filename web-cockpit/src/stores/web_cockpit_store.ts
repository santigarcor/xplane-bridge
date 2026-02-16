import {
  WebSocketStatus,
  type IncomingMessage,
  type OutgoingMessage,
  type SupportedAircraft,
} from '@/types'
import { defineStore } from 'pinia'
import { computed, ref } from 'vue'
import { use757Store } from '@/planes/ff_757/stores/757'
import { use737Store } from '@/planes/zibo_737/stores/737'

const webSocket = ref<WebSocket | null>(null)

export const useWebCockpitStore = defineStore('web_cockpit', () => {
  const wsStatus = ref<WebSocketStatus>(WebSocketStatus.DISCONNECTED)
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
    if (webSocket.value !== null) {
      return
    }
    console.log('[📱 ⇨ 💻] Attempting to establish WebSocket connection with Bridge...')
    webSocket.value = new WebSocket(`ws://${window.location.host}/ws`)

    webSocket.value.addEventListener('open', async () => {
      console.log('[📱 ⇨ 💻] ✅ WebSocket connection with Bridge established')
      wsStatus.value = WebSocketStatus.CONNECTED
    })

    webSocket.value.addEventListener('message', (event) => handleXPlaneUpdate(event.data))

    webSocket.value.addEventListener('close', () => {
      console.warn('[📱 ⇨ 💻] ⚠️ WebSocket connection closed. Reconnecting in 2 seconds...')
      wsStatus.value = WebSocketStatus.RECONNECTING
      setTimeout(() => {
        webSocket.value = null
        initializeWebSocket()
      }, 2000)
    })

    webSocket.value.addEventListener('error', (event) => {
      wsStatus.value = WebSocketStatus.ERROR
      console.error(`[📱 ⇨ 💻] ❌ WebSocket error:`, event)
    })
  }

  function sendCommand(command: string): void {
    if (webSocket.value && webSocket.value.readyState === WebSocket.OPEN) {
      const message = JSON.stringify({ user_input: command } as OutgoingMessage)
      webSocket.value.send(message)
      console.log(`[📱 ⇨ 💻] Sent command to Bridge: ${message}`)
    } else {
      console.warn('[📱 ⇨ 💻] ⚠️ WebSocket is not open. Cannot send command.')
    }
  }

  function reInitializeWebSocket(): void {
    if (webSocket.value) {
      webSocket.value.close()
      webSocket.value = null
    }
  }

  function toggleDebugMode(): void {
    if (activePlaneStore.value) {
      activePlaneStore.value.debug = !activePlaneStore.value.debug
    }
  }

  return {
    initializeWebSocket,
    reInitializeWebSocket,
    sendCommand,
    activePlane: computed(() => activePlane.value),
    debug: computed(() => activePlaneStore.value?.debug ?? false),
    setActivePlane,
    activePlaneStore,
    wsStatus,
    toggleDebugMode,
  }
})
