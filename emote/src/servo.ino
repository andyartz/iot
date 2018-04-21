#include "pins.h"
#include "servo.h"
#include "emit.h"

Servo myservo;
int pos = 0;
int position = 0;
int position1 = 40;
int position2 = -40;
int servodelay = 700;

void setupServo() {
  myservo.attach(SERVO_PIN);
  myservo.write(0);
}

void rotateServo(int position) {
  myservo.write(position);
  }

void oscillate(int position1, int position2, int servodelay, int times) {
  for (int i=0; i <= times; i++){
    emit("SERVO ALERT", "EMITTING SERVO - IN THE LOOP" + String(i));
    rotateServo(position1);
    delay(servodelay);
    rotateServo(position2);
    delay(servodelay);
  }
}
