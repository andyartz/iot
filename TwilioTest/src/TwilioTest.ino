/*
Basic example of sending a SMS with Twilio and a Particle Photon.

Author: Paul Kamp, Twilio Developer Education
License: MIT
*/

String body = "💩";

void setup() {
  Particle.publish("twilio_test", body, PRIVATE);
  Serial.begin(115200);
}

void loop() {
    Serial.println("That's all!  You can restart or edit the code now.");
    delay(60000);
}
