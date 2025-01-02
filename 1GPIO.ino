

#define DEBOUNCE_DELAY 500  // Debounce delay in milliseconds



String getInputStateGPIO(int inputPin) {
  int state = digitalRead(inputPin);
  return (state == HIGH) ? "high" : "low";  // Return "high" or "low"
}

void toggleOutputStateGPIO(int outputNumber) {
  if (outputNumber >= 0 && outputNumber < NUM_OUTPUTS) {
    int pin = outputPins[outputNumber];

    bool currentState = digitalRead(pin);  // Read the current state before toggling
    bool newState = (currentState == HIGH) ? LOW : HIGH;  // Toggle state
    digitalWrite(pin, newState);  // Set the new state

    // Confirm the state change
    Serial.print("Output ");
    Serial.print(outputNumber);
    Serial.print(": Toggled to ");
    Serial.println(newState ? "HIGH" : "LOW");

  } else {
    Serial.println("Error: Invalid output number.");
  }
}

// Function to initialize GPIO pins
void setupGPIO() {

  for (int i = 0; i < NUM_OUTPUTS; i++) {
    pinMode(outputPins[sortOrder[i]], OUTPUT);  // Ensure the pin is set to OUTPUT
    digitalWrite(outputPins[i], HIGH);
  }
  
  Serial.println("Outputs Initialized.");

  for (int i = 0; i < NUM_INPUTS; i++) {
    pinMode(inputPins[sortOrder[i]], INPUT_PULLUP);  // Ensure the pin is set to OUTPUT
  }
  
  Serial.println("Inputs Initialized.");
}

void loopGPIO() {


}



void executeMQTTBatch(String mqttMessage) { //start button
    
        
}
