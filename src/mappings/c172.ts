import { XPlaneBridge, ParserType, TOGGLE_DATAREF } from '../bridge/index.js'
import { atIndex, pipe } from '../bridge/helpers.js'

export function initializeMappings(bridge: XPlaneBridge): void {
  /**
   * PANEL POWER
   */
  bridge.addDataRef('sim/cockpit/electrical/battery_on', {
    arduino_cmd: 'power',
    threshold: 0,
    parser: ParserType.BOOLEAN,
  })
  // Array dataref variant (index then convert), for reference:
  //   bridge.addDataRef('sim/cockpit2/electrical/bus_volts', {
  //     arduino_cmd: 'power',
  //     threshold: 0,
  //     parser: pipe(atIndex(0), (v) => (v > 10 ? 1 : 0)),
  //   })

  /**
   * DISPLAYS
   */
  bridge.addDataRef('sim/cockpit/radios/nav1_obs_degm', {
    arduino_cmd: 'set_course_1',
    threshold: 1,
    parser: ParserType.ROUND,
  })
  bridge.addDataRef('sim/cockpit/autopilot/heading_mag', {
    arduino_cmd: 'set_heading',
    threshold: 1,
    parser: ParserType.ROUND,
  })
  bridge.addDataRef('sim/cockpit/autopilot/vertical_velocity', {
    arduino_cmd: 'set_vertical_speed',
    threshold: 50,
    parser: ParserType.ROUND,
  })
  bridge.addDataRef('sim/cockpit/radios/nav2_obs_degm2', {
    arduino_cmd: 'set_course_2',
    threshold: 1,
    parser: ParserType.ROUND,
  })
  bridge.addDataRef('sim/cockpit2/autopilot/altitude_mode', {
    arduino_cmd: 'toggle_display_set_vertical_speed',
    threshold: 1,
    parser: (v) => (v === 4 ? 1 : 0),
  })

  /**
   * ENCODERS
   */
  bridge.addRotaryEncoderCommands(
    'course_1_encoder',
    'sim/radios/obs1_up',
    'sim/radios/obs1_down',
  )
  bridge.addRotaryEncoderCommands(
    'speed_encoder',
    'sim/instruments/DG_sync_up',
    'sim/instruments/DG_sync_down',
  )
  bridge.addRotaryEncoderCommands(
    'heading_encoder',
    'sim/autopilot/heading_up',
    'sim/autopilot/heading_down',
  )
  bridge.addRotaryEncoderCommands(
    'vertical_speed_encoder',
    'sim/autopilot/vertical_speed_up',
    'sim/autopilot/vertical_speed_down',
  )
  bridge.addRotaryEncoderCommands(
    'course_2_encoder',
    'sim/radios/copilot_obs2_up',
    'sim/radios/copilot_obs2_down',
  )

  /**
   * SWITCHES
   */
  bridge.addToggleSwitchInputCommands(
    'at_arm',
    'sim/electrical/generator_1_on',
    'sim/electrical/generator_1_off',
  )
  bridge.addToggleSwitchInputCommands(
    'flight_director_1',
    'sim/electrical/battery_1_on',
    'sim/electrical/battery_1_off',
  )
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
  bridge.addToggleSwitchInputCommands(
    'anti_col',
    'sim/systems/avionics_on',
    'sim/systems/avionics_off',
  )
  bridge.addToggleSwitchInputCommands(
    'wing',
    'sim/electrical/cross_tie_on',
    'sim/electrical/cross_tie_off',
  )

  /**
   * MOMENTARY SWITCHES
   */
  bridge.addMomentarySwitchInputCommand(
    'c_o',
    'sim/instruments/timer_mode',
    0.1,
  )
  bridge.addMomentarySwitchInputCommand(
    'speed_intv',
    'sim/instruments/timer_start_stop',
    0.1,
  )
  bridge.addMomentarySwitchInputCommand('vnav', 'sim/GPS/g1000n3_vnv', 0.1)
  bridge.addMomentarySwitchInputCommand('lvl_chg', 'sim/GPS/g1000n3_flc', 0.1)
  bridge.addMomentarySwitchInputCommand('hdg_sel', 'sim/autopilot/heading', 0.1)
  bridge.addMomentarySwitchInputCommand('lnav', 'sim/autopilot/NAV', 0.1)
  bridge.addMomentarySwitchInputCommand('app', 'sim/autopilot/approach', 0.1)
  bridge.addMomentarySwitchInputCommand(
    'alt_hld',
    'sim/autopilot/altitude_hold',
    0.1,
  )
  bridge.addMomentarySwitchInputCommand(
    'vs_hld',
    'sim/autopilot/vertical_speed',
    0.1,
  )
  bridge.addMomentarySwitchInputCommand(
    'alt_intv',
    'sim/instruments/timer_reset',
    0.1,
  )

  /**
   * LEDS
   */
  bridge.addBooleanDataRef(
    'sim/cockpit2/autopilot/heading_status',
    'heading_led',
  )
  bridge.addBooleanDataRef('sim/cockpit2/autopilot/nav_status', 'l_nav_led')

  bridge.addBooleanDataRef('sim/cockpit2/autopilot/approach_status', 'app_led')
  bridge.addDataRef('sim/cockpit2/autopilot/altitude_hold_status', {
    arduino_cmd: 'alt_hld_led',
    threshold: 1,
    parser: (v) => (v === 2 ? 1 : 0),
  })
  bridge.addBooleanDataRef(
    'sim/cockpit2/autopilot/vvi_status',
    'vertical_speed_led',
  )
}
