

#define DEBOUNCE_DELAY 500  // Debounce delay in milliseconds

const int outputPins[8] = {PIN_O1, PIN_O2, PIN_O3, PIN_O4, PIN_O5, PIN_O6, PIN_O7, PIN_O8};


// Function to initialize GPIO pins
void setupGPIO() {
    // Initialize outputs
    for (int i = 0; i < 8; i++) {
        pinMode(outputPins[i], HIGH);
    }
    Serial.println("Outputs Initialized.");
}

void loopGPIO() {


}



void executeMQTTBatch(String mqttMessage) { //start button
    
        
}
