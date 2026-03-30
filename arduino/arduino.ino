#include <LedControl.h>
#include <Encoder.h>
#include <ArduinoJson.h>

#define ENCODER_USE_INTERRUPTS

const long BAUD_RATE = 57600;

/*
 * Rotary Encoders
 */
// Poll every 5ms
const unsigned long ENCODER_POLL_INTERVAL = 5;
// Rotary Encoders count
const int ENCODERS_COUNT = 6;

// Rotary encoders pins definition. Each one should be on interrupt pins.
const int ENCODER_COURSE_1_PIN_DT = 21;
const int ENCODER_COURSE_1_PIN_CLK = 17;

const int ENCODER_SPEED_PIN_DT = 20;
const int ENCODER_SPEED_PIN_CLK = 16;

const int ENCODER_HEADING_PIN_DT = 19;
const int ENCODER_HEADING_PIN_CLK = 15;

const int ENCODER_ALTITUDE_PIN_DT = 18;
const int ENCODER_ALTITUDE_PIN_CLK = 14;

const int ENCODER_VERTICAL_SPEED_PIN_DT = 2;
const int ENCODER_VERTICAL_SPEED_PIN_CLK = 44;

const int ENCODER_COURSE_2_PIN_DT = 3;
const int ENCODER_COURSE_2_PIN_CLK = 45;

struct EncoderData {
  Encoder encoder;
  const char *name;
  int lastPosition;
};

EncoderData encoders[ENCODERS_COUNT] = {
  { { ENCODER_COURSE_1_PIN_DT, ENCODER_COURSE_1_PIN_CLK }, "course_1_encoder", 0 },
  { { ENCODER_SPEED_PIN_DT, ENCODER_SPEED_PIN_CLK }, "speed_encoder", 0 },
  { { ENCODER_HEADING_PIN_DT, ENCODER_HEADING_PIN_CLK }, "heading_encoder", 0 },
  { { ENCODER_ALTITUDE_PIN_DT, ENCODER_ALTITUDE_PIN_CLK }, "altitude_encoder", 0 },
  { { ENCODER_VERTICAL_SPEED_PIN_DT, ENCODER_VERTICAL_SPEED_PIN_CLK }, "vertical_speed_encoder", 0 },
  { { ENCODER_COURSE_2_PIN_DT, ENCODER_COURSE_2_PIN_CLK }, "course_2_encoder", 0 }
};
// Timing variables for encoder polling
unsigned long lastEncoderPoll = 0;

/*
 * Switches (Momentary/Buttons, Toggle)
 */
const int SWITCH_MOMENTARY = 0;
const int SWITCH_TOGGLE = 1;
// Poll every 10ms
const unsigned long SWITCH_POLL_INTERVAL = 10;
// Debounce Momentary switches/buttons and toggles actions
const int DEBOUNCE_DELAY = 150;
// Switches count
const int SWITCHES_COUNT = 31;
// Switch pins definition
const int SWITCH_AT_ARM = 22;
const int SWITCH_FLIGHT_DIRECTOR_1 = 24;
const int SWITCH_N1 = 26;
const int SWITCH_SPEED = 28;
const int SWITCH_C_O = 30;
const int SWITCH_SPEED_INTV = 32;
const int SWITCH_VNAV = 34;
const int SWITCH_LVL_CHG = 36;
const int SWITCH_HDG_SEL = 38;
const int SWITCH_APP = 40;

const int SWITCH_LNAV = 23;
const int SWITCH_VOR_LOC = 25;
const int SWITCH_ALT_HLD = 27;
const int SWITCH_VS_HLD = 29;
const int SWITCH_ALT_INTV = 31;
const int SWITCH_CWS_A = 33;
const int SWITCH_CWS_B = 35;
const int SWITCH_CMD_A = 37;
const int SWITCH_CMD_B = 39;
const int SWITCH_DISENGAGE = 41;

const int SWITCH_FLIGHT_DIRECTOR_2 = 43;

const int SWITCH_LANDING_L = 7;
const int SWITCH_LANDING_R = 6;
const int SWITCH_RUNWAY_R = 4;
const int SWITCH_RUNWAY_L = 5;

const int SWITCH_TAXI = 8;
const int SWITCH_LOGO = 11;
const int SWITCH_POS_1 = 9;
const int SWITCH_POS_2 = 10;
const int SWITCH_ANTI_COL = 12;
const int SWITCH_WING = 13;

