#include <LedControl.h>
#include <Encoder.h>
#include <ArduinoJson.h>
#define ENCODER_USE_INTERRUPTS

const long BAUD_RATE = 57600;

// Debounce Momentary switches/buttons and toggles actions
const int DEBOUNCE_DELAY = 50;

/*
 * Rotary Encoders
 */
// Poll every 10ms
const unsigned long ENCODER_POLL_INTERVAL = 10; 
// Rotary Encoders count
const int ENCODERS_COUNT = 3;
// Rotary Encoders configuration pins. Each one should be on interrupt pins.
const int ENCODER_ALTITUDE_PIN_DT = 2;
const int ENCODER_ALTITUDE_PIN_CLK = 4;
const int ENCODER_HEADING_PIN_DT = 3;
const int ENCODER_HEADING_PIN_CLK = 6;
const int ENCODER_SPEED_PIN_DT = 18;
const int ENCODER_SPEED_PIN_CLK = 16;

struct EncoderData {
  Encoder encoder;
  const char* name;
  int lastPosition;
};

EncoderData encoders[ENCODERS_COUNT] = {
  {{ENCODER_ALTITUDE_PIN_DT, ENCODER_ALTITUDE_PIN_CLK}, "altitude_encoder", 0},
  {{ENCODER_HEADING_PIN_DT, ENCODER_HEADING_PIN_CLK}, "heading_encoder", 0},
  {{ENCODER_SPEED_PIN_DT, ENCODER_SPEED_PIN_CLK}, "speed_encoder", 0}
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
// Toggles count
const int SWITCHES_COUNT = 2;
const int SWITCH_FLIGHT_DIRECTOR = 5;
const int SWITCH_CMD = 8;

struct SwitchData {
  int pin;
  const char* name;
  int type;
  bool changed;
  bool lastState;
  unsigned long lastChangeTime;
}

SwitchData switches[SWITCHES_COUNT] = {
  {SWITCH_FLIGHT_DIRECTOR, "flight_director", SWITCH_TOGGLE, false,false, 0},
  {SWITCH_CMD, "autopilot_cmd", SWITCH_MOMENTARY, false,false, 0},
}

unsigned long lastSwitchPoll = 0;

/*
 * Displays
 */
const int DISPLAYS_COUNT = 3;
const int DISPLAY_PIN_DIN = 48;
const int DISPLAY_PIN_CS  = 49;
const int DISPLAY_PIN_CLK = 50;
const int DISPLAY_COMMANDS_COUNT = 4;

struct DisplayCommand {
  const char* name;
  int displayToShow;
  int firstDigitPosition;
  int maxLength;
};



LedControl lc = LedControl(DISPLAY_PIN_DIN, DISPLAY_PIN_CLK, DISPLAY_PIN_CS, DISPLAYS_COUNT);
// All displayCommands' name MUST start with set_
DisplayCommand displayCommands[DISPLAY_COMMANDS_COUNT] = {
  {"set_hdg", 0, 0, 3} // set_hdg on display #0, with first digit at 0, max length 3.
  {"set_spd", 0, 5, 3} // set_spd on display #0, with first digit at 5, max length 3.
  {"set_alt", 1, 0, 5}, // set_alt on display #1, with first digit at 0, max length 5.
  {"set_v_spd", 2, 3, 5}, // set_v_spd on display #2, with first digit at 3, max length 5.
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
void sendEncoderEvent(const char* encoder_name, const char* direction) {
  Serial.println("{\"user_input\":\"" + encoder_name + "_" + direction + "\"}")
  // Serial.print("{\"user_input\":\"");
  // Serial.print(encoder_name);
  // Serial.print("_");
  // Serial.print(direction);
  // Serial.println("\"}");
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
    bool currentPinState = digitalRead(toggles[i].pin);
    bool currentState = switches[i].type == SWITCH_TOGGLE 
      ? (currentPinState == LOW) // LOW = ON position
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
      Serial.print(switches[i].name + " (pin " + switches[i].pin + ")");
    } else if(switches[i].type == SWITCH_TOGGLE) {
      Serial.print("TOGGLE SWITCH CHANGED: ");
      if (currentState) {
        Serial.print(switches[i].name + " (switched ON)");
      } else {
        Serial.print(switches[i].name + " (switched OFF)");
      }

      Serial.print("Pin ");
      Serial.print(switches[i].pin);
      Serial.print(" state: ");
      Serial.println(currentState ? "LOW" : "HIGH");

      // Trigger on both ON and OFF positions
      toggles[i].lastState = newState;
      toggles[i].changed = true; // Use this flag to process in main loop
      toggles[i].lastChangeTime = millis();
    }
  }
}

