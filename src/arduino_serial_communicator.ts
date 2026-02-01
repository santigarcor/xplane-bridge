import { SerialPort } from 'serialport'
import { ReadlineParser } from '@serialport/parser-readline'
import type { ArduinoCommand, ArduinoMessage } from './types.js'

export class ArduinoSerialCommunicator {
  private serialPort: SerialPort | null = null
  private parser: ReadlineParser | null = null
  private isAttemptingConnection: boolean = false

  constructor(
    private baudRate: number = 9600,
    private onDataReceived: (data: ArduinoMessage) => void = () => {},
    private reconnect: boolean = true,
  ) {}

  async autodiscoverPort(): Promise<string | null> {
    const availablePorts = await SerialPort.list()
    const detectedPort = availablePorts.find(
      (port) =>
        /arduino|usb|serial/i.test(port.manufacturer || '') ||
        /arduino|usb|serial/i.test(port.pnpId || ''),
    )
    return detectedPort ? detectedPort.path : null
  }

  async connect(): Promise<void> {
    if (this.isAttemptingConnection) return
    this.isAttemptingConnection = true

    const portPath = await this.autodiscoverPort()

    if (!portPath) {
      console.error('[📟] ❌ Arduino not found. Retrying in 5 seconds...')
      this.scheduleReconnection()
      return
    }

    this.serialPort = new SerialPort({
      path: portPath,
      baudRate: this.baudRate,
      autoOpen: false,
    })

    this.parser = this.serialPort.pipe(new ReadlineParser({ delimiter: '\n' }))

    this.serialPort.open((error) => {
      if (error) {
        console.error(`[📟] ❌ Error opening port "${error.message}"`)
        this.scheduleReconnection()
        return
      }

      // Wait 2 seconds for Arduino reset
      setTimeout(() => {
        console.log(`[📟] ✅ Connected to Arduino on port "${portPath}"`)
        this.isAttemptingConnection = false
      }, 2000)
    })

    this.parser.on('data', (line: string) => {
      try {
        const jsonObject: ArduinoMessage = JSON.parse(line)
        this.onDataReceived(jsonObject)
      } catch (error) {
        // Ignore if not valid JSON (Arduino text logs)
      }
    })

    this.serialPort.on('close', () => {
      console.warn('[📟] ⚠️ Serial connection lost. Reconnecting...')
      this.scheduleReconnection()
    })
  }

  private scheduleReconnection() {
    if (!this.reconnect) return

    this.isAttemptingConnection = true
    setTimeout(() => {
      this.isAttemptingConnection = false
      this.connect()
    }, 5000)
  }

  public sendCommand(data: ArduinoCommand): void {
    if (this.serialPort && this.serialPort.isOpen) {
      this.serialPort.write(JSON.stringify(data) + '\n')
      console.log(`[⇨ 📟]Sent command to Arduino: ${JSON.stringify(data)}`)
    }
  }

  public disconnect(): void {
    if (!this.serialPort || !this.serialPort.isOpen) {
      return
    }

    this.reconnect = false
    this.serialPort.close((error) => {
      if (error) {
        console.error(`[📟] ❌ Error closing port: ${error.message}`)
      } else {
        console.log('[📟] ✅ Serial port closed successfully.')
      }
    })
  }
}
