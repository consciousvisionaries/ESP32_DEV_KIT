#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_PN532.h>

#define SDA_PIN A4          // SDA pin for NFC module
#define SCL_PIN A5          // SCL pin for NFC module
#define SCREEN_WIDTH 128    // OLED width, in pixels
#define SCREEN_HEIGHT 32    // OLED height, in pixels
#define OLED_RESET -1       // Reset pin (or -1 if not used)
#define I2C_ADDRESS 0x3C    // OLED I2C address

Adafruit_PN532 nfc(SDA_PIN, SCL_PIN);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Define card messages and UIDs
const String CARD_MESSAGES[] = {
    "The Fool", "The Hanged Man", "The Hermit", "The Star", "The Sun",
    "Strength", "The High Priestess", "The Wheel of Fortune", "The Moon", "The World"
};

const int OUTPUT_PINS[] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11}; // Output pins for each card

const uint8_t CARD_UIDS[][7] = {
    { 0x4, 0x6D, 0xE7, 0x52, 0x22, 0x4B, 0x80 }, // The Fool
    { 0x4, 0x3A, 0xE4, 0x52, 0x22, 0x4B, 0x80 }, // The Hanged Man
    { 0x4, 0x54, 0xEA, 0x52, 0x22, 0x4B, 0x80 }, // The Hermit
    { 0x4, 0x62, 0xE6, 0x52, 0x22, 0x4B, 0x80 }, // The Star
    { 0x4, 0x5D, 0xE7, 0x52, 0x22, 0x4B, 0x80 }, // The Sun
    { 0x4, 0x65, 0xEB, 0x52, 0x22, 0x4B, 0x80 }, // Strength
    { 0x4, 0x96, 0xE7, 0x52, 0x22, 0x4B, 0x80 }, // The High Priestess
    { 0x4, 0x7E, 0xE6, 0x52, 0x22, 0x4B, 0x80 }, // The Wheel of Fortune
    { 0x4, 0xDD, 0xE4, 0x52, 0x22, 0x4B, 0x80 }, // The Moon
    { 0x4, 0x43, 0xEA, 0x52, 0x22, 0x4B, 0x80 }  // The World
};

enum State { NO_CARD, CARD_PRESENT };
State currentState = NO_CARD;

int currentCardIndex = -1;
bool eyeActive = true;
unsigned long lastCardTime = 0;

// Add variables to monitor card status
uint8_t lastDetectedUID[7]; // Store last detected UID
bool lastUIDValid = false; // Flag to check if last UID is valid
const unsigned long inactivityThreshold = 1000; // 1 second
unsigned long lastReadCheckTime = 0; // Track last read check
const unsigned long readCheckInterval = 500; // 500 ms
const unsigned long animationDelay = 2000; // 2 seconds delay before animation starts

unsigned long cardRemovedTime = 0; // Time when card was removed
bool animationDelayActive = false; // Flag to indicate if animation delay is active
bool scannedCards[sizeof(CARD_UIDS) / sizeof(CARD_UIDS[0])] = { false };
float pupilX, pupilY;
int pupilSize = 6;
float moveSpeed = 0.5;
int eyebrowY = 3;
int direction = 1;
int eyebrowYChange = 0; // Variable to animate eyebrow position

void setup() {
    Serial.begin(115200);  
    nfc.begin();           

    if (!display.begin(SSD1306_SWITCHCAPVCC, I2C_ADDRESS)) {
        Serial.println(F("SSD1306 allocation failed"));
        for (;;);
    }

    // Initialize output pins
    for (int i = 0; i < 10; i++) {
        pinMode(OUTPUT_PINS[i], OUTPUT);
        digitalWrite(OUTPUT_PINS[i], LOW); // Initialize outputs to LOW
    }

    display.clearDisplay();
    pupilX = SCREEN_WIDTH / 2;
    pupilY = SCREEN_HEIGHT / 2;

    displayCommand("Waiting for card...");
}

void loop() {
    manageNFC();
    updateAnimation();
}

void manageNFC() {
    static unsigned long lastActiveTime = 0; // Variable to track the last active time
    static unsigned long lastDetectedUIDTime = 0; // Track the last time the UID was detected

    if (nfc.inListPassiveTarget()) {
        uint8_t uid[7];
        uint8_t uidLength;

        if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength)) {
            lastDetectedUIDTime = millis(); // Update time when UID is detected

            int newCardIndex = findCardIndex(uid, uidLength);
            if (newCardIndex != -1 && newCardIndex != currentCardIndex) {
                currentCardIndex = newCardIndex;
                currentState = CARD_PRESENT;
                eyeActive = false; // Stop eye animation on card detection
                displayCard(currentCardIndex);
                lastCardTime = millis();
                lastActiveTime = millis(); // Reset last active time

                // Store last detected UID
                memcpy(lastDetectedUID, uid, uidLength);
                lastUIDValid = true; // Mark last UID as valid
                animationDelayActive = false; // Reset animation delay flag
            }
            // Update last read check time
            lastReadCheckTime = millis();
        }
    } else {
        // No card detected, check if we've been inactive
        if (currentState == CARD_PRESENT && (millis() - lastActiveTime > inactivityThreshold)) {
            resetCardState();
            displayCommand("Card removed!");
        }
    }

    // Read check every interval
    if (millis() - lastReadCheckTime >= readCheckInterval) {
        lastReadCheckTime = millis();
        if (currentState == CARD_PRESENT && !nfc.inListPassiveTarget()) {
            // If no card is detected on read check, reset state
            resetCardState();
            displayCommand("Card removed!");
        }
    }

    // Check if the card has been removed
    if (currentState == CARD_PRESENT && !nfc.inListPassiveTarget()) {
        cardRemovedTime = millis(); // Store time when the card was removed
        animationDelayActive = true; // Set the animation delay flag
        resetCardState();
    }

   
}



