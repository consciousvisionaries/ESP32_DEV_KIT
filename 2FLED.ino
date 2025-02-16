#include <FastLED.h>

#define FASTLED_VERSION V1.1

CRGB leds[NUM_FLED_ADDLEDS];

void setupFASTLED() {
  
    if (NUM_FLED_OUTPUTS == 1) {
        FastLED.addLeds<WS2812, FLED_PIN1, RGB>(leds, NUM_FLED_ADDLEDS);
        FastLED.clear();
        FastLED.show();
        Serial.println("FAST LED Initialized");
    }
}

void testFLED() {

  for (int x = 0; x < NUM_FLED_ADDLEDS; x++) {
    updateFLED_address(x, "red");    
    delay(50);
            FastLED.show();
  }
  delay(1000);
  updateFLED_clearAll();
  FastLED.show();
}

void updateFASTLED() {
  
   if (String(NR_TYPE) == "3D_ROTARY_PULSE" && NUM_FLED_OUTPUTS == 1) {
        funcRotaryDialPuzzle();
   }
}

void updateFLED_address(int address, String color) {
  if (color == "red") {
    leds[address] = CRGB::Red; // Use CRGB::Red (capitalized R) for consistency
  } else if (color == "blue") {
    leds[address] = CRGB::Blue; // Use CRGB::Blue (capitalized B) for consistency
  } else if (color == "green") {
    leds[address] = CRGB::Green; // Use CRGB::Green (capitalized G) for consistency
  }
}


void updateFLED_clearAll() {
  fill_solid(leds, NUM_FLED_ADDLEDS, CRGB::Black);

}

void updateFLED_show() {
  FastLED.show();
}

void loopFASTLED() {
  
    updateFASTLED();
}