unsigned long lastSwitchPoll = 0;

struct SwitchData {
  int pin;
  const char *name;
  int type;
  bool changed;
  bool lastState;
  unsigned long lastChangeTime;
};

SwitchData switches[SWITCHES_COUNT] = {
  { SWITCH_AT_ARM, "at_arm", SWITCH_TOGGLE, false, false, 0 },
  { SWITCH_FLIGHT_DIRECTOR_1, "flight_director_1", SWITCH_TOGGLE, false, false, 0 },
  { SWITCH_C_O, "c_o", SWITCH_MOMENTARY, false, false, 0 },
  { SWITCH_N1, "n1", SWITCH_MOMENTARY, false, false, 0 },
  { SWITCH_SPEED, "speed", SWITCH_MOMENTARY, false, false, 0 },
  { SWITCH_SPEED_INTV, "speed_intv", SWITCH_MOMENTARY, false, false, 0 },
  { SWITCH_VNAV, "vnav", SWITCH_MOMENTARY, false, false, 0 },
  { SWITCH_LVL_CHG, "lvl_chg", SWITCH_MOMENTARY, false, false, 0 },
  { SWITCH_HDG_SEL, "hdg_sel", SWITCH_MOMENTARY, false, false, 0 },
  { SWITCH_LNAV, "lnav", SWITCH_MOMENTARY, false, false, 0 },
  { SWITCH_VOR_LOC, "vor_loc", SWITCH_MOMENTARY, false, false, 0 },
  { SWITCH_APP, "app", SWITCH_MOMENTARY, false, false, 0 },
  { SWITCH_ALT_HLD, "alt_hld", SWITCH_MOMENTARY, false, false, 0 },
  { SWITCH_VS_HLD, "vs_hld", SWITCH_MOMENTARY, false, false, 0 },
  { SWITCH_ALT_INTV, "alt_intv", SWITCH_MOMENTARY, false, false, 0 },
  { SWITCH_CMD_A, "cmd_a", SWITCH_MOMENTARY, false, false, 0 },
  { SWITCH_CMD_B, "cmd_b", SWITCH_MOMENTARY, false, false, 0 },
  { SWITCH_CWS_A, "cws_a", SWITCH_MOMENTARY, false, false, 0 },
  { SWITCH_CWS_B, "cws_b", SWITCH_MOMENTARY, false, false, 0 },
  { SWITCH_DISENGAGE, "disengage", SWITCH_TOGGLE, false, false, 0 },
  { SWITCH_FLIGHT_DIRECTOR_2, "flight_director_2", SWITCH_TOGGLE, false, false, 0 },
  { SWITCH_LANDING_L, "landing_l", SWITCH_TOGGLE, false, false, 0 },
  { SWITCH_LANDING_R, "landing_r", SWITCH_TOGGLE, false, false, 0 },
  { SWITCH_RUNWAY_L, "runway_l", SWITCH_TOGGLE, false, false, 0 },
  { SWITCH_RUNWAY_R, "runway_r", SWITCH_TOGGLE, false, false, 0 },
  { SWITCH_TAXI, "taxi", SWITCH_TOGGLE, false, false, 0 },
  { SWITCH_LOGO, "logo", SWITCH_TOGGLE, false, false, 0 },
  { SWITCH_POS_1, "position_steady", SWITCH_TOGGLE, false, false, 0 },
  { SWITCH_POS_2, "position_strobe", SWITCH_TOGGLE, false, false, 0 },
  { SWITCH_ANTI_COL, "anti_col", SWITCH_TOGGLE, false, false, 0 },
  { SWITCH_WING, "wing", SWITCH_TOGGLE, false, false, 0 },
};

/*
 * Displays
 */
const int DISPLAYS_COUNT = 5;
const int DISPLAY_PIN_DIN = 48;
const int DISPLAY_PIN_CS = 50;
const int DISPLAY_PIN_CLK = 52;
const int DISPLAY_COMMANDS_COUNT = 6;

struct DisplayCommand {
  const char *name;
  int displayToShow;
  int firstDigitPosition;
  int maxLength;
  bool showSign;
  bool showLeadingZeros;
  bool enabled;
  long lastValue;
};

