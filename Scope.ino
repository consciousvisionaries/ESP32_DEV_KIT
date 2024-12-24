#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <arduinoFFT.h>
#include <Wire.h>

// OLED Display Configurations
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Constants for FFT and signal processing
#define SAMPLES 128
#define SAMPLING_FREQ 6000
#define SIGNAL_PIN 35
#define LED_PIN_LOW 2 // Define the pin connected to the low beat LED

#define PEAK_THRESHOLD 700

// FFT Variables
double vReal[SAMPLES];
double vImag[SAMPLES];
arduinoFFT FFT = arduinoFFT();

// Timing variables for LED pulses
unsigned long lastPulseTime[3] = {0, 0, 0};
const unsigned long pulseDuration = 150;

// Function prototypes
void captureSignal();
void detectBeats();
void updateLEDs();
void updateOLED();
void initializeOscilloscope();

void oscilliscopeSetUp() {
    initializeOscilloscope();
}

void oscilliscopeLoop() {
    captureSignal();
    FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);
    FFT.ComplexToMagnitude(vReal, vImag, SAMPLES); // Fix: Pass vReal and vImag correctly
    detectBeats();
    updateLEDs();
    updateOLED();
}

void initializeOscilloscope() {
    Serial.begin(115200);
    Wire.begin(21, 22); // Initialize I2C communication on ESP32 pins

    if (!display.begin(0x3C, OLED_RESET)) {
        Serial.println(F("SSD1306 allocation failed"));
        while (true); // Halt if OLED initialization fails
    }
    display.clearDisplay();
    display.display();

    pinMode(SIGNAL_PIN, INPUT);
    pinMode(LED_PIN_LOW, OUTPUT);
   

    Serial.println("Oscilloscope initialized.");
}

void captureSignal() {
    for (int i = 0; i < SAMPLES; i++) {
        vReal[i] = analogRead(SIGNAL_PIN); // Read signal data
        vImag[i] = 0; // Imaginary part is set to zero for real input
        delayMicroseconds(1000000 / SAMPLING_FREQ); // Ensure sampling frequency
    }
}

void detectBeats() {
    // Frequency ranges for each beat type
    int lowStart = 1, lowEnd = 10;
    int midStart = 30, midEnd = 40;
    int highStart = 50, highEnd = 63;

    beatStatus[0] = beatStatus[1] = beatStatus[2] = false;

    for (int i = lowStart; i <= lowEnd; i++) {
        if (vReal[i] > PEAK_THRESHOLD) beatStatus[0] = true;
    }
    for (int i = midStart; i <= midEnd; i++) {
        if (vReal[i] > PEAK_THRESHOLD) beatStatus[1] = true;
    }
    for (int i = highStart; i <= highEnd; i++) {
        if (vReal[i] > PEAK_THRESHOLD) beatStatus[2] = true;
    }
}

void updateLEDs() {
    unsigned long currentMillis = millis();

    // Update low beat LED
    if (beatStatus[0]) {
        digitalWrite(LED_PIN_LOW, HIGH);
        lastPulseTime[0] = currentMillis;
    } else if (currentMillis - lastPulseTime[0] >= pulseDuration) {
        digitalWrite(LED_PIN_LOW, LOW);
    }

   
}

void updateOLED() {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    display.setCursor(0, 0);
    display.print("Low Beat: ");
    display.println(beatStatus[0] ? "YES" : "NO");

    display.setCursor(0, 10);
    display.print("Mid Beat: ");
    display.println(beatStatus[1] ? "YES" : "NO");

    display.setCursor(0, 20);
    display.print("High Beat: ");
    display.println(beatStatus[2] ? "YES" : "NO");

    display.display();
}
