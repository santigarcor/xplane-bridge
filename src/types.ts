export type ArduinoCommand = {
  cmd: string
  value: any
}

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

export enum XPlaneMessageType {
  COMMAND_SET_IS_ACTIVE = 'command_set_is_active',
  DATAREF_SET_VALUES = 'dataref_set_values',
  DATAREF_SUBSCRIBE_VALUES = 'dataref_subscribe_values',
}

export type XPlaneIdentifierType = 'datarefs' | 'commands'

export type XplaneWebsocketMessage = {
  type: string
  request_id?: number | undefined
  data?: Record<string, any> | undefined
  success?: boolean | undefined
  error_code?: string | undefined
  error_message?: string | undefined
}

export type DataRefMapping = {
  arduino_cmd: string
  threshold: number
  parser?: ParserType | undefined
  value_map?: ValueMap | undefined
}

export type DataRefMappings = {
  [key: string]: DataRefMapping
}

export type InputMapping = {
  type: InputType
  xplane_action: string
  value?: any | undefined
}

export type InputMappings = {
  [key: string]: InputMapping
}