int findCardIndex(uint8_t* uid, uint8_t uidLength) {
    for (int i = 0; i < sizeof(CARD_UIDS) / sizeof(CARD_UIDS[0]); i++) {
        if (isMatchingUID(uid, uidLength, CARD_UIDS[i], sizeof(CARD_UIDS[i]))) {
          scannedCards[i] = true; // Mark card as scanned

                    // Activate the corresponding output pin
                    digitalWrite(OUTPUT_PINS[i], HIGH);
                    delay(1000); // Keep the output high for 1 second
                    digitalWrite(OUTPUT_PINS[i], LOW);
          
                checkWinCondition(); // Check for win condition
            return i;
        }
    }
    return -1;
}

bool isMatchingUID(uint8_t* uid, uint8_t uidLength, const uint8_t* targetUID, size_t targetLength) {
    if (uidLength != targetLength) return false;
    for (size_t i = 0; i < uidLength; i++) {
        if (uid[i] != targetUID[i]) return false;
    }
    return true;
}

void displayCard(int cardIndex) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 11);
    display.print(CARD_MESSAGES[cardIndex]);
    display.display();
}

void resetCardState() {
    currentCardIndex = -1;
    eyeActive = true;
    currentState = NO_CARD;
    lastUIDValid = false; // Invalidate last UID
}

void displayCommand(const String &command) {
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.clearDisplay(); // Clear the display before printing new command
    display.display();
}

void checkWinCondition() {
    for (int i = 0; i < sizeof(scannedCards) / sizeof(scannedCards[0]); i++) {
        if (!scannedCards[i]) {
            return; // If any card is not scanned, exit
        }
    }
    displayWinMessage();
}

void displayWinMessage() {
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("You Win!");
    display.setTextSize(1);
    display.setCursor(0, 20);
    display.println("All cards scanned!");
    display.display();
}

bool angry = false; // Track if eyebrow is in angry position

void updateAnimation() {
    if (eyeActive) {
        if (millis() - lastCardTime < animationDelay) {
            // Animate eye while the card is present
            pupilX += (direction * moveSpeed);
            if (pupilX >= SCREEN_WIDTH - pupilSize || pupilX <= pupilSize) {
                direction *= -1; // Reverse direction
            }
            displayEyeAnimation();
        } else {
            // When the card is removed, begin animation
            displayEyeAnimation();
            if (animationDelayActive) {
                // Wait for the animation delay to complete
                if (millis() - cardRemovedTime > animationDelay) {
                    // Animation finished, reset animation state
                    animationDelayActive = false;
                }
            }
        }
    }
}

void displayEyeAnimation() {
//    // Adjust eyebrow curvature for angry look occasionally
    if (random(0, 20) < 2) { // Randomly emphasize curvature
        angry = true;
    } else {
        angry = false;
    }

   
    // Adjust pupil position based on direction
    if (direction == -1) {
        pupilX -= 1; // Move left
        if (pupilX <= SCREEN_WIDTH / 2 - 20) {
            direction = 0; // Change to center
        }
    } else if (direction == 1) {
        pupilX += 1; // Move right
        if (pupilX >= SCREEN_WIDTH / 2 + 20) {
            direction = 0; // Change to center
        }
    } else {
        // Randomly choose a new direction to move left or right
        if (random(0, 10) < 3) {
            direction = random(-1, 2); // -1 for left, 1 for right, 0 for center
        }
    }

    // Move pupil back to center smoothly
    if (direction == 0) {
        pupilX += (SCREEN_WIDTH / 2 - pupilX) * moveSpeed;
        if (abs(pupilX - (SCREEN_WIDTH / 2)) < 0.5) {
            pupilX = SCREEN_WIDTH / 2; // Snap to center
        }
    }

    // Draw enlarged pupil
        display.clearDisplay();

    display.fillCircle(pupilX, pupilY, pupilSize, SSD1306_WHITE);
     // Draw the curved eyebrow (top 10 rows only)
    for (int x = 0; x < SCREEN_WIDTH; x++) {
        int y;
        if (angry) {
            // More pronounced curve when angry
            y = eyebrowY + (3 * sin((x / (float)SCREEN_WIDTH) * 3.14) + 2); // Increase curvature
        } else {
            y = eyebrowY + (3 * sin((x / (float)SCREEN_WIDTH) * 3.14)); // Normal curvature
        }

        display.drawPixel(x, y, SSD1306_WHITE); // Draw each point of the curve
    }


    display.display(); // Update the display
}
