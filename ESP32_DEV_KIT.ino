
// esp32
#define MODULE "ESP32WROOM by FREENOVE"

// puzzle details
#define PUZZLE_NAME "President's Big Mistake"
#define DESIGNER_NAME "Beyond Belief Entertainment"
#define TECH_NAME "MYSTTECH Creations"
#define MYSTTECH_MODEL "MYST8 GPIO MASTER"

#define NR_TAB "PRESIDENTS BIG MISTAKE"
#define NR_GROUP "Stage 1"
#define NR_TYPE "8_OUTPUT" 

// digitalGPIO

#define NUM_INPUTS 8
String bodyInputTitle = "FLEXXMAX2 Input STATUS";
String bodyOutputTitle = "OVERRIDE";
String buttonsHTMLTitle = "Game Master INPUT Control";


String inputNames[NUM_INPUTS] = {
  "Override Levers", "Override Dials", "Override Doors", "Override Patch Panel", 
  "Input 5", "Input 6", "Input 7", "Input 8"  // Add more as needed
};


// outputs
#define NUM_OUTPUTS 8
#define NUM_OUTPUT_ROWS 2

String outputNames[NUM_OUTPUTS] = {
  "Override Levers", "Override Dials", "Override Doors", "Override Patch Panel", 
  "Output 5", "Output 6", "Output 7", "Output 8"  // Add more as needed
};


// fastled
#define NUM_LEDS 10 
#define NUM_CHANNELS 0

// wifi
String bup_ssid[] = { "TELUSDE0875_2.4G", "Beyond Belief Entertainment" };
String bup_password[] = { "3X3K22832E", "Gary2019" };

// access point
#define AP_SSID "MystTech 8IO"
#define AP_PASSWORD "MystTech" // 'localhost:1880/admin'

// github
const char* GITHUB_USER = "consciousvisionaries";
const char* GITHUB_REPO = "ESP32_DEV_KIT";
const char* GITHUB_BIN = "ESP32_DEV_KIT.ino.esp32da.bin"; // esp32 wroom
const char* GITHUB_BRANCH = "MYST8_GPIO_Master";

// MQTT
// Node-Red User Login (Device Login - Raspberry Pi)
const char* mqttUserName = "pro1polaris";
const char* mqttPassword = "CVr819P*!";

#define MQTT_TOPIC "/myst/er/bb/president"      // Topic for publish/subscribe
#define MQTT_SERVER "192.168.0.129" // Replace with your MQTT broker IP
#define MQTT_BROKER "broker.emqx.io"  // EMQX Public Broker
#define MQTT_CLIENT_ID "MystTech01" // Replace with a unique client ID

// If you plan to use TLS (secure connection)

// pins fastled
#define LED_PIN 34  // Pin for LEDs

// pins uart 2 MP3
#define PIN_MP3_RX 17 
#define PIN_MP3_TX 16 

// pins outputs
#define PIN_O1 13
#define PIN_O2 14
#define PIN_O3 27
#define PIN_O4 26
#define PIN_O5 25
#define PIN_O6 33
#define PIN_O7 32
#define PIN_O8 2

// pins inputs
#define PIN_B1 4
#define PIN_B2 5
#define PIN_B3 18
#define PIN_B4 19
#define PIN_B5 21
#define PIN_B6 22
#define PIN_B7 12
#define PIN_B8 23
