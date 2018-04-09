#include <MQTT.h>

const String VERSION = "v0.0.8";

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

void setup() {

  emit("version", VERSION);

  setupSwitches();
  setupSerialConnection();
  doConfig();
  setupMQTT();
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

  setupInputPin(REMOTE_CONFIG_PIN);
  config = digitalRead(REMOTE_CONFIG_PIN);

  if (isFlower()) {
    name == "Flower";
    emit("config", "Device is a flower.");
  } else if (isTardis()) {
    name == "Tardis";
    emit("config", "Device is a Tardis.");
  } else {
    emit("config", "ERROR! Device has no configuration: " + config );
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

    if ( client.isConnected()) {
      String subscribed = subscribeToMQTT();
      emit("subscribed", subscribed);
    } else {
      emit("subscribed", SAD + "ERROR! Thingspeak MQTT connection failed.");
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

////////////////////////////////////////////////////////////////////////////////
void loop() {
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
    handleTHUMBS_UP();
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
  emit("mqtt", "I don't do " + payload);
}

// Flower handlers /////////////////////////////////////////////////////////////

void handlePoop() {
  emit("mqtt", POOP);
}

void handleHeart() {
  emit("mqtt", HEART);
}

void handleTHUMBS_UP() {
  emit("mqtt", THUMBS_UP);
}

// Tardis handlers /////////////////////////////////////////////////////////////

void handleClap() {
  emit("mqtt", CLAP);
}

void handleSmile() {
  emit("mqtt", SMILE);
}

void handleSad() {
  emit("mqtt", SAD);
}

// Utility Code Starts here ///////////////////////////////////////////////

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
