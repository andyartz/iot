#include <MQTT.h>
#include <neopixel/neopixel.h>

const String VERSION = "v0.2.1";

const int BAUD_RATE = 115200;

const String THINGSPEAK_USERNAME = "ezequiell";
const String THINGSPEAK_MQTT_API_KEY = "KJD6QV2N7PJ2YM7K";

const String OUTBOUND_EVENTS_WEBHOOK_NAME = "twilio";

const String EMOTE_EVENT_THINGSPEAK_CHANNEL_NUMBER_FLOWER = "467440";
const String EMOTE_EVENT_THINGSPEAK_CHANNEL_API_READ_KEY_FLOWER = "80HXPU3QJX8H7CJF";
const String EMOTE_EVENT_THINGSPEAK_CHANNEL_NUMBER_TARDIS = "469924";
const String EMOTE_EVENT_THINGSPEAK_CHANNEL_API_READ_KEY_TARDIS = "EKTNY1SLEPZ4A9CM";

const String EMOTE_EVENT_THINGSPEAK_CHANNEL_FIELD = "field1";

const int THINGSPEAK_MQTT_PORT = 1883;

// Pins ////////////////////////////////////////////////////////////////////////
const int REMOTE_CONFIG_PIN = D0;
const int SOUND_PIN = D1;
const int MOTOR_PIN = D2;
const int LED_PIN = D3;
const int SWITCH_PIN_0 = A0;
const int SWITCH_PIN_1 = A1;
const int SWITCH_PIN_2 = A2;

// Emoji ///////////////////////////////////////////////////////////////////////
const String POOP = ":poop:";
const String HEART = ":heart:";
const String SAD = ":frowning:";
const String THUMBS_UP = ":thumbsup:";
const String CLAP = ":clap:";
const String SMILE = ":smile:";

const int ONE_SECOND_IN_MILLIS = 1000;
const int ONE_HUNDRED_MILLISECONDS = 100;
const int ONE_MILLISECOND = 1;
const int ONE_SECOND = 1;

const int NUMBER_OF_LEDS = 1;

const int FLOWER = 0;
const int TARDIS = 1;

int config = 0;
String name = "none";

// switch fields ///////////////////////////////////////////////////////////////
bool lastState0 = FALSE;
bool currentState0 = FALSE;
bool lastState1 = FALSE;
bool currentState1 = FALSE;
bool lastState2 = FALSE;
bool currentState2 = FALSE;

MQTT client("mqtt.thingspeak.com", THINGSPEAK_MQTT_PORT, mqttEventHandler);
Adafruit_NeoPixel ledStrip = Adafruit_NeoPixel(NUMBER_OF_LEDS, LED_PIN, WS2812);

void setup() {

  emit("version", VERSION);

  setupSwitches();
  setupSerialConnection();
  doConfig();
  setupMQTT();
  setupLEDs();
  setupSound();
  setupMotor();

  // testLeds(); //TODO remove this after development
}

void setupSwitches() {
  setupSwitch(SWITCH_PIN_0);
  setupSwitch(SWITCH_PIN_1);
  setupSwitch(SWITCH_PIN_2);
}

void setupSwitch(int switchPin) {
  pinMode(switchPin, INPUT_PULLDOWN);
}

void setupLEDs() {
  ledStrip.begin();
  for (int i=0; i<NUMBER_OF_LEDS; i++) {
    turnOffLed(i);
  }
}

void setupSound() {
  pinMode(SOUND_PIN, INPUT_PULLDOWN);
}

void setupMotor() {
  pinMode(MOTOR_PIN, OUTPUT);
  digitalWrite(MOTOR_PIN, 0);
}

// config //////////////////////////////////////////////////////////////////////

void doConfig() {

  setupInputPin(REMOTE_CONFIG_PIN);
  config = digitalRead(REMOTE_CONFIG_PIN);

  if (isFlower()) {
    name = "Flower";
    emit("config", "Device is a flower. Name is " + name);
  } else if (isTardis()) {
    name = "Tardis";
    emit("config", "Device is a Tardis. Name is " + name);
  } else {
    emit("error", "ERROR! Device has no configuration: " + config );
  }
}

