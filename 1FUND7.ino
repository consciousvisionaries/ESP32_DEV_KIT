void funcRotaryDialPuzzle() {
  
    if (gameOnFlag || restartFlag) {
      
        if (restartFlag && !gameOnFlag) {
            generateFUNCRandomSolution(8,16);
            gameOnFlag = true;
            restartFlag = false;
            gameOverFlag = false;
            
            // Reset last LED counts
            for (int i = 0; i < NUM_FLED_CHANNELS; i++) {
                lastLedCount[i] = -1;
                ledCount[i] = -1;
            }

            send3D_ROTARY_PULSEMQTTData(ledCount[0], ledCount[1], ledCount[2]);

            Serial.println("Game is set!");
        }

        int maxLEDCount = (NUM_FLED_ADDLEDS / NUM_FLED_CHANNELS);

        // Simulate fetching pulse counts
        for (int c = 0; c < NUM_FLED_CHANNELS; c++) {
          ledCount[c] = getFLEDCount(maxLEDCount, c);
        }
        
        // Check if the solution is found
        solutionFound = true;
        for (int s = 0; s < NUM_FLED_CHANNELS; s++) {
            if (ledCount[s] != solutionWin[s]) {
                solutionFound = false;
                break;
            }
        }

        checkForWin();

        // Update LED states only if there's a change
        bool ledChanged = false;
        for (int i = 0; i < NUM_FLED_CHANNELS; i++) {
            if (ledCount[i] != lastLedCount[i]) {
                ledChanged = true;
                lastLedCount[i] = ledCount[i];
            }
        }

        if (ledChanged) {
            updateFLED_clearAll();

            // Update solution LEDs
            for (int i = 0; i < NUM_FLED_CHANNELS; i++) {
                updateFLED_address(solutionWin[i] + (i * maxLEDCount), "orange");
            }
            
                     
            // Update LEDs for each dial dynamically based on NUM_FLED_CHANNELS
            for (int c = 0; c < NUM_FLED_CHANNELS; c++) {
                // Calculate the start and end LED indices for the current channel
                int startIdx = c * maxLEDCount;
                int endIdx = startIdx + ledCount[c];
            
                // Update LEDs for the current channel
                for (int i = startIdx; i < endIdx; i++) {
                    updateFLED_address(i, globalSettings.colorsFLEDChannels[c]);  // Use the color for the current channel
                }
            }
            
              

            updateFLED_show();

            Serial.println("Change in last dial detected! MQTT message to follow....");

            send3D_ROTARY_PULSEMQTTData(ledCount[0], ledCount[1], ledCount[2]);

            // Debug output
            for (int i = 0; i < NUM_FLED_CHANNELS; i++) {
                Serial.println("LED Strip " + String(i+1) + ": " + String(ledCount[i]));
            }
        }
    } else if (!restartFlag && !gameOverFlag) {
        gameOverFlag = true;

        // Set final LED counts to solution
        for (int i = 0; i < NUM_FLED_CHANNELS; i++) {
            ledCount[i] = solutionWin[i];
        }

        send3D_ROTARY_PULSEMQTTData(ledCount[0], ledCount[1], ledCount[2]);
        Serial.println("Game is over!");
    } else {
        // No action
    }
}


void send3D_ROTARY_PULSEMQTTData(int count1, int count2, int count3) {
    DynamicJsonDocument doc(1024);

    doc["mac"] = WiFi.macAddress();
    doc["puzzleName"] = PUZZLE_NAME;

    doc["outputs"] = NUM_DIGITAL_OUTPUTSA;
    doc["inputs"] = NUM_DIGITAL_INPUTSA;
    doc["analoginputs"] = NUM_ANALOG_INPUTPAIRS;
    doc["leds"] = NUM_FLED_ADDLEDS;
    doc["channels"] = NUM_FLED_CHANNELS;
    doc["ls1"] = count1;
    doc["ls2"] = count2;
    doc["ls3"] = count3;
    doc["s1"] = solutionWin[0];
    doc["s2"] = solutionWin[1];
    doc["s3"] = solutionWin[2];
    doc["version"] = wifiSettings.storedVersion;

    doc["sfound"] = solutionFound;

    String jsonPayload;
    serializeJson(doc, jsonPayload);
    sendJsonDocUpdateMQTTPayload(jsonPayload);
}
