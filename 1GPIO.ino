#include <FastLED.h>

#define DEBOUNCE_DELAY 500  // Debounce delay in milliseconds

const int outputPins[8] = {2, 13, 18, 19, 21, 22, 23, 12};
const int inputPins[8] = {4, 5, 14, 27, 26, 25, 33, 32};
const int outputPins_initState[8] = {0, 0, 0, 0, 0, 0, 0, 0};
const int analogInputPinsA[3] = {14, 27, 26};
const int analogInputPinsB[3] = {25, 33, 32};
const int fastLEDOutputPins[1] = {12};
const int RXTX_Pins[2] = {17, 16};


#define LED_PIN1 12


// Last States
volatile int lastStateAnalogInputs[3] = {LOW, LOW, LOW};

static int lastPulseCount[3] = {0, 0, 0};
int pulseCount[3] = {0, 0, 0};

static int lastLedCount[3] = {-1, -1, -1}; // Tracks the last LED count for Dial 1
int ledCount[3] = {0, 0, 0};


static unsigned long lastExecutionTime = 0; // Tracks the last execution time

bool solutionFound = false;
bool solutionStable = false; // Tracks if solutionFound is stable for 5 seconds
unsigned long solutionCheckStart = 0; // Timestamp to track 5-second period

String getInputStateGPIO(int inputPin) {
  int state = digitalRead(inputPin);
  return (state == HIGH) ? "high" : "low";  // Return "high" or "low"
}

// Function to map pulse counts to the number of LEDs
int getCount(int maxcount, int pulsecount) {
  int clampedPulseCount = max(0, min(15000, pulsecount)); // Clamp pulseCount between 0 and 15000
  return map(clampedPulseCount, 0, 15000, 0, maxcount);
}

void toggleOutputStateGPIO(int outputNumber) {
  if (outputNumber >= 0 && outputNumber < 8) {
    int pin = outputPins[outputNumber];

    bool currentState = digitalRead(pin);  // Read the current state before toggling
    bool newState = !currentState;         // Toggle state
    digitalWrite(pin, newState);          // Set the new state

    // Confirm the state change
    Serial.print("Output ");
    Serial.print(outputNumber);
    Serial.print(": Toggled to ");
    Serial.println(newState ? "HIGH" : "LOW");
  } else {
    Serial.println("Error: Invalid output number.");
  }
}

// Interrupt service routines for each analog input pair
void IRAM_ATTR handleInterruptA() {
  int stateA = digitalRead(analogInputPinsA[0]);
  int stateB = digitalRead(analogInputPinsB[0]);

  if (stateA != lastStateAnalogInputs[0]) {
    pulseCount[0] += (stateA != stateB) ? 1 : -1;
  }
  lastStateAnalogInputs[0] = stateA;
}

void IRAM_ATTR handleInterruptB() {
  int stateA = digitalRead(analogInputPinsA[1]);
  int stateB = digitalRead(analogInputPinsB[1]);

  if (stateA != lastStateAnalogInputs[1]) {
    pulseCount[1] += (stateA != stateB) ? 1 : -1;
  }
  lastStateAnalogInputs[1] = stateA;
}

void IRAM_ATTR handleInterruptC() {
  int stateA = digitalRead(analogInputPinsA[2]);
  int stateB = digitalRead(analogInputPinsB[2]);

  if (stateA != lastStateAnalogInputs[2]) {
    pulseCount[2] += (stateA != stateB) ? 1 : -1;
  }
  lastStateAnalogInputs[2] = stateA;
}

void setupGPIO() {
  Serial.begin(115200);

  // Initialize digital output pins
  for (int i = 0; i < 8; i++) {
    pinMode(outputPins[i], OUTPUT);
    digitalWrite(outputPins[i], outputPins_initState[i] ? HIGH : LOW);
  }
  Serial.println("Outputs Initialized.");

  // Initialize digital input pins
  for (int i = 0; i < 8; i++) {
    pinMode(inputPins[i], INPUT_PULLUP);
  }
  Serial.println("Inputs Initialized.");

  // Initialize analog input pins and attach interrupts
  for (int i = 0; i < 3; i++) {
    pinMode(analogInputPinsA[i], INPUT_PULLUP);
    pinMode(analogInputPinsB[i], INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(analogInputPinsA[i]), 
      (i == 0 ? handleInterruptA : (i == 1 ? handleInterruptB : handleInterruptC)), CHANGE);
  }
  Serial.println("Analog Input Pairs Initialized.");
}

void loopGPIO() {
  unsigned long currentTime = millis();

  // Loop through and manage pulse counts
  for (int i = 0; i < 3; i++) {
    pulseCount[i] = constrain(pulseCount[i], 0, 16000); // Clamp pulse count to range

    // Print pulse count if debounce time has passed
    if (currentTime - lastExecutionTime >= 10) {
      Serial.print("Dial ");
      Serial.print(i + 1);
      Serial.print(" Count: ");
      Serial.println(pulseCount[i]);
    }
  }

  lastExecutionTime = currentTime;
}

// Batch operation functions
void executeGPIOBatch1() {
  Serial.println("Solution stable for 5 seconds. Executing batch operation...");
  digitalWrite(outputPins[0], LOW);
  delay(5000);
  for (int i = 0; i < 3; i++) {
    pulseCount[i] = 0;
  }
  updateFASTLED();
  digitalWrite(outputPins[0], HIGH);
}

void executeGPIOBatch2() {
  digitalWrite(outputPins[1], LOW);
  delay(5000);
  digitalWrite(outputPins[1], HIGH);
}

void sendGPIO_MQTTPayload() {
  
  DynamicJsonDocument doc1(MQTT_MAX_PACKET_SIZE);
    doc1["mac"] = WiFi.macAddress();
    doc1["puzzleName"] = PUZZLE_NAME;
    doc1["leds"] = NUM_FLED_ADDLEDS;
    doc1["chan"] = NUM_FLED_CHANNELS;
    doc1["inputs"] = NUM_DIGITAL_INPUTS;
    doc1["outputs"] = NUM_DIGITAL_OUTPUTS;

    // Convert the doc to a JSON string and publish
  String jsonPayload;
  serializeJson(doc1, jsonPayload);

  sendJsonDocUpdateMQTTPayload(jsonPayload);
}
