#include "motor.h"
#include "emit.h"

int motorPin;

void setupMotor(int motorPinIn) {
  motorPin = motorPinIn;
  pinMode(motorPin, OUTPUT);
  digitalWrite(motorPin, 0);
}

void startMotor() {
  emit("motor", "on");
  digitalWrite(motorPin, 1);
}

void stopMotor() {
  emit("motor", "off");
  digitalWrite(motorPin, 0);
}
