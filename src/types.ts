export enum ParserType {
  BOOLEAN = 'boolean',
  ROUND = 'round',
  TO_DEGREES = 'to_degrees',
  VALUE_MAP = 'value_map',
}

export type ParserFunction = (v: any, extra?: any) => any

export type ValueMap = Record<string, any>

export type InputType = 'command' | 'dataref'

export type XPlaneIdentifierType = 'datarefs' | 'commands'

export type DataRefMapping = {
  arduino_cmd: string
  threshold: number
  parser?: ParserType | undefined
  value_map?: ValueMap | undefined
}

export type InputMapping = {
  type: InputType
  xplane_action: string
  value?: any | undefined
}

export type DataRefMappings = {
  [key: string]: DataRefMapping
}

export type InputMappings = {
  [key: string]: InputMapping
}
