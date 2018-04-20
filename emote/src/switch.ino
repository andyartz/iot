#include "switch.h"

bool switchPressed(int pin) {
  return digitalRead(pin) == 1;
}
