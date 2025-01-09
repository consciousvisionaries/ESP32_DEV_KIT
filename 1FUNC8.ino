
#define functionVersion "V1.8.x"

// Existing variables that you've defined
const int inputDigitalPinsA[8] = {DADIN_PIN4, DADIN_PIN5, DADIN_PIN6, DADIN_PIN7, DADIN_PIN8, ADC1_CH0, ADC1_CH1, RX_UART2};
const int inputDigitalPinsB[8] = {ADC1_CH0, ADC1_CH1, ADC1_CH2, ADC1_CH3, ADC1_CH6, ADC1_CH7, DADIN_PIN9, RX_UART2};

int inputDigitalPinStateA[8] = {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH};
int inputDigitalPinStateB[8] = {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH};

const int outputPinsA[8] = {DOUT_PIN1, DOUT_PIN2, DOUT_PIN3, DOUT_PIN4, I2C_PIN2, I2C_PIN1, FLED_PIN1, ADLED_PIN1};
const int outputPinsB[8] = {DADIN_PIN2, DADIN_PIN3, DADIN_PIN4, DADIN_PIN5, DADIN_PIN6, DADIN_PIN7, DADIN_PIN8, TX_UART2};
int outputPins_initStateA[8] = {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH};
int outputPins_initStateB[8] = {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH};

const int analogInputPinsA[6] = {ADC1_CH6, DADIN_PIN7, DADIN_PIN2, ADC1_CH2, ADC1_CH0, DADIN_PIN5};
const int analogInputPinsB[6] = {ADC1_CH7, DADIN_PIN8, DADIN_PIN3, ADC1_CH3, ADC1_CH1, DADIN_PIN6};
volatile int lastStateAnalogInputs[8] = {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH};


const int outputFLEDPins[2] = {FLED_PIN1, DOUT_PIN4};
const int RXTX_Pins[3][2] = {{RX_UART1, TX_UART1}, {RX_UART2, TX_UART2}, {RX_UART0, TX_UART0}};

int ledCount[8] = {0, 0, 0, 0, 0, 0 ,0 ,0};
int lastLedCount[8] = {-1, -1, -1, -1, -1, -1, -1, -1}; // Tracks the last LED count for each dial

volatile int pulseCount[8] = {0, 0, 0, 0, 0, 0, 0, 0};
static int lastPulseCount[8] = {0, 0, 0, 0, 0, 0, 0, 0};


volatile bool pulseUpdated[8] = {false, false, false, false, false, false, false, false};

bool solutionFound = false;
bool solutionStable = false;
unsigned long solutionCheckStart = 0;

bool gameOverFlag = false;
bool restartFlag = true;
bool gameOnFlag = false;

static unsigned long lastExecutionTime = 0; // Tracks the last execution time

int solutionWin[8] = {1, 3, 5, 7, 9, 11, 13, 15};

void generateFUNCRandomSolution(int numbersolutions, int rangemax) {
  
    // Seed the random number generator
    randomSeed(analogRead(0));
    Serial.print("Solution: ");
    
    // Create an array to track which numbers have already been used
    bool usedNumbers[numbersolutions] = {false};
    
    // Populate the solution array with random values between 0 and (NUM_FLED_ADDLEDS / NUM_FLED_CHANNELS) without repetition
    for (int i = 0; i < numbersolutions; i++) {
        int randNum;
        do {
            randNum = random(0, rangemax);
        } while (usedNumbers[randNum]); // Keep generating a new random number until we find one that hasn't been used
    
        solutionWin[i] = randNum;
        usedNumbers[randNum] = true;  // Mark this number as used
    
        Serial.print(solutionWin[i]);
        Serial.print(", ");
    }
    
    Serial.println("<end>");

}

void checkForWin() {
    if (solutionFound) {
        if (!solutionStable) {
            solutionCheckStart = millis();
            solutionStable = true;
        }

        if (millis() - solutionCheckStart >= 2000) {
            executeFUNCBatchGPIOPin1(ADLED_PIN1);
            solutionStable = false;
        }
    } else {
        solutionStable = false;
    }
}

// Function to map pulse counts to the number of LEDs
int getFLEDCount(int maxcount, int analoginput) {
    // Ensure pulseCount is within the valid range before using it
    int TOT_RANGE = (PULSE_MAX_RANGE * (NUM_FLED_ADDLEDS / NUM_FLED_CHANNELS));
    int clampedPulseCount = constrain(pulseCount[analoginput], 0, TOT_RANGE);

    pulseCount[analoginput] = clampedPulseCount;

    // Return the mapped value to the number of LEDs based on the constrained pulse count
    return map(clampedPulseCount, 0, TOT_RANGE, 0, maxcount);
}

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

// Batch operation functions
void executeFUNCBatchGPIOPin1(int outputPin) {
    Serial.println("Solution stable for 5 seconds. Executing GPIO1 batch 1 operation...");
    digitalWrite(outputPin, LOW);
    delayESPTask(5000);

    for (int i = 0; i < 3; i++) {
        pulseCount[i] = 0;
    }
    gameOnFlag = false;
    restartFlag = false;

    digitalWrite(outputPin, HIGH);
    Serial.println("Batch 1 completed... held for 5 secs.");
}

void executeFUNCBatchGPIOPin2(int outputPin) {
    Serial.print("Batch 2 requested. Executing game start batch 2 operation...");

    delayESPTask(5000);
    gameOnFlag = false;
    restartFlag = true;

    if (outputPin != -1) {
      digitalWrite(outputPin, LOW);
      delayESPTask(5000);
  
  
      digitalWrite(outputPin, HIGH);
    }
    Serial.println("Batch 2 completed... restarted game!");
}

void executeFUNCBatchGPIOPin3(int outputPin) {
    Serial.print("Batch 3 requested. Executing GPIO2 batch 3 operation...");
    if (outputPin != -1) {
      digitalWrite(outputPin, LOW);
      delayESPTask(5000);
  
  
      digitalWrite(outputPin, HIGH);
    }
    Serial.println("Batch 3 completed... held for 5 secs.");
}

void executeFUNCBatchMQTT(String activity) {
    Serial.println("Key 'activity': " + activity);
}
