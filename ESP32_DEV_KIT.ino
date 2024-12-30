
// esp32
#define MODULE "ESP32WROOM by FREENOVE"

// puzzle details
#define PUZZLE_NAME "ESP32 8GPIO MASTER"
#define DESIGNER_NAME "Appsations, Inc"
#define TECH_NAME "MYSTTECH Creations"

#define NR_TAB "PRESIDENTS BIG MISTAKE"
#define NR_GROUP "Stage 2"
#define NR_TYPE "8_OUTPUT" // 

#define NUM_INPUTS 0
#define NUM_OUTPUTS 8

// wifi
String bup_ssid[] = { "TELUSDE0875_2.4G", "Beyond Belief Entertainment" };
String bup_password[] = { "3X3K22832E", "Gary2019" };

// access point
#define AP_SSID "MystTech 8IO"
#define AP_PASSWORD "MystTech" // 'localhost:1880/admin'

// fastled
#define NUM_LEDS 10 
#define NUM_CHANNELS 0

// github
const char* GITHUB_USER = "consciousvisionaries";
const char* GITHUB_REPO = "ESP32_DEV_KIT";
const char* GITHUB_BIN = "ESP32_DEV_KIT.ino.esp32da.bin"; // esp32 wroom
const char* GITHUB_BRANCH = "ESP32_8GPIO_Master";

// MQTT
// Node-Red User Login (Device Login - Raspberry Pi)
const char* mqttUserName = "pro1polaris";
const char* mqttPassword = "CVr819P*!";

#define MQTT_TOPIC "/president"
#define MQTT_SERVER "192.168.0.129" // Replace with your MQTT broker IP

// pins fastled
#define LED_PIN 12  // Pin for LEDs

// pins uart 2 MP3
#define PIN_MP3_RX 17 
#define PIN_MP3_TX 16 

// pins buttons GPIO
#define PIN_O1 14
#define PIN_O2 27
#define PIN_O3 26
#define PIN_O4 33
#define PIN_O5 0
#define PIN_O6 4
#define PIN_O7 5
#define PIN_O8 19
