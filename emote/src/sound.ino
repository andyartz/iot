#include "sound.h"
#include "emit.h"
#include "switch.h"

int soundPin;

void setupSound(int soundPinIn) {
  soundPin = soundPinIn;
  pinMode(soundPin, INPUT_PULLDOWN);
}

void playSound(String sound) {
  emit("sound", "Playing sound: " + sound); //TODO hook this to mp3 player
}

bool soundIsPlaying() {
  bool soundIsPlaying = !switchPressed(soundPin); //TODO hook this to mp3 player
  if (!soundIsPlaying) {
    emit("sound", "sound has stopped.");
  }
  return soundIsPlaying;
}
