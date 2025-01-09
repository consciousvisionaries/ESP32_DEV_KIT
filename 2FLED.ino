#include <FastLED.h>

#define FASTLED_VERSION V1.1

CRGB leds[NUM_FLED_ADDLEDS + NUM_FLED_ADDLEDS2];

// Function to map pulse counts to the number of LEDs
int getFLEDCount(int maxcount, int analoginput) {
    // Ensure pulseCount is within the valid range before using it
    int TOT_RANGE = (PULSE_MAX_RANGE * (NUM_FLED_ADDLEDS / NUM_FLED_CHANNELS));
    int clampedPulseCount = constrain(pulseCount[analoginput], 0, TOT_RANGE);

    pulseCount[analoginput] = clampedPulseCount;

    // Return the mapped value to the number of LEDs based on the constrained pulse count
    return map(clampedPulseCount, 0, TOT_RANGE, 0, maxcount);
}

void setupFASTLED_GPIO() {
  
    if (NUM_FLED_OUTPUTS >= 1) {
        FastLED.addLeds<WS2812, FLED_PIN1, RGB>(leds, NUM_FLED_ADDLEDS);
        FastLED.clear();
        FastLED.show();
        Serial.println("<end> FAST LED Initialized!");
    }
    if (NUM_FLED_OUTPUTS >=2) {
        FastLED.addLeds<WS2812, DOUT_PIN4, RGB>(leds, NUM_FLED_ADDLEDS2);
        FastLED.clear();
        FastLED.show();
        Serial.println("<end> FAST LED 2 Initialized!");
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
