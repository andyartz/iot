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

const String VERSION = "v2.0.7";

const int BAUD_RATE = 115200;

const String THINGSPEAK_USERNAME = "ezequiell";
const String THINGSPEAK_MQTT_API_KEY = "KJD6QV2N7PJ2YM7K";

const String OUTBOUND_EVENTS_WEBHOOK_NAME = "twilio";

const String EMOTE_EVENT_THINGSPEAK_CHANNEL_NUMBER_DOG = "467440";
const String EMOTE_EVENT_THINGSPEAK_CHANNEL_API_READ_KEY_DOG = "80HXPU3QJX8H7CJF";
const String EMOTE_EVENT_THINGSPEAK_CHANNEL_NUMBER_TARDIS = "469924";
const String EMOTE_EVENT_THINGSPEAK_CHANNEL_API_READ_KEY_TARDIS = "EKTNY1SLEPZ4A9CM";
const String EMOTE_EVENT_THINGSPEAK_CHANNEL_NUMBER_TARDIS_BASE = "469924"; //TODO new channel?
const String EMOTE_EVENT_THINGSPEAK_CHANNEL_API_READ_KEY_TARDIS_BASE = "EKTNY1SLEPZ4A9CM"; // new key?

const String EMOTE_EVENT_THINGSPEAK_CHANNEL_FIELD = "field1";

const int THINGSPEAK_MQTT_PORT = 1883;

// Emoji ///////////////////////////////////////////////////////////////////////
const String POOP = ":poop:";
const String HEART = ":heart:";
const String SAD = ":frowning:";
const String THUMBS_UP = ":thumbsup:";
const String CLAP = ":clap:";
const String SMILE = ":smile:";

const int TARDIS_BASE = 2;
const int DOG = 1;
const int TARDIS = 0;

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
  setupServo();
  // testLeds(); //TODO remove this after development
  setupRumble();
  testRumble();
}

void setupSwitches() {
  setupSwitch(SWITCH_PIN_0);
  setupSwitch(SWITCH_PIN_1);
  setupSwitch(SWITCH_PIN_2);
}

void setupSwitch(int switchPin) {
  pinMode(switchPin, INPUT_PULLDOWN);
}

void setupRumble() {
    pinMode(RUMBLE_PIN, OUTPUT);
}

void testRumble() {
  startRumble();
  waitSeconds(1);
  stopRumble();
}
// config //////////////////////////////////////////////////////////////////////

