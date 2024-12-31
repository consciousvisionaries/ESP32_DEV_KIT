

#define DEBOUNCE_DELAY 500  // Debounce delay in milliseconds

const int outputPins[NUM_OUTPUTS] = {PIN_O1, PIN_O2, PIN_O3, PIN_O4, PIN_O5, PIN_O6, PIN_O7, PIN_O8};

const int inputPins[NUM_INPUTS] = {PIN_B1, PIN_B2, PIN_B3, PIN_B4, PIN_B5, PIN_B6, PIN_B7, PIN_B8};


// Function to initialize GPIO pins
void setupGPIO() {

  for (int i = 0; i < NUM_OUTPUTS; i++) {
    pinMode(outputPins[i], OUTPUT);  // Ensure the pin is set to OUTPUT
    digitalWrite(outputPins[i], HIGH);
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
