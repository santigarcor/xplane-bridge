import { XPlaneBridge, ParserType, TOGGLE_DATAREF } from '../bridge/index.js'

export function initializeMappings(bridge: XPlaneBridge): void {
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
  bridge.addDataRef('sim/cockpit2/autopilot/vvi_status', {
    arduino_cmd: 'toggle_display_set_vertical_speed',
    threshold: 1,
    parser: ParserType.BOOLEAN,
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
  bridge.addBooleanDataRef('1-sim/AP/lamp/2', 'speed_led')
  bridge.addBooleanDataRef('1-sim/AP/lamp/6', 'heading_led')
  bridge.addBooleanDataRef('1-sim/AP/lamp/8', 'altitude_led')
  bridge.addBooleanDataRef('1-sim/AP/lamp/7', 'vertical_speed_led')
  bridge.addBooleanDataRef('1-sim/AP/lamp/3', 'l_nav_led')
  bridge.addBooleanDataRef('1-sim/AP/lamp/4', 'v_nav_led')
  bridge.addBooleanDataRef('1-sim/AP/lamp/10', 'loc_led')
  bridge.addBooleanDataRef('1-sim/AP/lamp/11', 'app_led')
  bridge.addBooleanDataRef('1-sim/AP/lamp/12', 'cmd_led')

  /**
   * FMC
   */
  bridge.addDataRef('1-sim/cduL/display/symbols', {
    web_cockpit_cmd: 'set_fmc_text',
    threshold: 0,
    parser: ParserType.BASE64DECODE,
  })
  bridge.addDataRef('1-sim/cduL/display/symbolsColor', {
    web_cockpit_cmd: 'set_fmc_colors',
    threshold: 0,
    parser: ParserType.NONE,
  })
  bridge.addDataRef('1-sim/cduL/display/symbolsSize', {
    web_cockpit_cmd: 'set_fmc_sizes',
    threshold: 0,
    parser: ParserType.NONE,
  })
  bridge.addMomentarySwitchInputDataRef('LSK_L1', '757Avionics/CDU/LLSK1', 1)
  bridge.addMomentarySwitchInputDataRef('LSK_L2', '757Avionics/CDU/LLSK2', 1)
  bridge.addMomentarySwitchInputDataRef('LSK_L3', '757Avionics/CDU/LLSK3', 1)
  bridge.addMomentarySwitchInputDataRef('LSK_L4', '757Avionics/CDU/LLSK4', 1)
  bridge.addMomentarySwitchInputDataRef('LSK_L5', '757Avionics/CDU/LLSK5', 1)
  bridge.addMomentarySwitchInputDataRef('LSK_L6', '757Avionics/CDU/LLSK6', 1)
  bridge.addMomentarySwitchInputDataRef('LSK_R1', '757Avionics/CDU/RLSK1', 1)
  bridge.addMomentarySwitchInputDataRef('LSK_R2', '757Avionics/CDU/RLSK2', 1)
  bridge.addMomentarySwitchInputDataRef('LSK_R3', '757Avionics/CDU/RLSK3', 1)
  bridge.addMomentarySwitchInputDataRef('LSK_R4', '757Avionics/CDU/RLSK4', 1)
  bridge.addMomentarySwitchInputDataRef('LSK_R5', '757Avionics/CDU/RLSK5', 1)
  bridge.addMomentarySwitchInputDataRef('LSK_R6', '757Avionics/CDU/RLSK6', 1)
  bridge.addMomentarySwitchInputDataRef('A', '757Avionics/CDU/A', 1)
  bridge.addMomentarySwitchInputDataRef('B', '757Avionics/CDU/B', 1)
  bridge.addMomentarySwitchInputDataRef('C', '757Avionics/CDU/C', 1)
  bridge.addMomentarySwitchInputDataRef('D', '757Avionics/CDU/D', 1)
  bridge.addMomentarySwitchInputDataRef('E', '757Avionics/CDU/E', 1)
  bridge.addMomentarySwitchInputDataRef('F', '757Avionics/CDU/F', 1)
  bridge.addMomentarySwitchInputDataRef('G', '757Avionics/CDU/G', 1)
  bridge.addMomentarySwitchInputDataRef('H', '757Avionics/CDU/H', 1)
  bridge.addMomentarySwitchInputDataRef('I', '757Avionics/CDU/I', 1)
  bridge.addMomentarySwitchInputDataRef('J', '757Avionics/CDU/J', 1)
  bridge.addMomentarySwitchInputDataRef('K', '757Avionics/CDU/K', 1)
  bridge.addMomentarySwitchInputDataRef('L', '757Avionics/CDU/L', 1)
  bridge.addMomentarySwitchInputDataRef('M', '757Avionics/CDU/M', 1)
  bridge.addMomentarySwitchInputDataRef('N', '757Avionics/CDU/N', 1)
  bridge.addMomentarySwitchInputDataRef('O', '757Avionics/CDU/O', 1)
  bridge.addMomentarySwitchInputDataRef('P', '757Avionics/CDU/P', 1)
  bridge.addMomentarySwitchInputDataRef('Q', '757Avionics/CDU/Q', 1)
  bridge.addMomentarySwitchInputDataRef('R', '757Avionics/CDU/R', 1)
  bridge.addMomentarySwitchInputDataRef('S', '757Avionics/CDU/S', 1)
  bridge.addMomentarySwitchInputDataRef('T', '757Avionics/CDU/T', 1)
  bridge.addMomentarySwitchInputDataRef('U', '757Avionics/CDU/U', 1)
  bridge.addMomentarySwitchInputDataRef('V', '757Avionics/CDU/V', 1)
  bridge.addMomentarySwitchInputDataRef('W', '757Avionics/CDU/W', 1)
  bridge.addMomentarySwitchInputDataRef('X', '757Avionics/CDU/X', 1)
  bridge.addMomentarySwitchInputDataRef('Y', '757Avionics/CDU/Y', 1)
  bridge.addMomentarySwitchInputDataRef('Z', '757Avionics/CDU/Z', 1)
  bridge.addMomentarySwitchInputDataRef('SP', '757Avionics/CDU/space', 1)
  bridge.addMomentarySwitchInputDataRef('DEL', '757Avionics/CDU/delete', 1)
  bridge.addMomentarySwitchInputDataRef('/', '757Avionics/CDU/slash', 1)
  bridge.addMomentarySwitchInputDataRef('CLR', '757Avionics/CDU/clear', 1)
  bridge.addMomentarySwitchInputDataRef('1', '757Avionics/CDU/1', 1)
  bridge.addMomentarySwitchInputDataRef('2', '757Avionics/CDU/2', 1)
  bridge.addMomentarySwitchInputDataRef('3', '757Avionics/CDU/3', 1)
  bridge.addMomentarySwitchInputDataRef('4', '757Avionics/CDU/4', 1)
  bridge.addMomentarySwitchInputDataRef('5', '757Avionics/CDU/5', 1)
  bridge.addMomentarySwitchInputDataRef('6', '757Avionics/CDU/6', 1)
  bridge.addMomentarySwitchInputDataRef('7', '757Avionics/CDU/7', 1)
  bridge.addMomentarySwitchInputDataRef('8', '757Avionics/CDU/8', 1)
  bridge.addMomentarySwitchInputDataRef('9', '757Avionics/CDU/9', 1)
  bridge.addMomentarySwitchInputDataRef('.', '757Avionics/CDU/point', 1)
  bridge.addMomentarySwitchInputDataRef('0', '757Avionics/CDU/0', 1)
  bridge.addMomentarySwitchInputDataRef('+/-', '757Avionics/CDU/plusminus', 1)
  bridge.addMomentarySwitchInputDataRef(
    'INIT_REF',
    '757Avionics/CDU/init_ref',
    1,
  )
  bridge.addMomentarySwitchInputDataRef('RTE', '757Avionics/CDU/rte', 1)
  bridge.addMomentarySwitchInputDataRef('CLB', '757Avionics/CDU/clb', 1)
  bridge.addMomentarySwitchInputDataRef('CRZ', '757Avionics/CDU/crz', 1)
  bridge.addMomentarySwitchInputDataRef('DES', '757Avionics/CDU/des', 1)
  bridge.addMomentarySwitchInputDataRef('DIR_INTC', '757Avionics/CDU/dir', 1)
  bridge.addMomentarySwitchInputDataRef('LEGS', '757Avionics/CDU/legs', 1)
  bridge.addMomentarySwitchInputDataRef('DEP_ARR', '757Avionics/CDU/dep_arr', 1)
  bridge.addMomentarySwitchInputDataRef('HOLD', '757Avionics/CDU/hold', 1)
  bridge.addMomentarySwitchInputDataRef('PROG', '757Avionics/CDU/prog', 1)
  bridge.addMomentarySwitchInputDataRef('MENU', '757Avionics/CDU/mcdu_menu', 1)
  bridge.addMomentarySwitchInputDataRef('FIX', '757Avionics/CDU/fix', 1)
  bridge.addMomentarySwitchInputDataRef(
    'PREV_PAGE',
    '757Avionics/CDU/prev_page',
    1,
  )
  bridge.addMomentarySwitchInputDataRef(
    'NEXT_PAGE',
    '757Avionics/CDU/next_page',
    1,
  )
  bridge.addMomentarySwitchInputDataRef('EXEC', '757Avionics/CDU/exec', 1)
}

// position_strobe

// speed_led
