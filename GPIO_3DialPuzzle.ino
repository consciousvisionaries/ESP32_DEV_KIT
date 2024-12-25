#define NUM_INPUTS 3
#define NUM_OUTPUTS 2

// Pin Definitions for Dial 1
#define PIN_A1 14
#define PIN_B1 25

// Pin Definitions for Dial 2
#define PIN_A2 27
#define PIN_B2 33

// Pin Definitions for Dial 3
#define PIN_A3 26
#define PIN_B3 32

// Pulse Counters
volatile int pulseCount1 = 0;
volatile int pulseCount2 = 0;
volatile int pulseCount3 = 0;

// Last States
volatile int lastStateA1 = LOW;
volatile int lastStateA2 = LOW;
volatile int lastStateA3 = LOW;

// Interrupt Handlers
void IRAM_ATTR handleInterruptA1() {
    int stateA = digitalRead(PIN_A1);
    int stateB = digitalRead(PIN_B1);

    if (stateA != lastStateA1) {
        if (stateA != stateB) {
            pulseCount1++;
        } else {
            pulseCount1--;
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
        } else {
            pulseCount2--;
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
        } else {
            pulseCount3--;
        }
    }

    lastStateA3 = stateA;
}

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
    // Print pulse counts for each dial
    Serial.print("Dial 1 Count: ");
    Serial.println(pulseCount1);

    Serial.print("Dial 2 Count: ");
    Serial.println(pulseCount2);

    Serial.print("Dial 3 Count: ");
    Serial.println(pulseCount3);

    Serial.println("--------------------------");
    delay(500);
}
