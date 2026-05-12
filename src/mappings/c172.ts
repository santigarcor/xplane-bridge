import { XPlaneBridge, ParserType, TOGGLE_DATAREF } from '../bridge/index.js'

export function initializeMappings(bridge: XPlaneBridge): void {
  /**
   * DISPLAYS
   */
  bridge.addDataRef('sim/cockpit2/radios/actuators/nav1_course_deg_mag_pilot', {
    arduino_cmd: 'set_course_1',
    threshold: 1,
    parser: ParserType.ROUND,
  })
  // bridge.addDataRef('sim/cockpit2/gauges/indicators/airspeed_kts_pilot', {
  //   arduino_cmd: 'set_speed',
  //   threshold: 1,
  //   parser: ParserType.ROUND,
  // })
  bridge.addDataRef('sim/cockpit/autopilot/heading_mag', {
    arduino_cmd: 'set_heading',
    threshold: 1,
    parser: ParserType.ROUND,
  })
  bridge.addDataRef('sim/cockpit2/autopilot/altitude_dial_ft', {
    arduino_cmd: 'set_altitude',
    threshold: 100,
    parser: ParserType.ROUND,
  })
  bridge.addDataRef('sim/cockpit/autopilot/vertical_velocity', {
    arduino_cmd: 'set_vertical_speed',
    threshold: 50,
    parser: ParserType.ROUND,
  })
  bridge.addDataRef('sim/cockpit2/radios/actuators/nav2_course_deg_mag_pilot', {
    arduino_cmd: 'set_course_2',
    threshold: 1,
    parser: ParserType.ROUND,
  })
  bridge.addDataRef('sim/cockpit/autopilot/autopilot_state', {
    arduino_cmd: 'toggle_display_set_vertical_speed',
    threshold: 1,
    parser: (v) => (v === 50 ? 1 : 0),
  })

  /**
   * ENCODERS
   */
  bridge.addRotaryEncoderCommands(
    'course_1_encoder',
    'sim/GPS/g1000n1_crs_up',
    'sim/GPS/g1000n1_crs_down',
  )
  bridge.addRotaryEncoderCommands(
    'speed_encoder',
    'sim/instruments/DG_sync_up',
    'sim/instruments/DG_sync_down',
  )
  bridge.addRotaryEncoderCommands(
    'heading_encoder',
    'sim/GPS/g1000n1_hdg_up',
    'sim/GPS/g1000n1_hdg_down',
  )
  bridge.addRotaryEncoderCommands(
    'altitude_encoder',
    'sim/GPS/g1000n1_alt_inner_up',
    'sim/GPS/g1000n1_alt_inner_down',
  )
  bridge.addRotaryEncoderCommands(
    'vertical_speed_encoder',
    'sim/autopilot/vertical_speed_up',
    'sim/autopilot/vertical_speed_down',
  )
  bridge.addRotaryEncoderCommands(
    'course_2_encoder',
    'sim/GPS/g1000n1_crs_up',
    'sim/GPS/g1000n1_crs_down',
  )

  /**
   * SWITCHES
   */
  // bridge.addToggleSwitchInputCommands(
  //   'at_arm',
  //   'laminar/B738/autopilot/autothrottle_arm_toggle',
  //   'laminar/B738/autopilot/autothrottle_arm_toggle',
  // )
  bridge.addToggleSwitchInputCommands('flight_director_1', 'sim/GPS/g1000n3_fd')
  bridge.addToggleSwitchInputCommands(
    'landing_l',
    'sim/fuel/fuel_pumps_on',
    'sim/fuel/fuel_pumps_off',
  )
  bridge.addToggleSwitchInputCommands(
    'landing_r',
    'sim/lights/beacon_lights_on',
    'sim/lights/beacon_lights_off',
  )
  bridge.addToggleSwitchInputCommands(
    'runway_l',
    'sim/lights/landing_lights_on',
    'sim/lights/landing_lights_off',
  )
  bridge.addToggleSwitchInputCommands(
    'runway_r',
    'sim/lights/taxi_lights_on',
    'sim/lights/taxi_lights_off',
  )
  bridge.addToggleSwitchInputCommands(
    'logo',
    'sim/lights/strobe_lights_on',
    'sim/lights/strobe_lights_off',
  )
  bridge.addToggleSwitchInputCommands(
    'taxi',
    'sim/lights/nav_lights_on',
    'sim/lights/nav_lights_off',
  )
  bridge.addToggleSwitchInputCommands(
    'position_steady',
    ['sim/ice/pitot_heat_2_on', 'sim/ice/pitot_heat0_on'],
    ['sim/ice/pitot_heat_2_off', 'sim/ice/pitot_heat0_off'],
  )
  // bridge.addToggleSwitchInputCommands(
  //   'position_steady',
  //   'laminar/B738/toggle_switch/position_light_down',
  //   'laminar/B738/toggle_switch/position_light_up',
  // )
  bridge.addToggleSwitchInputCommands(
    'anti_col',
    'sim/lights/nav_lights_on',
    'sim/lights/nav_lights_off',
  )
  // bridge.addToggleSwitchInputDataRef(
  //   'wing',
  //   'laminar/B738/toggle_switch/wing_light',
  // )
  // bridge.addToggleSwitchInputDataRef(
  //   'logo',
  //   'laminar/B738/toggle_switch/logo_light',
  // )
  // bridge.addToggleSwitchInputCommands(
  //   'disengage',
  //   'laminar/B738/autopilot/disconnect_toggle',
  // )
  // bridge.addToggleSwitchInputCommands(
  //   'flight_director_2',
  //   'laminar/B738/autopilot/flight_director_fo_toggle',
  // )

  /**
   * MOMENTARY SWITCHES
   */
  bridge.addMomentarySwitchInputCommand(
    'c_o',
    'sim/instruments/timer_mode',
    0.1,
  )
  // bridge.addMomentarySwitchInputCommand(
  //   'n1',
  //   'laminar/B738/autopilot/n1_press',
  //   0.1,
  // )
  // bridge.addMomentarySwitchInputCommand(
  //   'speed',
  //   'laminar/B738/autopilot/speed_press',
  //   0.1,
  // )
  bridge.addMomentarySwitchInputCommand(
    'speed_intv',
    'sim/instruments/timer_reset',
    0.1,
  )
  bridge.addMomentarySwitchInputCommand('vnav', 'sim/GPS/g1000n3_vnv', 0.1)
  bridge.addMomentarySwitchInputCommand('lvl_chg', 'sim/GPS/g1000n3_flc', 0.1)
  bridge.addMomentarySwitchInputCommand('hdg_sel', 'sim/GPS/g1000n3_hdg', 0.1)
  bridge.addMomentarySwitchInputCommand('lnav', 'sim/GPS/g1000n3_nav', 0.1)
  // bridge.addMomentarySwitchInputCommand(
  //   'vor_loc',
  //   'laminar/B738/autopilot/vorloc_press',
  //   0.1,
  // )
  bridge.addMomentarySwitchInputCommand('app', 'sim/GPS/g1000n3_apr', 0.1)
  bridge.addMomentarySwitchInputCommand('alt_hld', 'sim/GPS/g1000n3_alt', 0.1)
  bridge.addMomentarySwitchInputCommand('vs_hld', 'sim/GPS/g1000n3_vs', 0.1)
  // bridge.addMomentarySwitchInputCommand(
  //   'alt_intv',
  //   'laminar/B738/autopilot/alt_interv',
  //   0.1,
  // )
  bridge.addMomentarySwitchInputCommand('cmd_a', 'sim/GPS/g1000n3_ap', 0.1)
  // bridge.addMomentarySwitchInputCommand(
  //   'cmd_b',
  //   'laminar/B738/autopilot/cmd_b_press',
  //   0.1,
  // )
  // bridge.addMomentarySwitchInputCommand(
  //   'cws_a',
  //   'laminar/B738/autopilot/cws_a_press',
  //   0.1,
  // )
  // bridge.addMomentarySwitchInputCommand(
  //   'cws_b',
  //   'laminar/B738/autopilot/cws_b_press',
  //   0.1,
  // )

  /**
   * LEDS
   */
  // bridge.addBooleanDataRef(
  //   'laminar/B738/autopilot/autothrottle_status1',
  //   'at_arm_led',
  // )
  // bridge.addBooleanDataRef(
  //   'laminar/B738/autopilot/master_capt_status',
  //   'flight_director_1_led',
  // )
  // bridge.addBooleanDataRef('laminar/B738/autopilot/n1_status', 'n1_led')
  // bridge.addBooleanDataRef('laminar/B738/autopilot/speed_status1', 'speed_led')
  bridge.addBooleanDataRef('sim/cockpit2/autopilot/speed_status', 'lvl_chg_led')
  bridge.addBooleanDataRef(
    'sim/cockpit2/autopilot/heading_status',
    'heading_led',
  )
  // bridge.addBooleanDataRef('laminar/B738/autopilot/vnav_status1', 'v_nav_led')
  bridge.addBooleanDataRef('sim/cockpit2/autopilot/nav_status', 'l_nav_led')

  // bridge.addBooleanDataRef(
  //   'laminar/B738/autopilot/vorloc_status',
  //   'vor_loc_led',
  // )
  bridge.addBooleanDataRef('sim/cockpit2/autopilot/approach_status', 'app_led')
  bridge.addBooleanDataRef(
    'sim/cockpit2/autopilot/altitude_hold_status',
    'alt_hld_led',
  )
  bridge.addBooleanDataRef(
    'sim/cockpit2/autopilot/vvi_status',
    'vertical_speed_led',
  )
  bridge.addBooleanDataRef('sim/cockpit2/annunciators/autopilot', 'cmd_a_led')
  // bridge.addBooleanDataRef('laminar/B738/autopilot/cmd_b_status', 'cmd_b_led')
  // bridge.addBooleanDataRef('laminar/B738/autopilot/cws_a_status', 'cws_a_led')
  // bridge.addBooleanDataRef('laminar/B738/autopilot/cws_b_status', 'cws_b_led')
  // bridge.addBooleanDataRef(
  //   'laminar/B738/autopilot/master_fo_status',
  //   'flight_director_2_led',
  // )
}
