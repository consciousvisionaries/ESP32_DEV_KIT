portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

unsigned long lastDebounceTimeA[8] = {0,0,0,0,0,0,0,0}; // Store last debounce time for each input
unsigned long lastDebounceTimeB[8] = {0,0,0,0,0,0,0,0}; // Store last debounce time for each input

unsigned long debounceDelay = 50; // 50ms debounce time
int lastInputDigitalPinStateA[8] = {HIGH,HIGH,HIGH,HIGH,HIGH,HIGH,HIGH,HIGH};  // Stores the last known state of the pins
int lastInputDigitalPinStateB[8] = {HIGH,HIGH,HIGH,HIGH,HIGH,HIGH,HIGH,HIGH};  // Stores the last known state of the pins

// Method to get the current state of a digital input pin
bool getDigitalInputStateGPIO(int inputPin) {
  int state = digitalRead(inputPin);
  return (state == HIGH) ? HIGH : LOW;  // Return "HIGH" or "LOW"
}

// Method to toggle the output pin state
void toggleOutputStateGPIOA(int outputNumber) {
  if (outputNumber >= 0 && outputNumber < NUM_DIGITAL_OUTPUTSA) {
    int pin = outputPinsA[outputNumber];
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

// Method to toggle the output pin state
void toggleOutputStateGPIOB(int outputNumber) {
  if (outputNumber >= 0 && outputNumber < NUM_DIGITAL_OUTPUTSB) {
    int pin = outputPinsB[outputNumber];
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
void handleDigitalInputChangeA(int pinIndex) {
  // Check for debounce and pin state change
  unsigned long currentTime = millis();

  if (currentTime - lastDebounceTimeA[pinIndex] > debounceDelay) { 
    lastDebounceTimeA[pinIndex] = currentTime; // Update debounce time
    int currentState = digitalRead(inputDigitalPinsA[pinIndex]);

    if (currentState != lastInputDigitalPinStateA[pinIndex]) {
      lastInputDigitalPinStateA[pinIndex] = currentState;  // Update the last known state

      // Trigger action only if the pin goes LOW (e.g., a button press)
      if (currentState == LOW) {
        if (pinIndex == 0) {
          //executeFUNCBatchGPIOPin1();
        } else if (pinIndex == 1) {
          //executeFUNCBatchGPIOPin2();
        } else if (pinIndex == 2) {
          //executeFUNCBatchGPIOPin3();
        }
      }
    }
  }
}

// Method to handle digital input changes
void handleDigitalInputChangeB(int pinIndex) {
  // Check for debounce and pin state change
  unsigned long currentTime = millis();

  if (currentTime - lastDebounceTimeB[pinIndex] > debounceDelay) { 
    lastDebounceTimeB[pinIndex] = currentTime; // Update debounce time
    int currentState = digitalRead(inputDigitalPinsB[pinIndex]);

    if (currentState != lastInputDigitalPinStateB[pinIndex]) {
      lastInputDigitalPinStateB[pinIndex] = currentState;  // Update the last known state

      // Trigger action only if the pin goes LOW (e.g., a button press)
      if (currentState == LOW) {
        if (pinIndex == 0) {
          //executeFUNCBatchGPIOPin1();
        } else if (pinIndex == 1) {
          //executeFUNCBatchGPIOPin2();
        } else if (pinIndex == 2) {
          //executeFUNCBatchGPIOPin3();
        }
      }
    }
  }
}


// Method to handle analog input state changes
void handleAnalogInputPairsChange(int pinIndex) {
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
  bool usedPins[50] = {false}; // Assuming a maximum of 50 GPIO pins on the microcontroller

  // Lambda function to check and track pin usage
  auto usePin = [&](int pin) {
    if (usedPins[pin]) {
        Serial.print("Error: Pin conflict detected on pin ");
        Serial.println(pin);
        delay(60000);  // Wait for 60 seconds before continuing
        return;  // Exit if a conflict is found
    }
    usedPins[pin] = true;
    Serial.print("Pin ");
    Serial.print(pin);
    Serial.println(" has been marked as used.");
  };

  // Initialize output pins (A pins)
  for (int i = 0; i < NUM_DIGITAL_OUTPUTSA; i++) {
    usePin(outputPinsA[i]);  // Check for conflicts
    
    // Handle TX_UART2 pin for output
    if (outputPinsA[i] == TX_UART2) {
        Serial.println("Setting up TX_UART2 as an output...");
        Serial2.end();  // Disable UART2 to avoid conflicts
        delay(10);  // Allow time for UART2 to fully disable
        
        pinMode(outputPinsA[i], OUTPUT); // Set pin as OUTPUT
        delay(10);  // Small delay to ensure pinMode change is stable
        digitalWrite(outputPinsA[i], LOW); // Initialize to LOW
        delay(10);  // Small delay to stabilize the LOW state
        
        Serial.println("TX_UART2 disabled and set as digital output.");
        
    } else if (outputPinsA[i] == TX_UART1) {
        Serial.println("Setting up TX_UART1 as an output...");
        Serial1.end();  // Disable UART1 to avoid conflicts
        delay(10);  // Allow time for UART1 to fully disable
        
        pinMode(outputPinsA[i], OUTPUT); // Set pin as OUTPUT
        delay(10);  // Small delay to ensure pinMode change is stable
        digitalWrite(outputPinsA[i], LOW); // Initialize to LOW
        delay(10);  // Small delay to stabilize the LOW state
        
        Serial.println("TX_UART1 disabled and set as digital output.");
        
    }
    
        pinMode(outputPinsA[i], OUTPUT);
        delay(10);  // Allow time for pinMode to take effect
        digitalWrite(outputPinsA[i], outputPins_initStateA[i] ? HIGH : LOW); // Initialize with state array
        delay(10);  // Small delay after setting the output state
        Serial.print("Output pin ");
        Serial.print(outputPinsA[i]);
        Serial.print(" set to ");
        Serial.println(outputPins_initStateA[i] ? "HIGH" : "LOW");
    
  }
  Serial.println("<end> " + String(NUM_DIGITAL_OUTPUTSA) + " A-Outputs Initialized.");

  // Initialize output pins (B pins)
  for (int i = 0; i < NUM_DIGITAL_OUTPUTSB; i++) {
    usePin(outputPinsB[i]); // Check for conflicts

    // Handle TX_UART2 pin for output
    if (outputPinsB[i] == TX_UART2) {
        Serial.println("Setting up TX_UART2 as an output...");
        Serial2.end();  // Disable UART2
        delay(10);  // Allow time for UART2 to fully disable
        
        pinMode(outputPinsB[i], OUTPUT); // Set pin as OUTPUT
        delay(10);  // Small delay to ensure pinMode change is stable
        digitalWrite(outputPinsB[i], LOW); // Initialize to LOW
        delay(10);  // Small delay to stabilize the LOW state
        
        Serial.println("TX_UART2 disabled and set as digital output.");
        
    } else if (outputPinsB[i] == TX_UART1) {
        Serial.println("Setting up TX_UART1 as an output...");
        Serial1.end();  // Disable UART1
        delay(10);  // Allow time for UART1 to fully disable
        
        pinMode(outputPinsB[i], OUTPUT); // Set pin as OUTPUT
        delay(10);  // Small delay to ensure pinMode change is stable
        digitalWrite(outputPinsB[i], LOW); // Initialize to LOW
        delay(10);  // Small delay to stabilize the LOW state
        
        Serial.println("TX_UART1 disabled and set as digital output.");
        
    }
        pinMode(outputPinsB[i], OUTPUT);
        delay(10);  // Allow time for pinMode to take effect
        digitalWrite(outputPinsB[i], outputPins_initStateB[i] ? HIGH : LOW); // Initialize with state array
        delay(10);  // Small delay after setting the output state
        Serial.print("Output pin ");
        Serial.print(outputPinsB[i]);
        Serial.print(" set to ");
        Serial.println(outputPins_initStateB[i] ? "HIGH" : "LOW");
    
  }
  Serial.println("<end> " + String(NUM_DIGITAL_OUTPUTSB) + " B-Outputs Initialized.");

  // Initialize input pins (A pins)
  for (int i = 0; i < NUM_DIGITAL_INPUTSA; i++) {
    usePin(inputDigitalPinsA[i]); // Check for conflicts
    
    if (inputDigitalPinsA[i] == RX_UART2) {
        Serial2.end();  // Disable UART2
        delay(10);  // Allow time for UART2 to fully disable
        
        pinMode(inputDigitalPinsA[i], INPUT_PULLUP); // Set RX pin to INPUT_PULLUP
        delay(10);  // Small delay after setting pin mode
        Serial.println("RX_UART2 disabled and set as digital input.");
    } else if (inputDigitalPinsA[i] == RX_UART1) {
        Serial1.end();  // Disable UART1
        delay(10);  // Allow time for UART1 to fully disable
        
        pinMode(inputDigitalPinsA[i], INPUT_PULLUP); // Set RX pin to INPUT_PULLUP
        delay(10);  // Small delay after setting pin mode
        Serial.println("RX_UART1 disabled and set as digital input.");
    } 
        pinMode(inputDigitalPinsA[i], INPUT_PULLUP); // General configuration for other pins
        delay(10);  // Small delay after setting pin mode
        Serial.print("Input pin ");
        Serial.print(inputDigitalPinsA[i]);
        Serial.println(" set to INPUT_PULLUP.");
    
  }
  Serial.println("<end> " + String(NUM_DIGITAL_INPUTSA) + " A-Inputs Initialized.");

  // Initialize input pins (B pins)
  for (int i = 0; i < NUM_DIGITAL_INPUTSB; i++) {
    usePin(inputDigitalPinsB[i]); // Check for conflicts

    if (inputDigitalPinsB[i] == RX_UART2) {
        Serial2.end();  // Disable UART2
        delay(10);  // Allow time for UART2 to fully disable
        
        pinMode(inputDigitalPinsB[i], INPUT_PULLUP); // Set RX pin to INPUT_PULLUP
        delay(10);  // Small delay after setting pin mode
        Serial.println("RX_UART2 disabled and set as digital input.");
    } else if (inputDigitalPinsB[i] == RX_UART1) {
        Serial1.end();  // Disable UART1
        delay(10);  // Allow time for UART1 to fully disable
        
        pinMode(inputDigitalPinsB[i], INPUT_PULLUP); // Set RX pin to INPUT_PULLUP
        delay(10);  // Small delay after setting pin mode
        Serial.println("RX_UART1 disabled and set as digital input.");
    } 
        pinMode(inputDigitalPinsB[i], INPUT_PULLUP); // General configuration for other pins
        delay(10);  // Small delay after setting pin mode
        Serial.print("Input pin ");
        Serial.print(inputDigitalPinsB[i]);
        Serial.println(" set to INPUT_PULLUP.");
    
  }
  Serial.println("<end> " + String(NUM_DIGITAL_INPUTSB) + " B-Inputs Initialized.");

  // Initialize analog input pairs (A and B)
  for (int i = 0; i < NUM_ANALOG_INPUTPAIRS; i++) {
    Serial.print("Initializing analog input pair: ");
    Serial.print(i);
    Serial.println("; ");
    
    usePin(analogInputPinsA[i]); // Check for conflicts
    usePin(analogInputPinsB[i]); // Check for conflicts

    pinMode(analogInputPinsA[i], INPUT);  // Configure analog pin A as INPUT
    delay(10);  // Small delay after setting pin mode
    pinMode(analogInputPinsB[i], INPUT);  // Configure analog pin B as INPUT
    delay(10);  // Small delay after setting pin mode
    Serial.print("Analog input pair ");
    Serial.print(i);
    Serial.println(" initialized.");
  }
  Serial.println("<end> " + String(NUM_ANALOG_INPUTPAIRS) + " Analog Input Pairs Initialized!");

  // Initialize FLED addressable outputs (if applicable)
  for (int i = 0; i < NUM_FLED_OUTPUTS; i++) {
    usePin(outputFLEDPins[i]);
    Serial.println("Initializing FLED addressable output at Pin " + String(outputFLEDPins[i]));
  }

  Serial.println("<<GPIO Setup Complete>>");
  Serial.println();
}




// Main loop to handle GPIO activities
void loopGPIO() {
  
  // Poll and handle digital inputs (instead of relying on interrupts)
  for (int i = 0; i < NUM_DIGITAL_INPUTSA; i++) {
    handleDigitalInputChangeA(i);
  }

  // Poll and handle digital inputs (instead of relying on interrupts)
  for (int i = 0; i < NUM_DIGITAL_INPUTSB; i++) {
    handleDigitalInputChangeB(i);
  }


  // Poll and handle analog input changes
  for (int i = 0; i < NUM_ANALOG_INPUTPAIRS; i++) {
    handleAnalogInputPairsChange(i);
  }

  // Process pulse counts and updates for analog inputs
  for (int i = 0; i < NUM_ANALOG_INPUTPAIRS; i++) {
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
  doc1["inputs"] = NUM_DIGITAL_INPUTSA;
  doc1["outputs"] = NUM_DIGITAL_OUTPUTSA;
  doc1["analogspairs"] = NUM_ANALOG_INPUTPAIRS;

  // Convert the doc to a JSON string and publish
  String jsonPayload;
  serializeJson(doc1, jsonPayload);

  sendJsonDocUpdateMQTTPayload(jsonPayload);
}
