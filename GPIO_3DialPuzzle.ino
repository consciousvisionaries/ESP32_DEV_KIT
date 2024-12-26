bool getSolutionCompleted;


void solutionCompleted () {

getSolutionCompleted = true;
  
for (int s = 0; s < 3; s++) {
  if (lastState[s] != solutionWin[s]) {
    getSolutionCompleted = false;
    break;
  } 
}

}

void activityDetected() {
  
}
  

// Interrupt Handlers
void IRAM_ATTR handleInterruptA1() {
    int stateA = digitalRead(PIN_A1);
    int stateB = digitalRead(PIN_B1);

    if (stateA != lastState[0]) {
        if (stateA != stateB) {
            pulseCount1++;
            activityDetected();
        } else {
            pulseCount1--;
            activityDetected();
        }
    }

    lastState[0] = stateA;
}

void IRAM_ATTR handleInterruptA2() {
    int stateA = digitalRead(PIN_A2);
    int stateB = digitalRead(PIN_B2);

    if (stateA != lastState[1]) {
        if (stateA != stateB) {
            pulseCount2++;
            activityDetected();
        } else {
            pulseCount2--;
            activityDetected();
        }
    }

    lastState[1] = stateA;
}

void IRAM_ATTR handleInterruptA3() {
    int stateA = digitalRead(PIN_A3);
    int stateB = digitalRead(PIN_B3);

    if (stateA != lastState[2]) {
        if (stateA != stateB) {
            pulseCount3++;
            activityDetected();
        } else {
            pulseCount3--;
            activityDetected();
        }
    }

    lastState[2] = stateA;
}

unsigned long lastExecutionTime = 0; // Tracks the last execution time



void setupGPIO() {
    Serial.begin(115200);

    // Set up Dial 1
    pinMode(PIN_A1, INPUT_PULLUP);
    pinMode(PIN_B1, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(PIN_A1), handleInterruptA1, CHANGE);

    // Set up Dial 2
    pinMode(PIN_A2, INPUT_PULLUP);
    pinMode(PIN_B2, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(PIN_A2), handleInterruptA2, CHANGE);

    // Set up Dial 3
    pinMode(PIN_A3, INPUT_PULLUP);
    pinMode(PIN_B3, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(PIN_A3), handleInterruptA3, CHANGE);

    Serial.println("Three Rotary Encoders Initialized");
}

void loopGPIO() {

  solutionCompleted();

  if (getSolutionCompleted == true)  {
    Serial.println("WIN");
  }

    static unsigned long lastExecutionTime = 0; // Tracks the last execution time
    static int lastPulseCount1 = 0; // Tracks the last state of pulseCount1
    static int lastPulseCount2 = 0; // Tracks the last state of pulseCount2
    static int lastPulseCount3 = 0; // Tracks the last state of pulseCount3

    unsigned long currentTime = millis(); // Get the current time in milliseconds

    // Check if pulse counts have changed
    if ((pulseCount1 != lastPulseCount1 || pulseCount2 != lastPulseCount2 || pulseCount3 != lastPulseCount3) &&
        (currentTime - lastExecutionTime >= 10)) {

        lastExecutionTime = currentTime; // Update the last execution time

        // Update the last pulse counts
        lastPulseCount1 = pulseCount1;
        lastPulseCount2 = pulseCount2;
        lastPulseCount3 = pulseCount3;

        updateFASTLED(); // Update LED display

        // Debug output
        Serial.print("Dial 1 Count: ");
        Serial.println(pulseCount1);

        Serial.print("Dial 2 Count: ");
        Serial.println(pulseCount2);

        Serial.print("Dial 3 Count: ");
        Serial.println(pulseCount3);

        Serial.println("--------------------------");
    }
}