void doConfig() {

  setupInputPin(CONFIG_PIN_0);
  setupInputPin(CONFIG_PIN_1);

  config = digitalRead(CONFIG_PIN_0) + (2*digitalRead(CONFIG_PIN_1));

  if (isDog()) {
    name = "Dog";
    emit("config", "Device is a Dog. Name is " + name);
  } else if (isTardis()) {
    name = "Tardis";
    emit("config", "Device is a Tardis. Name is " + name);
  } else if (isTardisBase()) {
    name = "Tardis Base";
    emit("config", "Device is a Tardis Base. Name is " + name);
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

bool isTardisBase() {
  return config == TARDIS_BASE;
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
  startRumble();
  waitSeconds(1);
  stopRumble();
}

String getThingspeakReadChannel() {
  return "channels/" + getChannelNumber() + "/subscribe/fields/" + getChannel() + "/" + getApiKey();
}

String getChannelNumber() {
  if (isDog()) {
    return EMOTE_EVENT_THINGSPEAK_CHANNEL_NUMBER_DOG;
  } else if (isTardis()) {
    return EMOTE_EVENT_THINGSPEAK_CHANNEL_NUMBER_TARDIS;
  } else if (isTardisBase()) {
    return EMOTE_EVENT_THINGSPEAK_CHANNEL_NUMBER_TARDIS_BASE;
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
  } else if (isTardisBase()) {
    return EMOTE_EVENT_THINGSPEAK_CHANNEL_API_READ_KEY_TARDIS_BASE;
  } else {
    emit("error", "No API Key is available for config: " + config);
    return "";
  }
}

// LED Code ////////////////////////////////////////////////////////////////////

int COLOR_RED[3] = {255, 0, 0};
int COLOR_GREEN[3] = {0, 255, 0};
int COLOR_BLUE[3] = {0, 0, 255};
int COLOR_YELLOW[3] = {255, 255, 0};
int COLOR_CYAN[3] = {0, 255, 255};
int COLOR_MAGENTA[3] = {255, 0, 255};
int COLOR_WHITE[3] = {255, 255, 255};
int COLOR_OFF_WHITE[3] = {255, 255, 80};
int COLOR_OFF[3] = {0, 0, 0};

void testLeds() {
  emit("testing leds", "testing...");
  for (int led=0; led<NUMBER_OF_LEDS; led++) {
    emit("testing led", String(led));
    testLed(led);
  }
}

void testLed(int led) {
  fadeToAndHoldColor(led, COLOR_WHITE, ONE_SECOND);
  fadeToAndHoldColor(led, COLOR_RED, ONE_SECOND);
  fadeToAndHoldColor(led, COLOR_GREEN, ONE_SECOND);
  fadeToAndHoldColor(led, COLOR_BLUE, ONE_SECOND);
  fadeToAndHoldColor(led, COLOR_CYAN, ONE_SECOND);
  fadeToAndHoldColor(led, COLOR_MAGENTA, ONE_SECOND);
  fadeToAndHoldColor(led, COLOR_YELLOW, ONE_SECOND);
  fadeToAndHoldColor(led, COLOR_OFF, ONE_SECOND);
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
    } else if (isTardisBase()) {
      handleAsTardisBase(emoji);
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
    emit("mqtt", "I can handle " + CLAP);
    doTardisFlying();
  } else if (emoji == SMILE) {
    emit("mqtt", "I can handle " + SMILE);
    doTardisLanding();
  } else if (emoji == SAD) {
    emit("mqtt",  "I can handle " + SAD);
    doTardisStruggle();
  } else {
    handleUnknown(emoji);
  }
}

void handleAsTardisBase(String emoji) {

  if (emoji == CLAP) {
    emit("mqtt", "I can handle " + CLAP);
    doTardisBaseFlying();
  } else if (emoji == SMILE) {
    emit("mqtt", "I can handle " + SMILE);
    doTardisBaseLanding();
  } else if (emoji == SAD) {
    emit("mqtt",  "I can handle " + SAD);
    doTardisBaseStruggle();
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

void doDogPoop() {

}

void doDogHeart() {

}

void doDogThumbsUp() {

}

// Tardis handlers /////////////////////////////////////////////////////////////

int TARDIS_LIGHT = MAIN_LED;
const int TARDIS_BREAKING_SOUND_EFFECT = 0;
const int DOCTOR_WHO_THEME = 1;
const int TARDIS_FLYING_SOUND_EFFECT = 2;

// frowning

void doTardisStruggle() {
  fadeToAndHoldColor(TARDIS_LIGHT, COLOR_RED, 10);
  fadeToAndHoldColor(TARDIS_LIGHT, COLOR_OFF, 0);
}

void doTardisBaseStruggle() {
  playSound(TARDIS_BREAKING_SOUND_EFFECT);
  for(int i=0; i<4; i++) {
    startMotor();
    waitSeconds(1);
    stopMotor();
    waitSeconds(0);
  }
  while(soundIsPlaying()) {
  }
}

// clap

void doTardisFlying() {
  waitSeconds(16);
}

void doTardisBaseFlying() {
  playSound(DOCTOR_WHO_THEME);
  startMotor();
  while(soundIsPlaying()) {
  }
  stopMotor();
}

// smile

void doTardisLanding() {
  for (int i=0; i<7; i++) {
    fadeToAndHoldColor(TARDIS_LIGHT, COLOR_WHITE, 0);
    fadeToAndHoldColor(TARDIS_LIGHT, COLOR_OFF, 0);
  }
}

void doTardisBaseLanding() {
  playSound(TARDIS_FLYING_SOUND_EFFECT);
  while(soundIsPlaying()) {
  }
}

// Rumble Code /////////////////////////////////////////////////////////////////

void startRumble() {
  emit("rumble", "on");
  digitalWrite(RUMBLE_PIN, 1);
}

void stopRumble() {
  emit("rumble", "off");
  digitalWrite(RUMBLE_PIN, 0);
}

// Utility Code ////////////////////////////////////////////////////////////////

void setupInputPin(int pin) {
  pinMode(pin, INPUT_PULLDOWN);
}

void setupSerialConnection() {
  Serial.begin(BAUD_RATE);
}
