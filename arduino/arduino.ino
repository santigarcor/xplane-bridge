#include <LedControl.h>
#include <Encoder.h>
#include <ArduinoJson.h>
#define ENCODER_USE_INTERRUPTS

const long BAUD_RATE = 57600;

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
 * Switches / Buttons
 */
const int SWITCH_TYPE_MOMENTARY = 1;
const int SWITCH_TYPE_TOGGLE = 2;
 // Poll every 10ms
const unsigned long SWITCH_POLL_INTERVAL = 10; 
// Switches / Buttons count
const int SWITCHES_COUNT = 3;
const int ENCODER_ALTITUDE_SWITCH = 5;
const int ENCODER_HEADING_SWITCH = 7;
const int ENCODER_SPEED_SWITCH = 17;


// Switch data structure (removed volatile since no interrupts)
struct SwitchData {
  int pin;
  const char* name;
  const char* name_on;        // Command name when switch is ON (for toggles)
  const char* name_off;       // Command name when switch is OFF (for toggles)
  int switch_type;            // SWITCH_TYPE_MOMENTARY or SWITCH_TYPE_TOGGLE
  bool pressed;               // For momentary switches
  bool last_state;            // Track previous pin state
  bool current_toggle_state;  // Current ON/OFF state for toggle switches
  unsigned long last_change_time;     // Last time state changed
};

// Switch configuration array
SwitchData switches[SWITCHES_COUNT] = {
  // Momentary switches (buttons) - send command on press only
  {SW_MASTER_CAUTION, "master_caution", "", "", SWITCH_TYPE_MOMENTARY, false, HIGH, false, 0},
  
  
  // Toggle switches - send different commands for ON and OFF positions
  {SW_FD_BARS, SWITCH_FD_BARS, "", "fd_bars_on", "fd_bars_off", SWITCH_TYPE_TOGGLE, false, HIGH, false, 0},
  {SW_ACT_PRE_PIN, SWITCH_ACT_PRE, "", "act_pre_on", "act_pre_off", SWITCH_TYPE_TOGGLE, false, HIGH, false, 0},
};



// Timing variables for switch polling
unsigned long lastSwitchPoll = 0;

/*
 * Displays
 */
const int DISPLAY_COUNT = 2;
const int DISPLAY_PIN_DIN = 48;
const int DISPLAY_PIN_CS  = 49;
const int DISPLAY_PIN_CLK = 50;
const int DISPLAY_COMMANDS_COUNT = 2;

struct DisplayCommand {
  const char* name;
  int displayToShow;
  int firstDigitPosition;
  int maxLength;
};



LedControl lc = LedControl(DISPLAY_PIN_DIN, DISPLAY_PIN_CLK, DISPLAY_PIN_CS, DISPLAY_COUNT);
DisplayCommand displayCommands[DISPLAY_COMMANDS_COUNT] = {
  {"set_alt", 0, 0, 5}, // set_alt on display #0, with first digit at 0, max length 5.
  {"set_hdg", 1, 0, 3} // set_alt on display #0, with first digit at 5, max length 3.
  {"set_spd", 1, 5, 3} // set_alt on display #0, with first digit at 5, max length 3.
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
    if (diff >= 4) {
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
}

// Non-blocking switch polling function
void pollSwitches() {
  // Only poll every 10ms for good responsiveness without overwhelming CPU
  if (millis() - lastSwitchPoll < SWITCH_POLL_INTERVAL) {
    return;
  }
  lastSwitchPoll = millis();

  // Check each switch for state changes
  for (int i = 0; i < SWITCHES_COUNT; i++) {
    bool current_state = digitalRead(switches[i].pin);
    
    // Detect state change
    if (current_state != switches[i].last_state) {
      // Debouncing - only process if enough time has passed
      if (millis() - switches[i].last_change_time > DEBOUNCE_DELAY) {
        
        if (switches[i].switch_type == SWITCH_TYPE_MOMENTARY) {
          // MOMENTARY SWITCH: Only trigger on press (HIGH to LOW)
          if (switches[i].last_state == HIGH && current_state == LOW) {
            switches[i].pressed = true;
            
            Serial.print("MOMENTARY SWITCH PRESSED: ");
            Serial.print(switches[i].name);
            Serial.print(" (pin ");
            Serial.print(switches[i].pin);
            Serial.println(")");
          }
        } 
        else if (switches[i].switch_type == SWITCH_TYPE_TOGGLE) {
          // TOGGLE SWITCH: Trigger on both ON and OFF positions
          bool new_toggle_state = (current_state == LOW); // LOW = ON position
          
          if (new_toggle_state != switches[i].current_toggle_state) {
            switches[i].current_toggle_state = new_toggle_state;
            switches[i].pressed = true; // Use this flag to process in main loop
            
            Serial.print("TOGGLE SWITCH CHANGED: ");
            if (new_toggle_state) {
              Serial.print(switches[i].name_on);
              Serial.println(" (switched ON)");
            } else {
              Serial.print(switches[i].name_off);
              Serial.println(" (switched OFF)");
            }
            Serial.print("Pin ");
            Serial.print(switches[i].pin);
            Serial.print(" state: ");
            Serial.println(current_state ? "HIGH" : "LOW");
          }
        }
        
        // Update state tracking
        switches[i].last_state = current_state;
        switches[i].last_change_time = millis();
      }
    }
  }
}


// Send encoder rotation events to Python
void sendEncoderEvent(const char* encoder_name, const char* direction) {
  Serial.print("{\"user_input\":\"");
  Serial.print(encoder_name);
  Serial.print("_");
  Serial.print(direction);
  Serial.println("\"}");
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


void setup() {
  Serial.begin(BAUD_RATE);
  delay(1000);


  // Initialize LED Displays
  for (int display = 0; display < lc.getDeviceCount(); display++) {
    lc.shutdown(display, false);
    lc.setIntensity(display, 1);
    lc.clearDisplay(display);
    // lc.setScanLimit(display, 8);
  }

  // Initialize encoders
  for (int i = 0; i < ENCODERS_COUNT; i++) {
    encoders[i].lastPosition = encoders[i].encoder.read();
  }

  Serial.println("Encoders initialized");

  Serial.println("POLLING system initialized");
  Serial.println("Ready for input...");
}

void loop() {
  // POLLING-based main loop
  // poll_switches();           // Check all switches every 10ms
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