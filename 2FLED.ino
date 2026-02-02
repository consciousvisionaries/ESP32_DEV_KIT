#include <FastLED.h>

#define FASTLED_VERSION V1.1

//#define LED_PIN 12  // Pin for LEDs

// LED Array
CRGB leds[NUM_LEDS];

void updateFASTLED() {

   
       


        // Show updated LED states
        FastLED.show();

       
    
}



void setupFASTLED() {

 //   FastLED.addLeds<WS2812, LED_PIN, RGB>(leds, NUM_LEDS);
    FastLED.clear();
    FastLED.show();


}

void loopFASTLED() {
  
   

    // Additional logic or updates...
}
