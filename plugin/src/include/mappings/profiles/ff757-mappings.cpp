// FlightFactor 757. Port of src/mappings/ff_757.ts.

#include "aircraft-mappings.h"
#include "bridge.h"
#include "parsers.h"

#include <memory>
#include <optional>

void initFF757Mappings(Bridge &bridge) {
    /**
     * PANEL POWER
     */
    bridge.addDataRef("sim/cockpit/electrical/battery_on", {.cmd = "power", .threshold = 0, .parser = parsers::toBoolean});

    /**
     * DISPLAYS
     */
    // bridge.addDataRef("sim/cockpit2/radios/actuators/nav1_obs_deg_mag_pilot", {.cmd = "set_course_1", .threshold = 1, .parser = parsers::round});
    bridge.addDataRef("1-sim/AP/spdSetting", {.cmd = "set_speed", .threshold = 1, .parser = parsers::round});
    bridge.addDataRef("1-sim/AP/hdgSetting", {.cmd = "set_heading", .threshold = 1, .parser = parsers::round});
    bridge.addDataRef("1-sim/AP/altSetting", {.cmd = "set_altitude", .threshold = 100, .parser = parsers::round});
    bridge.addDataRef("1-sim/AP/vviSetting", {.cmd = "set_vertical_speed", .threshold = 50, .parser = parsers::round});
    // bridge.addDataRef("1-sim/radios/nav3_obs_deg_mag_pilot", {.cmd = "set_course_2", .threshold = 1, .parser = parsers::round});
    bridge.addDataRef("757Avionics/mcp/vs/label", {.cmd = "toggle_display_set_vertical_speed", .threshold = 1, .parser = parsers::toBoolean});
    bridge.addDataRef("757Avionics/mcp/speed/label", {.cmd = "toggle_display_set_speed", .threshold = 1, .parser = parsers::toBoolean});

    /**
     * ENCODERS
     */
    bridge.addRotaryEncoderCommands("course_1_encoder", "1-sim/comm/vor1crsRotaryUP", "1-sim/comm/vor1crsRotaryDN");
    bridge.addRotaryEncoderCommands("speed_encoder", "1-sim/comm/AP/spdUP", "1-sim/comm/AP/spdDN");
    bridge.addRotaryEncoderCommands("heading_encoder", "1-sim/comm/AP/hdgUP", "1-sim/comm/AP/hdgDN");
    bridge.addRotaryEncoderCommands("altitude_encoder", "1-sim/comm/AP/altUP", "1-sim/comm/AP/altDN");
    bridge.addRotaryEncoderCommands("vertical_speed_encoder", "1-sim/comm/AP/vviUP", "1-sim/comm/AP/vviDN");
    bridge.addRotaryEncoderCommands("course_2_encoder", "1-sim/comm/vor2crsRotaryUP", "1-sim/comm/vor2crsRotaryDN");

    /**
     * SWITCHES
     */
    bridge.addToggleSwitchInputCommands("at_arm", "1-sim/command/AP/atSwitcher_trigger");
    bridge.addToggleSwitchInputCommands("flight_director_1", "1-sim/command/AP/fd1Switcher_trigger");
    bridge.addToggleSwitchInputCommands("disengage", "1-sim/command/AP/desengageLever_button");
    bridge.addToggleSwitchInputCommands("flight_director_2", "1-sim/command/AP/fd2Switcher_trigger");

    bridge.addToggleSwitchInputCommands("landing_l", "1-sim/command/lights/landingL/switch_trigger");
    bridge.addToggleSwitchInputCommands("landing_r", "1-sim/command/lights/landingR/switch_trigger");
    bridge.addToggleSwitchInputCommands("runway_l", "1-sim/command/lights/runwayL/switch_trigger");
    bridge.addToggleSwitchInputCommands("runway_r", "1-sim/command/lights/runwayR/switch_trigger");
    bridge.addToggleSwitchInputCommands("taxi", "1-sim/command/lights/landingN/switch_trigger");
    bridge.addToggleSwitchInputDataRef("position_steady", {"anim/43/button", "anim/43/button/anim"});
    bridge.addToggleSwitchInputDataRef("position_strobe", {"anim/43/button", "anim/43/button/anim", "anim/45/button", "anim/45/button/anim"});
    bridge.addToggleSwitchInputDataRef("anti_col", {"anim/44/button", "anim/44/button/anim"});
    bridge.addToggleSwitchInputDataRef("wing", {"anim/46/button", "anim/46/button/anim"});
    bridge.addToggleSwitchInputDataRef("logo", {"anim/52/button", "anim/52/button/anim"});

    /**
     * MOMENTARY SWITCHES
     */
    // "c_o" is what the sketch sends. ff_757.ts maps "co", so the C/O button did nothing there.
    bridge.addMomentarySwitchInputCommand("c_o", "1-sim/command/AP/iasmach_button", 0.1);
    bridge.addMomentarySwitchInputCommand("n1", "1-sim/command/AP/eprButton_button", 0.1);
    bridge.addMomentarySwitchInputCommand("speed", "1-sim/command/AP/spdButton_button", 0.1);
    bridge.addMomentarySwitchInputCommand("vnav", "1-sim/command/AP/vnavButton_button", 0.1);
    bridge.addMomentarySwitchInputCommand("lvl_chg", "1-sim/command/AP/flchButton_button", 0.1);
    bridge.addMomentarySwitchInputCommand("hdg_sel", "1-sim/command/AP/hdgConfButton_button", 0.1);
    bridge.addMomentarySwitchInputCommand("lnav", "1-sim/command/AP/lnavButton_button");
    bridge.addMomentarySwitchInputCommand("vor_loc", "1-sim/command/AP/locButton_button", 0.1);
    bridge.addMomentarySwitchInputCommand("app", "1-sim/command/AP/appButton_button", 0.1);
    bridge.addMomentarySwitchInputCommand("alt_hld", "1-sim/command/AP/altHoldButton_button", 0.1);
    bridge.addMomentarySwitchInputCommand("vs_hld", "1-sim/command/AP/vviButton_button", 0.1);
    bridge.addMomentarySwitchInputCommand("cmd_a", "1-sim/command/AP/cmd_L_Button_button", 0.1);
    bridge.addMomentarySwitchInputCommand("cmd_b", "1-sim/command/AP/cmd_R_Button_button", 0.1);
    // Same command as cmd_a, as in ff_757.ts
    bridge.addMomentarySwitchInputCommand("cws_a", "1-sim/command/AP/cmd_L_Button_button", 0.1);

    /**
     * LEDS
     */
    // bridge.addBooleanDataRef("1-sim/AP/atSwitcher", "at_arm_led");

    // Only the master FD LED is lit: the first FD switched on becomes master, and if it's switched
    // off the other one takes over if it's still on. The state lives in this initializer, so it
    // resets when the plane is re-detected.
    enum class Side { None, Left, Right };
    struct FdState {
            bool left = false;
            bool right = false;
            Side master = Side::None;
    };
    auto fdState = std::make_shared<FdState>();
    auto fdLedParser = [fdState](Side side) -> Parser {
        return [fdState, side](const Value &value) -> Value {
            (side == Side::Left ? fdState->left : fdState->right) = num(value) > 0;
            bool masterOn = fdState->master == Side::Left ? fdState->left : fdState->master == Side::Right ? fdState->right : false;
            if (fdState->master != Side::None && !masterOn) {
                fdState->master = Side::None;
            }
            if (fdState->master == Side::None) {
                fdState->master = fdState->left ? Side::Left : fdState->right ? Side::Right : Side::None;
            }
            return fdState->master == side ? 1.0 : 0.0;
        };
    };
    bridge.addDataRef("757Avionics/ap/leftFdState", {.cmd = "flight_director_1_led", .threshold = 0, .parser = fdLedParser(Side::Left)});
    bridge.addDataRef("757Avionics/ap/rghtFdState", {.cmd = "flight_director_2_led", .threshold = 0, .parser = fdLedParser(Side::Right)});
    bridge.addBooleanDataRef("1-sim/AP/lamp/1", "n1_led");
    bridge.addBooleanDataRef("1-sim/AP/lamp/2", "speed_led");
    bridge.addBooleanDataRef("1-sim/AP/lamp/3", "l_nav_led");
    bridge.addBooleanDataRef("1-sim/AP/lamp/4", "v_nav_led");
    bridge.addBooleanDataRef("1-sim/AP/lamp/5", "lvl_chg_led");
    bridge.addBooleanDataRef("1-sim/AP/hdgConfButton", "heading_led");
    // bridge.addBooleanDataRef("1-sim/AP/lamp/6", "heading_led");
    bridge.addBooleanDataRef("1-sim/AP/lamp/7", "vertical_speed_led");
    bridge.addBooleanDataRef("1-sim/AP/lamp/8", "alt_hld_led");
    bridge.addBooleanDataRef("1-sim/AP/lamp/10", "vor_loc_led");
    bridge.addBooleanDataRef("1-sim/AP/lamp/11", "app_led");
    bridge.addBooleanDataRef("1-sim/AP/lamp/12", "cmd_a_led");
    bridge.addBooleanDataRef("1-sim/AP/lamp/14", "cmd_b_led");
    bridge.addBooleanDataRef("1-sim/AP/lamp/13", "cws_a_led");

    // The FMC mappings commented out in ff_757.ts (CDU display and keys) were not ported. They were
    // meant for the removed web cockpit.
}
