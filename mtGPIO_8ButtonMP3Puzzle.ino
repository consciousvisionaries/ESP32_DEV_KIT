
#define DEBOUNCE_DELAY 500  // Debounce delay in milliseconds

unsigned long lastDebounceTime[9] = {0};  // Last debounce time for each button
volatile int lastState[9] = {HIGH};

// Game logic
int solutionWin[] = {1, 2, 3, 4, 5, 6, 7, 8};  // Correct button order
int solutionIndex = 0;
bool gameActive = false;  // Indicates if the game is active
bool solutionFound = false;

// Batch execution variables
unsigned long batchStartTime1 = 0;
unsigned long batchStartTime2 = 0;
bool batch1Running = false;
bool batch2Running = false;

// Pin definitions
//#define PIN_B1 14
//#define PIN_B2 27
//#define PIN_B3 26
//#define PIN_B4 33
//#define PIN_B5 0
//#define PIN_B6 4
//#define PIN_B7 5
//#define PIN_B8 19
//#define PIN_B9 18  // Start button

//#define PIN_OUTPUT1 22
//#define PIN_OUTPUT2 23

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

void loopGPIO() {
    checkButtons();       // Monitor button presses
    handleBatches(); // Handle batch executions
}


// Debounce logic for button presses
void checkButtons() {
    if (gameActive) {
  
    for (int i = 0; i < 9; i++) {
        int currentState = digitalRead(buttonPins[i]);
        unsigned long currentTime = millis();

        // Check for state change and debounce
        if (currentState != lastState[i] && currentTime - lastDebounceTime[i] > DEBOUNCE_DELAY) {
            lastDebounceTime[i] = currentTime;
            lastState[i] = currentState;

            if (currentState == LOW) {  // Button pressed
                Serial.print("Button ");
                Serial.print(i + 1);
                Serial.println(" pressed!");
                
                if (i == 8) {  // Start button (PIN_B9)
                    startGame();
                } else {
                    handleGameInput(i + 1);
                }
            }
        }
    }
    }
}

// Handle button input during the game
void handleGameInput(int buttonIndex) {

    unsigned long currentTime = millis();

        playMP3(buttonIndex);  // Play corresponding song
        
        // Check if the correct button was pressed
        if (solutionWin[solutionIndex] == buttonIndex) {
            solutionIndex++;
            Serial.println("Correct button!");

            if (solutionIndex == 8) {  // Sequence complete
                Serial.println("Solution correct.... begin Win Game");
                winGame();
            }
        } else {
            Serial.println("Incorrect button! Try again.");
            solutionIndex = 0;
        }
   
}


// Start the game and play the starting song
void startGame() {
  
        Serial.println("Preparing game start...");
        playMP3(9);
        gameActive = true;
        solutionIndex = 0;
        solutionFound = false;
        Serial.println("Game started! Press buttons in the correct order.");
    
}

// Reset the game
void resetGame() {
    solutionFound = false;
    solutionIndex = 0;
    Serial.println("Game reset.");
}

// Handle winning the game
void winGame() {
    
    executeGPIOBatch1();
}

// Execute batch operation 1
void executeGPIOBatch1() {
    if (!batch1Running) {
        Serial.println("Executing batch 1...");
        delay(1000);
        gameActive = false;
        solutionFound = true;
        playMP3(10);  // Play success song (track 10)
        Serial.println("Congratulations! You've won!");
        delay(3000);
        batchStartTime1 = millis();
        digitalWrite(PIN_OUTPUT1, LOW);  // Activate relay 1
        batch1Running = true;
    }
}

// Execute batch operation 2
void executeGPIOBatch2() {
    if (!batch2Running) {
        batchStartTime2 = millis();
        digitalWrite(PIN_OUTPUT2, LOW);  // Activate relay 2
        batch2Running = true;
        Serial.println("Executing batch 2...");
    }
}

// Handle batch execution timings
void handleBatches() {
    if (batch1Running && millis() - batchStartTime1 >= 5000) {
        digitalWrite(PIN_OUTPUT1, HIGH);  // Deactivate relay 1
        batch1Running = false;
        
        playMP3(9);
        Serial.println("Batch 1 complete.");
    }

    if (batch2Running && millis() - batchStartTime2 >= 5000) {
        digitalWrite(PIN_OUTPUT2, HIGH);  // Deactivate relay 2
        batch2Running = false;
        Serial.println("Batch 2 complete.");
    }
}

void executeMQTTBatch(String mqttMessage) { //start button
    if (!gameActive) {
        if (mqttMessage == "Start Edna's...") {
           startGame();
           Serial.println("Executed MQTT...START");
        } else if (mqttMessage == "Stage 1 Button 2 Pressed") {
          resetGame();
        } else {
          Serial.println("Actvity not recognized!");
        }
    } else {
      Serial.println("Game already started!");
    }
}
