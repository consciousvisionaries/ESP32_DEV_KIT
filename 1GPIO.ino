#include <FastLED.h>

#define DEBOUNCE_DELAY 500  // Debounce delay in milliseconds
#define FLED_PIN1 12

const int outputPins[2] = {18, 19};
const int inputPins[2] = {22, 21};
const int outputPins_initState[8] = {1, 1, 1, 1, 1, 1, 1, 1};
const int analogInputPinsA[3] = {14, 26, 33};
const int analogInputPinsB[3] = {27, 25, 32};
const int fastLEDOutputPins[1] = {FLED_PIN1};
const int RXTX_Pins[2] = {17, 16};



portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

String getInputStateGPIO(int inputPin) {
  int state = digitalRead(inputPin);
  return (state == HIGH) ? "high" : "low";  // Return "high" or "low"
}


void toggleOutputStateGPIO(int outputNumber) {
  if (outputNumber >= 0 && outputNumber < 8) {
    int pin = outputPins[outputNumber];
    bool currentState = digitalRead(pin);  // Read the current state before toggling
    bool newState = !currentState;         // Toggle state
    digitalWrite(pin, newState);          // Set the new state

    Serial.print("Output ");
    Serial.print(outputNumber);
    Serial.print(": Toggled to ");
    Serial.println(newState ? "HIGH" : "LOW");
  } else {
    Serial.println("Error: Invalid output number.");
  }


}
volatile int interruptCounter = 0;

// Interrupt service routines for each analog input pair
void IRAM_ATTR handleInterruptA() {
  interruptCounter++;
  int stateA = digitalRead(analogInputPinsA[0]);
  int stateB = digitalRead(analogInputPinsB[0]);

  portENTER_CRITICAL(&mux);
  if (stateA != lastStateAnalogInputs[0]) {
    pulseCount[0] += (stateA != stateB) ? 1 : -1;
    pulseUpdated[0] = true;
  }
  lastStateAnalogInputs[0] = stateA;
  portEXIT_CRITICAL(&mux);
}

void IRAM_ATTR handleInterruptB() {
  interruptCounter++;
  int stateA = digitalRead(analogInputPinsA[1]);
  int stateB = digitalRead(analogInputPinsB[1]);

  portENTER_CRITICAL(&mux);
  if (stateA != lastStateAnalogInputs[1]) {
    pulseCount[1] += (stateA != stateB) ? 1 : -1;
    pulseUpdated[1] = true;
  }
  lastStateAnalogInputs[1] = stateA;
  portEXIT_CRITICAL(&mux);
}

void IRAM_ATTR handleInterruptC() {
  interruptCounter++;
  int stateA = digitalRead(analogInputPinsA[2]);
  int stateB = digitalRead(analogInputPinsB[2]);

  portENTER_CRITICAL(&mux);
  if (stateA != lastStateAnalogInputs[2]) {
    pulseCount[2] += (stateA != stateB) ? 1 : -1;
    pulseUpdated[2] = true;
  }
  lastStateAnalogInputs[2] = stateA;
  portEXIT_CRITICAL(&mux);
}

void setupGPIO() {
  Serial.begin(115200);

  // Array to track used pins
  bool usedPins[50] = {false}; // Assuming a maximum of 40 GPIO pins on the microcontroller

  // Helper function to check and mark pins
  auto usePin = [&](int pin) {
    Serial.print(pin);
    Serial.print(" - ");
    if (usedPins[pin]) {
      Serial.print("Error: Pin conflict detected on pin ");
      Serial.println(pin);
      while (true); // Halt execution
    }
    usedPins[pin] = true;
  };

  // Initialize output pins
  for (int i = 0; i < NUM_DIGITAL_OUTPUTS; i++) {
    usePin(outputPins[i]); // Check for conflicts
    pinMode(outputPins[i], OUTPUT);
    digitalWrite(outputPins[i], outputPins_initState[i] ? HIGH : LOW);
  }
  Serial.println(String(NUM_DIGITAL_INPUTS) + " Outputs Initialized.");

  // Initialize input pins
  for (int i = 0; i < NUM_DIGITAL_OUTPUTS; i++) {
    usePin(inputPins[i]); // Check for conflicts
    pinMode(inputPins[i], INPUT_PULLUP);
  }
  Serial.println(String(NUM_DIGITAL_INPUTS) + " Inputs Initialized.");

  for (int i = 0; i < NUM_ANALOG_INPUTPAIRS; i++) {
    Serial.print("Initializing analog input pair ");
    Serial.print(i);
    usePin(analogInputPinsA[i]); // Check for conflicts
    usePin(analogInputPinsB[i]); // Check for conflicts
    pinMode(analogInputPinsA[i], INPUT_PULLUP);
    pinMode(analogInputPinsB[i], INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(analogInputPinsA[i]),
                    (i == 0 ? handleInterruptA : (i == 1 ? handleInterruptB : handleInterruptC)), CHANGE);
  }
  Serial.println(String(NUM_ANALOG_INPUTPAIRS) + " Analog Input Pairs Initialized.");
}


void loopGPIO() {

  static unsigned long lastPrint = 0;
  if (lastPrint != interruptCounter) {
    Serial.print("Interrupt A,B or C fired: ");
    Serial.println(interruptCounter);
    lastPrint = interruptCounter;
  }
  
unsigned long currentTime = millis();

for (int i = 0; i < 3; i++) {
  portENTER_CRITICAL(&mux);
  int count = pulseCount[i];
  bool updated = pulseUpdated[i];
  pulseUpdated[i] = false;
  portEXIT_CRITICAL(&mux);

  count = constrain(count, 0, PULSE_MAX_RANGE);

  if (updated) {
    Serial.print("Dial ");
    Serial.print(i + 1);
    Serial.print(" Count: ");
    Serial.println(count);
  }
}

esp_task_wdt_reset(); // Feed the watchdog
}

// Batch operation functions
void executeGPIOBatchPin1() {
  Serial.println("Solution stable for 5 seconds. Executing batch operation...");
  digitalWrite(outputPins[0], LOW);
  delayESPTask(5000);
  for (int i = 0; i < 3; i++) {
    pulseCount[i] = 0;
  }
  updateFASTLED();
  digitalWrite(outputPins[0], HIGH);
}

void executeGPIOBatchPin2() {
  digitalWrite(outputPins[1], LOW);
  delayESPTask(5000);
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
  doc1["analogspairs"] = NUM_ANALOG_INPUTPAIRS;

  // Convert the doc to a JSON string and publish
  String jsonPayload;
  serializeJson(doc1, jsonPayload);

  sendJsonDocUpdateMQTTPayload(jsonPayload);
}
