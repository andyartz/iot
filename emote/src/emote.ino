#include <MQTT.h>
#include "log.h"
#include "emit.h"
#include "motor.h"
#include "sound.h"
#include "switch.h"
#include "led.h"
#include "wait.h"
#include "pins.h"
#include "servo.h"

const String VERSION = "v1.0.1";

const int BAUD_RATE = 115200;

const String THINGSPEAK_USERNAME = "ezequiell";
const String THINGSPEAK_MQTT_API_KEY = "KJD6QV2N7PJ2YM7K";

const String OUTBOUND_EVENTS_WEBHOOK_NAME = "twilio";

const String EMOTE_EVENT_THINGSPEAK_CHANNEL_NUMBER_DOG = "467440";
const String EMOTE_EVENT_THINGSPEAK_CHANNEL_API_READ_KEY_DOG = "80HXPU3QJX8H7CJF";
const String EMOTE_EVENT_THINGSPEAK_CHANNEL_NUMBER_TARDIS = "469924";
const String EMOTE_EVENT_THINGSPEAK_CHANNEL_API_READ_KEY_TARDIS = "EKTNY1SLEPZ4A9CM";

const String EMOTE_EVENT_THINGSPEAK_CHANNEL_FIELD = "field1";

const int THINGSPEAK_MQTT_PORT = 1883;

// Emoji ///////////////////////////////////////////////////////////////////////
const String POOP = ":poop:";
const String HEART = ":heart:";
const String SAD = ":frowning:";
const String THUMBS_UP = ":thumbsup:";
const String CLAP = ":clap:";
const String SMILE = ":smile:";

const int DOG = 0;
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

void setup() {

  emit("version", VERSION);

  setupSwitches();
  setupSerialConnection();
  doConfig();
  setupMQTT();
  setupLEDs(LED_PIN);
  setupSound();
  setupMotor(MOTOR_PIN);
  setupServo();
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


// config //////////////////////////////////////////////////////////////////////

void doConfig() {

  setupInputPin(CONFIG_PIN);
  config = digitalRead(CONFIG_PIN);

  if (isDog()) {
    name = "Dog";
    emit("config", "Device is a Dog. Name is " + name);
  } else if (isTardis()) {
    name = "Tardis";
    emit("config", "Device is a Tardis. Name is " + name);
  } else {
    emit("error", "ERROR! Device has no configuration: " + config );
  }
}

bool isDog() {
  return config == DOG;
}

bool isTardis() {
  return config == TARDIS;
}

// Thingspeak Publish/Subscribe ////////////////////////////////////////////////

void setupMQTT() {
    client.connect( name, THINGSPEAK_USERNAME, THINGSPEAK_MQTT_API_KEY, NULL, MQTT::QOS0, 0, NULL, true);

    waitSeconds(1);

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
  if (isDog()) {
    return EMOTE_EVENT_THINGSPEAK_CHANNEL_NUMBER_DOG;
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
  if (isDog()) {
    return EMOTE_EVENT_THINGSPEAK_CHANNEL_API_READ_KEY_DOG;
  } else if (isTardis()) {
    return EMOTE_EVENT_THINGSPEAK_CHANNEL_API_READ_KEY_TARDIS;
  } else {
    emit("error", "No API Key is available for config: " + config);
    return "";
  }
}

// LED Code ////////////////////////////////////////////////////////////////////

int RED[3] = {255, 0, 0};
int GREEN[3] = {0, 255, 0};
int BLUE[3] = {0, 0, 255};
int YELLOW[3] = {255, 255, 0};
int CYAN[3] = {0, 255, 255};
int MAGENTA[3] = {255, 0, 255};
int WHITE[3] = {255, 255, 255};
int OFF_WHITE[3] = {255, 255, 80};
int OFF[3] = {0, 0, 0};

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
     } else if (isDog()) {
       send(SMILE);
     }
   }

  if (switchPressedJustNow(SWITCH_PIN_1)) {
    if (isTardis()) {
      send(THUMBS_UP);
    } else if (isDog()) {
      send(CLAP);
    }
  }

  if (switchPressedJustNow(SWITCH_PIN_2)) {
    if (isTardis()) {
      send(POOP);
    } else if (isDog()) {
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

void mqttEventHandler( char* topic, byte* payload, unsigned int length ) {
  emit("mqtt", "emote event detected");

    char emoji[ length + 1 ]; // Leave an extra space to null terminate the string.
    memcpy( emoji, payload, length );
    emoji[ length ] = NULL;  // Terminate the string.

    if (isDog()) {
      handleAsDOG(emoji);
    } else if (isTardis()) {
      handleAsTardis(emoji);
    }
}

void handleAsDOG(String emoji) {

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

// DOG handlers /////////////////////////////////////////////////////////////

void handlePoop() {
  emit("mqtt", "I can handle " + POOP);
  oscillate(40,-40,350,10);
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
  // fadeToAndHoldColor(SECONDARY_LED, WHITE, 0);
  // playIntro();
  fadeToAndHoldColor(SECONDARY_LED, RED, 0);
  playStruggle();
  startRumble();
  while(soundIsPlaying()) {
  }
  stopRumble();
  // fadeToAndHoldColor(SECONDARY_LED, WHITE, 0);
  // playOutro();
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

const int TARDIS_FLYING_SOUND_EFFECT = 0;
const int DOCTOR_WHO_THEME = 1;
const int DOCTOR_TALKING_INTRO = 2;
const int DALEK_EXTERMINATE = 2;
const int DOCTOR_TALKING_OUTRO = 2;

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

// Rumble Code /////////////////////////////////////////////////////////////////

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
