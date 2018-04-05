#include <ArduinoJson.h>
#include <MQTT.h>

const int BAUD_RATE = 115200;

const String THINGSPEAK_USERNAME = "andyartz";
const String THINGSPEAK_MQTT_API_KEY = "SZTZGZVSNCSQ3L73";

const String EMOTE_EVENT_PARTICLE_WEBHOOK_NAME = "count";
const String EMOTE_EVENT_THINGSPEAK_CHANNEL_NUMBER = "428136";
const String EMOTE_EVENT_THINGSPEAK_CHANNEL_API_READ_KEY = "DD3L474U2F2F1G5V";
const String EMOTE_EVENT_THINGSPEAK_CHANNEL_FIELD = "field1";
const String EMOTE_EVENT_THINGSPEAK_CHANNEL = "channels/"
                                              + EMOTE_EVENT_THINGSPEAK_CHANNEL_NUMBER
                                              + "/subscribe/fields/"
                                              + EMOTE_EVENT_THINGSPEAK_CHANNEL_FIELD
                                              + "/"
                                              + EMOTE_EVENT_THINGSPEAK_CHANNEL_API_READ_KEY;

const int THINGSPEAK_MQTT_PORT = 1883;

const String POOP = "💩";
const String HEART = "";// TODO
const String KISS = "";// TODO
const String CLAP = "";// TODO
const String SAD = "";// TODO
const String SMILE = "";// TODO

const String OUTBOUND = "twilio";
const String INBOUND = "twilio"; // TODO

const int ONE_SECOND_IN_MILLIS = 1000;

StaticJsonBuffer<200> jsonBuffer;
MQTT client("mqtt.thingspeak.com", THINGSPEAK_MQTT_PORT, mqttEventHandler);

void setup() {
  setupSerialConnection();
  subscribeMQTT();
}

void subscribeMQTT() {
    client.connect( "meaninglessID", THINGSPEAK_USERNAME, THINGSPEAK_MQTT_API_KEY, NULL, MQTT::QOS0, 0, NULL, true);

    delay( ONE_SECOND_IN_MILLIS );

    if ( client.isConnected())   {
      String subscribed = String(client.subscribe( EMOTE_EVENT_THINGSPEAK_CHANNEL ));
      emit("subscribed", subscribed);
    }

    delay( ONE_SECOND_IN_MILLIS );
    emit(EMOTE_EVENT_PARTICLE_WEBHOOK_NAME, POOP); // this is for testing
}

void loop() {
   client.loop();
}

void mqttEventHandler( char* topic, byte* payload, unsigned int length ) {
  emit("mqtt", "emote event detected");

    char p[ length + 1 ]; // Leave an extra space to null terminate the string.
    memcpy( p, payload, length );
    p[ length ] = NULL;  // Terminate the string.

    if (strcmp(p, POOP) == 0) {
      handlePoop();
    } else if (strcmp(p, HEART) == 0) {
      handleHeart();
    } else if (strcmp(p, KISS) == 0) {
      handleKiss();
    } else if (strcmp(p, CLAP) == 0) {
      handleClap();
    } else if (strcmp(p, SMILE) == 0) {
      handleSmile();
    } else if (strcmp(p, SAD) == 0) {
      handleSad();
    } else {
      handleUnknown(p);
    }
}

void handlePoop() {
  emit("mqtt", POOP);
}

void handleHeart() {
  emit("mqtt", HEART);
}

void handleKiss() {
  emit("mqtt", KISS);
}

void handleClap() {
  emit("mqtt", CLAP);
}

void handleSmile() {
  emit("mqtt", SMILE);
}

void handleSad() {
  emit("mqtt", SAD);
}

void handleUnknown(String payload) {
  emit("mqtt", payload);
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
