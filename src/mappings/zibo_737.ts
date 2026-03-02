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
    'laminar/B738/autopilot/lvl_chg_press',
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
  bridge.addBooleanDataRef('laminar/B738/autopilot/speed_status1', 'speed_led')
  bridge.addBooleanDataRef(
    'laminar/B738/autopilot/hdg_sel_status',
    'heading_led',
  )
  bridge.addBooleanDataRef(
    'laminar/B738/autopilot/lvl_chg_status',
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
  bridge.addDataRef('laminar/B738/fmc/fmc_message', {
    web_cockpit_cmd: 'fmc_msg_light',
    threshold: 0,
    parser: ParserType.BOOLEAN,
  })

  bridge.addMomentarySwitchInputCommand(
    'LSK_L1',
    'laminar/B738/button/fmc1_1L',
    0.1,
  )
  bridge.addMomentarySwitchInputCommand(
    'LSK_L2',
    'laminar/B738/button/fmc1_2L',
    0.1,
  )
  bridge.addMomentarySwitchInputCommand(
    'LSK_L3',
    'laminar/B738/button/fmc1_3L',
    0.1,
  )
  bridge.addMomentarySwitchInputCommand(
    'LSK_L4',
    'laminar/B738/button/fmc1_4L',
    0.1,
  )
  bridge.addMomentarySwitchInputCommand(
    'LSK_L5',
    'laminar/B738/button/fmc1_5L',
    0.1,
  )
  bridge.addMomentarySwitchInputCommand(
    'LSK_L6',
    'laminar/B738/button/fmc1_6L',
    0.1,
  )
  bridge.addMomentarySwitchInputCommand(
    'LSK_R1',
    'laminar/B738/button/fmc1_1R',
    0.1,
  )
  bridge.addMomentarySwitchInputCommand(
    'LSK_R2',
    'laminar/B738/button/fmc1_2R',
    0.1,
  )
  bridge.addMomentarySwitchInputCommand(
    'LSK_R3',
    'laminar/B738/button/fmc1_3R',
    0.1,
  )
  bridge.addMomentarySwitchInputCommand(
    'LSK_R4',
    'laminar/B738/button/fmc1_4R',
    0.1,
  )
  bridge.addMomentarySwitchInputCommand(
    'LSK_R5',
    'laminar/B738/button/fmc1_5R',
    0.1,
  )
  bridge.addMomentarySwitchInputCommand(
    'LSK_R6',
    'laminar/B738/button/fmc1_6R',
    0.1,
  )

  bridge.addMomentarySwitchInputCommand(
    'INIT_REF',
    'laminar/B738/button/fmc1_init_ref',
    0.1,
  )
  bridge.addMomentarySwitchInputCommand(
    'RTE',
    'laminar/B738/button/fmc1_rte',
    0.1,
  )
  bridge.addMomentarySwitchInputCommand(
    'DEP_ARR',
    'laminar/B738/button/fmc1_clb',
    0.1,
  )
  bridge.addMomentarySwitchInputCommand(
    'ATC',
    'laminar/B738/button/fmc1_crz',
    0.1,
  )
  bridge.addMomentarySwitchInputCommand(
    'VNAV',
    'laminar/B738/button/fmc1_des',
    0.1,
  )
  bridge.addMomentarySwitchInputCommand(
    'FIX',
    'laminar/B738/button/fmc1_menu',
    0.1,
  )
  bridge.addMomentarySwitchInputCommand(
    'LEGS',
    'laminar/B738/button/fmc1_legs',
    0.1,
  )
  bridge.addMomentarySwitchInputCommand(
    'HOLD',
    'laminar/B738/button/fmc1_dep_app',
    0.1,
  )
  bridge.addMomentarySwitchInputCommand(
    'FMC_COMM',
    'laminar/B738/button/fmc1_hold',
    0.1,
  )
  bridge.addMomentarySwitchInputCommand(
    'PROG',
    'laminar/B738/button/fmc1_prog',
    0.1,
  )
  bridge.addMomentarySwitchInputCommand(
    'MENU',
    'laminar/B738/button/fmc1_n1_lim',
    0.1,
  )
  bridge.addMomentarySwitchInputCommand(
    'N1_LIMIT',
    'laminar/B738/button/fmc1_fix',
    0.1,
  )
  bridge.addMomentarySwitchInputCommand(
    'PREV_PAGE',
    'laminar/B738/button/fmc1_prev_page',
    0.1,
  )
  bridge.addMomentarySwitchInputCommand(
    'NEXT_PAGE',
    'laminar/B738/button/fmc1_next_page',
    0.1,
  )
  bridge.addMomentarySwitchInputCommand(
    'EXEC',
    'laminar/B738/button/fmc1_exec',
    0.1,
  )

  bridge.addMomentarySwitchInputCommand('A', 'laminar/B738/button/fmc1_A', 0.1)
  bridge.addMomentarySwitchInputCommand('B', 'laminar/B738/button/fmc1_B', 0.1)
  bridge.addMomentarySwitchInputCommand('C', 'laminar/B738/button/fmc1_C', 0.1)
  bridge.addMomentarySwitchInputCommand('D', 'laminar/B738/button/fmc1_D', 0.1)
  bridge.addMomentarySwitchInputCommand('E', 'laminar/B738/button/fmc1_E', 0.1)
  bridge.addMomentarySwitchInputCommand('F', 'laminar/B738/button/fmc1_F', 0.1)
  bridge.addMomentarySwitchInputCommand('G', 'laminar/B738/button/fmc1_G', 0.1)
  bridge.addMomentarySwitchInputCommand('H', 'laminar/B738/button/fmc1_H', 0.1)
  bridge.addMomentarySwitchInputCommand('I', 'laminar/B738/button/fmc1_I', 0.1)
  bridge.addMomentarySwitchInputCommand('J', 'laminar/B738/button/fmc1_J', 0.1)
  bridge.addMomentarySwitchInputCommand('K', 'laminar/B738/button/fmc1_K', 0.1)
  bridge.addMomentarySwitchInputCommand('L', 'laminar/B738/button/fmc1_L', 0.1)
  bridge.addMomentarySwitchInputCommand('M', 'laminar/B738/button/fmc1_M', 0.1)
  bridge.addMomentarySwitchInputCommand('N', 'laminar/B738/button/fmc1_N', 0.1)
  bridge.addMomentarySwitchInputCommand('O', 'laminar/B738/button/fmc1_O', 0.1)
  bridge.addMomentarySwitchInputCommand('P', 'laminar/B738/button/fmc1_P', 0.1)
  bridge.addMomentarySwitchInputCommand('Q', 'laminar/B738/button/fmc1_Q', 0.1)
  bridge.addMomentarySwitchInputCommand('R', 'laminar/B738/button/fmc1_R', 0.1)
  bridge.addMomentarySwitchInputCommand('S', 'laminar/B738/button/fmc1_S', 0.1)
  bridge.addMomentarySwitchInputCommand('T', 'laminar/B738/button/fmc1_T', 0.1)
  bridge.addMomentarySwitchInputCommand('U', 'laminar/B738/button/fmc1_U', 0.1)
  bridge.addMomentarySwitchInputCommand('V', 'laminar/B738/button/fmc1_V', 0.1)
  bridge.addMomentarySwitchInputCommand('W', 'laminar/B738/button/fmc1_W', 0.1)
  bridge.addMomentarySwitchInputCommand('X', 'laminar/B738/button/fmc1_X', 0.1)
  bridge.addMomentarySwitchInputCommand('Y', 'laminar/B738/button/fmc1_Y', 0.1)
  bridge.addMomentarySwitchInputCommand('Z', 'laminar/B738/button/fmc1_Z', 0.1)
  bridge.addMomentarySwitchInputCommand(
    'SP',
    'laminar/B738/button/fmc1_SP',
    0.1,
  )
  bridge.addMomentarySwitchInputCommand(
    'DEL',
    'laminar/B738/button/fmc1_del',
    0.1,
  )
  bridge.addMomentarySwitchInputCommand(
    '/',
    'laminar/B738/button/fmc1_slash',
    0.1,
  )
  bridge.addMomentarySwitchInputCommand(
    'CLR',
    'laminar/B738/button/fmc1_clr',
    0.1,
  )
  bridge.addMomentarySwitchInputCommand('1', 'laminar/B738/button/fmc1_1', 0.1)
  bridge.addMomentarySwitchInputCommand('2', 'laminar/B738/button/fmc1_2', 0.1)
  bridge.addMomentarySwitchInputCommand('3', 'laminar/B738/button/fmc1_3', 0.1)
  bridge.addMomentarySwitchInputCommand('4', 'laminar/B738/button/fmc1_4', 0.1)
  bridge.addMomentarySwitchInputCommand('5', 'laminar/B738/button/fmc1_5', 0.1)
  bridge.addMomentarySwitchInputCommand('6', 'laminar/B738/button/fmc1_6', 0.1)
  bridge.addMomentarySwitchInputCommand('7', 'laminar/B738/button/fmc1_7', 0.1)
  bridge.addMomentarySwitchInputCommand('8', 'laminar/B738/button/fmc1_8', 0.1)
  bridge.addMomentarySwitchInputCommand('9', 'laminar/B738/button/fmc1_9', 0.1)
  bridge.addMomentarySwitchInputCommand('0', 'laminar/B738/button/fmc1_0', 0.1)
  bridge.addMomentarySwitchInputCommand(
    '.',
    'laminar/B738/button/fmc1_period',
    0.1,
  )
  bridge.addMomentarySwitchInputCommand(
    '+/-',
    'laminar/B738/button/fmc1_minus',
    0.1,
  )

  /**
   * NAV
   */
  bridge.addDataRef('laminar/B738/mmr/cpt/err', {
    web_cockpit_cmd: 'nav_error',
    threshold: 0,
    parser: ParserType.BOOLEAN,
  })
  bridge.addDataRef('laminar/B738/mmr/cpt/act_value', {
    web_cockpit_cmd: 'nav_active_value',
    threshold: 0,
    parser: ParserType.NONE,
  })
  bridge.addDataRef('laminar/B738/mmr/cpt/act_mode', {
    web_cockpit_cmd: 'nav_active_mode',
    threshold: 0,
    parser: ParserType.ROUND,
  })
  bridge.addDataRef('laminar/B738/mmr/cpt/stby_value', {
    web_cockpit_cmd: 'nav_standby_value',
    threshold: 0,
    parser: ParserType.NONE,
  })
  bridge.addDataRef('laminar/B738/mmr/cpt/stby_cursor', {
    web_cockpit_cmd: 'nav_standby_cursor',
    threshold: 0,
    parser: ParserType.NONE,
  })
  bridge.addDataRef('laminar/B738/mmr/cpt/stby_mode', {
    web_cockpit_cmd: 'nav_standby_mode',
    threshold: 0,
    parser: ParserType.ROUND,
  })
  bridge.addMomentarySwitchInputCommand(
    'NAV_1',
    ['laminar/B738/push_button/mmr1_1', 'laminar/B738/push_button/mmr2_1'],
    0.1,
  )
  bridge.addMomentarySwitchInputCommand(
    'NAV_2',
    ['laminar/B738/push_button/mmr1_2', 'laminar/B738/push_button/mmr2_2'],
    0.1,
  )
  bridge.addMomentarySwitchInputCommand(
    'NAV_3',
    ['laminar/B738/push_button/mmr1_3', 'laminar/B738/push_button/mmr2_3'],
    0.1,
  )
  bridge.addMomentarySwitchInputCommand(
    'NAV_4',
    ['laminar/B738/push_button/mmr1_4', 'laminar/B738/push_button/mmr2_4'],
    0.1,
  )
  bridge.addMomentarySwitchInputCommand(
    'NAV_5',
    ['laminar/B738/push_button/mmr1_5', 'laminar/B738/push_button/mmr2_5'],
    0.1,
  )
  bridge.addMomentarySwitchInputCommand(
    'NAV_6',
    ['laminar/B738/push_button/mmr1_6', 'laminar/B738/push_button/mmr2_6'],
    0.1,
  )
  bridge.addMomentarySwitchInputCommand(
    'NAV_7',
    ['laminar/B738/push_button/mmr1_7', 'laminar/B738/push_button/mmr2_7'],
    0.1,
  )
  bridge.addMomentarySwitchInputCommand(
    'NAV_8',
    ['laminar/B738/push_button/mmr1_8', 'laminar/B738/push_button/mmr2_8'],
    0.1,
  )
  bridge.addMomentarySwitchInputCommand(
    'NAV_9',
    ['laminar/B738/push_button/mmr1_9', 'laminar/B738/push_button/mmr2_9'],
    0.1,
  )
  bridge.addMomentarySwitchInputCommand(
    'NAV_0',
    ['laminar/B738/push_button/mmr1_0', 'laminar/B738/push_button/mmr2_0'],
    0.1,
  )
  bridge.addMomentarySwitchInputCommand(
    'NAV_CLR',
    ['laminar/B738/push_button/mmr1_clr', 'laminar/B738/push_button/mmr2_clr'],
    0.1,
  )
  bridge.addMomentarySwitchInputCommand(
    'NAV_UP',
    [
      'laminar/B738/push_button/mmr1_mode_up',
      'laminar/B738/push_button/mmr2_mode_up',
    ],
    0.1,
  )
  bridge.addMomentarySwitchInputCommand(
    'NAV_DN',
    [
      'laminar/B738/push_button/mmr1_mode_dn',
      'laminar/B738/push_button/mmr2_mode_dn',
    ],
    0.1,
  )
  bridge.addMomentarySwitchInputCommand(
    'NAV_ACT_STBY',
    [
      'laminar/B738/push_button/mmr2_act_stby',
      'laminar/B738/push_button/mmr1_act_stby',
    ],
    0.1,
  )

  bridge.addDataRef('sim/cockpit/radios/transponder_code', {
    web_cockpit_cmd: 'xpdr_code',
    threshold: 0,
    parser: ParserType.NONE,
  })
  bridge.addDataRef('laminar/B738/knob/transponder_pos', {
    web_cockpit_cmd: 'xpdr_mode',
    threshold: 0,
    parser: ParserType.ROUND,
  })
  bridge.addMomentarySwitchInputCommand(
    'XPDR_MODE_DN',
    'laminar/B738/knob/transponder_mode_dn',
    0.1,
  )
  bridge.addMomentarySwitchInputCommand(
    'XPDR_MODE_UP',
    'laminar/B738/knob/transponder_mode_up',
    0.1,
  )
  bridge.addMomentarySwitchInputCommand(
    'XPDR_1000_DN',
    'laminar/B738/knob/xpndr_1000_dn',
    0.1,
  )
  bridge.addMomentarySwitchInputCommand(
    'XPDR_1000_UP',
    'laminar/B738/knob/xpndr_1000_up',
    0.1,
  )
  bridge.addMomentarySwitchInputCommand(
    'XPDR_100_DN',
    'laminar/B738/knob/xpndr_100_dn',
    0.1,
  )
  bridge.addMomentarySwitchInputCommand(
    'XPDR_100_UP',
    'laminar/B738/knob/xpndr_100_up',
    0.1,
  )
  bridge.addMomentarySwitchInputCommand(
    'XPDR_10_DN',
    'laminar/B738/knob/xpndr_10_dn',
    0.1,
  )
  bridge.addMomentarySwitchInputCommand(
    'XPDR_10_UP',
    'laminar/B738/knob/xpndr_10_up',
    0.1,
  )
  bridge.addMomentarySwitchInputCommand(
    'XPDR_1_DN',
    'laminar/B738/knob/xpndr_1_dn',
    0.1,
  )
  bridge.addMomentarySwitchInputCommand(
    'XPDR_1_UP',
    'laminar/B738/knob/xpndr_1_up',
    0.1,
  )
}
