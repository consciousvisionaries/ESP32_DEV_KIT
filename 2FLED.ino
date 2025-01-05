#include <FastLED.h>

#define FASTLED_VERSION V1.1

CRGB leds[NUM_FLED_ADDLEDS];

void setupFASTLED_GPIO() {
  
    if (NUM_FLED_OUTPUTS == 1) {
        FastLED.addLeds<WS2812, FLED_PIN1, RGB>(leds, NUM_FLED_ADDLEDS);
        FastLED.clear();
        FastLED.show();
        Serial.println("FAST LED Initialized");
    }
}

void updateFLED_address(int address, String color) {
  if (color == "red") {
    leds[address] = CRGB::Red; 
  } else if (color == "blue") {
    leds[address] = CRGB::Blue; 
  } else if (color == "green") {
    leds[address] = CRGB::Green;
  } else if (color == "orange") {
    leds[address] = CRGB::Orange;
  }
}


void updateFLED_clearAll() {
  fill_solid(leds, NUM_FLED_ADDLEDS, CRGB::Black);
}

void updateFLED_show() {
  FastLED.show();
}

void loopFASTLED() {
  
}
