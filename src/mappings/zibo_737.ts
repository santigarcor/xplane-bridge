import { XPlaneBridge, ParserType, TOGGLE_DATAREF } from '../bridge/index.js'

export function initializeMappings(bridge: XPlaneBridge): void {
  /**
   * DISPLAYS
   */
  bridge.addDataRef('laminar/B738/autopilot/mcp_speed_dial_kts', {
    arduino_cmd: 'set_speed',
    threshold: 1,
    parser: ParserType.ROUND,
  })
  bridge.addDataRef('laminar/B738/autopilot/mcp_hdg_dial', {
    arduino_cmd: 'set_heading',
    threshold: 1,
    parser: ParserType.ROUND,
  })
  bridge.addDataRef('laminar/B738/autopilot/mcp_alt_dial', {
    arduino_cmd: 'set_altitude',
    threshold: 100,
    parser: ParserType.ROUND,
  })

  // bridge.addDataRef('laminar/autopilot/ap_vvi_dial', {
  //   arduino_cmd: 'set_vertical_speed',
  //   threshold: 50,
  //   parser: ParserType.ROUND,
  // })
  bridge.addDataRef('laminar/B738/autopilot/course_pilot', {
    arduino_cmd: 'set_vertical_speed',
    threshold: 1,
    parser: ParserType.ROUND,
  })

  /**
   * ENCODERS
   */
  bridge.addRotaryEncoderCommands(
    'speed_encoder',
    'sim/autopilot/airspeed_up',
    'sim/autopilot/airspeed_down',
  )
  bridge.addRotaryEncoderCommands(
    'heading_encoder',
    'laminar/B738/autopilot/heading_up',
    'laminar/B738/autopilot/heading_dn',
  )
  bridge.addRotaryEncoderCommands(
    'altitude_encoder',
    'laminar/B738/autopilot/altitude_up',
    'laminar/B738/autopilot/altitude_dn',
  )
  // bridge.addRotaryEncoderCommands(
  //   'vertical_speed_encoder',
  //   'sim/autopilot/vertical_speed_up',
  //   'sim/autopilot/vertical_speed_down',
  // )
  bridge.addRotaryEncoderCommands(
    'vertical_speed_encoder',
    [
      'laminar/B738/autopilot/course_pilot_up',
      'laminar/B738/autopilot/course_copilot_up',
    ],
    [
      'laminar/B738/autopilot/course_pilot_dn',
      'laminar/B738/autopilot/course_copilot_dn',
    ],
  )

  /**
   * SWITCHES
   */
  bridge.addToggleSwitchInputCommands(
    'at_arm',
    'laminar/B738/autopilot/autothrottle_arm_toggle',
    'laminar/B738/autopilot/autothrottle_arm_toggle',
  )
  bridge.addToggleSwitchInputCommands(
    'flight_director',
    'laminar/B738/autopilot/flight_director_toggle',
    'laminar/B738/autopilot/flight_director_toggle',
  )
  bridge.addToggleSwitchInputDataRef(
    'landing_l',
    'laminar/B738/switch/land_lights_left_pos',
  )
  bridge.addToggleSwitchInputDataRef(
    'landing_r',
    'laminar/B738/switch/land_lights_right_pos',
  )
  bridge.addToggleSwitchInputDataRef(
    'runway_l',
    'laminar/B738/toggle_switch/rwy_light_left',
  )
  bridge.addToggleSwitchInputDataRef(
    'runway_r',
    'laminar/B738/toggle_switch/rwy_light_right',
  )
  bridge.addToggleSwitchInputCommands(
    'taxi',
    'laminar/B738/toggle_switch/taxi_light_brigh_toggle',
    'laminar/B738/toggle_switch/taxi_light_brigh_toggle',
  )
  bridge.addToggleSwitchInputCommands(
    'position_strobe',
    'laminar/B738/toggle_switch/position_light_up',
    'laminar/B738/toggle_switch/position_light_down',
  )
  bridge.addToggleSwitchInputCommands(
    'position_steady',
    'laminar/B738/toggle_switch/position_light_down',
    'laminar/B738/toggle_switch/position_light_up',
  )
  bridge.addToggleSwitchInputDataRef(
    'anti_col',
    'sim/cockpit2/switches/beacon_on',
  )
  bridge.addToggleSwitchInputDataRef(
    'wing',
    'laminar/B738/toggle_switch/wing_light',
  )
  bridge.addToggleSwitchInputDataRef(
    'logo',
    'laminar/B738/toggle_switch/logo_light',
  )
  bridge.addToggleSwitchInputCommands(
    'disengage',
    'laminar/B738/autopilot/disconnect_toggle',
  )

  /**
   * MOMENTARY SWITCHES
   */

  bridge.addMomentarySwitchInputCommand(
    'speed_hold',
    'laminar/B738/autopilot/speed_press',
    0.1,
  )
  bridge.addMomentarySwitchInputCommand(
    'heading_hold',
    'laminar/B738/autopilot/hdg_sel_press',
    0.1,
  )
  bridge.addMomentarySwitchInputCommand(
    'l_nav',
    'laminar/B738/autopilot/lnav_press',
    0.1,
  )
  bridge.addMomentarySwitchInputCommand(
    'v_nav',
    'laminar/B738/autopilot/vnav_press',
    0.1,
  )
  bridge.addMomentarySwitchInputCommand(
    'altitude_hold',
    'laminar/B738/autopilot/alt_hld_press',
    0.1,
  )
  bridge.addMomentarySwitchInputCommand(
    'vertical_speed_hold',
    'laminar/B738/autopilot/vs_press',
    0.1,
  )
  bridge.addMomentarySwitchInputCommand(
    'app',
    'laminar/B738/autopilot/app_press',
    0.1,
  )
  bridge.addMomentarySwitchInputCommand(
    'loc',
    'laminar/B738/autopilot/vorloc_press',
    0.1,
  )
  bridge.addMomentarySwitchInputCommand(
    'cmd',
    'laminar/B738/autopilot/cmd_a_press',
    0.1,
  )

  /**
   * LEDS
   */
  bridge.addBooleanDataRef(
    'laminar/B738/autopilot/hdg_sel_status',
    'heading_led',
  )
  bridge.addBooleanDataRef(
    'laminar/B738/autopilot/alt_hld_status',
    'altitude_led',
  )
  bridge.addBooleanDataRef(
    'laminar/B738/autopilot/vs_status',
    'vertical_speed_led',
  )
  bridge.addBooleanDataRef('laminar/B738/autopilot/lnav_status', 'l_nav_led')
  bridge.addBooleanDataRef('laminar/B738/autopilot/vnav_status1', 'v_nav_led')
  bridge.addBooleanDataRef('laminar/B738/autopilot/vorloc_status', 'loc_led')
  bridge.addBooleanDataRef('laminar/B738/autopilot/app_status', 'app_led')
  bridge.addBooleanDataRef('laminar/B738/autopilot/cmd_a_status', 'cmd_led')

  /**
   * FMC
   */

  Array.from({ length: 6 }, (_, i) => i + 1).forEach((lineNumber) => {
    ;[
      'laminar/B738/fmc1/Line0#_G',
      'laminar/B738/fmc1/Line0#_GX',
      'laminar/B738/fmc1/Line0#_I',
      'laminar/B738/fmc1/Line0#_L',
      'laminar/B738/fmc1/Line0#_LX',
      'laminar/B738/fmc1/Line0#_M',
      'laminar/B738/fmc1/Line0#_S',
      'laminar/B738/fmc1/Line0#_X',
    ].forEach((dataRefTemplate) => {
      const dataRef = dataRefTemplate.replace('#', lineNumber.toString())
      const suffix = dataRef.split('_').pop() as string
      const webCockpitCmd = `fmc_line-${lineNumber}_${suffix}`

      bridge.addDataRef(dataRef, {
        web_cockpit_cmd: webCockpitCmd,
        threshold: 0,
        parser: ParserType.BASE64DECODE,
      })
    })
  })
  ;[
    'laminar/B738/fmc1/Line00_C',
    'laminar/B738/fmc1/Line00_G',
    'laminar/B738/fmc1/Line00_I',
    'laminar/B738/fmc1/Line00_L',
    'laminar/B738/fmc1/Line00_M',
    'laminar/B738/fmc1/Line00_S',
  ].forEach((dataRef) => {
    const suffix = dataRef.split('_').pop() as string
    const webCockpitCmd = `fmc_line-00_${suffix}`

    bridge.addDataRef(dataRef, {
      web_cockpit_cmd: webCockpitCmd,
      threshold: 0,
      parser: ParserType.BASE64DECODE,
    })
  })

  bridge.addDataRef('laminar/B738/fmc1/Line_entry', {
    web_cockpit_cmd: 'fmc_line-07_L',
    threshold: 0,
    parser: ParserType.BASE64DECODE,
  })
  bridge.addDataRef('laminar/B738/fmc1/Line_entry_I', {
    web_cockpit_cmd: 'fmc_line-07_I',
    threshold: 0,
    parser: ParserType.BASE64DECODE,
  })
  bridge.addDataRef('laminar/B738/fmod/fmc_exec_lights', {
    web_cockpit_cmd: 'fmc_exec_light',
    threshold: 0,
    parser: ParserType.BOOLEAN,
  })

  bridge.addMomentarySwitchInputCommand('LSK_L1', 'laminar/B738/button/fmc1_1L')
  bridge.addMomentarySwitchInputCommand('LSK_L2', 'laminar/B738/button/fmc1_2L')
  bridge.addMomentarySwitchInputCommand('LSK_L3', 'laminar/B738/button/fmc1_3L')
  bridge.addMomentarySwitchInputCommand('LSK_L4', 'laminar/B738/button/fmc1_4L')
  bridge.addMomentarySwitchInputCommand('LSK_L5', 'laminar/B738/button/fmc1_5L')
  bridge.addMomentarySwitchInputCommand('LSK_L6', 'laminar/B738/button/fmc1_6L')
  bridge.addMomentarySwitchInputCommand('LSK_R1', 'laminar/B738/button/fmc1_1R')
  bridge.addMomentarySwitchInputCommand('LSK_R2', 'laminar/B738/button/fmc1_2R')
  bridge.addMomentarySwitchInputCommand('LSK_R3', 'laminar/B738/button/fmc1_3R')
  bridge.addMomentarySwitchInputCommand('LSK_R4', 'laminar/B738/button/fmc1_4R')
  bridge.addMomentarySwitchInputCommand('LSK_R5', 'laminar/B738/button/fmc1_5R')
  bridge.addMomentarySwitchInputCommand('LSK_R6', 'laminar/B738/button/fmc1_6R')

  bridge.addMomentarySwitchInputCommand(
    'INIT_REF',
    'laminar/B738/button/fmc1_init_ref',
  )
  bridge.addMomentarySwitchInputCommand('RTE', 'laminar/B738/button/fmc1_rte')
  bridge.addMomentarySwitchInputCommand(
    'DEP_ARR',
    'laminar/B738/button/fmc1_clb',
  )
  bridge.addMomentarySwitchInputCommand('ATC', 'laminar/B738/button/fmc1_crz')
  bridge.addMomentarySwitchInputCommand('VNAV', 'laminar/B738/button/fmc1_des')
  bridge.addMomentarySwitchInputCommand('FIX', 'laminar/B738/button/fmc1_menu')
  bridge.addMomentarySwitchInputCommand('LEGS', 'laminar/B738/button/fmc1_legs')
  bridge.addMomentarySwitchInputCommand(
    'HOLD',
    'laminar/B738/button/fmc1_dep_app',
  )
  bridge.addMomentarySwitchInputCommand(
    'FMC_COMM',
    'laminar/B738/button/fmc1_hold',
  )
  bridge.addMomentarySwitchInputCommand('PROG', 'laminar/B738/button/fmc1_prog')
  bridge.addMomentarySwitchInputCommand(
    'MENU',
    'laminar/B738/button/fmc1_n1_limit',
  )
  bridge.addMomentarySwitchInputCommand(
    'N1_LIMIT',
    'laminar/B738/button/fmc1_fix',
  )
  bridge.addMomentarySwitchInputCommand(
    'PREV_PAGE',
    'laminar/B738/button/fmc1_prev_page',
  )
  bridge.addMomentarySwitchInputCommand(
    'NEXT_PAGE',
    'laminar/B738/button/fmc1_next_page',
  )
  bridge.addMomentarySwitchInputCommand('EXEC', 'laminar/B738/button/fmc1_exec')

  bridge.addMomentarySwitchInputCommand('A', 'laminar/B738/button/fmc1_A')
  bridge.addMomentarySwitchInputCommand('B', 'laminar/B738/button/fmc1_B')
  bridge.addMomentarySwitchInputCommand('C', 'laminar/B738/button/fmc1_C')
  bridge.addMomentarySwitchInputCommand('D', 'laminar/B738/button/fmc1_D')
  bridge.addMomentarySwitchInputCommand('E', 'laminar/B738/button/fmc1_E')
  bridge.addMomentarySwitchInputCommand('F', 'laminar/B738/button/fmc1_F')
  bridge.addMomentarySwitchInputCommand('G', 'laminar/B738/button/fmc1_G')
  bridge.addMomentarySwitchInputCommand('H', 'laminar/B738/button/fmc1_H')
  bridge.addMomentarySwitchInputCommand('I', 'laminar/B738/button/fmc1_I')
  bridge.addMomentarySwitchInputCommand('J', 'laminar/B738/button/fmc1_J')
  bridge.addMomentarySwitchInputCommand('K', 'laminar/B738/button/fmc1_K')
  bridge.addMomentarySwitchInputCommand('L', 'laminar/B738/button/fmc1_L')
  bridge.addMomentarySwitchInputCommand('M', 'laminar/B738/button/fmc1_M')
  bridge.addMomentarySwitchInputCommand('N', 'laminar/B738/button/fmc1_N')
  bridge.addMomentarySwitchInputCommand('O', 'laminar/B738/button/fmc1_O')
  bridge.addMomentarySwitchInputCommand('P', 'laminar/B738/button/fmc1_P')
  bridge.addMomentarySwitchInputCommand('Q', 'laminar/B738/button/fmc1_Q')
  bridge.addMomentarySwitchInputCommand('R', 'laminar/B738/button/fmc1_R')
  bridge.addMomentarySwitchInputCommand('S', 'laminar/B738/button/fmc1_S')
  bridge.addMomentarySwitchInputCommand('T', 'laminar/B738/button/fmc1_T')
  bridge.addMomentarySwitchInputCommand('U', 'laminar/B738/button/fmc1_U')
  bridge.addMomentarySwitchInputCommand('V', 'laminar/B738/button/fmc1_V')
  bridge.addMomentarySwitchInputCommand('W', 'laminar/B738/button/fmc1_W')
  bridge.addMomentarySwitchInputCommand('X', 'laminar/B738/button/fmc1_X')
  bridge.addMomentarySwitchInputCommand('Y', 'laminar/B738/button/fmc1_Y')
  bridge.addMomentarySwitchInputCommand('Z', 'laminar/B738/button/fmc1_Z')
  bridge.addMomentarySwitchInputCommand('SP', 'laminar/B738/button/fmc1_SP')
  bridge.addMomentarySwitchInputCommand('DEL', 'laminar/B738/button/fmc1_del')
  bridge.addMomentarySwitchInputCommand('/', 'laminar/B738/button/fmc1_slash')
  bridge.addMomentarySwitchInputCommand('CLR', 'laminar/B738/button/fmc1_clr')
  bridge.addMomentarySwitchInputCommand('1', 'laminar/B738/button/fmc1_1')
  bridge.addMomentarySwitchInputCommand('2', 'laminar/B738/button/fmc1_2')
  bridge.addMomentarySwitchInputCommand('3', 'laminar/B738/button/fmc1_3')
  bridge.addMomentarySwitchInputCommand('4', 'laminar/B738/button/fmc1_4')
  bridge.addMomentarySwitchInputCommand('5', 'laminar/B738/button/fmc1_5')
  bridge.addMomentarySwitchInputCommand('6', 'laminar/B738/button/fmc1_6')
  bridge.addMomentarySwitchInputCommand('7', 'laminar/B738/button/fmc1_7')
  bridge.addMomentarySwitchInputCommand('8', 'laminar/B738/button/fmc1_8')
  bridge.addMomentarySwitchInputCommand('9', 'laminar/B738/button/fmc1_9')
  bridge.addMomentarySwitchInputCommand('0', 'laminar/B738/button/fmc1_0')
  bridge.addMomentarySwitchInputCommand('.', 'laminar/B738/button/fmc1_period')
  bridge.addMomentarySwitchInputCommand('+/-', 'laminar/B738/button/fmc1_minus')
}
