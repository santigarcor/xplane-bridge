import WebSocket, { type RawData } from 'ws'
import fs from 'fs'
import 'dotenv/config'
import { ArduinoSerialCommunicator } from './arduino_serial_communicator.js'
import type {
  ArduinoMessage,
  DataRefMapping,
  DataRefMappings,
  InputMappings,
  ValueMap,
  XPlaneIdentifierType,
  XplaneWebsocketMessage,
} from './types.js'
import { ParserType, TOGGLE_DATAREF, XPlaneMessageType } from './types.js'
import { ensureArray } from './helpers.js'

const parserLibrary: Record<ParserType, (v: any, extra?: any) => any> = {
  [ParserType.BOOLEAN]: (v) => (v > 0 ? 1 : 0),
  [ParserType.ROUND]: (v) => Math.round(v),
  [ParserType.TO_DEGREES]: (v) => Math.round(v * (180 / Math.PI)),
  [ParserType.VALUE_MAP]: (v, map) =>
    map && map[v] !== undefined ? map[v] : v,
}

export class XPlaneArduinoBridge {
  private webSocket: WebSocket | null = null
  private arduino: ArduinoSerialCommunicator
  private requestIdCounter: number = 1
  private websocketsUrl: string
  private restUrl: string

  /**
   * Mappings from X-Plane data references to Arduino commands.
   */
  private dataRefMappings: DataRefMappings = {}

  /**
   * Mappings from Arduino inputs to X-Plane commands or data references.
   */
  private inputMappings: InputMappings = {}

  private idCache: Record<XPlaneIdentifierType, Map<string, any>> = {
    datarefs: new Map<string, any>(),
    commands: new Map<string, any>(),
  }

  private previousValues: Record<string, any> = {}

  constructor() {
    this.websocketsUrl = `ws://${process.env.XPLANE_HOST}:${process.env.XPLANE_PORT}/api/v2`
    this.restUrl = `http://${process.env.XPLANE_HOST}:${process.env.XPLANE_PORT}/api/v2`

    this.arduino = new ArduinoSerialCommunicator(
      parseInt(process.env.ARDUINO_BAUD || '9600'),
      (data: ArduinoMessage) => this.handleArduinoMessage(data),
      false,
    )
  }

  /**
   * Adds a mapping from an X-Plane data reference to an Arduino command.
   * @param dataRefName - The name of the X-Plane data reference.
   * @param mapping - The mapping configuration.
   */
  public addDataRef(dataRefName: string, mapping: DataRefMapping): void {
    this.dataRefMappings[dataRefName] = mapping
    console.log(
      `➕ X-Plane to Arduino mapping added: ${dataRefName} -> ${mapping.arduino_cmd}` +
        (mapping.threshold ? ` (threshold: ${mapping.threshold})` : '') +
        (mapping.parser ? ` (parser: ${mapping.parser})` : '') +
        (mapping.value_map
          ? ` (value_map: ${JSON.stringify(mapping.value_map)})`
          : ''),
    )
  }

  /**
   * Adds a boolean data reference mapping from X-Plane to Arduino.
   * X-Plane send any value and we convert it to boolean.
   * @param dataRefName - The name of the X-Plane data reference.
   * @param arduinoCmd - The Arduino command to send.
   */
  public addBooleanDataRef(dataRefName: string, arduinoCmd: string): void {
    this.addDataRef(dataRefName, {
      arduino_cmd: arduinoCmd,
      threshold: 0,
      parser: ParserType.BOOLEAN,
    })
  }

  /**
   * Adds a value map data reference mapping from X-Plane to Arduino.
   * X-Plane send any value and we map it using the provided value map.
   * @param dataRefName - The name of the X-Plane data reference.
   * @param arduinoCmd - The Arduino command to send.
   * @param valueMap - The value map to use for mapping values.
   */
  public addValueMapDataRef(
    dataRefName: string,
    arduinoCmd: string,
    valueMap: ValueMap,
  ): void {
    this.addDataRef(dataRefName, {
      arduino_cmd: arduinoCmd,
      threshold: 0,
      parser: ParserType.VALUE_MAP,
      value_map: valueMap,
    })
  }

  /**
   * Adds a toggle switch input mapping from Arduino to set a X-Plane data reference.
   * Arduino sends on/off commands and X-Plane receives 1/0 values.
   * @param switchName - The name of the Arduino switch.
   * @param dataRefName - The name of the X-Plane data reference.
   */
  public addToggleSwitchInputDataRef(
    switchName: string,
    dataRefNames: string | string[],
    inverse: boolean = false,
  ): void {
    this.inputMappings[`${switchName}_on`] = {
      type: 'dataref',
      xplane_actions: ensureArray(dataRefNames),
      value: inverse ? 0 : 1,
    }
    this.inputMappings[`${switchName}_off`] = {
      type: 'dataref',
      xplane_actions: ensureArray(dataRefNames),
      value: inverse ? 1 : 0,
    }
    console.log(
      `➕ [toggle switch] Arduino to X-Plane dataref mapping added: ${switchName} -> ${ensureArray(dataRefNames).join(', ')}`,
    )
  }

