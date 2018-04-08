#include <ArduinoJson.h>
#include <MQTT.h>

const int BAUD_RATE = 115200;

const String THINGSPEAK_USERNAME = "ezequiell";
const String THINGSPEAK_MQTT_API_KEY = "KJD6QV2N7PJ2YM7K";

const String EMOTE_EVENT_PARTICLE_WEBHOOK_NAME = "emote";
const String EMOTE_EVENT_THINGSPEAK_CHANNEL_NUMBER = "467440";
const String EMOTE_EVENT_THINGSPEAK_CHANNEL_API_READ_KEY = "80HXPU3QJX8H7CJF";
const String EMOTE_EVENT_THINGSPEAK_CHANNEL_FIELD = "field1";
const String EMOTE_EVENT_THINGSPEAK_CHANNEL = "channels/"
                                              + EMOTE_EVENT_THINGSPEAK_CHANNEL_NUMBER
                                              + "/subscribe/fields/"
                                              + EMOTE_EVENT_THINGSPEAK_CHANNEL_FIELD
                                              + "/"
                                              + EMOTE_EVENT_THINGSPEAK_CHANNEL_API_READ_KEY;

const int THINGSPEAK_MQTT_PORT = 1883;
const int REMOTE_CONFIG_PIN = D0;

const String POOP = "💩";
const String HEART = "❤️";
const String SAD = "☹️";
const String KISS = "😘";
const String CLAP = "👏";
const String SMILE = "😀";

const String OUTBOUND = "twilio";
const String INBOUND = "twilio"; // TODO

const int ONE_SECOND_IN_MILLIS = 1000;

const int FLOWER = 0;
const int TARDIS = 1;

int config = 0;
String name = "none";

StaticJsonBuffer<200> jsonBuffer;
MQTT client("mqtt.thingspeak.com", THINGSPEAK_MQTT_PORT, mqttEventHandler);

void setup() {

  emit("version", "v0.0.2");

  setupSerialConnection();
  doConfig();
  subscribeMQTT();
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

////////////////////////////////////////////////////////////////////////////////

void subscribeMQTT() {
    client.connect( name, THINGSPEAK_USERNAME, THINGSPEAK_MQTT_API_KEY, NULL, MQTT::QOS0, 0, NULL, true);

    delay( ONE_SECOND_IN_MILLIS );

    if ( client.isConnected()) {
      String subscribed = String(client.subscribe( EMOTE_EVENT_THINGSPEAK_CHANNEL ));
      emit("subscribed", subscribed);
    } else {
      emit("subscribed", SAD + "ERROR! Thingspeak MQTT connection failed.");
    }
}

void loop() {
   client.loop();
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
  } else if (emoji == KISS) {
    handleKiss();
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
  emit("mqtt", payload);
}

// Flower handlers /////////////////////////////////////////////////////////////

void handlePoop() {
  emit("mqtt", POOP);
}

void handleHeart() {
  emit("mqtt", HEART);
}

void handleKiss() {
  emit("mqtt", KISS);
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


// void handleInboundMessage(const char *event, const char *data) {
//   log("Incoming message detected!!!");
//   log(data);
//   JsonObject& root = jsonBuffer.parseObject((char*)data);
//   if (!root.success()) {
//     log("Couldn't parse incoming message :(");
//     return;
//   }
//   const char* jsonData = root["data"];
//   log(jsonData);
// }

void send(String message) {
  emit(OUTBOUND, message);
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
