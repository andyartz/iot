// -----------------------------------------------------------------------------
// SENG 5199: IoT
//
// Assignment: Particle Photon - Sensors and Peripherals
//
// Gain experience with sensors, actuators, and switches.
// -----------------------------------------------------------------------------

const int SPDT_SWITCH_YELLOW = D1;
const int SPDT_SWITCH_WHITE = D2;
const int MOMENTARY_SWITCH_BLUE = D5;

const int LOCAL_LED = D7;
const int ONE_SECOND_IN_MILLIS = 1000;
const int ONE_HUNDRED_MILLISECONDS = 100;
const int ONE_MILLISECOND = 1;
const int ONE_SECOND = 1;
const int MAXIMUM_OUTPUT = 100;
const int THRESHOLD = MAXIMUM_OUTPUT / 2;
const int BAUD_RATE = 115200;
const float MILLIVOLTS_CONVERSION_FACTOR = 0.806;

const int BLINK_TIME = ONE_SECOND_IN_MILLIS;

void setup() {
  setupLeds();
  setupSwitches();
  setupSerialConnection();
}

void setupSwitches() {
  setupSwitch(SPDT_SWITCH_WHITE);
  setupSwitch(SPDT_SWITCH_YELLOW);
  setupSwitch(MOMENTARY_SWITCH_BLUE);
}

void setupSwitch(int switchPin) {
  pinMode(switchPin, INPUT_PULLDOWN);
}

void setupLeds() {
  setupLed(LOCAL_LED);
}

void setupLed(int led) {
  pinMode(led, OUTPUT);
}

void setupSerialConnection() {
  Serial.begin(BAUD_RATE);
}

void loop() {
  log(" ");
  log("Lab3 v4.3.2");

  blink(LOCAL_LED);
  readSwitchesAndEmit();
  waitSeconds(10);
}

void readSwitchesAndEmit() {
  int yellow = readSwitch(SPDT_SWITCH_YELLOW, "SPDT Yellow");
  int white = readSwitch(SPDT_SWITCH_WHITE, "SPDT White");
  int momentary = readSwitch(MOMENTARY_SWITCH_BLUE, "Momentary");
  emit("switches", String(10*momentary + 2*white)); // we can ignore yellow, since it is always the opposite of white. So 0/2/10/12.
}

int readSwitch(int switchPin, String name) {
  int reading = digitalRead(switchPin);
  log(name + ": " + String(reading));
  return reading;
}

void blink(int pin) {
  log("blinking pin " + String(pin));
  turnOn(pin);
  delay(BLINK_TIME);
  turnOff(pin);
  delay(BLINK_TIME);
}

void turnOn(int pin) {
  digitalWrite(pin, HIGH);
}

void turnOff(int pin) {
  digitalWrite(pin, LOW);
}

void waitSeconds(int secondsToWait) {
  logSameLine("Waiting");
  for(int i=0; i<secondsToWait; i++) {
    delay(ONE_SECOND_IN_MILLIS);
    logSameLine(".");
  }
  logSameLine("\n");
}

void log(String message) {
  Serial.println(message);
}

void logSameLine(String message) {
  Serial.printf(message);
}

void emit(String channel, int value) {
  emit(channel, String(value));
}

void emit(String channel, String message) {
  Particle.publish(channel, message, PRIVATE);
  log("Emitted on channel '" + channel + "': " + message);
}
