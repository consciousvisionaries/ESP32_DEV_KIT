portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

unsigned long lastDebounceTime[NUM_DIGITAL_INPUTS] = {0}; // Store last debounce time for each input
unsigned long debounceDelay = 50; // 50ms debounce time
int lastInputDigitalPinState[NUM_DIGITAL_INPUTS] = {HIGH};  // Stores the last known state of the pins

// Method to get the current state of a digital input pin
bool getDigitalInputStateGPIO(int inputPin) {
  int state = digitalRead(inputPin);
  return (state == HIGH) ? HIGH : LOW;  // Return "HIGH" or "LOW"
}

// Method to toggle the output pin state
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

// Method to handle digital input changes
void handleDigitalInputChange(int pinIndex) {
  // Check for debounce and pin state change
  unsigned long currentTime = millis();

  if (currentTime - lastDebounceTime[pinIndex] > debounceDelay) { 
    lastDebounceTime[pinIndex] = currentTime; // Update debounce time
    int currentState = digitalRead(inputDigitalPins[pinIndex]);

    if (currentState != lastInputDigitalPinState[pinIndex]) {
      lastInputDigitalPinState[pinIndex] = currentState;  // Update the last known state

      // Trigger action only if the pin goes LOW (e.g., a button press)
      if (currentState == LOW) {
        if (pinIndex == 0) {
          executeFUNCBatchGPIOPin1();
        } else if (pinIndex == 1) {
          executeFUNCBatchGPIOPin2();
        } else if (pinIndex == 2) {
          executeFUNCBatchGPIOPin3();
        }
      }
    }
  }
}

// Method to handle analog input state changes
void handleAnalogInputChange(int pinIndex) {
  int stateA = digitalRead(analogInputPinsA[pinIndex]);
  int stateB = digitalRead(analogInputPinsB[pinIndex]);

  portENTER_CRITICAL(&mux);
  if (stateA != lastStateAnalogInputs[pinIndex]) {
    pulseCount[pinIndex] += (stateA != stateB) ? 1 : -1;
    pulseUpdated[pinIndex] = true;
  }
  lastStateAnalogInputs[pinIndex] = stateA;
  portEXIT_CRITICAL(&mux);
}

// Method to set up GPIO pins (outputs, inputs, analog)
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
    Serial.println("Output " + String(i) + " = " + String(outputPins_initState[i]));
  }
  Serial.println("<end> " + String(NUM_DIGITAL_OUTPUTS) + " Outputs Initialized.");

  // Initialize input pins
  for (int i = 0; i < NUM_DIGITAL_INPUTS; i++) {
    usePin(inputDigitalPins[i]); // Check for conflicts
    pinMode(inputDigitalPins[i], INPUT_PULLUP);
  }
  Serial.println("<end> " + String(NUM_DIGITAL_INPUTS) + " Inputs Initialized.");

  // Initialize analog input pairs
  for (int i = 0; i < NUM_ANALOG_INPUTPAIRS; i++) {
    Serial.print("Initializing analog input pair : ");
    Serial.print(i);
    Serial.println("; ");
    usePin(analogInputPinsA[i]); // Check for conflicts
    usePin(analogInputPinsB[i]); // Check for conflicts

    pinMode(analogInputPinsA[i], INPUT_PULLUP);
    pinMode(analogInputPinsB[i], INPUT_PULLUP);
  }
  Serial.println("<end> " + String(NUM_ANALOG_INPUTPAIRS) + " Analog Input Pairs Initialized!");

  // Initialize FLED addressable outputs
  for (int i = 0; i < NUM_FLED_OUTPUTS; i++) {
    Serial.print("Initializing FLED addressable outputs: ");
    usePin(outputFLEDPins[i]);
    Serial.println("FLED Pin");
  }
  Serial.println("<<GPIO Setup Complete>>");
  Serial.println();
}

// Main loop to handle GPIO activities
void loopGPIO() {
  // Poll and handle digital inputs (instead of relying on interrupts)
  for (int i = 0; i < NUM_DIGITAL_INPUTS; i++) {
    handleDigitalInputChange(i);
  }

  // Poll and handle analog input changes
  for (int i = 0; i < NUM_ANALOG_INPUTPAIRS; i++) {
    handleAnalogInputChange(i);
  }

  // Process pulse counts and updates for analog inputs
  for (int i = 0; i < 3; i++) {
    portENTER_CRITICAL(&mux);
    int count = pulseCount[i];
    bool updated = pulseUpdated[i];
    pulseUpdated[i] = false;
    portEXIT_CRITICAL(&mux);

    count = constrain(count, 0, (NUM_FLED_ADDLEDS / NUM_FLED_CHANNELS));

    if (updated) {
      Serial.print("Dial ");
      Serial.print(i + 1);
      Serial.print(" Count: ");
      Serial.println(count);
    }
  }
}

// Function to send GPIO state via MQTT
void sendGPIO_MQTTPayload() {
  Serial.println("Prepare to initialize UIDashboard Stage " + globalSettings.nrGroup + " >>>>>");

  DynamicJsonDocument doc1(MQTT_MAX_PACKET_SIZE);
  doc1["mac"] = WiFi.macAddress();
  doc1["puzzleName"] = PUZZLE_NAME;
  doc1["version"] = wifiSettings.storedVersion;
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
