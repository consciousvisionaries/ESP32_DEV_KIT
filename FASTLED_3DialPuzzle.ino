#include <FastLED.h>

// LED Array
CRGB leds[NUM_LEDS];

// Function to map pulse counts to the number of LEDs
int getLEDCount(int pulseCount) {
    pulseCount = max(0, min(15000, pulseCount)); // Clamp pulseCount between 0 and 15000
    return map(pulseCount, 0, 15000, 0, 10);
}

void updateFASTLED() {

    int ledCount1 = getLEDCount(pulseCount1);
    int ledCount2 = getLEDCount(pulseCount2);
    int ledCount3 = getLEDCount(pulseCount3);
    
    static int lastLedCount1 = 0; // Tracks the last LED count for Dial 1
    static int lastLedCount2 = 0; // Tracks the last LED count for Dial 2
    static int lastLedCount3 = 0; // Tracks the last LED count for Dial 3

    // Only update if any of the LED counts have changed
    if (ledCount1 != lastLedCount1 || ledCount2 != lastLedCount2 || ledCount3 != lastLedCount3) {
        
        // Update last LED counts
        lastLedCount1 = ledCount1;
        lastLedCount2 = ledCount2;
        lastLedCount3 = ledCount3;

        // Clear all LEDs
        fill_solid(leds, NUM_LEDS, CRGB::Black);

        // Light up LEDs for Dial 1
        for (int i = 0; i < ledCount1; i++) {
            leds[i] = CRGB::Red;
        }

        // Light up LEDs for Dial 2
        for (int i = 10; i < 10 + ledCount2; i++) {
            leds[i] = CRGB::Green;
        }

        // Light up LEDs for Dial 3
        for (int i = 20; i < 20 + ledCount3; i++) {
            leds[i] = CRGB::Blue;
        }

        // Show updated LED states
        FastLED.show();

        prepareLEDData(ledCount1, ledCount2, ledCount3);

        // Debug output
        Serial.println("LED Strip 1: " + String(ledCount1));
        Serial.println("LED Strip 2: " + String(ledCount2));
        Serial.println("LED Strip 3: " + String(ledCount3));
    }
}

void prepareLEDData(int count1, int count2, int count3) {
  
    DynamicJsonDocument doc(1024);  // Create a JSON document with 1KB capacity
    doc["mac"] = WiFi.macAddress();
    doc["puzzleName"] = PUZZLE_NAME;
    doc["ipAddress"] = WiFi.localIP().toString();
    doc["timestamp"] = millis();
    doc["tab"] = NR_TAB;
    doc["group"] = NR_GROUP;
    doc["output_type"] = NR_TYPE;
    doc["version"] = storedVersion;
    doc["num_outputs"] = NUM_OUTPUTS;
    doc["num_inputs"] = NUM_INPUTS;
    doc["num_leds"] = NUM_LEDS;
    doc["num_channels"] = NUM_CHANNELS;

    
    doc["led_strip_1"] = count1;
    doc["led_strip_2"] = count2;
    doc["led_strip_3"] = count3;

    // Send the JSON payload via MQTT
    String jsonPayload;

    // Serialize the JSON document into a String
    serializeJson(doc, jsonPayload);

    publishDataMQTTPayload_Doc(jsonPayload);
}

void setupFASTLED() {

    // Initialize FastLED
    FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
    FastLED.clear();
    FastLED.show();
}

void loopFASTLED() {
   
}
