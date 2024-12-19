


void handlePattern(unsigned long currentTime) {
   
  // Update outputs based on current pattern
  if (currentPattern == "blink") {
    if (currentTime - lastUpdateTime > blinkInterval) {
      lastUpdateTime = currentTime;
      for (int i = 0; i < NUM_OUTPUTS; i++) {
        outputStates[i] = !outputStates[i];  // Toggle all outputs
      }
      getOutputStates();
    }
  }
  else if (currentPattern == "chase") {
    if (currentTime - lastUpdateTime > blinkInterval) {
      lastUpdateTime = currentTime;
      outputStates[chaseIndex] = true;
      if (chaseIndex > 0) outputStates[chaseIndex - 1] = false;
      chaseIndex = (chaseIndex + 1) % NUM_OUTPUTS;
      getOutputStates();
    }
  }
  else if (currentPattern == "reverseChase") {
    if (currentTime - lastUpdateTime > blinkInterval) {
      lastUpdateTime = currentTime;
      outputStates[chaseIndex] = true;
      if (chaseIndex < (NUM_OUTPUTS-1)) outputStates[chaseIndex + 1] = false;
      chaseIndex = (chaseIndex - 1 + NUM_OUTPUTS) % NUM_OUTPUTS;
      getOutputStates();
    }
  }
  else if (currentPattern == "randomBlink") {
    if (currentTime - lastUpdateTime > blinkInterval) {
      lastUpdateTime = currentTime;
      int randOutput = random(0, NUM_OUTPUTS);
      outputStates[randOutput] = !outputStates[randOutput];  // Toggle random output
      getOutputStates();
    }
  }
  else if (currentPattern == "wave") {
    if (currentTime - lastUpdateTime > blinkInterval) {
      lastUpdateTime = currentTime;
      for (int i = 0; i < NUM_OUTPUTS; i++) {
        outputStates[i] = false;  // Turn off all outputs
      }
      outputStates[waveIndex] = true;  // Turn on the "wave" output
      waveIndex = (waveIndex + 1) % 8;
      getOutputStates();
    }
  }
  else if (currentPattern == "static") {
    // In "static" pattern, you can keep the outputs in a fixed state
    // For example, set all to ON or OFF based on your choice
    for (int i = 0; i < NUM_OUTPUTS; i++) {
      outputStates[i] = true;  // Keep all outputs on in static mode
    }
        getOutputStates();

  }
  else if (currentPattern == "off") {
    // In "off" pattern, turn all outputs off
    for (int i = 0; i < NUM_OUTPUTS; i++) {
      outputStates[i] = false;  // Turn all outputs off
    }
        getOutputStates();

  }

}



void setAllOutputs(bool state) {
  for (int i = 0; i < NUM_OUTPUTS; i++) {
    digitalWrite(outputPins[i], state ? HIGH : LOW);
    outputStates[i] = state;  // Update the tracked states
  }
}

void toggleAllOutputs() {
  for (int i = 0; i < NUM_OUTPUTS; i++) {
    bool currentState = digitalRead(outputPins[i]);  // Read the current state of the pin
    digitalWrite(outputPins[i], !currentState);     // Write the opposite state
    outputStates[i] = !currentState;                // Update the tracked state
  }
}
