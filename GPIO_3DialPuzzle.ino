
// Last States
volatile int lastStateA1 = LOW;
volatile int lastStateA2 = LOW;
volatile int lastStateA3 = LOW;

static unsigned long lastExecutionTime = 0; // Tracks the last execution time
static int lastPulseCount1 = 0; // Tracks the last state of pulseCount1
static int lastPulseCount2 = 0; // Tracks the last state of pulseCount2
static int lastPulseCount3 = 0; // Tracks the last state of pulseCount3

// Batch operation function
void executeBatch1() {
  
    // Add your batch operation code here
    Serial.println("Solution stable for 5 seconds. Executing batch operation...");
    digitalWrite(PIN_OUTPUT1, LOW);
    delay(5000);
    pulseCount1 = 0;
    pulseCount2 = 0;
    pulseCount3 = 0;
    updateFASTLED();
    digitalWrite(PIN_OUTPUT1, HIGH);
    prepareLEDData(1, 1, 1);
}

void executeBatch2() {
  
    digitalWrite(PIN_OUTPUT2, LOW);
    delay(5000);
    digitalWrite(PIN_OUTPUT2, HIGH);
  
}

// Interrupt Handlers
void IRAM_ATTR handleInterruptA1() {
    int stateA = digitalRead(PIN_A1);
    int stateB = digitalRead(PIN_B1);

    if (stateA != lastStateA1) {
        if (stateA != stateB) {
            pulseCount1++;
            activityDetected();
        } else {
            pulseCount1--;
            activityDetected();
        }
    }

    lastStateA1 = stateA;
}

void IRAM_ATTR handleInterruptA2() {
    int stateA = digitalRead(PIN_A2);
    int stateB = digitalRead(PIN_B2);

    if (stateA != lastStateA2) {
        if (stateA != stateB) {
            pulseCount2++;
            activityDetected();
        } else {
            pulseCount2--;
            activityDetected();
        }
    }

    lastStateA2 = stateA;
}

void IRAM_ATTR handleInterruptA3() {
    int stateA = digitalRead(PIN_A3);
    int stateB = digitalRead(PIN_B3);

    if (stateA != lastStateA3) {
        if (stateA != stateB) {
            pulseCount3++;
            activityDetected();
        } else {
            pulseCount3--;
            activityDetected();
        }
    }

    lastStateA3 = stateA;
}


void activityDetected() {
  
}

void setupGPIO() {
    Serial.begin(115200);

    pinMode(PIN_OUTPUT1, OUTPUT);
    digitalWrite(PIN_OUTPUT1,HIGH);
    pinMode(PIN_OUTPUT2, OUTPUT);
    digitalWrite(PIN_OUTPUT2,HIGH);

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

    
    unsigned long currentTime = millis(); // Get the current time in milliseconds

    // Check if pulse counts have changed
    if ((pulseCount1 != lastPulseCount1 || pulseCount2 != lastPulseCount2 || pulseCount3 != lastPulseCount3) &&
        (currentTime - lastExecutionTime >= 10)) {

        lastExecutionTime = currentTime; // Update the last execution time

        // Update the last pulse counts
        if (pulseCount1 > 16000) { pulseCount1 = 16000; }
        if (pulseCount2 > 16000) { pulseCount2 = 16000; }
        if (pulseCount3 > 16000) { pulseCount3 = 16000; }
        // Update the last pulse counts
        if (pulseCount1 < 0) { pulseCount1 = 0; }
        if (pulseCount2 < 0) { pulseCount2 = 0; }
        if (pulseCount3 < 0) { pulseCount3 = 0; }

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