  /**
   * Adds a toggle switch input mapping from Arduino to trigger X-Plane commands.
   * Arduino sends on/off commands and X-Plane triggers the corresponding commands.
   * @param switchName - The name of the Arduino switch.
   * @param onCommand - The X-Plane command to trigger when the switch is turned on.
   * @param offCommand - The X-Plane command to trigger when the switch is turned off. If not provided, the onCommand will be used.
   */
  public addToggleSwitchInputCommands(
    switchName: string,
    onCommands: string | string[],
    offCommands?: string | string[] | undefined,
  ): void {
    this.inputMappings[`${switchName}_on`] = {
      type: 'command',
      xplane_actions: ensureArray(onCommands),
    }
    this.inputMappings[`${switchName}_off`] = {
      type: 'command',
      xplane_actions: ensureArray(offCommands || onCommands),
    }
    console.log(
      `➕ [toggle switch] Arduino to X-Plane command mapping added: ${switchName} -> ${ensureArray(onCommands).join(', ')}/${offCommands ? ensureArray(offCommands).join(', ') : ''}`,
    )
  }

  /**
   * Adds a momentary switch input mapping from Arduino to trigger a X-Plane command.
   * Arduino sends a single command when the switch is pressed.
   * @param switchName - The name of the Arduino switch.
   * @param command - The X-Plane command to trigger when the switch is pressed.
   */
  public addMomentarySwitchInputCommand(
    switchName: string,
    commands: string | string[],
  ): void {
    this.inputMappings[switchName] = {
      type: 'command',
      xplane_actions: ensureArray(commands),
    }

    console.log(
      `➕ [momentary switch] Arduino to X-Plane command mapping added: ${switchName} -> ${ensureArray(commands).join(', ')}`,
    )
  }

  /**
   * Adds a momentary switch input mapping from Arduino to set a X-Plane dataref.
   * Arduino sends a single command when the switch is pressed.
   * @param switchName - The name of the Arduino switch.
   * @param dataRefName - The name of the X-Plane data reference.
   * @param value - The value to set the data reference to when the switch is pressed.
   */
  public addMomentarySwitchInputDataRef(
    switchName: string,
    dataRefNames: string | string[],
    value: number | typeof TOGGLE_DATAREF,
  ): void {
    this.inputMappings[switchName] = {
      type: 'dataref',
      xplane_actions: ensureArray(dataRefNames),
      value: value,
    }

    console.log(
      `➕ [momentary switch] Arduino to X-Plane dataref mapping added: ${switchName} -> ${ensureArray(dataRefNames).join(', ')}`,
    )
  }

  /**
   * Adds a rotary encoder input mapping from Arduino to trigger X-Plane commands.
   * Arduino sends increment/decrement commands and X-Plane triggers the corresponding commands.
   * @param encoderName - The name of the Arduino rotary encoder.
   * @param incrementCommand - The X-Plane command to trigger when the encoder is turned clockwise. *_increment
   * @param decrementCommand - The X-Plane command to trigger when the encoder is turned counter-clockwise. *_decrement
   */
  public addRotaryEncoderCommands(
    encoderName: string,
    incrementCommands: string | string[],
    decrementCommands: string | string[],
  ): void {
    this.inputMappings[`${encoderName}_increment`] = {
      type: 'command',
      xplane_actions: ensureArray(incrementCommands),
    }

    this.inputMappings[`${encoderName}_decrement`] = {
      type: 'command',
      xplane_actions: ensureArray(decrementCommands),
    }

    console.log(
      `➕ [rotary encoder] Arduino to X-Plane command mapping added: ${encoderName} -> ${ensureArray(incrementCommands).join(', ')}/${ensureArray(decrementCommands).join(', ')}`,
    )
  }

  // Traducción de get_dataref_id usando fetch nativo
  async getXPlaneIdentifierId(
    type: XPlaneIdentifierType,
    name: string,
  ): Promise<number | null> {
    if (this.idCache[type].has(name)) {
      return this.idCache[type].get(name)
    }

    try {
      const url = `${this.restUrl}/${type}?filter[name]=${name}`

      const response = await fetch(url)
      const jsonResponse: any = await response.json()

      if (!jsonResponse.data || jsonResponse.data.length === 0) {
        console.warn(`[✈️] ⚠️ ${type} "${name}" not found in X-Plane API`)
        return null
      }

      const { id: identifierId } = jsonResponse.data[0]

      console.log(`[✈️] ✅ Found ${type} "${name}" ID: ${identifierId}`)
      this.idCache[type].set(name, identifierId)
      return identifierId
    } catch (error) {
      console.error(`[✈️] ❌ Error fetching ${type} "${name}" ID: `, error)
      return null
    }
  }

