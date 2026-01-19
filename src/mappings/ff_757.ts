import { XPlaneArduinoBridge } from '../bridge.js'
import { ParserType } from '../types.js'

export function initializeMappings(bridge: XPlaneArduinoBridge): void {
  bridge.addDataRef('1-sim/AP/altSetting', {
    arduino_cmd: 'set_alt',
    threshold: 100,
    parser: ParserType.ROUND,
  })
  bridge.addRotaryEncoderCommands(
    'altitude_encoder',
    '1-sim/comm/AP/altUP',
    '1-sim/comm/AP/altDN',
  )

  bridge.addDataRef('1-sim/AP/dig3/hdgSetting', {
    arduino_cmd: 'set_hdg',
    threshold: 1,
    parser: ParserType.ROUND,
  })
  bridge.addRotaryEncoderCommands(
    'heading_encoder',
    '1-sim/comm/AP/hdgUP',
    '1-sim/comm/AP/hdgDN',
  )
}

// B757
// Dataref:
// 1-sim/AP/altSetting
// 1-sim/AP/dig3/hdgSetting

// Commands
// 1-sim/comm/AP/altDN
// 1-sim/comm/AP/altUP

// 1-sim/comm/AP/hdgDN
// 1-sim/comm/AP/hdgUP

1 - sim / AP / hdgHoldButton
