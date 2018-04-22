#include <neopixel/neopixel.h>
#include "led.h"
#include "wait.h"
#include "pins.h"

Adafruit_NeoPixel ledStrip = Adafruit_NeoPixel(NUMBER_OF_LEDS, LED_PIN, WS2812);

int currentColor[2][3] = {{0,0,0}, {0,0,0}};

void setupLEDs(int ledPin) {

  ledStrip.begin();
  for (int i=0; i<NUMBER_OF_LEDS; i++) {
    turnOffLed(i);
  }
}

void turnOffLed(int ledOffset) {
  int COLOR_OFF[3] = {0, 0, 0};

  displayColor(ledOffset, COLOR_OFF);
}

void fadeToAndHoldColor(int ledOffset, int targetColor[3], int secondsToHold) {
  fadeToColor(ledOffset, targetColor);
  waitSeconds(secondsToHold);
}

void fadeToColor(int ledOffset, int targetColor[3]) {
  while(!isColor(ledOffset, targetColor)) {
    stepColor(ledOffset, targetColor);
  }
}

bool isColor(int ledOffset, int targetColor[3]) {
  return currentColor[ledOffset][0] == targetColor[0] && currentColor[ledOffset][1] == targetColor[1] && currentColor[ledOffset][2] == targetColor[2];
}

void stepColor(int ledOffset, int targetColor[3]) {
  currentColor[ledOffset][0] += compare(currentColor[ledOffset][0], targetColor[0]);
  currentColor[ledOffset][1] += compare(currentColor[ledOffset][1], targetColor[1]);
  currentColor[ledOffset][2] += compare(currentColor[ledOffset][2], targetColor[2]);

  displayColor(ledOffset, currentColor[ledOffset]);

  delay(FADE_SPEED);
}

void displayColor(int ledOffset, int targetColor[3]){
    for(int i=0; i<ledStrip.numPixels(); i++) {
        ledStrip.setPixelColor(ledOffset, targetColor[0], targetColor[1], targetColor[2]);
    }
    ledStrip.show();
}

int compare(int current, int target) { // this returns a one unit step from current to target
  if (current < target) return 1;
  if (current > target) return -1;
  return 0;
}
