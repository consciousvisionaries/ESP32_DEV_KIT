
int solutionWin[] = {6,5,9};

// Access Point
#define AP_SSID "BB LOST ESP"
#define AP_PASSWORD "Lost"

#define PUZZLE_NAME "LOST 3 Dial Puzzle"
#define DESIGNER_NAME "Beyond Belief"
#define TECH_NAME "MYSTTECH Creations"
#define MYSTTECH_MODEL "MYST3DIAL_FLED_PUZZLE"

#define NR_TAB "LOST"
#define NR_GROUP "Stage 1"

// types include:

// GPIO8 8 Input/8 Output 
#define NR_TYPE "3D_ROTARY_PULSE"
#define PULSE_MAX_RANGE "15000" // NR_TYPE "3D_ROTARY_PULSE"

String bodyInputTitle = "3 DIAL VALVEs";
String bodyOutputTitle = "OVERRIDE";
String buttonsHTMLTitle = "Game Master INPUT Control";

// access point
#define AP_SSID "MystTech 3 Dial Puzzle"
#define AP_PASSWORD "MystTech" // 'localhost:1880/admin'

// github
const char* GITHUB_USER = "consciousvisionaries";
const char* GITHUB_REPO = "ESP32_DEV_KIT";
const char* GITHUB_BIN = "ESP32_DEV_KIT.ino.esp32da.bin"; // esp32 wroom
const char* GITHUB_BRANCH = "ESPDEVKIT_3DialLEDStrip";

#define MQTT_TOPIC "/myst/er/bb/lost" // Topic for publish/subscribe
#define MQTT_BROKER "broker.emqx.io"  // EMQX Public Broker

String MQTT_CLIENT_ID = "";           // Replace with MAC Address

#define NUM_DIGITAL_INPUTS 0
#define NUM_ANALOG_INPUTPAIRS 3
#define NUM_DIGITAL_OUTPUTS 2
#define NUM_FLED_OUTPUTS 1
#define NUM_FLED_CHANNELS 3
#define NUM_FLED_ADDLEDS 30  // Total number of LEDs (10 per dial)
