import { XPlaneArduinoBridge } from '../bridge.js'
import { ParserType, TOGGLE_DATAREF } from '../types.js'

export function initializeMappings(bridge: XPlaneArduinoBridge): void {
  /**
   * DISPLAYS
   */
  bridge.addDataRef('1-sim/AP/spdSetting', {
    arduino_cmd: 'set_speed',
    threshold: 1,
    parser: ParserType.ROUND,
  })
  bridge.addDataRef('1-sim/AP/dig3/hdgSetting', {
    arduino_cmd: 'set_heading',
    threshold: 1,
    parser: ParserType.ROUND,
  })
  bridge.addDataRef('1-sim/AP/altSetting', {
    arduino_cmd: 'set_altitude',
    threshold: 100,
    parser: ParserType.ROUND,
  })
  bridge.addDataRef('1-sim/AP/vviSetting', {
    arduino_cmd: 'set_vertical_speed',
    threshold: 50,
    parser: ParserType.ROUND,
  })

  /**
   * ENCODERS
   */
  bridge.addRotaryEncoderCommands(
    'speed_encoder',
    '1-sim/comm/AP/spdUP',
    '1-sim/comm/AP/spdDN',
  )
  bridge.addRotaryEncoderCommands(
    'heading_encoder',
    '1-sim/comm/AP/hdgUP',
    '1-sim/comm/AP/hdgDN',
  )
  bridge.addRotaryEncoderCommands(
    'altitude_encoder',
    '1-sim/comm/AP/altUP',
    '1-sim/comm/AP/altDN',
  )
  bridge.addRotaryEncoderCommands(
    'vertical_speed_encoder',
    '1-sim/comm/AP/vviUP',
    '1-sim/comm/AP/vviDN',
  )

  /**
   * SWITCHES
   */
  bridge.addToggleSwitchInputDataRef(
    'at_arm',
    ['1-sim/AP/atSwitcher', '1-sim/AP/atSwitcher/anim'],
    true,
  )
  bridge.addToggleSwitchInputDataRef(
    'flight_director',
    ['1-sim/AP/fd1Switcher', '1-sim/AP/fd1Switcher/anim'],
    true,
  )
  bridge.addToggleSwitchInputDataRef('landing_l', [
    '1-sim/lights/landingL/switch',
    '1-sim/lights/landingL/switch/anim',
  ])
  bridge.addToggleSwitchInputDataRef('landing_r', [
    '1-sim/lights/landingR/switch',
    '1-sim/lights/landingR/switch/anim',
  ])
  bridge.addToggleSwitchInputDataRef('runway_l', [
    '1-sim/lights/runwayL/switch',
    '1-sim/lights/runwayL/switch/anim',
  ])
  bridge.addToggleSwitchInputDataRef('runway_r', [
    '1-sim/lights/runwayR/switch',
    '1-sim/lights/runwayR/switch/anim',
  ])
  bridge.addToggleSwitchInputDataRef('taxi', [
    '1-sim/lights/landingN/switch',
    '1-sim/lights/landingN/switch/anim',
  ])
  bridge.addToggleSwitchInputDataRef('position_steady', [
    'anim/43/button',
    'anim/43/button/anim',
  ])
  bridge.addToggleSwitchInputDataRef('anti_col', [
    'anim/44/button',
    'anim/44/button/anim',
  ])
  bridge.addToggleSwitchInputDataRef('wing', [
    'anim/46/button',
    'anim/46/button/anim',
  ])
  bridge.addToggleSwitchInputDataRef('logo', [
    'anim/45/button',
    'anim/45/button/anim',
  ])
  bridge.addToggleSwitchInputDataRef('disengage', [
    '1-sim/AP/desengageLever',
    '1-sim/AP/desengageLever/anim',
  ])

  /**
   * MOMENTARY SWITCHES
   */

  bridge.addMomentarySwitchInputDataRef(
    'speed_hold',
    '1-sim/AP/spdButton',
    TOGGLE_DATAREF,
  )
  bridge.addMomentarySwitchInputDataRef(
    'heading_hold',
    '1-sim/AP/hdgHoldButton',
    TOGGLE_DATAREF,
  )
  bridge.addMomentarySwitchInputDataRef(
    'l_nav',
    '1-sim/AP/lnavButton',
    TOGGLE_DATAREF,
  )
  bridge.addMomentarySwitchInputDataRef(
    'v_nav',
    '1-sim/AP/vnavButton',
    TOGGLE_DATAREF,
  )
  bridge.addMomentarySwitchInputDataRef(
    'altitude_hold',
    '1-sim/AP/altHoldButton',
    TOGGLE_DATAREF,
  )
  bridge.addMomentarySwitchInputDataRef(
    'vertical_speed_hold',
    '1-sim/AP/vviButton',
    TOGGLE_DATAREF,
  )
  bridge.addMomentarySwitchInputDataRef(
    'app',
    '1-sim/AP/appButton',
    TOGGLE_DATAREF,
  )
  bridge.addMomentarySwitchInputDataRef(
    'loc',
    '1-sim/AP/locButton',
    TOGGLE_DATAREF,
  )
  bridge.addMomentarySwitchInputDataRef(
    'cmd',
    '1-sim/AP/cmd_L_Button',
    TOGGLE_DATAREF,
  )

  /**
   * LEDS
   */
  bridge.addBooleanDataRef('1-sim/AP/lamp/6', 'heading_led')
  bridge.addBooleanDataRef('1-sim/AP/lamp/8', 'altitude_led')
  bridge.addBooleanDataRef('1-sim/AP/lamp/7', 'vertical_speed_led')
  bridge.addBooleanDataRef('1-sim/AP/lamp/3', 'l_nav_led')
  bridge.addBooleanDataRef('1-sim/AP/lamp/4', 'v_nav_led')
  bridge.addBooleanDataRef('1-sim/AP/lamp/10', 'loc_led')
  bridge.addBooleanDataRef('1-sim/AP/lamp/11', 'app_led')
  bridge.addBooleanDataRef('1-sim/AP/lamp/12', 'cmd_led')
}

// position_strobe

// speed_led
