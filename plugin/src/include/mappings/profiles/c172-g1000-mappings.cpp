// Laminar Cessna 172 SP G1000. Port of src/mappings/c172_g1000.ts.

#include "aircraft-mappings.h"
#include "bridge.h"
#include "parsers.h"

void initC172G1000Mappings(Bridge &bridge) {
    /**
     * PANEL POWER
     */
    bridge.addDataRef("sim/cockpit/electrical/battery_on", {.cmd = "power", .threshold = 0, .parser = parsers::toBoolean});

    /**
     * DISPLAYS
     */
    bridge.addDataRef("sim/cockpit2/radios/actuators/hsi_obs_deg_mag_pilot", {.cmd = "set_course_1", .threshold = 1, .parser = parsers::round});
    bridge.addDataRef("sim/cockpit/autopilot/heading_mag", {.cmd = "set_heading", .threshold = 1, .parser = parsers::round});
    bridge.addDataRef("sim/cockpit2/autopilot/altitude_dial_ft", {.cmd = "set_altitude", .threshold = 100, .parser = parsers::round});
    bridge.addDataRef("sim/cockpit/autopilot/vertical_velocity", {.cmd = "set_vertical_speed", .threshold = 50, .parser = parsers::round});
    bridge.addDataRef("sim/cockpit/autopilot/autopilot_state", {.cmd = "toggle_display_set_vertical_speed", .threshold = 1, .parser = [](const Value &v) -> Value {
                                                                     return num(v) == 50 ? 1.0 : 0.0;
                                                                 }});

    /**
     * ENCODERS
     */
    bridge.addRotaryEncoderCommands("course_1_encoder", "sim/GPS/g1000n1_crs_up", "sim/GPS/g1000n1_crs_down");
    bridge.addRotaryEncoderCommands("speed_encoder", "sim/instruments/DG_sync_up", "sim/instruments/DG_sync_down");
    bridge.addRotaryEncoderCommands("heading_encoder", "sim/GPS/g1000n1_hdg_up", "sim/GPS/g1000n1_hdg_down");
    bridge.addRotaryEncoderCommands("altitude_encoder", "sim/GPS/g1000n1_alt_inner_up", "sim/GPS/g1000n1_alt_inner_down");
    bridge.addRotaryEncoderCommands("vertical_speed_encoder", "sim/autopilot/vertical_speed_up", "sim/autopilot/vertical_speed_down");
    bridge.addRotaryEncoderCommands("course_2_encoder", "sim/GPS/g1000n1_crs_up", "sim/GPS/g1000n1_crs_down");

    /**
     * SWITCHES
     */
    bridge.addToggleSwitchInputCommands("flight_director_1", "sim/GPS/g1000n3_fd");
    bridge.addToggleSwitchInputCommands("landing_l", "sim/fuel/fuel_pumps_on", Names("sim/fuel/fuel_pumps_off"));
    bridge.addToggleSwitchInputCommands("landing_r", "sim/lights/beacon_lights_on", Names("sim/lights/beacon_lights_off"));
    bridge.addToggleSwitchInputCommands("runway_l", "sim/lights/landing_lights_on", Names("sim/lights/landing_lights_off"));
    bridge.addToggleSwitchInputCommands("runway_r", "sim/lights/taxi_lights_on", Names("sim/lights/taxi_lights_off"));
    bridge.addToggleSwitchInputCommands("logo", "sim/lights/strobe_lights_on", Names("sim/lights/strobe_lights_off"));
    bridge.addToggleSwitchInputCommands("taxi", "sim/lights/nav_lights_on", Names("sim/lights/nav_lights_off"));
    bridge.addToggleSwitchInputCommands("position_steady", {"sim/ice/pitot_heat_2_on", "sim/ice/pitot_heat0_on"}, Names{"sim/ice/pitot_heat_2_off", "sim/ice/pitot_heat0_off"});
    bridge.addToggleSwitchInputCommands("anti_col", "sim/lights/nav_lights_on", Names("sim/lights/nav_lights_off"));

    /**
     * MOMENTARY SWITCHES
     */
    bridge.addMomentarySwitchInputCommand("c_o", "sim/instruments/timer_mode", 0.1);
    bridge.addMomentarySwitchInputCommand("speed_intv", "sim/instruments/timer_reset", 0.1);
    bridge.addMomentarySwitchInputCommand("vnav", "sim/GPS/g1000n3_vnv", 0.1);
    bridge.addMomentarySwitchInputCommand("lvl_chg", "sim/GPS/g1000n3_flc", 0.1);
    bridge.addMomentarySwitchInputCommand("hdg_sel", "sim/GPS/g1000n3_hdg", 0.1);
    bridge.addMomentarySwitchInputCommand("lnav", "sim/GPS/g1000n3_nav", 0.1);
    bridge.addMomentarySwitchInputCommand("app", "sim/GPS/g1000n3_apr", 0.1);
    bridge.addMomentarySwitchInputCommand("alt_hld", "sim/GPS/g1000n3_alt", 0.1);
    bridge.addMomentarySwitchInputCommand("vs_hld", "sim/GPS/g1000n3_vs", 0.1);
    bridge.addMomentarySwitchInputCommand("cmd_a", "sim/GPS/g1000n3_ap", 0.1);

    /**
     * LEDS
     */
    bridge.addBooleanDataRef("sim/cockpit2/autopilot/speed_status", "lvl_chg_led");
    bridge.addBooleanDataRef("sim/cockpit2/autopilot/heading_status", "heading_led");
    bridge.addBooleanDataRef("sim/cockpit2/autopilot/nav_status", "l_nav_led");
    bridge.addBooleanDataRef("sim/cockpit2/autopilot/approach_status", "app_led");
    bridge.addBooleanDataRef("sim/cockpit2/autopilot/altitude_hold_status", "alt_hld_led");
    bridge.addBooleanDataRef("sim/cockpit2/autopilot/vvi_status", "vertical_speed_led");
    bridge.addBooleanDataRef("sim/cockpit2/annunciators/autopilot", "cmd_a_led");
}
