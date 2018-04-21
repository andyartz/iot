#include "sound.h"
#include "emit.h"
#include "switch.h"
#include "pins.h"

void setupSound() {
  pinMode(SOUND_PIN_0, OUTPUT);
  pinMode(SOUND_PIN_1, OUTPUT);
  pinMode(SOUND_PIN_2, OUTPUT);
  pinMode(SOUND_ACTIVE_PIN, INPUT_PULLDOWN);
}

void playSound(int sound) {
  emit("sound", "Playing sound: " + String(sound));

  int currentPin;
  switch(sound) {
    case 0: currentPin = SOUND_PIN_0; break;
    case 1: currentPin = SOUND_PIN_1; break;
    case 2: currentPin = SOUND_PIN_2; break;
    default: emit("error", "there is no sound " + String(sound)); return;
  }

  emit("sound", "Playing sound on pin: " + String(currentPin));

  digitalWrite(currentPin, 1);
  while(!soundIsPlaying) {
  }
  digitalWrite(currentPin, 0);
}

bool soundIsPlaying() {
  bool soundIsPlaying = !digitalRead(SOUND_ACTIVE_PIN);
  if (!soundIsPlaying) {
    emit("sound", "sound has stopped.");
  }
  return soundIsPlaying;
}
