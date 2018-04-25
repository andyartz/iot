#include "motor.h"
#include "emit.h"
#include "PowerFunctions.h"
#include "pins.h"

int output = RED;

PowerFunctions pf(INFRARED_PIN, 0);

void startMotor() {
  emit("motor", "on");
  pf.single_pwm(output, PWM_FWD4);
}

void stopMotor() {
  emit("motor", "off");

  pf.single_pwm(output, PWM_BRK);
  delay(30);
  pf.single_pwm(output, PWM_FLT);
}