void setDisplay(DisplayCommand displayCommand,long value) {
  int digitPosition = displayCommand.firstDigitPosition;
  while (digitPosition < (displayCommand.maxLength + displayCommand.firstDigitPosition)) {
    byte digitValue = value % 10;      // Get the last number (ex: of 123, we get 3)
    lc.setDigit(displayCommand.displayToShow, digitPosition, digitValue, false);
    value = value / 10;                // Remove last number (ex: from 123 to 12)
    digitPosition++;                 // next digit position
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
  if (cmd.startsWith("set_")) {
    for (int i = 0; i < DISPLAY_COMMANDS_COUNT; i++) {
      if (cmd == displayCommands[i].name){
        setDisplay(displayCommands[i], doc["value"]);
        break;
      }
    }
  } else {

  }

  // if (cmd == "set_course1") {
  // } else {
  //   bool led_found = false;
  //   for (int i = 0; i < num_led_commands; i++) {
  //     if (cmd == led_commands[i].cmd_name) {
  //       set_LED(led_commands[i].led_pin, doc["value"]);
  //       led_found = true;
  //       break;
  //     }
  //   }
  // }
}


void initSwitches() {
  for (int i = 0; i < SWITCHES_COUNT; i++){
    pinMode(switches[i].pin, INPUT_PULLUP);
  }
  // Let the pins settle
  delay(100); 
  
  // Initialize switch states
  for (int i = 0; i < SWITCHES_COUNT; i++) {
    bool pinState = digitalRead(switches[i].pin);
    switch (switches[i].type) {
      case SWITCH_TOGGLE:
        switches[i].lastState = (pinState == LOW);// LOW = ON position
        break;
      case SWITCH_MOMENTARY:
        switches[i].lastState = pinState;
        break;
    }
  }
}

void initDisplays() {
  // Initialize 8 digits Displays
  for (int display = 0; display < lc.getDeviceCount(); display++) {
    lc.shutdown(display, false);
    lc.setIntensity(display, 3);
    lc.clearDisplay(display);
    // lc.setScanLimit(display, 8);
  }
}

void initEncoders() {
  // Initialize encoders
  for (int i = 0; i < ENCODERS_COUNT; i++) {
    encoders[i].lastPosition = encoders[i].encoder.read();
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

void test_lcd() {
  for (int display = 0; display < DISPLAYS_COUNT; display++) {
    for (int pos = 7; pos >= 0; pos--) {
      int digit = 8 - pos;
      lc.setDigit(display, pos, digit, false);
      delay(200);
    }
  }
  
  // Clear all displays
  for (int display = 0; display < DISPLAYS_COUNT; display++) {
    lc.clearDisplay(display);
  }
}


void setup() {
  Serial.begin(BAUD_RATE);
  delay(1000);


  initSwitches();
  initDisplays();
  initEncoders();

  Serial.println("POLLING system initialized");
  Serial.println("Ready for input...");
}

void loop() {
  // POLLING-based main loop
  pollSwitches();             // Check all switches every 10ms
  pollEncoders();           // Check all encoders every 2ms
  // process_button_inputs();   // Send any button press events
  processSerialInput();    // Handle incoming commands from NodeJS
  
  // Optional: Performance monitoring
  // print_performance_stats();
  
  // Optional: Test LCD displays
  // test_lcd();
  
  // No delays! Let the loop run as fast as possible
  // Typical loop time will be 1-2ms = 500-1000 Hz
}