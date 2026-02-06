import express from 'express'
import { createServer, Server as HttpServer } from 'node:http'
import { WebSocketServer, WebSocket, type RawData } from 'ws'
import path from 'node:path'
import type { Communicator, IncomingMessage, OutgoingMessage } from './types.js'

export class WebCockpitServiceCommunicator implements Communicator {
  private app = express()
  private server: HttpServer
  private wss: WebSocketServer
  private onMessageReceived: (data: IncomingMessage) => void = () => {}

  constructor(
    private port: number = 8080,
    private __dirname: string,
  ) {
    this.server = createServer(this.app)
    this.wss = new WebSocketServer({ server: this.server })
  }

  public onMessage(onMessageReceived: (data: IncomingMessage) => void) {
    this.onMessageReceived = onMessageReceived
  }

  async connect(): Promise<void> {
    // 1. Serve the static files for the web cockpit (Vue app) from the public directory.
    const publicPath = path.join(this.__dirname, '..', 'public')
    this.app.use(express.static(publicPath))
    console.log(`[🌐] Serving WebCockpit static files from ${publicPath}`)

    // 2. Handle WebSocket connections
    this.wss.on('connection', (ws: WebSocket) => {
      console.log('[ 📱🔗🌐 ] Device connected to WebCockpit')

      ws.on('message', (rawData: RawData) => {
        try {
          const data: IncomingMessage = JSON.parse(rawData.toString())
          this.onMessageReceived(data)
        } catch (e) {
          console.error('[🌐] Error parsing web command:', e)
        }
      })
    })

    this.server.listen(this.port, '0.0.0.0', () => {
      console.log(
        `[🌐] Servidor Web/FMC listo en http://localhost:${this.port}`,
      )
    })
  }

  /**
   * Sends data from the Bridge to the mobile device (Screen update)
   */
  public sendMessage(data: OutgoingMessage) {
    this.wss.clients.forEach((client: WebSocket) => {
      if (client.readyState === WebSocket.OPEN) {
        client.send(JSON.stringify(data))
        console.log(`[⇨ 📱]Sent command to WebCockpit: ${JSON.stringify(data)}`)
      }
    })
  }

  public disconnect() {
    this.wss.close()
    this.server.close()
  }
}
