export type CockpitImageKey = {
  id: string // ID que recibirá el Bridge (ej: 'LSK_L1', 'KEY_A')
  top: number // % desde arriba
  left: number // % desde la izquierda
  width: number // % de ancho
  height: number // % de alto
}

export type OutgoingMessage = {
  /** User input identifier (e.g., "set_alt", "fmc_l_0") */
  user_input: string
}

/**
 * Message sent from XPlane Bridge to WebCockpit
 * when an update occurs in the flight simulator
 */
export type IncomingMessage = {
  /** Command/dataref identifier (e.g., "sim/cockpit/autopilot/altitude") */
  cmd: string
  /** Current value from the simulator */
  value: number | string
}

export enum WebSocketStatus {
  CONNECTED = 'connected',
  DISCONNECTED = 'disconnected',
  RECONNECTING = 'disconnected - attempting to reconnect',
  ERROR = 'error',
}

export type SupportedAircraft = 'ff_757' | 'zibo_737'
