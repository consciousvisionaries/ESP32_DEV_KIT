#include <FastLED.h>

int ledCount[3];
bool solutionFound = false;

// LED Array
CRGB leds[NUM_LEDS];

// Function to map pulse counts to the number of LEDs
int getLEDCount(int pulseCount) {
    pulseCount = max(0, min(15000, pulseCount)); // Clamp pulseCount between 0 and 15000
    return map(pulseCount, 0, 15000, 0, 10);
}

void updateFASTLED() {

    ledCount[0] = getLEDCount(pulseCount1);
    ledCount[1] = getLEDCount(pulseCount2);
    ledCount[2] = getLEDCount(pulseCount3);

    for (int s=0; s < 3; s++) {
      if (ledCount[s] != solutionWin[s]) {
        solutionFound = false;
        break;
      }
      solutionFound = true;
      Serial.println("Solution Found");
    }
    
    static int lastLedCount1 = 0; // Tracks the last LED count for Dial 1
    static int lastLedCount2 = 0; // Tracks the last LED count for Dial 2
    static int lastLedCount3 = 0; // Tracks the last LED count for Dial 3

    // Only update if any of the LED counts have changed
    if (ledCount[0] != lastLedCount1 || ledCount[1] != lastLedCount2 || ledCount[2] != lastLedCount3) {
        
        // Update last LED counts
        lastLedCount1 = ledCount[0];
        lastLedCount2 = ledCount[1];
        lastLedCount3 = ledCount[2];

        // Clear all LEDs
        fill_solid(leds, NUM_LEDS, CRGB::Black);

        // Light up LEDs for Dial 1
        for (int i = 0; i < ledCount[0]; i++) {
            leds[i] = CRGB::Red;
        }

        // Light up LEDs for Dial 2
        for (int i = 10; i < 10 + ledCount[1]; i++) {
            leds[i] = CRGB::Green;
        }

        // Light up LEDs for Dial 3
        for (int i = 20; i < 20 + ledCount[3]; i++) {
            leds[i] = CRGB::Blue;
        }

        // Show updated LED states
        FastLED.show();

        prepareLEDData(ledCount[0], ledCount[1], ledCount[2]);

        // Debug output
        Serial.println("LED Strip 1: " + String(ledCount[0]));
        Serial.println("LED Strip 2: " + String(ledCount[1]));
        Serial.println("LED Strip 3: " + String(ledCount[2]));
    }
}

void prepareLEDData(int count1, int count2, int count3) {
  
    DynamicJsonDocument doc(1024);  // Create a JSON document with 1KB capacity
    //doc["mac"] = WiFi.macAddress();
    doc["puzzleName"] = PUZZLE_NAME;
    //doc["ipAddress"] = WiFi.localIP().toString();
    //doc["timestamp"] = millis();
    doc["tab"] = NR_TAB;
    doc["group"] = NR_GROUP;
    //doc["type"] = NR_TYPE;
    //doc["version"] = storedVersion;
    //doc["outputs"] = NUM_OUTPUTS;
    //doc["inputs"] = NUM_INPUTS;
    doc["leds"] = NUM_LEDS;
    doc["channels"] = NUM_CHANNELS;

    
    doc["ls1"] = count1;
    doc["ls2"] = count2;
    doc["ls3"] = count3;
    doc["sfound"] = solutionFound;

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