LedControl ledControl = LedControl(DISPLAY_PIN_DIN, DISPLAY_PIN_CLK, DISPLAY_PIN_CS, DISPLAYS_COUNT);
// All displayCommands' name MUST start with set_
DisplayCommand displayCommands[DISPLAY_COMMANDS_COUNT] = {
  { "set_course_1", 0, 5, 3, false, true, true },        // set_course_1 on display #0, with first digit at 5, max length 3.
  { "set_speed", 0, 0, 3, false, true, true },           // set_spd on display #0, with first digit at 0, max length 3.
  { "set_heading", 1, 0, 3, false, true, true },         // set_hdg on display #1, with first digit at 0, max length 3.
  { "set_altitude", 2, 1, 5, false, false, true },       // set_alt on display #2, with first digit at 1, max length 5.
  { "set_vertical_speed", 3, 3, 5, true, false, true },  // set_v_spd on display #3, with first digit at 3, max length 5.
  { "set_course_2", 4, 0, 3, false, true, true }         // set_course_2 on display #4, with first digit at 5, max length 3.
};

/*
 * LEDs
 */
const int LEDS_COUNT = 17;
struct LedCommand {
  const char *name;
  int pin;
};

LedCommand leds[LEDS_COUNT] = {
  { "at_arm_led", A15 },
  { "flight_director_1_led", A14 },
  { "n1_led", A13 },
  { "speed_led", A12 },
  { "lvl_chg_led", A11 },
  { "heading_led", A10 },
  { "v_nav_led", A9 },
  { "l_nav_led", A8 },

  { "vor_loc_led", A7 },
  { "app_led", A6 },
  { "alt_hld_led", A5 },
  { "vertical_speed_led", A4 },
  { "cmd_a_led", A3 },
  { "cmd_b_led", A2 },
  { "cws_a_led", A1 },
  { "cws_b_led", A0 },

  { "flight_director_2_led", 42 }
};

// Non-blocking encoder polling function
void pollEncoders() {
  // Only poll every 2ms for high responsiveness
  if (millis() - lastEncoderPoll < ENCODER_POLL_INTERVAL) {
    return;
  }
  lastEncoderPoll = millis();

  // Check each encoder for rotation
  for (int i = 0; i < ENCODERS_COUNT; i++) {
    int currentPosition = encoders[i].encoder.read();
    int diff = abs(currentPosition - encoders[i].lastPosition);

    // Detect encoder rotation on A channel change
    if (diff < 4) {
      continue;
    }

    if (currentPosition > encoders[i].lastPosition) {
      // Clockwise rotation
      sendEncoderEvent(encoders[i].name, "increment");
    } else {
      // Counter-clockwise rotation
      sendEncoderEvent(encoders[i].name, "decrement");
    }
    // Update state tracking
    encoders[i].lastPosition = currentPosition;
  }
}

// Send encoder rotation events to Python
void sendEncoderEvent(const char *encoder_name, const char *direction) {
  Serial.print("{\"user_input\":\"");
  Serial.print(encoder_name);
  Serial.print("_");
  Serial.print(direction);
  Serial.println("\"}");
}

// Non-blocking toggle switches polling function
void pollSwitches() {
  // Only poll every 10ms for good responsiveness without overwhelming CPU
  if (millis() - lastSwitchPoll < SWITCH_POLL_INTERVAL) {
    return;
  }
  lastSwitchPoll = millis();

  // Check each switch for state changes
  for (int i = 0; i < SWITCHES_COUNT; i++) {
    bool currentPinState = digitalRead(switches[i].pin);
    bool currentState = switches[i].type == SWITCH_TOGGLE
                          ? (currentPinState == LOW)  // LOW = ON position
                          : currentPinState;

    // If it didn't change continue to next switch
    if (currentState == switches[i].lastState) {
      continue;
    }

    // If debounce time hasn't passed continue until is happens
    if (millis() - switches[i].lastChangeTime <= DEBOUNCE_DELAY) {
      continue;
    }

    // MOMENTARY SWITCH: Only trigger on press (HIGH to LOW)
    if (switches[i].type == SWITCH_MOMENTARY && switches[i].lastState == HIGH && currentState == LOW) {
      switches[i].changed = true;
      switches[i].lastState = currentState;
      switches[i].lastChangeTime = millis();

      Serial.print("MOMENTARY SWITCH PRESSED: ");
      Serial.print(switches[i].name);
      Serial.print(" (pin ");
      Serial.print(switches[i].pin);
      Serial.println(")");
    } else if (switches[i].type == SWITCH_MOMENTARY && switches[i].lastState == LOW) {
      // MOMENTARY SWITCH: If was LOW and now is HIGH revert to HIGH.
      switches[i].lastState = currentState;
    } else if (switches[i].type == SWITCH_TOGGLE) {
      // TOGGLE SWITCH: Trigger on both ON and OFF positions
      Serial.print("TOGGLE SWITCH CHANGED: ");
      Serial.print(switches[i].name);
      if (currentState) {
        Serial.print(" (switched ON)");
      } else {
        Serial.print(" (switched OFF)");
      }

      Serial.print("Pin ");
      Serial.print(switches[i].pin);
      Serial.print(" state: ");
      Serial.println(currentState ? "LOW" : "HIGH");

      switches[i].lastState = currentState;
      switches[i].changed = true;  // Use this flag to process in main loop
      switches[i].lastChangeTime = millis();
    }
  }
}