  async getDataRefValue(dataRefId: number, dataRefName: string): Promise<any> {
    try {
      const url = `${this.restUrl}/datarefs/${dataRefId}/value`

      const response = await fetch(url)
      const jsonResponse: any = await response.json()

      if (jsonResponse.data !== undefined) {
        console.log(
          `[✈️] ✅ Retrieved DataRef "${dataRefName}" value: ${jsonResponse.data}`,
        )
        return jsonResponse.data
      }
    } catch (error) {
      console.error(
        `[✈️] ❌ Error fetching DataRef "${dataRefName}" value: `,
        error,
      )
    }
    return null
  }

  public close() {
    this.webSocket?.close()
    this.arduino.disconnect()
  }

  public async run() {
    await this.arduino.connect()
    await new Promise((resolve) => setTimeout(resolve, 2000))
    this.initializeWebSocket()
  }

  private initializeWebSocket() {
    this.webSocket = new WebSocket(this.websocketsUrl)

    this.webSocket.on('open', async () => {
      console.log('[✈️] ✅ WebSocket connection with X-Plane established')
      await this.subscribeToAllDataReferences()
      console.log('Press Ctrl+C to exit')
      console.log('---------------------------------------------')
    })

    this.webSocket.on('message', this.processXPlaneUpdate.bind(this))

    this.webSocket.on('close', () => {
      console.warn(
        '[✈️] ⚠️ WebSocket connection closed. Reconnecting in 5 seconds...',
      )
      setTimeout(() => this.initializeWebSocket(), 5000)
    })

    this.webSocket.on('error', (error) => {
      console.error('[✈️] ❌ WebSocket error: ', error)
    })
  }

  private async subscribeToAllDataReferences(): Promise<void> {
    const subscriptionList = []

    console.log('[🏗️] Building dataref ID mappings')
    for (const key in this.dataRefMappings) {
      const dataRefId = await this.getXPlaneIdentifierId('datarefs', key)
      if (dataRefId !== null) {
        subscriptionList.push({ id: dataRefId, name: key })
      }
    }

    if (subscriptionList.length === 0) {
      console.warn('[✈️] ⚠️ No valid datarefs to monitor.')
      return
    }

    const requestId = this.sendWebSocketMessage('dataref_subscribe_values', {
      datarefs: subscriptionList.map((item) => ({ id: item.id })),
    })
    console.log(
      `[✈️] ✅ Subscribed to ${subscriptionList.length} datarefs (ID: ${requestId})`,
    )
  }

  private findDataRefNameById(dataRefId: number): string | null {
    for (const [name, cachedId] of this.idCache.datarefs) {
      if (cachedId === dataRefId) {
        return name
      }
    }
    return null
  }

  private parseValue(
    value: any,
    parserType?: ParserType | undefined,
    valueMap?: ValueMap | undefined,
  ) {
    return !parserType ? value : parserLibrary[parserType](value, valueMap)
  }

  private shouldSendUpdate(
    arduinoCmd: string,
    newValue: any,
    threshold: number,
  ): boolean {
    if (this.previousValues[arduinoCmd] === undefined) {
      return true
    }

    const oldValue = this.previousValues[arduinoCmd]

    if (typeof newValue === 'number' && typeof oldValue === 'number') {
      return Math.abs(newValue - oldValue) >= threshold
    }

    return newValue !== oldValue
  }

  private processXPlaneUpdate(rawData: RawData) {
    try {
      const message: XplaneWebsocketMessage = JSON.parse(rawData.toString())

      switch (message.type) {
        case XPlaneMessageType.RESULT:
          if (!message.success) {
            console.error(
              `[✈️] ❌ X-Plane request ${message.request_id} failed: ${message.error_code} - ${message.error_message}`,
            )
            return
          }
          console.log(`[✈️] ✅ X-Plane request ${message.request_id} succeeded`)
          break
        case XPlaneMessageType.DATAREF_UPDATE_VALUES:
          const updates = message.data || {}

          for (const dataRefId in updates) {
            const updatedValue = updates[dataRefId]
            let dataRefName: string | null = this.findDataRefNameById(
              parseInt(dataRefId),
            )

            if (!dataRefName || !this.dataRefMappings[dataRefName]) {
              return
            }

            const mapping = this.dataRefMappings[dataRefName]
            const arduinoCmd = mapping.arduino_cmd
            const parserType = mapping.parser
            const valueMap = mapping.value_map
            const threshold = mapping.threshold || 0

            let parsedValue = null
            try {
              parsedValue = this.parseValue(updatedValue, parserType, valueMap)
            } catch (error) {
              console.error(
                `[✈️] ❌ Error parsing value for "${dataRefName}": `,
                error,
              )
              continue
            }

            if (!this.shouldSendUpdate(arduinoCmd, parsedValue, threshold)) {
              continue
            }

            this.arduino.sendCommand({ cmd: arduinoCmd, value: parsedValue })
            this.previousValues[arduinoCmd] = parsedValue
          }
          break
      }
    } catch (error) {
      console.error('[✈️] ❌ Error processing X-Plane update: ', error)
      return
    }
  }

