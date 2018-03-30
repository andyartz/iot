const String POOP = "💩";
const String OUTBOUND = "twilio";

void setup() {
  emitPoop();
}

void emitPoop() {
  emit(OUTBOUND, POOP);
}

void loop() {
  log("Sent message.");
}

void emit(String channel, String message) {
  Particle.publish(channel, message, PRIVATE);
  log("Emitted on channel '" + channel + "': " + message);
}

void log(String message) {
  Serial.println(message);
}