bool isFlower() {
  return config == FLOWER;
}

bool isTardis() {
  return config == TARDIS;
}

// Thingspeak Publish/Subscribe ////////////////////////////////////////////////

void setupMQTT() {
    client.connect( name, THINGSPEAK_USERNAME, THINGSPEAK_MQTT_API_KEY, NULL, MQTT::QOS0, 0, NULL, true);

    delay( ONE_SECOND_IN_MILLIS );

    if ( client.isConnected() ) {
      String subscribed = subscribeToMQTT();
      emit("subscribed", subscribed);
    } else {
      emit("error", SAD + "ERROR! Thingspeak MQTT connection failed.");
    }
}

void checkConnection() {
   if (!client.isConnected()) {
     emit("error", "MQTT is no longer conneccted. Reconnecting...");
     setupMQTT();
   }
}

String subscribeToMQTT() {
  return String(client.subscribe(getThingspeakReadChannel()));
}

void send(String message) {
  emit(OUTBOUND_EVENTS_WEBHOOK_NAME, message);
}

String getThingspeakReadChannel() {
  return "channels/" + getChannelNumber() + "/subscribe/fields/" + getChannel() + "/" + getApiKey();
}

String getChannelNumber() {
  if (isFlower()) {
    return EMOTE_EVENT_THINGSPEAK_CHANNEL_NUMBER_FLOWER;
  } else if (isTardis()) {
    return EMOTE_EVENT_THINGSPEAK_CHANNEL_NUMBER_TARDIS;
  } else {
    emit("error", "No Channel Number is available for config: " + config);
    return "";
  }
}

String getChannel() {
  return EMOTE_EVENT_THINGSPEAK_CHANNEL_FIELD;
}

String getApiKey() {
  if (isFlower()) {
    return EMOTE_EVENT_THINGSPEAK_CHANNEL_API_READ_KEY_FLOWER;
  } else if (isTardis()) {
    return EMOTE_EVENT_THINGSPEAK_CHANNEL_API_READ_KEY_TARDIS;
  } else {
    emit("error", "No API Key is available for config: " + config);
    return "";
  }
}

// LED Code ////////////////////////////////////////////////////////////////////

const int BLINK_TIME = ONE_HUNDRED_MILLISECONDS;
const int FADE_SPEED = 5 * ONE_MILLISECOND;
const int COLOR_HOLD_TIME = ONE_SECOND;

const int MAIN_LED = 0;
const int SECONDARY_LED = 1;

int RED[3] = {255, 0, 0};
int GREEN[3] = {0, 255, 0};
int BLUE[3] = {0, 0, 255};
int YELLOW[3] = {255, 255, 0};
int CYAN[3] = {0, 255, 255};
int MAGENTA[3] = {255, 0, 255};
int WHITE[3] = {255, 255, 255};
int OFF_WHITE[3] = {255, 255, 80};
int OFF[3] = {0, 0, 0};

int currentColor[2][3] = {{0,0,0}, {0,0,0}};

void testLeds() {
  emit("testing leds", "testing...");
  for (int led=0; led<NUMBER_OF_LEDS; led++) {
    emit("testing led", String(led));
    testLed(led);
  }
}

void testLed(int led) {
  fadeToAndHoldColor(led, WHITE, ONE_SECOND);
  fadeToAndHoldColor(led, RED, ONE_SECOND);
  fadeToAndHoldColor(led, GREEN, ONE_SECOND);
  fadeToAndHoldColor(led, BLUE, ONE_SECOND);
  fadeToAndHoldColor(led, CYAN, ONE_SECOND);
  fadeToAndHoldColor(led, MAGENTA, ONE_SECOND);
  fadeToAndHoldColor(led, YELLOW, ONE_SECOND);
  fadeToAndHoldColor(led, OFF, ONE_SECOND);
}

void turnOffLed(int ledOffset) {
  displayColor(ledOffset, OFF);
}

