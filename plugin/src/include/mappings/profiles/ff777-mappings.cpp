// FlightFactor 777v2. Port of src/mappings/ff_777.ts.

#include "aircraft-mappings.h"
#include "bridge.h"
#include "parsers.h"

void initFF777Mappings(Bridge &bridge) {
    using parsers::atIndex;
    using parsers::pipe;

    /**
     * PANEL POWER
     */
    // bus_load_amps[0] reads 56 with power on and 55 with power off.
    bridge.addDataRef("sim/cockpit2/electrical/bus_load_amps", {.cmd = "power", .threshold = 0, .parser = pipe({atIndex(0), [](const Value &v) -> Value {
                                                                                                                     return num(v) > 55 ? 1.0 : 0.0;
                                                                                                                 }})});
    // Previous source, kept in case it differs between passenger and freighter variants:
    //   bridge.addDataRef("sim/cockpit/electrical/cockpit_lights", {.cmd = "power", .threshold = 0, .parser = parsers::toBoolean});

    /**
     * DISPLAYS
     */
    // bridge.addDataRef("laminar/B738/autopilot/course_pilot", {.cmd = "set_course_1", .threshold = 1, .parser = parsers::round});
    bridge.addDataRef("1-sim/output/mcp/spd", {.cmd = "set_speed", .threshold = 1, .parser = parsers::round});
    bridge.addDataRef("1-sim/output/mcp/hdg", {.cmd = "set_heading", .threshold = 1, .parser = parsers::round});
    bridge.addDataRef("1-sim/output/mcp/alt", {.cmd = "set_altitude", .threshold = 100, .parser = parsers::round});
    bridge.addDataRef("1-sim/output/mcp/vs", {.cmd = "set_vertical_speed", .threshold = 50, .parser = parsers::round});
    // bridge.addDataRef("laminar/B738/autopilot/course_copilot", {.cmd = "set_course_2", .threshold = 1, .parser = parsers::round});
    bridge.addDataRef("1-sim/output/mcp/isVsOpen", {.cmd = "toggle_display_set_vertical_speed", .threshold = 1, .parser = parsers::toBoolean});
    bridge.addDataRef("1-sim/output/mcp/isSpdOpen", {.cmd = "toggle_display_set_speed", .threshold = 1, .parser = parsers::toBoolean});

    /**
     * ENCODERS
     */
    // bridge.addRotaryEncoderCommands("course_1_encoder", "laminar/B738/autopilot/course_pilot_up", "laminar/B738/autopilot/course_pilot_dn");
    bridge.addRotaryEncoderCommands("speed_encoder", "1-sim/command/mcpSpdRotary_rotary+", "1-sim/command/mcpSpdRotary_rotary-");
    bridge.addRotaryEncoderCommands("heading_encoder", "1-sim/command/mcpHdgRotary_rotary+", "1-sim/command/mcpHdgRotary_rotary-");
    bridge.addRotaryEncoderCommands("altitude_encoder", "1-sim/command/mcpAltRotary_rotary+", "1-sim/command/mcpAltRotary_rotary-");
    bridge.addRotaryEncoderCommands("vertical_speed_encoder", "1-sim/command/mcpVsRotary_rotary+", "1-sim/command/mcpVsRotary_rotary-");
    // bridge.addRotaryEncoderCommands("course_2_encoder", "laminar/B738/autopilot/course_copilot_up", "laminar/B738/autopilot/course_copilot_dn");

    /**
     * SWITCHES
     */
    bridge.addToggleSwitchInputCommands("at_arm", {"1-sim/command/mcpAtSwitchL_trigger", "1-sim/command/mcpAtSwitchR_trigger"});
    bridge.addToggleSwitchInputCommands("flight_director_1", "1-sim/command/mcpFdLSwitch_trigger");
    bridge.addToggleSwitchInputCommands("landing_l", {"1-sim/command/landingLightLeftSwitch_trigger", "1-sim/command/landingLightNoseSwitch_trigger"});
    bridge.addToggleSwitchInputCommands("landing_r", {"1-sim/command/landingLightRightSwitch_trigger"});
    bridge.addToggleSwitchInputCommands("runway_l", "1-sim/command/runwayTurnoffLeftSwitch_trigger");
    bridge.addToggleSwitchInputCommands("runway_r", "1-sim/command/runwayTurnoffRightSwitch_trigger");
    bridge.addToggleSwitchInputCommands("taxi", "1-sim/command/taxiLightSwitch_trigger");
    bridge.addToggleSwitchInputCommands("position_strobe", {"1-sim/command/strobeLightSwitch_trigger", "1-sim/command/navLightSwitch_button"});
    bridge.addToggleSwitchInputCommands("position_steady", "1-sim/command/navLightSwitch_button");
    bridge.addToggleSwitchInputCommands("anti_col", "1-sim/command/beaconLightSwitch_button");
    bridge.addToggleSwitchInputCommands("wing", "1-sim/command/wingLightSwitch_button");
    bridge.addToggleSwitchInputCommands("logo", "1-sim/command/logoLightSwitch_button");
    bridge.addToggleSwitchInputCommands("disengage", "1-sim/command/mcpApDiscSwitch_trigger");
    bridge.addToggleSwitchInputCommands("flight_director_2", "1-sim/command/mcpFdRSwitch_trigger");

    /**
     * MOMENTARY SWITCHES
     */
    bridge.addMomentarySwitchInputCommand("c_o", "1-sim/command/mcpIasMachButton_button", 0.1);
    bridge.addMomentarySwitchInputCommand("n1", "1-sim/command/mcpClbButton_button", 0.1);
    bridge.addMomentarySwitchInputCommand("speed", "1-sim/command/mcpAtButton_button", 0.1);
    bridge.addMomentarySwitchInputCommand("speed_intv", "1-sim/command/mcpHdgTrkButton_button", 0.1);
    bridge.addMomentarySwitchInputCommand("vnav", "1-sim/command/mcpVnavButton_button", 0.1);
    bridge.addMomentarySwitchInputCommand("lvl_chg", "1-sim/command/mcpFlchButton_button", 0.1);
    // Check if that one or 1-sim/command/mcpHdgHoldButton_button
    bridge.addMomentarySwitchInputCommand("hdg_sel", "1-sim/command/mcpHdgCelButton_button", 0.1);
    bridge.addMomentarySwitchInputCommand("lnav", "1-sim/command/mcpLnavButton_button", 0.1);
    bridge.addMomentarySwitchInputCommand("vor_loc", "1-sim/command/mcpLocButton_button", 0.1);
    bridge.addMomentarySwitchInputCommand("app", "1-sim/command/mcpAppButton_button", 0.1);
    bridge.addMomentarySwitchInputCommand("alt_hld", "1-sim/command/mcpAltHoldButton_button", 0.1);
    bridge.addMomentarySwitchInputCommand("vs_hld", "1-sim/command/mcpVsButton_button", 0.1);
    bridge.addMomentarySwitchInputCommand("alt_intv", "1-sim/command/mcpVsFpaButton_button", 0.1);
    bridge.addMomentarySwitchInputCommand("cmd_a", "1-sim/command/mcpApLButton_button", 0.1);
    bridge.addMomentarySwitchInputCommand("cmd_b", "1-sim/command/mcpApRButton_button", 0.1);

    /**
     * LEDS
     */
    bridge.addBooleanDataRef("1-sim/ckpt/lamps/mcpAT", "speed_led");
    bridge.addBooleanDataRef("1-sim/ckpt/lamps/mcpFLCH", "lvl_chg_led");
    bridge.addBooleanDataRef("1-sim/ckpt/lamps/mcpHdgHOLD", "heading_led");
    bridge.addBooleanDataRef("1-sim/ckpt/lamps/mcpVNAV", "v_nav_led");
    bridge.addBooleanDataRef("1-sim/ckpt/lamps/mcpLNAV", "l_nav_led");
    bridge.addBooleanDataRef("1-sim/ckpt/lamps/mcpLOC", "vor_loc_led");
    bridge.addBooleanDataRef("1-sim/ckpt/lamps/mcpAPP", "app_led");
    bridge.addBooleanDataRef("1-sim/ckpt/lamps/mcpAltHOLD", "alt_hld_led");
    bridge.addBooleanDataRef("1-sim/ckpt/lamps/mcpVS", "vertical_speed_led");
    bridge.addBooleanDataRef("1-sim/ckpt/lamps/mcpCaptAP", "cmd_a_led");
    bridge.addBooleanDataRef("1-sim/ckpt/lamps/mcpFoAP", "cmd_b_led");
}
