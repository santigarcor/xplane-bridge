import { XPlaneArduinoBridge } from '../bridge.js'
import { ParserType, TOGGLE_DATAREF } from '../types.js'

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

  bridge.addMomentarySwitchInputDataRef(
    'fd_toggle',
    // ['1-sim/AP/fd1Switcher', '1-sim/AP/fd1Switcher/anim'],
    ['anim/43/button', 'anim/43/button/anim'],
    TOGGLE_DATAREF,
  )
}

// B757

// ALTITUDE:
// Dataref:
// 1-sim/AP/altSetting
// 1-sim/AP/altHoldButton
// Commands
// 1-sim/comm/AP/altDN
// 1-sim/comm/AP/altUP

// HEADING:
// Dataref:
// 1-sim/AP/dig3/hdgSetting
// 1-sim/AP/hdgHoldButton
// Commands
// 1-sim/comm/AP/hdgDN
// 1-sim/comm/AP/hdgUP

// APP
// Dataref:
// 1-sim/AP/appButton

// V/S
// Dataref:
// 1-sim/AP/vviButton
// 1-sim/AP/vviSetting
// Commands
// 1-sim/comm/AP/vviUP
// 1-sim/comm/AP/vviDN

// CMD
// Dataref:
// 1-sim/AP/cmd_L_Button

// LNAV
// Dataref:
// 1-sim/AP/lnavButton

// VNAV
// Dataref:
// 1-sim/AP/vnavButton

// SPD
// Dataref:
// 1-sim/AP/spdSetting
// 1-sim/AP/spdButton
// Commands
// 1-sim/comm/AP/spdDN
// 1-sim/comm/AP/spdUP

// F/D
// Dataref:
// 1-sim/AP/fd1Switcher

// A/T
// Dataref:
// 1-sim/AP/atSwitcher

// Disengage A/P
// Dataref:
// 1-sim/AP/desengageLever

// LIGHTS
// Dataref:
// 1-sim/lights/runwayL/switch
// 1-sim/lights/runwayL/switch/anim
// 1-sim/lights/runwayR/switch
// 1-sim/lights/runwayR/switch/anim
// 1-sim/lights/landingR/switch
// 1-sim/lights/landingR/switch/anim
// 1-sim/lights/landingL/switch
// 1-sim/lights/landingL/switch/anim
// 1-sim/lights/landingN/switch
// 1-sim/lights/landingN/switch/anim
// Position
// anim/43/button
// anim/43/button/anim
// Red
// anim/44/button
// anim/44/button/anim
// white
// anim/45/button
// anim/45/button/anim
// anim/46/button
// anim/46/button/anim
