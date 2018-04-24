void setupLEDs(int ledPin);

void turnOffLed(int ledOffset);

void fadeToAndHoldColor(int ledOffset, int targetColor[3], int secondsToHold);

void fadeToColor(int ledOffset, int targetColor[3]);

bool isColor(int ledOffset, int targetColor[3]);

void stepColor(int ledOffset, int targetColor[3]);

void displayColor(int ledOffset, int targetColor[3]);

int compare(int current, int target);

const int NUMBER_OF_LEDS = 2;

const int ONE_HUNDRED_MILLISECONDS = 100;
const int ONE_MILLISECOND = 1;
const int ONE_SECOND = 1;

const int BLINK_TIME = ONE_HUNDRED_MILLISECONDS;
const int FADE_SPEED = 5 * ONE_MILLISECOND;
const int COLOR_HOLD_TIME = ONE_SECOND;

const int MAIN_LED = 0;
const int SECONDARY_LED = 1;
