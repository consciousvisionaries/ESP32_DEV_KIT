

// access point
#define AP_SSID "LOST 3 Dial Puzzle"
#define AP_PASSWORD "MystTech" // 'localhost:1880/admin'

#define PUZZLE_NAME "LOST 3 Dial Puzzle"
#define DESIGNER_NAME "Beyond Belief"
#define TECH_NAME "MYSTTECH Creations"
#define MYSTTECH_MODEL "MYST3DIAL_FLED_PUZZLE"

#define NR_TAB "LOST"
#define NR_GROUP "Stage 1"

// types include:

// GPIO8 8 Input/8 Output 
#define NR_TYPE "3D_ROTARY_PULSE"
#define PULSE_MAX_RANGE 20 // NR_TYPE "3D_ROTARY_PULSE"

String bodyInputTitle = "3 DIAL VALVEs";
String bodyOutputTitle = "OVERRIDE";
String buttonsHTMLTitle = "Game Master INPUT Control";

// github
const char* GITHUB_USER = "consciousvisionaries";
const char* GITHUB_REPO = "ESP32_DEV_KIT";
const char* GITHUB_BIN = "ESP32_DEV_KIT.ino.esp32da.bin"; // esp32 wroom
const char* GITHUB_BRANCH = "ESPDEVKIT_3DialLEDStrip";

String MQTT_CLIENT_ID = "";           // Replace with MAC Address

#define NUM_DIGITAL_INPUTS 3
#define NUM_ANALOG_INPUTPAIRS 3
#define NUM_DIGITAL_OUTPUTS 2
#define NUM_FLED_OUTPUTS 2
#define NUM_FLED_CHANNELS 3  // 30/3
#define NUM_FLED_ADDLEDS 90  // Total number of LEDs (10 per dial)
#define NUM_FLED_ADDLEDS2 180 // total for around the room.
#define NUM_RXTX_PORTS 0

#define RX_UART0 3
#define TX_UART0 1
#define RX_UART1 9
#define TX_UART1 10
#define RX_UART2 17
#define TX_UART2 16
#define DOUT_PIN1 23 // digital out 1 - win game
#define I2C_PIN2 22
#define I2C_PIN1 21
#define DOUT_PIN2 19 // digital out 2 - other
#define DOUT_PIN3 18
#define DOUT_PIN4 13 // fled 2
#define FLED_PIN1 12 // fled
#define ADLED_PIN1 2
#define DADIN_PIN2 4 // dial 1
#define DADIN_PIN3 5 // dial 1
#define AIN_PIN4 26 // digital input 1 /// end game
#define AIN_PIN5 32 // digital input 2 /// start game.... new sort
#define AIN_PIN6 27 // digital input 3 /// trigger relay 3
#define AIN_PIN7 33 // dial 2
#define AIN_PIN8 14 // dial 2
#define AIN_PIN9 25
#define ADC1_CH0 36
#define ADC1_CH1 37
#define ADC1_CH2 38
#define ADC1_CH3 39
#define ADC1_CH6 34 // dial 3
#define ADC1_CH7 35 // dial 3