void fadeToAndHoldColor(int ledOffset, int targetColor[3], int secondsToHold) {
  fadeToColor(ledOffset, targetColor);
  waitSeconds(secondsToHold);
}

void fadeToColor(int ledOffset, int targetColor[3]) {
  while(!isColor(ledOffset, targetColor)) {
    stepColor(ledOffset, targetColor);
  }
}

bool isColor(int ledOffset, int targetColor[3]) {
  return currentColor[ledOffset][0] == targetColor[0] && currentColor[ledOffset][1] == targetColor[1] && currentColor[ledOffset][2] == targetColor[2];
}

void stepColor(int ledOffset, int targetColor[3]) {
  currentColor[ledOffset][0] += compare(currentColor[ledOffset][0], targetColor[0]);
  currentColor[ledOffset][1] += compare(currentColor[ledOffset][1], targetColor[1]);
  currentColor[ledOffset][2] += compare(currentColor[ledOffset][2], targetColor[2]);

  displayColor(0, currentColor[ledOffset]);

  delay(FADE_SPEED);
}

void displayColor(int ledOffset, int targetColor[3]){
    for(int i=0; i<ledStrip.numPixels(); i++) {
        ledStrip.setPixelColor(ledOffset, targetColor[0], targetColor[1], targetColor[2]);
    }
    ledStrip.show();
}

int compare(int current, int target) { // this returns a one unit step from current to target
  if (current < target) return 1;
  if (current > target) return -1;
  return 0;
}

////////////////////////////////////////////////////////////////////////////////
void loop() {

   checkConnection();

   client.loop();

   lastState0 = currentState0;
   lastState1 = currentState1;
   lastState2 = currentState2;
   currentState0 = switchPressed(SWITCH_PIN_0);
   currentState1 = switchPressed(SWITCH_PIN_1);
   currentState2 = switchPressed(SWITCH_PIN_2);

   if (switchPressedJustNow(SWITCH_PIN_0)) {
     if (isTardis()) {
       send(HEART);
     } else if (isFlower()) {
       send(SMILE);
     }
   }

  if (switchPressedJustNow(SWITCH_PIN_1)) {
    if (isTardis()) {
      send(THUMBS_UP);
    } else if (isFlower()) {
      send(CLAP);
    }
  }

  if (switchPressedJustNow(SWITCH_PIN_2)) {
    if (isTardis()) {
      send(POOP);
    } else if (isFlower()) {
      send(SAD);
    }
  }
  waitSeconds(1);
}

bool switchPressedJustNow(int pin) {
  if (pin == SWITCH_PIN_0) {
    return !lastState0 && currentState0;
  } else if (pin == SWITCH_PIN_1) {
    return !lastState1 && currentState1;
  } else if (pin == SWITCH_PIN_2) {
    return !lastState2 && currentState2;
  } else {
    return FALSE;
  }
}

bool switchPressed(int pin) {
  return digitalRead(pin) == 1;
}

void mqttEventHandler( char* topic, byte* payload, unsigned int length ) {
  emit("mqtt", "emote event detected");

    char emoji[ length + 1 ]; // Leave an extra space to null terminate the string.
    memcpy( emoji, payload, length );
    emoji[ length ] = NULL;  // Terminate the string.

    if (isFlower()) {
      handleAsFlower(emoji);
    } else if (isTardis()) {
      handleAsTardis(emoji);
    }
}

void handleAsFlower(String emoji) {

  if (emoji == POOP) {
    handlePoop();
  } else if (emoji == HEART) {
    handleHeart();
  } else if (emoji == THUMBS_UP) {
    handleThumbsUp();
  } else {
    handleUnknown(emoji);
  }
}

void handleAsTardis(String emoji) {

  if (emoji == CLAP) {
    handleClap();
  } else if (emoji == SMILE) {
    handleSmile();
  } else if (emoji == SAD) {
    handleSad();
  } else {
    handleUnknown(emoji);
  }
}

void handleUnknown(String payload) {
  emit("mqtt", name + " doesn't do " + payload);
}

// Flower handlers /////////////////////////////////////////////////////////////

