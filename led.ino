/*
 * Project GPS
 * Description:
 * Author:
 * Date:
 */


 // This #include statement was automatically added by the Particle IDE.
// Search for neopixel 0.0.13 (I think or somethign .13)
 #include "neopixel/neopixel.h"

 Adafruit_NeoPixel strip = Adafruit_NeoPixel(4, D3, WS2812);

 int red = 0;
 int green = 0;
 int blue = 0;

 void setup() {
     strip.begin();
     Particle.function("setColor", setColor);
 }

 void loop() {
     displayColor();
 }

 void displayColor(){
     for(int i=0; i<strip.numPixels(); i++) {
         strip.setPixelColor(i, red, green, blue);
     }
     strip.show();
 }

 int setColor(String command){
     int commaIndex1 = command.indexOf(',');
     int commaIndex2 = command.indexOf(',', commaIndex1+1);

     red = command.substring(0, commaIndex1).toInt();
     green = command.substring(commaIndex1+1, commaIndex2).toInt();
     blue = command.substring(commaIndex2+1).toInt();
 }
