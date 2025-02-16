int ledCount[3] = {0, 0, 0};
int lastLedCount[3] = { -1, -1, -1}; // Tracks the last LED count for each dial

static int lastPulseCount[3] = {0, 0, 0};

volatile int lastStateAnalogInputs[3] = {LOW, LOW, LOW};
volatile int pulseCount[3] = {0, 0, 0};
volatile bool pulseUpdated[3] = {false, false, false};

bool solutionFound = false;
bool solutionStable = false;
unsigned long solutionCheckStart = 0;

static unsigned long lastExecutionTime = 0; // Tracks the last execution time

int solutionWin[] = {6, 5, 9};

void checkForWin() {
  if (solutionFound) {
    if (!solutionStable) {
      solutionCheckStart = millis();
      solutionStable = true;
    }

    if (millis() - solutionCheckStart >= 2000) {
      executeGPIOBatchPin1();
      solutionStable = false;
    }
  } else {
    solutionStable = false;
  }
}

// Function to map pulse counts to the number of LEDs
int getCount(int maxcount, int analoginputpin) {
  // Ensure pulseCount is within the valid range before using it
  int clampedPulseCount = constrain(pulseCount[analoginputpin], 0, PULSE_MAX_RANGE);

  pulseCount[analoginputpin] = clampedPulseCount;

  // Return the mapped value to the number of LEDs based on the constrained pulse count
  return map(clampedPulseCount, 0, PULSE_MAX_RANGE, 0, maxcount);
}

void funcRotaryDialPuzzle() {

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

  int currentLedCount[3] = { ledCount[0], ledCount[1], ledCount[2] };

  if (currentLedCount[0] != lastLedCount[0] ||
      currentLedCount[1] != lastLedCount[1] ||
      currentLedCount[2] != lastLedCount[2]) {

    lastLedCount[0] = currentLedCount[0];
    lastLedCount[1] = currentLedCount[1];
    lastLedCount[2] = currentLedCount[2];

    send3D_ROTARY_PULSEMQTTData(ledCount[0], ledCount[1], ledCount[2]);

    // Debug output
    Serial.println("LED Strip 1: " + String(ledCount[0]));
    Serial.println("LED Strip 2: " + String(ledCount[1]));
    Serial.println("LED Strip 3: " + String(ledCount[2]));

    updateFLED_clearAll();

    for (int i = 0; i < currentLedCount[0]; i++) {
      updateFLED_address(i, "red");
    }
    for (int i = NUM_FLED_POSITIONS; i < NUM_FLED_POSITIONS + currentLedCount[1]; i++) {
      updateFLED_address(i, "green");
    }
    for (int i = (NUM_FLED_POSITIONS * 2); i < (NUM_FLED_POSITIONS * 2) + currentLedCount[2]; i++) {
      updateFLED_address(i, "blue");
    }

    updateFLED_show();



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
  doc["sfound"] = solutionFound;
  doc["scode"] = String(solutionWin[0]) + String(solutionWin[1]) + String(solutionWin[2]);

  String jsonPayload;
  serializeJson(doc, jsonPayload);
  sendJsonDocUpdateMQTTPayload(jsonPayload);
}