bool hasMoreThanXDigits(long number, int x) {
  return abs(number) >= pow(10, x);
}

void setDisplay(DisplayCommand *displayCommand, long value) {
  displayCommand->lastValue = value;
  if (!displayCommand->enabled) {
    return;
  }

  int digitPosition = displayCommand->firstDigitPosition;
  int maxLength = displayCommand->maxLength - (displayCommand->showSign ? 1 : 0);
  int maxDigitPosition = maxLength + displayCommand->firstDigitPosition - 1;  // If first digit is 0 and max is 5 the max digit position is 4 starting at 0

  // Clear the display before update
  // ledControl.clearDisplay(displayCommand.displayToShow);

  // If the value exceds the display max length we display Err
  if (hasMoreThanXDigits(value, maxLength)) {
    ledControl.setChar(displayCommand->displayToShow, displayCommand->firstDigitPosition + 2, 'E', false);
    ledControl.setRow(displayCommand->displayToShow, displayCommand->firstDigitPosition + 1, B00000101);  // r
    ledControl.setRow(displayCommand->displayToShow, displayCommand->firstDigitPosition, B00000101);      // r
    return;
  }

  if (displayCommand->showSign && value < 0) {
    ledControl.setChar(displayCommand->displayToShow, displayCommand->firstDigitPosition + maxLength, '-', false);
  } else if (displayCommand->showSign && value > 0) {
    ledControl.setChar(displayCommand->displayToShow, displayCommand->firstDigitPosition + maxLength, ' ', false);
  }

  while (digitPosition <= maxDigitPosition) {
    byte digitValue = abs(value) % 10;  // Get the last number (ex: of 123, we get 3)
    if (!displayCommand->showLeadingZeros && value == 0 && digitPosition > displayCommand->firstDigitPosition) {
      ledControl.setRow(displayCommand->displayToShow, digitPosition, B00000000);  // blank leading zeros
    } else {
      ledControl.setDigit(displayCommand->displayToShow, digitPosition, digitValue, false);
    }
    value = value / 10;  // Remove last number (ex: from 123 to 12)
    digitPosition++;     // next digit position
  }
}

/**
 * Toggles the display off
 */
void toggleDisplay(String command, long value) {
  String cmd = command.substring(15, command.length());

  for (int i = 0; i < DISPLAY_COMMANDS_COUNT; i++) {
    if (cmd == displayCommands[i].name) {
      displayCommands[i].enabled = (value != 0);
      if (! displayCommands[i].enabled) {
        for (int j = displayCommands[i].firstDigitPosition; j < (displayCommands[i].firstDigitPosition + displayCommands[i].maxLength); j++) {
          ledControl.setRow(displayCommands[i].displayToShow, j, B00000000);
        }
      } else { 
        setDisplay(&displayCommands[i], displayCommands[i].lastValue);
      }
      break;
    }
  }
}

