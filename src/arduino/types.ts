/**
 * Represents the message sent to the Arduino containing a command and its value.
 */
export type ArduinoCommand = {
  cmd: string
  value: any
}

/**
 * Represents the message received from the Arduino containing user input.
 */
export type ArduinoMessage = {
  user_input: string
}
