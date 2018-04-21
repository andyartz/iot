#include "wait.h"
#include "log.h"

const int ONE_SECOND_IN_MILLIS = 1000;

void waitSeconds(int secondsToWait) {
  logSameLine("Waiting");
  for(int i=0; i<secondsToWait; i++) {
    delay(ONE_SECOND_IN_MILLIS);
    logSameLine(".");
  }
  logSameLine("\n");
}
