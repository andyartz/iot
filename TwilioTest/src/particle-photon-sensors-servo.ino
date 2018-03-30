// -----------------------------------------------------------------------------
// SENG 5199: IoT
//
// Assignment: Particle Photon - Sensors and Peripherals
//
// Gain experience with sensors, actuators, and switches.
// -----------------------------------------------------------------------------

const int SERVO_PIN = D0;
const int SWITCH_PIN = A0;

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
const int INITIAL_POSITION = 0;

Servo servo1;
int servoPosition = 0;
int inputPosition = 0;
int resetCount = 0;
bool lastState = FALSE;
bool currentState = FALSE;

void setup() {
  setupSwitches();
  setupServo();
  setupLeds();
  setupSerialConnection();
}

void setupSwitches() {
  setupSwitch(SWITCH_PIN);
}

void setupSwitch(int switchPin) {
  pinMode(switchPin, INPUT_PULLDOWN);
}

void setupServo() {
  servo1.attach(SERVO_PIN);
  setServoPosition(INITIAL_POSITION);
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
  log("Lab3 v5.3.4");

  blink(LOCAL_LED);

  lastState = currentState;
  currentState = switchPressed();
  log("Last state: " + String(lastState));
  log("Current state: " + String(switchPressed()));

  if (switchPressedJustNow()) {
    log("Switch pressed!");
    inputPosition = (inputPosition + 20) % 200;
    rotateServoTo(inputPosition);
    if (inputPosition == 0) {
      resetCount++;
      emit("servo", String(resetCount));
    }
  }

  waitSeconds(1);
}

bool switchPressedJustNow() {
    return !lastState && currentState;
}

bool switchPressed() {
  return digitalRead(SWITCH_PIN) == 1;
}

void rotateServoTo(int targetPosition) {
  while(servoPosition != targetPosition) {
    rotateToward(targetPosition);
  }
}

void rotateToward(int targetPosition) {
  int direction = compare(servoPosition, targetPosition);
  int newPosition = servoPosition + direction;
  setServoPosition(newPosition);
}

void setServoPosition(int positionInDegrees) {
  log("Setting servoPosition to " + String(positionInDegrees));
  servo1.write(positionInDegrees);
  delay(2);
  servoPosition = positionInDegrees;
}

int compare(int current, int target) { // this returns a one unit step from current to target
  if (current < target) return 1;
  if (current > target) return -1;
  return 0;
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