  /**
   * Executes an X-Plane command via WebSocket.
   * Duration 0 means press and release immediately.
   * @param xplaneCommands - The X-Plane commands to execute.
   * @param duration - The duration to hold the command active (in seconds).
   */
  private async executeXPlaneCommands(
    xplaneCommands: string[],
    duration: number = 0,
  ): Promise<void> {
    let commands: { id: number; is_active: boolean; duration: number }[] = []
    for (const xplaneCommand of xplaneCommands) {
      const id = await this.getXPlaneIdentifierId('commands', xplaneCommand)

      if (id === null) {
        console.error(`❌ Command "${xplaneCommand}" not found in X-Plane`)
        return
      }
      commands.push({ id, is_active: true, duration })
    }

    const requestId = this.sendWebSocketMessage(
      XPlaneMessageType.COMMAND_SET_IS_ACTIVE,
      { commands },
    )
    console.log(
      `[💻] ➡️ [✈️] X-Plane commands "${xplaneCommands.join(', ')}" (Duration ${duration}, Request ID: ${requestId})`,
    )
  }

  private async setXPlaneDataRefs(
    dataRefNames: string[],
    value: any,
  ): Promise<void> {
    let datarefs: { id: number; value: any }[] = []
    for (const dataRefName of dataRefNames) {
      const id = await this.getXPlaneIdentifierId('datarefs', dataRefName)

      if (id === null) {
        console.error(`❌ DataRef "${dataRefName}" not found in X-Plane`)
        return
      }

      if (value === TOGGLE_DATAREF) {
        const currentValue = await this.getDataRefValue(id, dataRefName)
        console.log(currentValue)
        if (Number.isInteger(currentValue)) {
          value = currentValue === 0 ? 1 : 0
        } else if (typeof currentValue === 'boolean') {
          value = !currentValue
        } else {
          console.error(
            `❌ DataRef "${dataRefName}" has non-integer/boolean value: ${currentValue}`,
          )
          return
        }
      }

      datarefs.push({ id, value })
    }

    const requestId = this.sendWebSocketMessage(
      XPlaneMessageType.DATAREF_SET_VALUES,
      { datarefs },
    )
    console.log(
      `[💻] ➡️ [✈️] X-Plane DataRef set: "${dataRefNames.join(', ')}" = ${value} (Request ID: ${requestId})`,
    )
  }

  private handleArduinoMessage(message: ArduinoMessage) {
    try {
      console.log(`[📟] ➡️ [💻]: ${JSON.stringify(message)}`)
      if (!message.user_input) {
        console.warn('[📟] ⚠️ Unknown format from Arduino')
        return
      }
      const userInput = message.user_input

      if (this.inputMappings[userInput] === undefined) {
        console.warn(`[📟] ⚠️ Unknown Arduino input: ${userInput}`)
        return
      }

      const mapping = this.inputMappings[userInput]
      const actionType = mapping.type

      if (actionType === 'command') {
        // Execute X-Plane commands (for momentary switches/buttons)
        this.executeXPlaneCommands(mapping.xplane_actions)
      } else if (actionType === 'dataref') {
        // Write X-Plane datarefs (for toggle switches)
        this.setXPlaneDataRefs(mapping.xplane_actions, mapping.value)
      } else {
        console.error(`[📟] ❌ Unknown action type: ${actionType}`)
      }
    } catch (error) {
      console.error('[📟] ❌ Error handling Arduino message: ', error)
    }
  }

  private sendWebSocketMessage(
    messageType: string,
    parameters: Record<string, any> | undefined = undefined,
  ) {
    const requestId = this.requestIdCounter++
    const payload: Record<string, any> = {
      req_id: requestId,
      type: messageType,
    }

    if (parameters) {
      payload['params'] = parameters
    }

    if (!this.webSocket || this.webSocket?.readyState !== WebSocket.OPEN) {
      return -1
    }

    this.webSocket.send(JSON.stringify(payload))
    return requestId
  }
}
