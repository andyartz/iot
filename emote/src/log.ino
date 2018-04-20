#include <log.h>

void logSameLine(String message) {
  Serial.printf(message);
}

void log(String message) {
  Serial.println(message);
}
