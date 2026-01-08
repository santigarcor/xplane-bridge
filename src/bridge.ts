import WebSocket from 'ws'
import fs from 'fs'
import 'dotenv/config'
import { Arduino } from './arduino.js'
import type {
  DataRefMapping,
  DataRefMappings,
  InputMapping,
  InputMappings,
  ValueMap,
  XPlaneIdentifierType,
} from './types.js'
import { ParserType } from './types.js'

const parserLibrary: Record<string, (v: any, extra?: any) => any> = {
  [ParserType.BOOLEAN]: (v) => (v > 0.5 ? 1 : 0),
  [ParserType.ROUND]: (v) => Math.round(v),
  [ParserType.TO_DEGREES]: (v) => Math.round(v * (180 / Math.PI)),
  [ParserType.VALUE_MAP]: (v, map) =>
    map && map[v] !== undefined ? map[v] : v,
}

export class XPlaneArduinoBridge {
  private webSocket: WebSocket | null = null
  private arduino: Arduino
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

    this.arduino = new Arduino(
      parseInt(process.env.ARDUINO_BAUD || '9600'),
      (data) => this.handleArduinoMessage(data),
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
    dataRefName: string,
  ): void {
    this.inputMappings[`${switchName}_on`] = {
      type: 'dataref',
      xplane_action: dataRefName,
      value: 1,
    }
    this.inputMappings[`${switchName}_off`] = {
      type: 'dataref',
      xplane_action: dataRefName,
      value: 0,
    }
    console.log(
      `➕ [toggle switch] Arduino to X-Plane dataref mapping added: ${switchName} -> ${dataRefName}`,
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
    onCommand: string,
    offCommand?: string | undefined,
  ): void {
    this.inputMappings[`${switchName}_on`] = {
      type: 'command',
      xplane_action: onCommand,
    }
    this.inputMappings[`${switchName}_off`] = {
      type: 'command',
      xplane_action: offCommand || onCommand,
    }
    console.log(
      `➕ [toggle switch] Arduino to X-Plane command mapping added: ${switchName} -> ${onCommand}/${offCommand}`,
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
    command: string,
  ): void {
    this.inputMappings[switchName] = {
      type: 'command',
      xplane_action: command,
    }

    console.log(
      `➕ [momentary switch] Arduino to X-Plane command mapping added: ${switchName} -> ${command}`,
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
    dataRefName: string,
    value: number,
  ): void {
    this.inputMappings[switchName] = {
      type: 'dataref',
      xplane_action: dataRefName,
      value: value,
    }

    console.log(
      `➕ [momentary switch] Arduino to X-Plane dataref mapping added: ${switchName} -> ${dataRefName}`,
    )
  }

  /**
   * Adds a rotary encoder input mapping from Arduino to trigger X-Plane commands.
   * Arduino sends increment/decrement commands and X-Plane triggers the corresponding commands.
   * @param encoderName - The name of the Arduino rotary encoder.
   * @param incrementCommand - The X-Plane command to trigger when the encoder is turned clockwise.
   * @param decrementCommand - The X-Plane command to trigger when the encoder is turned counter-clockwise.
   */
  public addRotaryEncoderCommands(
    encoderName: string,
    incrementCommand: string,
    decrementCommand: string,
  ): void {
    this.inputMappings[`${encoderName}_increment`] = {
      type: 'command',
      xplane_action: incrementCommand,
    }

    this.inputMappings[`${encoderName}_decrement`] = {
      type: 'command',
      xplane_action: decrementCommand,
    }

    console.log(
      `➕ [rotary encoder] Arduino to X-Plane command mapping added: ${encoderName} -> ${incrementCommand}/${decrementCommand}`,
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

  // async run() {
  //   await this.arduino.connect()
  //   this.initializeWebSocket()
  // }

  // private initializeWebSocket() {
  //   const url = `ws://${process.env.XPLANE_HOST}:${process.env.XPLANE_PORT}/api/v2`
  //   this.webSocket = new WebSocket(url)

  //   this.webSocket.on("open", async () => {
  //     console.log("✓ Conexión WebSocket con X-Plane establecida")
  //     await this.subscribeToAllDataReferences()
  //   })

  //   this.webSocket.on("message", (messageBuffer) => {
  //     const messageData = JSON.parse(messageBuffer.toString())
  //     if (messageData.type === "dataref_update_values") {
  //       this.processXPlaneUpdate(messageData.data)
  //     }
  //   })

  //   this.webSocket.on("close", () => {
  //     console.log("⚠️ Conexión con X-Plane perdida. Reconectando...")
  //     setTimeout(() => this.initializeWebSocket(), 5000)
  //   })
  // }

  // private async subscribeToAllDataReferences() {
  //   const subscriptionList = []

  //   for (const config of this.dataReferenceConfigurations) {
  //     const internalId = await this.getXPlaneInternalIdentifier(
  //       "datarefs",
  //       config.name,
  //     )
  //     if (internalId) {
  //       this.dataReferenceIdentifierCache.set(internalId, config)
  //       this.dataReferenceNameToIdentifier.set(config.name, internalId)
  //       subscriptionList.push({ id: internalId })
  //     }
  //   }

  //   this.sendWebSocketMessage("dataref_subscribe_values", {
  //     datarefs: subscriptionList,
  //   })
  // }

  // private processXPlaneUpdate(updates: Record<string, number>) {
  //   for (const [identifierString, rawValue] of Object.entries(updates)) {
  //     const internalId = parseInt(identifierString)
  //     const config = this.dataReferenceIdentifierCache.get(internalId)

  //     if (!config) continue

  //     // Lógica de transformación de valores (Booleanos y Enums)
  //     let processedValue = rawValue
  //     if (config.type === "boolean") {
  //       processedValue = rawValue > 0.5 ? 1 : 0
  //     } else if (config.type === "enum" && config.mapping) {
  //       const roundedStringValue = Math.round(rawValue).toString()
  //       processedValue =
  //         config.mapping[roundedStringValue] !== undefined
  //           ? config.mapping[roundedStringValue]
  //           : rawValue
  //     }

  //     const threshold = config.threshold || 0
  //     const previousValue = this.previousValuesCache.get(config.command)

  //     // Verificación de umbral (Threshold)
  //     const hasChangedSignificantly =
  //       previousValue === undefined ||
  //       Math.abs(Number(processedValue) - Number(previousValue)) >= threshold

  //     if (hasChangedSignificantly) {
  //       this.arduino.sendJson({ cmd: config.command, value: processedValue })
  //       this.previousValuesCache.set(config.command, processedValue)
  //     }
  //   }
  // }

  // private async handleArduinoMessage(data: any) {
  //   const userInputKey = data.user_input
  //   const mapping = this.userInputMappings[userInputKey]
  //   if (!mapping) return

  //   if (mapping.type === "command") {
  //     const commandId = await this.getXPlaneInternalIdentifier(
  //       "commands",
  //       mapping.action,
  //     )
  //     if (commandId) {
  //       this.sendWebSocketMessage("command_set_is_active", {
  //         commands: [{ id: commandId, is_active: true, duration: 0 }],
  //       })
  //     }
  //   } else if (mapping.type === "dataref") {
  //     const dataReferenceId = await this.getXPlaneInternalIdentifier(
  //       "datarefs",
  //       mapping.action,
  //     )
  //     if (dataReferenceId) {
  //       this.sendWebSocketMessage("dataref_set_values", {
  //         datarefs: [{ id: dataReferenceId, value: mapping.value }],
  //       })
  //     }
  //   }
  // }

  // private sendWebSocketMessage(messageType: string, parameters: any) {
  //   if (this.webSocket && this.webSocket.readyState === WebSocket.OPEN) {
  //     this.webSocket.send(
  //       JSON.stringify({
  //         req_id: this.requestIdentifierCounter++,
  //         type: messageType,
  //         params: parameters,
  //       }),
  //     )
  //   }
  // }
}
