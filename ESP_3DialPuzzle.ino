const char* GITHUB_USER = "consciousvisionaries";
const char* GITHUB_REPO = "ESP32_DEV_KIT";
const char* GITHUB_BIN = "ESP32_DEV_KIT2.ino.esp32.bin";
const char* GITHUB_BRANCH = "ESPDEVKIT_3DialLEDStrip";

String storedVersion;
String ssid;
String password;

#define MQTT_TOPIC "/lost"
#define MQTT_SERVER "192.168.0.129" // Replace with your MQTT broker IP
#define MQTT_PORT 1883

// Node-Red User Login (Device Login - Raspberry Pi)
const char* mqttUserName = "pro1polaris";
const char* mqttPassword = "CVr819P*!";

// Access Point
#define AP_SSID "BB LOST ESP"
#define AP_PASSWORD "Lost"

#define PUZZLE_NAME "LOST 3 Dial Puzzle"
#define DESIGNER_NAME "Beyond Belief"
#define TECH_NAME "MYSTTECH Creations"

#define NR_TAB "LOST"
#define NR_GROUP "Stage 1"

//GPIO
// Pin Definitions for Rotary Encoders
#define NUM_INPUTS 3
#define NUM_OUTPUTS 2

// Dial 1
#define PIN_A1 14
#define PIN_B1 25

// Dial 2
#define PIN_A2 27
#define PIN_B2 33

// Dial 3
#define PIN_A3 26
#define PIN_B3 32

// FASTLED

#define LED_PIN     12  // Pin for LEDs
#define NUM_LEDS    30  // Total number of LEDs (10 per dial)

// Pulse Counters
volatile int pulseCount1 = 0;
volatile int pulseCount2 = 0;
volatile int pulseCount3 = 0;

// Last States
volatile int lastStateA1 = LOW;
volatile int lastStateA2 = LOW;
volatile int lastStateA3 = LOW;
