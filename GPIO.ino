int beatSource = 1;

void handlePatterns() {
  static int chaseIndex = 0;

  if (beatStatus[beatSource]) {  // Check if beatStatus[0] is true to trigger pattern update
    // Update the outputs according to the current pattern
    if (currentPattern == "off") {
      setAllOutputs(false); // Turn all off
    } else if (currentPattern == "blink") {
      static bool state = false;
      state = !state;
      setAllOutputs(state);
    } else if (currentPattern == "chase") {
      for (int i = 0; i < NUM_OUTPUTS; i++) {
        digitalWrite(outputPins[i], i == chaseIndex ? HIGH : LOW);
        outputStates[i] = i == chaseIndex; // Update output states
      }
      chaseIndex = (chaseIndex + 1) % NUM_OUTPUTS;
    } else if (currentPattern == "reverseChase") {
      for (int i = 0; i < NUM_OUTPUTS; i++) {
        digitalWrite(outputPins[i], i == (NUM_OUTPUTS - chaseIndex - 1) ? HIGH : LOW);
        outputStates[i] = i == (NUM_OUTPUTS - chaseIndex - 1); // Update output states
      }
      chaseIndex = (chaseIndex + 1) % NUM_OUTPUTS;
    } else if (currentPattern == "randomBlink") {
      for (int i = 0; i < NUM_OUTPUTS; i++) {
        bool newState = random(2);
        digitalWrite(outputPins[i], newState ? HIGH : LOW);
        outputStates[i] = newState; // Update output states
      }
    } else if (currentPattern == "wave") {
      static int waveStep = 0;
      for (int i = 0; i < NUM_OUTPUTS; i++) {
        int brightness = max(0, 255 - abs((i - waveStep) * 50)); // Adjust wave shape
        analogWrite(outputPins[i], brightness);
        outputStates[i] = brightness > 0; // Update output states
      }
      waveStep = (waveStep + 1) % NUM_OUTPUTS;
    } else if (currentPattern == "static") {
      setAllOutputs(true); // Turn all on
    }
    
    // Reset beatStatus[0] after the pattern update, to prevent constant updates
    beatStatus[beatSource] = false;
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
