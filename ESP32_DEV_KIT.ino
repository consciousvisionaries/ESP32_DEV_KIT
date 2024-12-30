
// esp32
#define MODULE "ESP32WROOM by FREENOVE"

// puzzle details
#define PUZZLE_NAME "EDNA 8 Button MP3 Puzzle"
#define DESIGNER_NAME "Beyond Belief"
#define TECH_NAME "MYSTTECH Creations"

#define NR_TAB "EDNA"
#define NR_GROUP "Stage 1"
#define NR_TYPE "8_BUTTONMP3"

#define NUM_INPUTS 8
#define NUM_OUTPUTS 2

// wifi
String bup_ssid[] = { "TELUSDE0875_2.4G", "Beyond Belief Entertainment" };
String bup_password[] = { "3X3K22832E", "Gary2019" };

// access point
#define AP_SSID "BB EDNA ESP"
#define AP_PASSWORD "Edna" // 'localhost:1880/admin'

// fastled
#define NUM_LEDS 10 
#define NUM_CHANNELS 1

// github
const char* GITHUB_USER = "consciousvisionaries";
const char* GITHUB_REPO = "ESP32_DEV_KIT";
const char* GITHUB_BIN = "ESP32_DEV_KIT.ino.esp32da.bin"; // esp32 wroom
const char* GITHUB_BRANCH = "ESP32DEVKIT_8ButtonMP3Puzzle";

// MQTT
#define MQTT_TOPIC "/edna"
#define MQTT_SERVER "192.168.0.129" // Replace with your MQTT broker IP

// pins fastled
#define LED_PIN 12  // Pin for LEDs

// pins uart 2 MP3
#define PIN_MP3_RX 17 
#define PIN_MP3_TX 16 

// pins buttons GPIO
#define PIN_B1 14
#define PIN_B2 27
#define PIN_B3 26
#define PIN_B4 33
#define PIN_B5 0
#define PIN_B6 4
#define PIN_B7 5
#define PIN_B8 19

#define PIN_B9 18  // Start button

// pins outputs
#define PIN_OUTPUT1 22
#define PIN_OUTPUT2 23
