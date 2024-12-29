// INPUT Buttons

//#define PIN_B1 14
//#define PIN_B2 27
//#define PIN_B3 26
//#define PIN_B4 33
//#define PIN_B5 0
//#define PIN_B6 4
//#define PIN_B7 5
//#define PIN_B8 19

//#define PIN_B9 18  // Start button

// DIGITAL OUTPUTS

//#define PIN_OUTPUT1 22
//#define PIN_OUTPUT2 23

// Debounce variables
unsigned long lastDebounceTime[9] = {0};
const unsigned long debounceDelay1 = 50;
volatile int lastState[9] = {HIGH};

// Game logic
int solutionWin[] = {1, 2, 3, 4, 5, 6, 7, 8};  // Correct button order
int solutionIndex = 0;
bool gameActive = false;  // Indicates if the game is active
bool solutionFound = false;

// Batch execution variables
unsigned long batchStartTime = 0;
bool batch1Running = false;
bool batch2Running = false;
bool batch3Running = false;


// Pin mapping for buttons
const int buttonPins[9] = {PIN_B1, PIN_B2, PIN_B3, PIN_B4, PIN_B5, PIN_B6, PIN_B7, PIN_B8, PIN_B9};

// Function to initialize GPIO pins
void setupGPIO() {
    // Initialize buttons as INPUT_PULLUP
    for (int i = 0; i < 9; i++) {
        pinMode(buttonPins[i], INPUT_PULLUP);
    }

    // Initialize outputs
    pinMode(PIN_OUTPUT1, OUTPUT);
    digitalWrite(PIN_OUTPUT1, HIGH);
    pinMode(PIN_OUTPUT2, OUTPUT);
    digitalWrite(PIN_OUTPUT2, HIGH);

    Serial.println("Buttons and Outputs Initialized!");
}

// Function to handle GPIO operations in the main loop
void loopGPIO() {
  
    checkButtons();       // Monitor button presses
    handleBatches();      // Handle batch executions
}

// Check button states with debouncing
void checkButtons() {
  
    for (int i = 0; i < 9; i++) {
        int currentState = digitalRead(buttonPins[i]);

        if (currentState != lastState[i]) {
            lastDebounceTime[i] = millis();
        }

        if ( currentState != lastState[i]) {
            lastState[i] = currentState;

            if (currentState == LOW) {  // Button pressed
                Serial.print("Button ");
                Serial.print(i + 1);
                Serial.println(" pressed!");

                if (i == 8) {  // Start button (PIN_B9)
                    startGame();
                } else if (gameActive && !solutionFound) {
                    handleGameInput(i + 1); 
                }
            }
        }
    }
}

// Start the game and play the starting song
void startGame() {
    if (!gameActive) {
        gameActive = true;
        solutionIndex = 0;
        solutionFound = false;
       
        // Play starting song (track 10)
        Serial.println("Game started! Press buttons in the correct order.");
    } else {
        Serial.println("Game is already started! Stop game to and press start to begin again.");
    }
}

// Handle button input during the game
void handleGameInput(int buttonIndex) {
    
    //playMP3(buttonIndex, 30);  // Play corresponding song
    
    if (solutionWin[solutionIndex] == buttonIndex) {
        solutionIndex++;
        Serial.println("Correct button!");

        if (solutionIndex == 8) {  // Sequence complete
            solutionFound = true;
            winGame();
        }
    } else {
        playMP3(11, 30);  // Play failure sound (track 11)
        Serial.println("Incorrect button! Try again.");
       
    }
}

// Reset the game
void resetGame() {
    gameActive = false;
    solutionIndex = 0;
    Serial.println("Game reset.");
}

// Handle winning the game
void winGame() {
    gameActive = false;
    playMP3(12,30);  // Play winning song (track 12)
    executeGPIOBatch1();
    Serial.println("Congratulations! You've won!");
}

// Execute batch operation 1
void executeGPIOBatch1() {
    if (!batch1Running) {
        batchStartTime = millis();
        digitalWrite(PIN_OUTPUT1, LOW);  // Activate relay 1
        batch1Running = true;
        Serial.println("Executing batch 1...");
    }
}

// Execute batch operation 2
void executeGPIOBatch2() {
    if (!batch2Running) {
        batchStartTime = millis();
        digitalWrite(PIN_OUTPUT2, LOW);  // Activate relay 2
        batch2Running = true;
        Serial.println("Executing batch 2...");
    }
}

// Execute batch operation 2
void executeMQTTBatch(String mqttMessage) { //start button
    if (!gameActive) {
        if (mqttMessage == "Start Edna's...") {
           delay(500);
           startGame();
           delay(500);
           Serial.println("Executed MQTT...START");
        } else {
          Serial.println("Actvity not recognized!");
        }
    } else {
      Serial.println("Game already started!");
    }
}

// Handle batch execution timings
void handleBatches() {
    if (batch1Running && millis() - batchStartTime >= 5000) {
        digitalWrite(PIN_OUTPUT1, HIGH);  // Deactivate relay 1
        batch1Running = false;
        Serial.println("Batch 1 complete.");
    }

    if (batch2Running && millis() - batchStartTime >= 5000) {
        digitalWrite(PIN_OUTPUT2, HIGH);  // Deactivate relay 2
        batch2Running = false;
        Serial.println("Batch 2 complete.");
    }
}
