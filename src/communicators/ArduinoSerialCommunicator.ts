import { SerialPort } from 'serialport'
import { ReadlineParser } from '@serialport/parser-readline'
import type { OutgoingMessage, IncomingMessage, Communicator } from './types.js'

/** Printed by the sketch at the end of setup() */
const BOOT_BANNER = 'Ready for input'
/** The sketch sends a heartbeat every 2s, so this much silence means the link is dead */
const LIVENESS_TIMEOUT = 6000
const LIVENESS_CHECK_INTERVAL = 1000
/** Silences longer than this are logged once data resumes */
const SILENCE_LOG_THRESHOLD = 3000
/** How long close() may take to call back before the USB link is logged as not responding */
const CLOSE_TIMEOUT = 3000
const RECONNECT_DELAY = 5000

export class ArduinoSerialCommunicator implements Communicator {
  private serialPort: SerialPort | null = null
  private isConnectedFlag: boolean = false
  /** Set while connect() looks for the port, so two calls can't overlap */
  private isConnecting: boolean = false

  private reconnectTimer: NodeJS.Timeout | null = null
  private livenessTimer: NodeJS.Timeout | null = null

  /** Time of the last line received from Arduino */
  private lastRx: number = 0
  /** Uptime reported by the last heartbeat, null until the first one after a (re)boot */
  private lastHb: number | null = null

  /**
   * Callback that will be executed when Arduino sends a user interaction
   */
  private onMessageReceived: (data: IncomingMessage) => void = () => {}
  private onConnectionStablished: (connection: Communicator) => void = () => {}

  constructor(
    private baudRate: number = 57600,
    private reconnect: boolean = true,
  ) {}

  onMessage(onMessageReceived: (data: IncomingMessage) => void): this {
    this.onMessageReceived = onMessageReceived
    return this
  }

  onConnection(onNewConnection: (connection: Communicator) => void): this {
    this.onConnectionStablished = onNewConnection
    return this
  }

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
    if (this.serialPort || this.isConnecting) return

    // Covers the await below, until this.serialPort is assigned or a reconnect is scheduled
    this.isConnecting = true
    let portPath: string | null = null
    try {
      portPath = await this.autodiscoverPort()
    } catch (error) {
      console.error(`[📟] ❌ Error listing serial ports: ${error}`)
    }
    this.isConnecting = false

    if (!portPath) {
      console.error('[📟] ❌ Arduino not found. Retrying in 5 seconds...')
      this.scheduleReconnection()
      return
    }

    const port = new SerialPort({
      path: portPath,
      baudRate: this.baudRate,
      autoOpen: false,
    })
    this.serialPort = port
    this.lastHb = null

    const parser = port.pipe(new ReadlineParser({ delimiter: '\n' }))

    // Every handler ignores events from old port instances (port !== this.serialPort)
    port.open((error) => {
      if (port !== this.serialPort) return

      if (error) {
        console.error(`[📟] ❌ Error opening port "${error.message}"`)
        this.dropPort()
        this.scheduleReconnection()
        return
      }

      // Opening the port resets the Arduino through DTR, wait for setup() to finish.
      // The liveness check also covers the boot, in case the board never starts sending.
      console.log(`[📟] 🔌 Port "${portPath}" opened, waiting for Arduino to boot...`)
      this.lastRx = Date.now()
      this.startLivenessCheck(port)
    })

    parser.on('data', (line: string) => {
      if (port !== this.serialPort) return
      this.handleLine(port, line.trim())
    })

    port.on('error', (error) => {
      if (port !== this.serialPort) return
      console.error(`[📟] ❌ Serial port error: ${error.message}`)
    })

