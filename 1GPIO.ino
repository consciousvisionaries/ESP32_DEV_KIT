#include <FastLED.h>

#define DEBOUNCE_DELAY 500  // Debounce delay in milliseconds
#define FLED_PIN1 12

const int outputPins[12] = {23, 22, 21, 19, 18, 13, 32, 33, 26, 27, 14, 2};
const int inputPins[8] = {4, 5, 14, 27, 26, 25, 33, 32};
      int inputDigitalPinState[8] = {HIGH,HIGH,HIGH,HIGH,HIGH,HIGH,HIGH,HIGH};
const int outputPins_initState[12] = {HIGH,HIGH,HIGH,HIGH,HIGH,HIGH,HIGH,HIGH,HIGH,HIGH,HIGH,HIGH};
const int analogInputPinsA[3] = {14, 27, 26};
const int analogInputPinsB[3] = {25, 33, 32};
const int fastLEDOutputPins[1] = {FLED_PIN1};
const int RXTX_Pins[2] = {17, 16};



portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

String getInputStateGPIO(int inputPin) {
  int state = digitalRead(inputPin);
  return (state == HIGH) ? "high" : "low";  // Return "high" or "low"
}


void toggleOutputStateGPIO(int outputNumber) {
  if (outputNumber >= 0 && outputNumber < 8) {
    int pin = outputPins[outputNumber];
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

// Interrupt service routines for each analog input pair
void IRAM_ATTR handleInterruptA() {
  int stateA = digitalRead(analogInputPinsA[0]);
  int stateB = digitalRead(analogInputPinsB[0]);

  portENTER_CRITICAL(&mux);
  if (stateA != lastStateAnalogInputs[0]) {
    pulseCount[0] += (stateA != stateB) ? 1 : -1;
    pulseUpdated[0] = true;
  }
  lastStateAnalogInputs[0] = stateA;
  portEXIT_CRITICAL(&mux);
}

void IRAM_ATTR handleInterruptB() {
  int stateA = digitalRead(analogInputPinsA[1]);
  int stateB = digitalRead(analogInputPinsB[1]);

  portENTER_CRITICAL(&mux);
  if (stateA != lastStateAnalogInputs[1]) {
    pulseCount[1] += (stateA != stateB) ? 1 : -1;
    pulseUpdated[1] = true;
  }
  lastStateAnalogInputs[1] = stateA;
  portEXIT_CRITICAL(&mux);
}

void IRAM_ATTR handleInterruptC() {
  int stateA = digitalRead(analogInputPinsA[2]);
  int stateB = digitalRead(analogInputPinsB[2]);

  portENTER_CRITICAL(&mux);
  if (stateA != lastStateAnalogInputs[2]) {
    pulseCount[2] += (stateA != stateB) ? 1 : -1;
    pulseUpdated[2] = true;
  }
  lastStateAnalogInputs[2] = stateA;
  portEXIT_CRITICAL(&mux);
}

volatile bool restartFlag = false;
volatile bool gameStartFlag = false;

// Interrupt service routine
void IRAM_ATTR handleInterrupt1() {
    for (int i = 0; i < NUM_DIGITAL_INPUTS; i++) {
        inputDigitalPinState[i] = digitalRead(inputPins[i]);

        if (i == 0 && inputDigitalPinState[i] == LOW) {
            restartFlag = true; // Set a flag for restart
        } else if (i == 1 && inputDigitalPinState[i] == LOW) {
            gameStartFlag = true; // Set a flag to start the game
        }
    }
}


void setupGPIO() {
  Serial.begin(115200);

  // Array to track used pins
  bool usedPins[50] = {false}; // Assuming a maximum of 40 GPIO pins on the microcontroller

  // Helper function to check and mark pins
  auto usePin = [&](int pin) {
    Serial.print(pin);
    Serial.print(" - ");
    if (usedPins[pin]) {
      Serial.print("Error: Pin conflict detected on pin ");
      Serial.println(pin);
      while (true); // Halt execution
    }
    usedPins[pin] = true;
  };

  // Initialize output pins
  for (int i = 0; i < NUM_DIGITAL_OUTPUTS; i++) {
    usePin(outputPins[i]); // Check for conflicts
    pinMode(outputPins[i], OUTPUT);
    digitalWrite(outputPins[i], outputPins_initState[i] ? HIGH : LOW);
    Serial.println("Output " + String(i) + " = " + String(outputPins_initState[i]));
  }
  Serial.println(String(NUM_DIGITAL_INPUTS) + " Outputs Initialized.");

  // Initialize input pins
  for (int i = 0; i < NUM_DIGITAL_OUTPUTS; i++) {
    usePin(inputPins[i]); // Check for conflicts
    pinMode(inputPins[i], INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(inputPins[i]), handleInterrupt1, CHANGE);
  }
  Serial.println(String(NUM_DIGITAL_INPUTS) + " Inputs Initialized.");

  for (int i = 0; i < NUM_ANALOG_INPUTPAIRS; i++) {
    Serial.print("Initializing analog input pair ");
    Serial.println(i);
    usePin(analogInputPinsA[i]); // Check for conflicts
    usePin(analogInputPinsB[i]); // Check for conflicts
    pinMode(analogInputPinsA[i], INPUT_PULLUP);
    pinMode(analogInputPinsB[i], INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(analogInputPinsA[i]), 
      (i == 0 ? handleInterruptA : (i == 1 ? handleInterruptB : handleInterruptC)), CHANGE);
  }
  Serial.println(String(NUM_ANALOG_INPUTPAIRS) + " Analog Input Pairs Initialized.");

  for (int i = 0; i < NUM_FLED_OUTPUTS; i++) {
    Serial.print("Initializing FLED addressable outputs");
    usePin(FLED_PIN1);
    setupFASTLED_GPIO();
  }
}


void loopGPIO() {
  
  unsigned long currentTime = millis();

  for (int i = 0; i < 3; i++) {
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

  if (restartFlag) {
        restartFlag = false; // Clear the flag
        delay(5000); // Safe to use here
        ESP.restart();
    }

    if (gameStartFlag) {
        gameStartFlag = false; // Clear the flag
        generateFUNCRandomSolution(); // Perform game setup
        updateFLED_clearAll();
        for (int i = 0; i < NUM_FLED_CHANNELS; i++) {
            ledCount[i] = 0;
        }
        updateFLED_show();
        funcGameStart = true;
        Serial.println("Start game....");
    }

  esp_task_wdt_reset(); // Feed the watchdog
}

// Batch operation functions
void executeGPIOBatchPin1() {
  Serial.println("Solution stable for 5 seconds. Executing batch operation...");
  digitalWrite(outputPins[0], LOW);
  delayESPTask(5000);
  for (int i = 0; i < 3; i++) {
    pulseCount[i] = 0;
  }
  funcGameStart = false;
  digitalWrite(outputPins[0], HIGH);
}

void executeGPIOBatchPin2() {
  digitalWrite(outputPins[1], LOW);
  delayESPTask(5000);
  digitalWrite(outputPins[1], HIGH);
}

void sendGPIO_MQTTPayload() {
  
  DynamicJsonDocument doc1(MQTT_MAX_PACKET_SIZE);
    doc1["mac"] = WiFi.macAddress();
    doc1["puzzleName"] = PUZZLE_NAME;
    doc1["leds"] = NUM_FLED_ADDLEDS;
    doc1["chan"] = NUM_FLED_CHANNELS;
    doc1["inputs"] = NUM_DIGITAL_INPUTS;
    doc1["outputs"] = NUM_DIGITAL_OUTPUTS;
    doc1["analogspairs"] = NUM_ANALOG_INPUTPAIRS;

    // Convert the doc to a JSON string and publish
  String jsonPayload;
  serializeJson(doc1, jsonPayload);

  sendJsonDocUpdateMQTTPayload(jsonPayload);
}
