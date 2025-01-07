// Existing variables that you've defined
const int inputDigitalPins[10] = {AIN_PIN4, AIN_PIN5, AIN_PIN6, AIN_PIN7, AIN_PIN8, ADC1_CH0, ADC1_CH1, RX_UART2, DADIN_PIN2, DADIN_PIN3};
int inputDigitalPinState[10] = {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH};

const int outputPins[10] = {DOUT_PIN1, DOUT_PIN2, DOUT_PIN3, DOUT_PIN4, I2C_PIN2, I2C_PIN1, DADIN_PIN3, TX_UART2, AIN_PIN6, AIN_PIN5};
int outputPins_initState[10] = {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH};

const int analogInputPinsA[8] = {ADC1_CH6, AIN_PIN7, DADIN_PIN2, ADC1_CH2, ADC1_CH0, AIN_PIN5, RX_UART2, RX_UART1};
const int analogInputPinsB[8] = {ADC1_CH7, AIN_PIN8, DADIN_PIN3, ADC1_CH3, ADC1_CH1, AIN_PIN6, TX_UART2, TX_UART1};

const int outputFLEDPins[2] = {FLED_PIN1, DOUT_PIN4};
const int RXTX_Pins[3][2] = {{RX_UART1, TX_UART1}, {RX_UART2, TX_UART2}, {RX_UART0, TX_UART0}};

int ledCount[3] = {0, 0, 0};
int lastLedCount[3] = {-1, -1, -1}; // Tracks the last LED count for each dial

volatile int pulseCount[3] = {0, 0, 0};
static int lastPulseCount[3] = {0, 0, 0};

volatile int lastStateAnalogInputs[3] = {LOW, LOW, LOW};
volatile bool pulseUpdated[3] = {false, false, false};

bool solutionFound = false;
bool solutionStable = false;
unsigned long solutionCheckStart = 0;

bool gameOverFlag = false;
bool restartFlag = true;
bool gameOnFlag = false;

static unsigned long lastExecutionTime = 0; // Tracks the last execution time

int solutionWin[] = {6, 5, 9};

void generateFUNCRandomSolution() {
    // Seed the random number generator
    randomSeed(analogRead(0));
    Serial.print("Solution: ");
    // Populate the solution array with random values between 0 and 9
    for (int i = 0; i < 3; i++) {
        solutionWin[i] = random(5, (NUM_FLED_ADDLEDS / NUM_FLED_CHANNELS)); 
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
            executeFUNCBatchGPIOPin1();
            solutionStable = false;
        }
    } else {
        solutionStable = false;
    }
}

// Function to map pulse counts to the number of LEDs
int getCount(int maxcount, int analoginput) {
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
            generateFUNCRandomSolution();
            gameOnFlag = true;
            restartFlag = false;
            gameOverFlag = false;
            lastLedCount[0] = -1;
            lastLedCount[1] = -1;
            lastLedCount[2] = -1;
            ledCount[0] = -1;
            ledCount[1] = -1;
            ledCount[2] = -1;

            send3D_ROTARY_PULSEMQTTData(ledCount[0], ledCount[1], ledCount[2]);

            Serial.println("Game is set!");
        }

        int maxLEDCount = (NUM_FLED_ADDLEDS / NUM_FLED_CHANNELS);

        // Simulate fetching pulse counts
        ledCount[0] = getCount(maxLEDCount, 0);
        ledCount[1] = getCount(maxLEDCount, 1);
        ledCount[2] = getCount(maxLEDCount, 2);

        // Check if the solution is found
        solutionFound = true;
        for (int s = 0; s < 3; s++) {
            if (ledCount[s] != solutionWin[s]) {
                solutionFound = false;
                break;
            }
        }

        checkForWin();

        // Update LED states only if there's a change
        if (ledCount[0] != lastLedCount[0] || ledCount[1] != lastLedCount[1] || ledCount[2] != lastLedCount[2]) {
            lastLedCount[0] = ledCount[0];
            lastLedCount[1] = ledCount[1];
            lastLedCount[2] = ledCount[2];

            updateFLED_clearAll();

            updateFLED_address(solutionWin[0], "orange");
            updateFLED_address(solutionWin[1] + maxLEDCount, "orange");
            updateFLED_address(solutionWin[2] + maxLEDCount + maxLEDCount, "orange");

            // Update LEDs for each dial
            for (int i = 0; i < ledCount[0]; i++) {
                updateFLED_address(i, "red");
            }
            for (int i = maxLEDCount; i < maxLEDCount + ledCount[1]; i++) {
                updateFLED_address(i, "green");
            }
            for (int i = maxLEDCount + maxLEDCount; i < maxLEDCount + maxLEDCount + ledCount[2]; i++) {
                updateFLED_address(i, "blue");
            }

            updateFLED_show();

            Serial.println("Change in last dial detected! MQTT message to follow....");

            send3D_ROTARY_PULSEMQTTData(ledCount[0], ledCount[1], ledCount[2]);

            // Debug output
            Serial.println("LED Strip 1: " + String(ledCount[0]));
            Serial.println("LED Strip 2: " + String(ledCount[1]));
            Serial.println("LED Strip 3: " + String(ledCount[2]));
        }
    } else if (!restartFlag && !gameOverFlag) {
        gameOverFlag = true;
        ledCount[0] = solutionWin[0];
        ledCount[1] = solutionWin[1];
        ledCount[2] = solutionWin[2];
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

    doc["outputs"] = NUM_DIGITAL_OUTPUTS;
    doc["inputs"] = NUM_DIGITAL_INPUTS;
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
void executeFUNCBatchGPIOPin1() {
    Serial.println("Solution stable for 5 seconds. Executing GPIO1 batch 1 operation...");
    digitalWrite(outputPins[0], LOW);
    delayESPTask(5000);

    for (int i = 0; i < 3; i++) {
        pulseCount[i] = 0;
    }
    gameOnFlag = false;
    restartFlag = false;

    digitalWrite(outputPins[0], HIGH);
    Serial.println("Batch 1 completed... Relay 1 trigger; forced 'Game Win'");
}

void executeFUNCBatchGPIOPin2() {
    Serial.print("Batch 2 requested. Executing game start batch 2 operation...");
    delayESPTask(5000);
    gameOnFlag = false;
    restartFlag = true;
    Serial.println("Batch 2 completed... restarted game!");
}

void executeFUNCBatchGPIOPin3() {
    Serial.print("Batch 3 requested. Executing GPIO2 batch 3 operation...");
    digitalWrite(outputPins[1], LOW);
    delayESPTask(5000);


    digitalWrite(outputPins[1], HIGH);
    Serial.println("Batch 3 completed... Relay two trigger; held for 5 secs.");
}

void executeFUNCBatchMQTT(String activity) {
    Serial.println("Key 'activity': " + activity);
}