void processSerialInput() {
  if (!Serial.available()) {
    return;
  }
  String input = Serial.readStringUntil('\n');

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, input);

  if (error) {
    return;
  }

  String cmd = doc["cmd"];

  // Command for displays
  if (cmd.startsWith("set_")) {
    for (int i = 0; i < DISPLAY_COMMANDS_COUNT; i++) {
      if (cmd == displayCommands[i].name) {
        setDisplay(&displayCommands[i], doc["value"]);
        break;
      }
    }
  } else if (cmd.startsWith("toggle_display")) {
    toggleDisplay(cmd, doc["value"]);
  } else {
    // Instruction for LEDs
    int i = 0;
    bool ledFound = false;

    while (i < LEDS_COUNT && !ledFound) {
      if (cmd == leds[i].name) {
        ledFound = true;
      } else {
        i++;
      }
    }

    if (ledFound) {
      digitalWrite(leds[i].pin, doc["value"] == 0 ? LOW : HIGH);
    }
  }
}

void sendSwitchInputs() {
  for (int i = 0; i < SWITCHES_COUNT; i++) {
    if (switches[i].changed) {
      switches[i].changed = false;  // Reset flag

      if (switches[i].type == SWITCH_MOMENTARY) {
        // MOMENTARY SWITCH: Send single command
        Serial.print("{\"user_input\":\"");
        Serial.print(switches[i].name);
        Serial.println("\"}");
      } else if (switches[i].type == SWITCH_TOGGLE) {
        // TOGGLE SWITCH: Send appropriate ON or OFF command
        Serial.print("{\"user_input\":\"");
        Serial.print(switches[i].name);  // Switch is ON
        Serial.print(switches[i].lastState ? "_on" : "_off");
        Serial.println("\"}");
      }
    }
  }
}

void initSwitches() {
  for (int i = 0; i < SWITCHES_COUNT; i++) {
    pinMode(switches[i].pin, INPUT_PULLUP);
  }
  // Let the pins settle
  delay(100);

  // Initialize switch states
  for (int i = 0; i < SWITCHES_COUNT; i++) {
    bool pinState = digitalRead(switches[i].pin);
    switch (switches[i].type) {
      case SWITCH_TOGGLE:
        switches[i].lastState = (pinState == LOW);  // LOW = ON position
        break;
      case SWITCH_MOMENTARY:
        switches[i].lastState = pinState;
        break;
    }
  }
}

void initDisplays() {
  // Initialize 8 digits Displays
  for (int display = 0; display < ledControl.getDeviceCount(); display++) {
    ledControl.shutdown(display, false);
    ledControl.setIntensity(display, 0);
    ledControl.clearDisplay(display);
    // ledControl.setScanLimit(display, 8);
  }
}

void initEncoders() {
  // Initialize encoders
  for (int i = 0; i < ENCODERS_COUNT; i++) {
    encoders[i].lastPosition = encoders[i].encoder.read();
  }
}

void initLeds() {
  // Initialize LED pins
  for (int i = 0; i < LEDS_COUNT; i++) {
    pinMode(leds[i].pin, OUTPUT);
    digitalWrite(leds[i].pin, LOW);
  }
}

// Performance monitoring (optional)
void print_performance_stats() {
  static unsigned long last_stats = 0;
  static unsigned long loop_count = 0;

  loop_count++;

  // Print stats every 5 seconds
  if (millis() - last_stats > 5000) {
    Serial.print("Loop frequency: ");
    Serial.print(loop_count / 5);
    Serial.println(" Hz");
    loop_count = 0;
    last_stats = millis();
  }
}

void test_led() {
  for (int display = 0; display < DISPLAYS_COUNT; display++) {
    for (int pos = 7; pos >= 0; pos--) {
      int digit = 8 - pos;
      ledControl.setDigit(display, pos, digit, false);
      delay(200);
    }
  }

  // Clear all displays
  for (int display = 0; display < DISPLAYS_COUNT; display++) {
    ledControl.clearDisplay(display);
  }
}

void setup() {
  Serial.begin(BAUD_RATE);
  delay(1000);

  initSwitches();
  initDisplays();
  initEncoders();
  initLeds();

  Serial.println("POLLING system initialized");
  Serial.println("Ready for input...");
}

void loop() {
  // POLLING-based main loop
  pollSwitches();        // Check all switches every 10ms
  pollEncoders();        // Check all encoders every 2ms
  sendSwitchInputs();    // Send any button press events
  processSerialInput();  // Handle incoming commands from NodeJS

  // Optional: Performance monitoring
  // print_performance_stats();

  // Optional: Test LED displays
  // test_led();

  // No delays! Let the loop run as fast as possible
  // Typical loop time will be 1-2ms = 500-1000 Hz
}