
const String POOP = "💩";
const String HEART = "";// TODO
const String KISS = "";// TODO
const String CLAP = "";// TODO
const String SAD = "";// TODO
const String SMILE = "";// TODO

const String OUTBOUND = "twilio";
const String INBOUND = "twilio"; // TODO

void setup() {
  send(POOP);
  Particle.subscribe("hook-response/" + INBOUND, handleInboundMessage, MY_DEVICES);
}

void loop() {
  log("Sent message.");
}

void handleInboundMessage(const char *event, const char *data) {
  log(data);
}

void send(String message) {
  emit(OUTBOUND, message);
}

void emit(String channel, String message) {
  Particle.publish(channel, message, PRIVATE);
  log("Emitted on channel '" + channel + "': " + message);
}

void log(String message) {
  Serial.println(message);
}