    port.on('close', () => {
      if (port !== this.serialPort) return
      console.warn('[📟] ⚠️ Serial connection lost. Reconnecting...')
      this.dropPort()
      this.scheduleReconnection()
    })
  }

  private handleLine(port: SerialPort, line: string) {
    const now = Date.now()
    const silence = now - this.lastRx
    if (this.isConnectedFlag && silence > SILENCE_LOG_THRESHOLD) {
      console.warn(`[📟] ⚠️ Link silent for ${(silence / 1000).toFixed(1)}s`)
    }
    this.lastRx = now

    // Lines right after a reset can carry garbage from the bootloader, so don't match exactly
    if (line.includes(BOOT_BANNER)) {
      if (this.isConnectedFlag) {
        this.handleReset('boot banner')
      } else {
        this.markConnected(port)
      }
      return
    }

    let message: any
    try {
      message = JSON.parse(line)
    } catch {
      message = null
    }

    if (typeof message !== 'object' || message === null) {
      if (line) console.log(`[📟] text: ${line}`)
      return
    }

    if (message.hb !== undefined) {
      // A heartbeat also proves the sketch is running, in case the banner got lost
      if (!this.isConnectedFlag) this.markConnected(port)
      this.handleHeartbeat(message.hb)
      return
    }

    this.onMessageReceived(message)
  }

  private handleHeartbeat(uptime: number) {
    if (this.isConnectedFlag && this.lastHb !== null && uptime < this.lastHb) {
      this.handleReset(`uptime went from ${this.lastHb}s to ${uptime}s`)
    }
    this.lastHb = uptime
  }

  private handleReset(reason: string) {
    console.warn(`[📟] ⚠️ Arduino reset detected (${reason}). Resending state...`)
    // So the banner and a later backwards uptime don't both fire for the same reset
    this.lastHb = null
    this.onConnectionStablished(this)
  }

  private markConnected(port: SerialPort) {
    console.log(`[📟] ✅ Connected to Arduino on port "${port.path}"`)
    this.isConnectedFlag = true
    this.onConnectionStablished(this)
  }

  private startLivenessCheck(port: SerialPort) {
    this.clearTimer('livenessTimer')
    this.livenessTimer = setInterval(() => {
      if (port !== this.serialPort) return

      const silence = Date.now() - this.lastRx
      if (silence <= LIVENESS_TIMEOUT) return

      console.error(
        `[📟] ❌ No data for ${(silence / 1000).toFixed(1)}s, closing port`,
      )
      this.closeSilentPort(port)
    }, LIVENESS_CHECK_INTERVAL)
  }

  /**
   * Drops a port that stopped sending data and reconnects. Reopening resets the Arduino through DTR.
   */
  private closeSilentPort(port: SerialPort) {
    this.dropPort()
    this.scheduleReconnection()

    // If the USB driver stopped responding, close() may never call back. Only log it,
    // the reconnect is already scheduled.
    const notRespondingTimer = setTimeout(() => {
      console.error(
        '[📟] ❌ USB link not responding: the port did not close. Replug the USB cable if the panel does not recover',
      )
    }, CLOSE_TIMEOUT)

    port.close((error) => {
      clearTimeout(notRespondingTimer)
      if (error) console.error(`[📟] ❌ Error closing port: ${error.message}`)
    })
  }

  /**
   * Forgets the current port instance, so any late events from it are ignored.
   */
  private dropPort() {
    this.clearTimer('livenessTimer')
    this.isConnectedFlag = false
    this.serialPort = null
  }

  private clearTimer(timer: 'reconnectTimer' | 'livenessTimer') {
    if (this[timer]) {
      clearTimeout(this[timer])
      this[timer] = null
    }
  }

  private scheduleReconnection() {
    if (!this.reconnect || this.reconnectTimer) return

    this.reconnectTimer = setTimeout(() => {
      this.reconnectTimer = null
      this.connect()
    }, RECONNECT_DELAY)
  }

  public sendMessage(data: OutgoingMessage): void {
    // Only write once the Arduino finished booting, so nothing piles up on a booting or unresponsive port
    if (!this.serialPort || !this.isConnectedFlag) return

    this.serialPort.write(JSON.stringify(data) + '\n', (error) => {
      if (error) console.error(`[📟] ❌ Error writing to port: ${error.message}`)
    })
    console.log(`[⇨ 📟]Sent command to Arduino: ${JSON.stringify(data)}`)
  }

  public disconnect(): void {
    this.reconnect = false
    this.clearTimer('reconnectTimer')

    const port = this.serialPort
    this.dropPort()

    if (!port || !port.isOpen) {
      return
    }

    port.close((error) => {
      if (error) {
        console.error(`[📟] ❌ Error closing port: ${error.message}`)
      } else {
        console.log('[📟] ✅ Serial port closed successfully.')
      }
    })
  }

  public isConnected(): boolean {
    return this.isConnectedFlag
  }
}
