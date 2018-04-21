#include "pins.h"
#include "servo.h"

Servo myservo;
int pos = 0;
int position = 0;
int position1 = 40;
int position2 = -40;

void setupServo() {
  myservo.attach(SERVO_PIN);
  myservo.write(0);
}

void rotateServo(int position1, int position2) {
  myservo.write(position1);
  delay(350);
  myservo.write(position2);
  delay(350);
  myservo.write(position1);
  delay(350);
  myservo.write(position2);
  delay(350);
  myservo.write(position1);
  delay(350);
  myservo.write(position2);
  delay(350);
  myservo.write(position1);
  delay(350);
  myservo.write(position2);
  }