void handlePoop() {
  emit("mqtt", "I can handle " + POOP);
}

void handleHeart() {
  emit("mqtt", "I can handle " + HEART);
}

void handleThumbsUp() {
  emit("mqtt", "I can handle " + THUMBS_UP);
}

// Tardis handlers /////////////////////////////////////////////////////////////

void handleClap() {
  emit("mqtt", "I can handle " + CLAP);
  doTardisFlying();
}

void handleSmile() {
  emit("mqtt", "I can handle " + SMILE);
  doTardisLanding();
}

void handleSad() {
  emit("mqtt",  "I can handle " + SAD);

  doTardisStruggle();
}

void doTardisStruggle() {
  fadeToAndHoldColor(SECONDARY_LED, WHITE, 0);
  playIntro();
  fadeToAndHoldColor(SECONDARY_LED, RED, 0);
  playStruggle();
  startRumble();
  while(soundIsPlaying()) {
  }
  stopRumble();
  fadeToAndHoldColor(SECONDARY_LED, WHITE, 0);
  playOutro();
  fadeToAndHoldColor(SECONDARY_LED, OFF, 0);
}

void doTardisFlying() {
  startThemeSong();

  startMotor();
  while(soundIsPlaying()) {
  }
  stopMotor();
}

void doTardisLanding() {
  startTardisSound();

  while (soundIsPlaying()) {
    fadeToAndHoldColor(MAIN_LED, WHITE, 0);
    fadeToAndHoldColor(MAIN_LED, OFF, 0);
  }
}

const String DOCTOR_TALKING_INTRO = "Doctor Talking Intro";
const String DALEK_EXTERMINATE = "Dalek Exterminate";
const String DOCTOR_TALKING_OUTRO = "Doctor Who Outro";
const String DOCTOR_WHO_THEME = "Doctor Who Theme Song";
const String TARDIS_FLYING_SOUND_EFFECT = "Tardis Flying";

void startThemeSong() {
  playSound(DOCTOR_WHO_THEME);
}

void startTardisSound() {
  playSound(TARDIS_FLYING_SOUND_EFFECT);
}

void playIntro() {
  playSound(DOCTOR_TALKING_INTRO);
}

void playStruggle() {
  playSound(DALEK_EXTERMINATE);
}

void playOutro() {
  playSound(DOCTOR_TALKING_OUTRO);
}

// Sound Code //////////////////////////////////////////////////////////////////

void playSound(String sound) {
  emit("sound", "Playing sound: " + sound); //TODO hook this to mp3 player
}

bool soundIsPlaying() {
  bool soundIsPlaying = !switchPressed(SOUND_PIN); //TODO hook this to mp3 player
  if (!soundIsPlaying) {
    emit("sound", "sound has stopped.");
  }
  return soundIsPlaying;
}

// Motor Code //////////////////////////////////////////////////////////////////

void startMotor() {
  emit("motor", "on");
  digitalWrite(MOTOR_PIN, 1);
}

void stopMotor() {
  emit("motor", "off");
  digitalWrite(MOTOR_PIN, 0);
}

void startRumble() {
    emit("rumble", "on");
    //TODO turn on the rumbler
}

void stopRumble() {
    emit("rumble", "off");
    //TODO turn off the rumbler
}

// Utility Code ////////////////////////////////////////////////////////////////

void setupInputPin(int pin) {
  pinMode(pin, INPUT_PULLDOWN);
}

void setupSerialConnection() {
  Serial.begin(BAUD_RATE);
}

void emit(String channel, String message) {
  Particle.publish(channel, message, PRIVATE);
  log("Emitted on channel '" + channel + "': " + message);
}

void waitSeconds(int secondsToWait) {
  logSameLine("Waiting");
  for(int i=0; i<secondsToWait; i++) {
    delay(ONE_SECOND_IN_MILLIS);
    logSameLine(".");
  }
  logSameLine("\n");
}

void logSameLine(String message) {
  Serial.printf(message);
}

void log(String message) {
  Serial.println(message);
}
