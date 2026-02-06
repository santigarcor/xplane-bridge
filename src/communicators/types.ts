/**
 * Message sent from Arduino/WebCockpit to XPlane Bridge
 * when user makes an interaction (button press, knob turn, etc.)
 */
export type IncomingMessage = {
  /** User input identifier (e.g., "set_alt", "fmc_l_0") */
  user_input: string
}

/**
 * Message sent from XPlane Bridge to Arduino/WebCockpit
 * when an update occurs in the flight simulator
 */
export type OutgoingMessage = {
  /** Command/dataref identifier (e.g., "sim/cockpit/autopilot/altitude") */
  cmd: string
  /** Current value from the simulator */
  value: number | string
}

export interface Communicator {
  /**
   * Receives the callback that will be executed when Arduino/WebCockpit sends a user interaction
   */
  onMessage(onMessageReceived: (data: IncomingMessage) => void): void

  /**
   * Sends data from the Bridge to Arduino/WebCockpit when an update occurs in the flight simulator
   */
  sendMessage(data: OutgoingMessage): void
  connect(): Promise<void>
  disconnect(): void
}
