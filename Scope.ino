#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128  // OLED width, in pixels
#define SCREEN_HEIGHT 64  // OLED height, in pixels
#define OLED_RESET -1     // Reset pin for OLED

#define SAMPLING_FREQ 6000  // Sampling frequency in Hz (3kHz)
#define SAMPLE_SIZE 128     // Number of samples per loop
#define SIGNAL_PIN 35       // Pin for analog signal input
#define PEAK_THRESHOLD 700  // Threshold for detecting peaks in the signal
#define LED_PIN 2           // Pin for the LED

#define SSD1306_I2C_ADDRESS 0x3C  // I2C address for the SSD1306 OLED
#define SDA_PIN 21   // GPIO 21 for SDA
#define SCL_PIN 22   // GPIO 22 for SCL

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

unsigned long samplingPeriod = 1000000 / SAMPLING_FREQ;  // Sampling period in microseconds
unsigned long lastPeakTime = 0;  // Time of the last detected peak
float totalBPM = 0;  // Total BPM for averaging
int bpmCount = 0;    // Count of valid BPM readings
float avgBPM = 0;    // Average BPM over time
unsigned long lastPulseTime = 0;  // Last time the LED and dot were pulsed
bool pulseVisible = false;  // Flag for pulse visibility
unsigned long pulseDuration = 100;  // Default pulse duration (ms)


void oscilliscopeSetUp() {
  // Initialize OLED display
    if (!display.begin(SSD1306_SWITCHCAPVCC, SSD1306_I2C_ADDRESS)) {
        Serial.println(F("SSD1306 allocation failed"));
        for (;;);  // Halt if allocation fails
    }

    display.clearDisplay();
    Wire.begin(SDA_PIN, SCL_PIN);
}

void oscilliscopeLoop() {
   int signal[SAMPLE_SIZE];
    unsigned long startMicros = micros();

    // Read the signal into the buffer
    for (int i = 0; i < SAMPLE_SIZE; i++) {
        while (micros() - startMicros < samplingPeriod);  // Timing control
        startMicros += samplingPeriod;
        signal[i] = analogRead(SIGNAL_PIN);
    }

    // Detect peaks and calculate BPM
    detectPeaksAndCalculateBPM(signal);

    // Update OLED display
    display.clearDisplay();
    displayAvgBPM();
    displayWaveform(signal);
    displayPulseOnEvent();  // Show or hide the pulsing dot
    display.display();

    // Pulse the LED based on peak or BPM event
    pulseLED();
}

void detectPeaksAndCalculateBPM(int signal[]) {
    unsigned long currentMillis = millis();

    // Look for peaks in the signal
    for (int i = 1; i < SAMPLE_SIZE - 1; i++) {
        if (signal[i] > PEAK_THRESHOLD && signal[i] > signal[i - 1] && signal[i] > signal[i + 1]) {
            unsigned long peakTime = currentMillis;

            // If it's been enough time since the last peak, calculate BPM
            if (peakTime - lastPeakTime > 200) {  // 200ms minimum between peaks
                unsigned long interval = peakTime - lastPeakTime;

                if (lastPeakTime > 0 && interval > 0) {
                    // Calculate BPM based on the interval between peaks
                    float bpm = 60000.0 / interval;

                    // Avoid unrealistic BPM values (outliers)
                    if (bpm > 40 && bpm < 200) {
                        // Smooth the BPM by averaging over time
                        totalBPM += bpm;
                        bpmCount++;

                        // Update the average BPM
                        avgBPM = totalBPM / bpmCount;
                    }
                }

                lastPeakTime = peakTime;  // Update the time of the last peak

                // Trigger pulse immediately on peak detection or BPM calculation
                pulseVisible = true;
                lastPulseTime = currentMillis;  // Start the pulse immediately
            }
        }
    }

    // Adjust the pulse duration based on the BPM
    // The faster the BPM, the shorter the pulse duration
    if (avgBPM > 0) {
        pulseDuration = max((unsigned long)50, (unsigned long)(60000 / avgBPM));  // Ensure minimum pulse duration of 50ms
    }
}

void displayAvgBPM() {
    display.setCursor(0, 0);
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);
    display.print("Avg BPM: ");
    display.print(avgBPM, 1);  // Display average BPM with 1 decimal point
}

void displayWaveform(int signal[]) {
    // Draw waveform
    for (int i = 0; i < SAMPLE_SIZE - 1; i++) {
        int y1 = map(signal[i], 0, 1023, 0, SCREEN_HEIGHT);
        int y2 = map(signal[i + 1], 0, 1023, 0, SCREEN_HEIGHT);
        display.drawLine(i, SCREEN_HEIGHT - y1, i + 1, SCREEN_HEIGHT - y2, SSD1306_WHITE);
    }
}

void displayPulseOnEvent() {
    unsigned long currentMillis = millis();

    // Keep the dot pulsing for the calculated duration after the event, then turn it off
    if (pulseVisible && currentMillis - lastPulseTime < pulseDuration) {
        // Draw the pulsing dot on top-right corner
        display.fillCircle(SCREEN_WIDTH - 6, 2, 2, SSD1306_WHITE);  
    } else {
        pulseVisible = false;  // Turn off the dot after the calculated pulse duration
    }
}

void pulseLED() {
    unsigned long currentMillis = millis();

    // Pulse the LED based on peak or BPM event timing
    if (pulseVisible && currentMillis - lastPulseTime < pulseDuration) {  // Pulse during the same period
        digitalWrite(LED_PIN, HIGH);  // Turn on LED
    } else {
        digitalWrite(LED_PIN, LOW);   // Turn off LED
    }
}
