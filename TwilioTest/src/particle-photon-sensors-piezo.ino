// -----------------------------------------------------------------------------
// SENG 5199: IoT
//
// Assignment: Particle Photon - Sensors and Peripherals
//
// Gain experience with sensors, actuators, and switches.
// -----------------------------------------------------------------------------

const int BUZZER = D0;

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

// frequencies of notes in Hertz

const int NOTE_G2 = 98;
const int NOTE_F_SHARP2 = 87;
const int NOTE_E2 = 82;
const int NOTE_D2 = 73;

const int NOTE_G4 = 392;
const int NOTE_F_SHARP4 = 370;
const int NOTE_E4 = 330;
const int NOTE_D4 = 294;

const int NOTE_G6 = 1568;
const int NOTE_F_SHARP6 = 1480;
const int NOTE_E6 = 1319;
const int NOTE_D6 = 1175;

const int NOTE_REST = 0;

// measure timing

const int MEASURE_LENGTH_IN_MILLIS = 2000; // 120 BPM (Beats Per Minute), 4 beats/ measure
const int QUARTER_NOTE = (MEASURE_LENGTH_IN_MILLIS / 4);
const int EIGHTH_NOTE = (MEASURE_LENGTH_IN_MILLIS / 8);

void setup() {
  setupLeds();
  setupBuzzer();
  setupSerialConnection();
}

void setupBuzzer() {
  pinMode(BUZZER, OUTPUT);
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
  log("Lab3 v3.3.0");

  blink(LOCAL_LED);
  buzzTheBuzzer();
  waitSeconds(3);
  singASongLow();
  waitSeconds(3);
  singASongMid();
  waitSeconds(3);
  singASongHigh();
  waitSeconds(3);
}

void buzzTheBuzzer() {
  for(int i=0; i<5; i++) {
    digitalWrite(BUZZER, HIGH);
    delay(50);
    digitalWrite(BUZZER, LOW);
    delay(10);
  }
}

void singASongLow() {
  playQuarterNote(NOTE_G2);
  playEighthNote(NOTE_D2);
  playEighthNote(NOTE_D2);
  playQuarterNote(NOTE_E2);
  playQuarterNote(NOTE_D2);
  playQuarterNote(NOTE_REST);
  playQuarterNote(NOTE_F_SHARP2);
  playQuarterNote(NOTE_G2);
}

void singASongMid() {
  playQuarterNote(NOTE_G4);
  playEighthNote(NOTE_D4);
  playEighthNote(NOTE_D4);
  playQuarterNote(NOTE_E4);
  playQuarterNote(NOTE_D4);
  playQuarterNote(NOTE_REST);
  playQuarterNote(NOTE_F_SHARP4);
  playQuarterNote(NOTE_G4);
}

void singASongHigh() {
  playQuarterNote(NOTE_G6);
  playEighthNote(NOTE_D6);
  playEighthNote(NOTE_D6);
  playQuarterNote(NOTE_E6);
  playQuarterNote(NOTE_D6);
  playQuarterNote(NOTE_REST);
  playQuarterNote(NOTE_F_SHARP6);
  playQuarterNote(NOTE_G6);
}

void playQuarterNote(int frequencyInHertz) {
  playNote(QUARTER_NOTE, frequencyInHertz);
}

void playEighthNote(int frequencyInHertz) {
  playNote(EIGHTH_NOTE, frequencyInHertz);
}

void playNote(int durationInMillis, int frequencyInHertz) {
  log("playing note -- duration: " + String(durationInMillis) + ", frequency: " + String(frequencyInHertz));
  tone(BUZZER, frequencyInHertz, durationInMillis * 0.8);
  delay(durationInMillis);
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
