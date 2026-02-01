import { XPlaneArduinoBridge } from '../bridge.js'
import { ParserType, TOGGLE_DATAREF } from '../types.js'

export function initializeMappings(bridge: XPlaneArduinoBridge): void {
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
}
