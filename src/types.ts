/**
 * Represents the message sent to the Arduino containing a command and its value.
 */
export type ArduinoCommand = {
  cmd: string
  value: any
}

/**
 * Represents the message received from the Arduino containing user input.
 */
export type ArduinoMessage = {
  user_input: string
}

export enum ParserType {
  BOOLEAN = 'boolean',
  ROUND = 'round',
  TO_DEGREES = 'to_degrees',
  VALUE_MAP = 'value_map',
}
export type ParserFunction = (v: any, extra?: any) => any

export type ValueMap = Record<string, any>

export type InputType = 'command' | 'dataref'

/**
 * Special constant to indicate a toggle dataref behavior for momentary switch inputs.
 */
export const TOGGLE_DATAREF = 'TOGGLE_DATAREF'

/**
 * Enum of message types that can be sent to or received from X-Plane via WebSocket.
 * Defines the different operations and notifications exchanged between the bridge and X-Plane.
 */
export enum XPlaneMessageType {
  RESULT = 'result',
  DATAREF_UPDATE_VALUES = 'dataref_update_values',
  COMMAND_SET_IS_ACTIVE = 'command_set_is_active',
  DATAREF_SET_VALUES = 'dataref_set_values',
  DATAREF_SUBSCRIBE_VALUES = 'dataref_subscribe_values',
}

/**
 * The type of X-Plane identifier that can be cached and looked up.
 * Represents either X-Plane data references or X-Plane commands.
 */
export type XPlaneIdentifierType = 'datarefs' | 'commands'

/**
 * Represents a message received from X-Plane via WebSocket connection.
 * Contains the message type, optional request ID for tracking, data payload, and status information.
 */
export type XplaneWebsocketMessage = {
  type: XPlaneMessageType
  request_id?: number | undefined
  data?: Record<string, any> | undefined
  success?: boolean | undefined
  error_code?: string | undefined
  error_message?: string | undefined
}

/**
 * Mapping configuration of what command to send to Arduino for a specific X-Plane dataref,
 * including threshold and parser type for value conversion.
 */
export type DataRefMapping = {
  arduino_cmd: string
  threshold: number
  parser: ParserType | undefined
  value_map?: ValueMap | undefined
}

export type DataRefMappings = {
  [key: string]: DataRefMapping
}

/**
 * Mapping configuration for user inputs from Arduino to X-Plane actions,
 * specifying the type of input, associated X-Plane actions, and optional value.
 */
export type InputMapping = {
  type: InputType
  xplane_actions: string[]
  value?: any | undefined
}

export type InputMappings = {
  [key: string]: InputMapping
}
