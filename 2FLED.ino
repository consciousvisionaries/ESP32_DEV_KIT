#include <FastLED.h>

#define FASTLED_VERSION V1.1

// LED Array
CRGB leds[NUM_FLED_ADDLEDS];


void setupFASTLED() {

    if (NUM_FLED_OUTPUTS == 1) {
    FastLED.addLeds<WS2812, LED_PIN1, RGB>(leds, NUM_FLED_ADDLEDS);
   
    FastLED.clear();
    FastLED.show();
    Serial.println("FAST LED Initialized");

    }
}

void updateFASTLED() {
  
  if (String(NR_TYPE) == "3D Rotary Pulse" && NUM_FLED_OUTPUTS == 1) {
    Serial.println(NR_TYPE);
    displayFLED_3DRotaryPulse();
  }
}

void loopFASTLED() {

    if (solutionFound) {
        if (!solutionStable) {
            solutionCheckStart = millis(); // Record the current time
            solutionStable = true; // Mark as stable
        }

        // Check if 2 seconds have passed without any change
        if (millis() - solutionCheckStart >= 2000) {
            executeGPIOBatch1(); // Execute the batch operation
            solutionStable = false; // Reset the state
        }
    } else {
        solutionStable = false; // Reset if solutionFound becomes false
    }

    // Additional logic or updates...
}

void displayFLED_3DRotaryPulse() {

    for (int i = 0; i < NUM_FLED_CHANNELS; i++) {
        ledCount[i] = getCount(NUM_FLED_ADDLEDS / NUM_FLED_CHANNELS, i); // Mocked pulseCount[i] as 'i'
    }

    solutionFound = true; // Reset solutionFound
    for (int s = 0; s < NUM_FLED_CHANNELS; s++) {
        if (ledCount[s] != solutionWin[s]) {
            solutionFound = false;
            break;
        }
    }

    if (solutionFound) {
        Serial.println("Solution Found");
    }

    for (int i = 0; i < NUM_FLED_CHANNELS; i++) {
        if (ledCount[i] != lastLedCount[i]) {
            lastLedCount[i] = ledCount[i];
            fill_solid(leds, NUM_FLED_ADDLEDS, CRGB::Black);
            for (int l = 0; l < ledCount[i]; l++) {
                leds[(l + i)] = CHSV(0, 255, 255);
            }
            Serial.println("LED Strip " + String(i) + ": " + String(ledCount[i]));
        }
    }

    // Show updated LED states
    FastLED.show();
    prepareLEDMQTTData(ledCount[0], ledCount[1], ledCount[2]);
}


void prepareLEDMQTTData(int count1, int count2, int count3) {
  
    DynamicJsonDocument doc(MQTT_MAX_PACKET_SIZE);  // Create a JSON document with 1KB capacity
    doc["mac"] = WiFi.macAddress();
    doc["puzzleName"] = PUZZLE_NAME;
    
    doc["outputs"] = NUM_DIGITAL_OUTPUTS;
    doc["inputs"] = NUM_DIGITAL_INPUTS;
    doc["analoginputs"] = NUM_ANALOG_INPUTPAIRS;
    doc["leds"] = NUM_FLED_ADDLEDS;
    doc["channels"] = NUM_FLED_CHANNELS;
    doc["ls1"] = count1;
    doc["ls2"] = count2;
    doc["ls3"] = count3;
    doc["sfound"] = solutionFound;
    doc["scode"] = (String(solutionWin[0]) + String(solutionWin[1]) + String(solutionWin[2]));
    
    // Convert the doc to a JSON string and publish
    String jsonPayload;
    serializeJson(doc, jsonPayload);

    sendJsonDocUpdateMQTTPayload(jsonPayload);
}
