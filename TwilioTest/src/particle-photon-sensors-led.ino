// -----------------------------------------------------------------------------
// SENG 5199: IoT
//
// Assignment: Particle Photon - Sensors and Peripherals
//
// Gain experience with sensors, actuators, and switches.
// -----------------------------------------------------------------------------

const int REMOTE_LED_RED = D0;
const int REMOTE_LED_GREEN = D1;
const int REMOTE_LED_BLUE = D2;

const int LOCAL_LED = D7;
const int ONE_SECOND_IN_MILLIS = 1000;
const int ONE_HUNDRED_MILLISECONDS = 100;
const int ONE_MILLISECOND = 1;
const int ONE_SECOND = 1;
const int MAXIMUM_OUTPUT = 100;
const int THRESHOLD = MAXIMUM_OUTPUT / 2;
const int BAUD_RATE = 115200;
const float MILLIVOLTS_CONVERSION_FACTOR = 0.806;

const int BLINK_TIME = ONE_HUNDRED_MILLISECONDS;
const int FADE_SPEED = ONE_MILLISECOND;
const int COLOR_HOLD_TIME = ONE_SECOND;

int RED[3] = {255, 0, 0};
int GREEN[3] = {0, 255, 0};
int BLUE[3] = {0, 0, 255};
int YELLOW[3] = {255, 255, 0};
int CYAN[3] = {0, 255, 255};
int MAGENTA[3] = {255, 0, 255};
int WHITE[3] = {255, 255, 255};
int OFF[3] = {0, 0, 0};

int red = 0;
int green = 0;
int blue = 0;

void setup() {
  setupLeds();
  setupSerialConnection();
}

void setupLeds() {
  setupLed(REMOTE_LED_RED);
  setupLed(REMOTE_LED_GREEN);
  setupLed(REMOTE_LED_BLUE);
  setupLed(LOCAL_LED);
}

void setupLed(int led) {
  pinMode(led, OUTPUT);
}

void setupSerialConnection() {
  Serial.begin(BAUD_RATE);
}

void loop() {
  log("Lab3 v2.3.2");

  blinkLEDs();
  fadeLEDs();
}

void blinkLEDs() {
  blink(REMOTE_LED_RED);
  blink(REMOTE_LED_GREEN);
  blink(REMOTE_LED_BLUE);
}

void fadeLEDs() {
  fadeToAndHoldColor(WHITE);
  fadeToAndHoldColor(RED);
  fadeToAndHoldColor(GREEN);
  fadeToAndHoldColor(BLUE);
  fadeToAndHoldColor(CYAN);
  fadeToAndHoldColor(MAGENTA);
  fadeToAndHoldColor(YELLOW);
  fadeToAndHoldColor(OFF);
}

void fadeToAndHoldColor(int targetColor[3]) {
  fadeToColor(targetColor);
  waitSeconds(COLOR_HOLD_TIME);
}

void fadeToColor(int targetColor[3]) {
  while(!isColor(targetColor)) {
    stepColor(targetColor);
  }
}

bool isColor(int targetColor[3]) {
  return red == targetColor[0] && green == targetColor[1] && blue == targetColor[2];
}

void stepColor(int targetColor[3]) {
  red = red + compare(red, targetColor[0]);
  green = green + compare(green, targetColor[1]);
  blue = blue + compare(blue, targetColor[2]);

  analogWrite(REMOTE_LED_RED, red);
  analogWrite(REMOTE_LED_GREEN, green);
  analogWrite(REMOTE_LED_BLUE, blue);

  delay(FADE_SPEED);
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
