

#define DEBOUNCE_DELAY 500  // Debounce delay in milliseconds

const int outputPins[NUM_OUTPUTS] = {PIN_O2, PIN_O7, PIN_O1, PIN_O6, PIN_O3, PIN_O4, PIN_O5, PIN_O8};
const int inputPins[8] = {PIN_B2, PIN_B7, PIN_B1, PIN_B6, PIN_B3, PIN_B4, PIN_B5, PIN_B8};
const int outputPins_initState[NUM_OUTPUTS] = {0,0,0,0,1,0,0,0};

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
    pinMode(outputPins[i], OUTPUT);  // Ensure the pin is set to OUTPUT
    //custom
    if (outputPins_initState[i] == LOW) {
        digitalWrite(outputPins[i], HIGH);
    } else {
        digitalWrite(outputPins[i], LOW);
    }
  }
  
  Serial.println("Outputs Initialized.");

  for (int i = 0; i < NUM_INPUTS; i++) {
    pinMode(inputPins[i], INPUT_PULLUP);  // Ensure the pin is set to OUTPUT
  }
  
  Serial.println("Inputs Initialized.");
}

void loopGPIO() {


}



void executeMQTTBatch(String mqttMessage) { //start button
    
        
}
