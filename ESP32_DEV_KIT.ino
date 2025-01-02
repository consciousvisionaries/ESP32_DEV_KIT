
// esp32
#define MODULE "ESP32WROOM by FREENOVE"

// puzzle details
#define PUZZLE_NAME "President's Big Mistake"
#define DESIGNER_NAME "Beyond Belief Entertainment"
#define TECH_NAME "MYSTTECH Creations"
#define MYSTTECH_MODEL "MYST8 GPIO MASTER"
#define NR_TYPE "8_OUTPUT" 


// digitalGPIO

#define NUM_INPUTS 8

String bodyInputTitle = "Monitoring: FLEXXMAX2 Master";
String bodyOutputTitle = "OVERRIDE";
String buttonsHTMLTitle = "Game Master INPUT Control";

// outputs
#define NUM_OUTPUTS 8
#define NUM_OUTPUT_ROWS 2

// fastled
#define NUM_LEDS 10 
#define NUM_CHANNELS 0

// access point
#define AP_SSID "MystTech 8GPIO"
#define AP_PASSWORD "MystTech" // 'localhost:1880/admin'

// github
const char* GITHUB_USER = "consciousvisionaries";
const char* GITHUB_REPO = "ESP32_DEV_KIT";
const char* GITHUB_BIN = "ESP32_DEV_KIT.ino.esp32da.bin"; // esp32 wroom
const char* GITHUB_BRANCH = "MYST8_GPIO_Master";

#define MQTT_TOPIC "/myst/er/bb/president"      // Topic for publish/subscribe
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
#define PIN_O7 2
#define PIN_O8 32

// pins inputs
#define PIN_B1 4
#define PIN_B2 5
#define PIN_B3 18
#define PIN_B4 19
#define PIN_B5 21
#define PIN_B6 22
#define PIN_B7 12
#define PIN_B8 23
