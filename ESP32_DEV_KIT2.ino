//#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <Update.h>
#include <ESPAsyncWebServer.h>
#include <Preferences.h>

Preferences preferences;
WiFiClient espClient;
PubSubClient client(espClient);

String clientId = "";
bool allServicesActive = false;

String storedVersion; //"1.3.4d";  // Default to "0.0.0" if no version is stord
String ssid;   // Replace with your WiFi SSID
String password;     // Replace with your WiFi password

const char* mqttServer = "192.168.0.129"; // Replace with your MQTT broker IP
const int mqttPort = 1883;
const char* mqttUserName = "pro1polaris";
const char* mqttPassword = "CVr819P*!";

const char* githubUser = "consciousvisionaries";
const char* githubRepo = "ESP32_DEV_KIT";
const char* firmwareFile = "ESP32_DEV_KIT2.ino.esp32.bin";
const char* branch = "ESPDEVKIT_8-OUTPUT-DRIVER"; // Branch where the firmware file is located
const char* topicData = "/christmaslights";
const int NUM_OUTPUTS = 8;

// Define the pins
const int ledPin = 2; // GPIO2 is commonly used for the onboard LED on ESP32 boards
int outputPins[] = {12, 14, 27, 26, 33, 32, 5, 18};  // Define the GPIO pins for the 8 outputs
bool outputStates[NUM_OUTPUTS] = {false, false, false, false, false, false, false, false};

// Pattern variables
String currentPattern = "chase";  // Default pattern
unsigned long patternLastTime = 0;
unsigned long lastUpdateTime = 0;
unsigned long blinkInterval = 500; 
int chaseIndex = 0;
int reverseChaseIndex = NUM_OUTPUTS - 1;
static unsigned long lastMillis = 0;
String audioPinStateText;

// Example output states
int waveIndex = 0;  // Declare waveIndex (or change to chaseIndex if that was the intention)

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
