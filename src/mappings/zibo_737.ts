import { XPlaneBridge, ParserType, TOGGLE_DATAREF } from '../bridge/index.js'

export function initializeMappings(bridge: XPlaneBridge): void {
  /**
   * PANEL POWER
   */
  bridge.addDataRef('laminar/B738/electric/battery_pos', {
    arduino_cmd: 'power',
    threshold: 0,
    parser: ParserType.BOOLEAN,
  })

  /**
   * DISPLAYS
   */
  bridge.addDataRef('laminar/B738/autopilot/course_pilot', {
    arduino_cmd: 'set_course_1',
    threshold: 1,
    parser: ParserType.ROUND,
  })
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
  bridge.addDataRef('laminar/autopilot/ap_vvi_dial', {
    arduino_cmd: 'set_vertical_speed',
    threshold: 50,
    parser: ParserType.ROUND,
  })
  bridge.addDataRef('laminar/B738/autopilot/course_copilot', {
    arduino_cmd: 'set_course_2',
    threshold: 1,
    parser: ParserType.ROUND,
  })
  bridge.addDataRef('laminar/B738/autopilot/vvi_dial_show', {
    arduino_cmd: 'toggle_display_set_vertical_speed',
    threshold: 1,
    parser: ParserType.BOOLEAN,
  })
  bridge.addDataRef('laminar/B738/autopilot/speed_mode', {
    arduino_cmd: 'toggle_display_set_speed',
    threshold: 1,
    parser: (v) => (v === 7 || v === 9 ? 0 : 1),
  })

  /**
   * ENCODERS
   */
  bridge.addRotaryEncoderCommands(
    'course_1_encoder',
    'laminar/B738/autopilot/course_pilot_up',
    'laminar/B738/autopilot/course_pilot_dn',
  )
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
  bridge.addRotaryEncoderCommands(
    'vertical_speed_encoder',
    'sim/autopilot/vertical_speed_up',
    'sim/autopilot/vertical_speed_down',
  )
  bridge.addRotaryEncoderCommands(
    'course_2_encoder',
    'laminar/B738/autopilot/course_copilot_up',
    'laminar/B738/autopilot/course_copilot_dn',
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
    'flight_director_1',
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
  bridge.addToggleSwitchInputCommands(
    'flight_director_2',
    'laminar/B738/autopilot/flight_director_fo_toggle',
  )

  /**
   * MOMENTARY SWITCHES
   */
  bridge.addMomentarySwitchInputCommand(
    'c_o',
    'laminar/B738/autopilot/change_over_press',
    0.1,
  )
  bridge.addMomentarySwitchInputCommand(
    'n1',
    'laminar/B738/autopilot/n1_press',
    0.1,
  )
  bridge.addMomentarySwitchInputCommand(
    'speed',
    'laminar/B738/autopilot/speed_press',
    0.1,
  )
  bridge.addMomentarySwitchInputCommand(
    'speed_intv',
    'laminar/B738/autopilot/spd_interv',
    0.1,
  )
  bridge.addMomentarySwitchInputCommand(
    'vnav',
    'laminar/B738/autopilot/vnav_press',
    0.1,
  )
  bridge.addMomentarySwitchInputCommand(
    'lvl_chg',
    'laminar/B738/autopilot/lvl_chg_press',
    0.1,
  )
  bridge.addMomentarySwitchInputCommand(
    'hdg_sel',
    'laminar/B738/autopilot/hdg_sel_press',
    0.1,
  )
  bridge.addMomentarySwitchInputCommand(
    'lnav',
    'laminar/B738/autopilot/lnav_press',
    0.1,
  )
  bridge.addMomentarySwitchInputCommand(
    'vor_loc',
    'laminar/B738/autopilot/vorloc_press',
    0.1,
  )
  bridge.addMomentarySwitchInputCommand(
    'app',
    'laminar/B738/autopilot/app_press',
    0.1,
  )
  bridge.addMomentarySwitchInputCommand(
    'alt_hld',
    'laminar/B738/autopilot/alt_hld_press',
    0.1,
  )
  bridge.addMomentarySwitchInputCommand(
    'vs_hld',
    'laminar/B738/autopilot/vs_press',
    0.1,
  )
  bridge.addMomentarySwitchInputCommand(
    'alt_intv',
    'laminar/B738/autopilot/alt_interv',
    0.1,
  )
  bridge.addMomentarySwitchInputCommand(
    'cmd_a',
    'laminar/B738/autopilot/cmd_a_press',
    0.1,
  )
  bridge.addMomentarySwitchInputCommand(
    'cmd_b',
    'laminar/B738/autopilot/cmd_b_press',
    0.1,
  )
  bridge.addMomentarySwitchInputCommand(
    'cws_a',
    'laminar/B738/autopilot/cws_a_press',
    0.1,
  )
  bridge.addMomentarySwitchInputCommand(
    'cws_b',
    'laminar/B738/autopilot/cws_b_press',
    0.1,
  )

  /**
   * LEDS
   */
  bridge.addBooleanDataRef(
    'laminar/B738/autopilot/autothrottle_status1',
    'at_arm_led',
  )
  bridge.addBooleanDataRef(
    'laminar/B738/autopilot/master_capt_status',
    'flight_director_1_led',
  )
  bridge.addBooleanDataRef('laminar/B738/autopilot/n1_status', 'n1_led')
  bridge.addBooleanDataRef('laminar/B738/autopilot/speed_status1', 'speed_led')
  bridge.addBooleanDataRef(
    'laminar/B738/autopilot/lvl_chg_status',
    'lvl_chg_led',
  )
  bridge.addBooleanDataRef(
    'laminar/B738/autopilot/hdg_sel_status',
    'heading_led',
  )
  bridge.addBooleanDataRef('laminar/B738/autopilot/vnav_status1', 'v_nav_led')
  bridge.addBooleanDataRef('laminar/B738/autopilot/lnav_status', 'l_nav_led')

  bridge.addBooleanDataRef(
    'laminar/B738/autopilot/vorloc_status',
    'vor_loc_led',
  )
  bridge.addBooleanDataRef('laminar/B738/autopilot/app_status', 'app_led')
  bridge.addBooleanDataRef(
    'laminar/B738/autopilot/alt_hld_status',
    'alt_hld_led',
  )
  bridge.addBooleanDataRef(
    'laminar/B738/autopilot/vs_status',
    'vertical_speed_led',
  )
  bridge.addBooleanDataRef('laminar/B738/autopilot/cmd_a_status', 'cmd_a_led')
  bridge.addBooleanDataRef('laminar/B738/autopilot/cmd_b_status', 'cmd_b_led')
  bridge.addBooleanDataRef('laminar/B738/autopilot/cws_a_status', 'cws_a_led')
  bridge.addBooleanDataRef('laminar/B738/autopilot/cws_b_status', 'cws_b_led')
  bridge.addBooleanDataRef(
    'laminar/B738/autopilot/master_fo_status',
    'flight_director_2_led',
  )
}
