

// access point
#define AP_SSID "LOST 4x4 Matrix MP3"
#define AP_PASSWORD "MystTech" // 'localhost:1880/admin'

#define PUZZLE_NAME "LOST 4x4 Matrix MP3"
#define DESIGNER_NAME "Beyond Belief"
#define TECH_NAME "MYSTTECH Creations"
#define MYSTTECH_MODEL "MYST4XMATRIX_MP3_PUZZLE"
#define DNSLOCAL "MYST4XMATRIX_MP3_PUZZLE.LOCAL"

#define NR_TAB "LOST"
#define NR_GROUP "Stage 1"

// types include:

// GPIO8 8 Input/8 Output
#define NR_TYPE "4X4 MATRIX"
#define PULSE_MAX_RANGE 20 // NR_TYPE "3D_ROTARY_PULSE"

// github
const char* GITHUB_USER = "consciousvisionaries";
const char* GITHUB_REPO = "ESP32_DEV_KIT";
const char* GITHUB_BIN = "ESP32_DEV_KIT.ino.esp32da.bin"; // esp32 wroom
const char* GITHUB_BRANCH = "ESPDEVKIT_4x4Matrix_MP3";

String MQTT_CLIENT_ID = "";           // Replace with MAC Address

#define NUM_DIGITAL_INPUTSA 0
#define NUM_DIGITAL_INPUTSB 8
#define NUM_ANALOG_INPUTPAIRS 0
#define NUM_DIGITAL_OUTPUTSA 8
#define NUM_OUTPUT_ROWSA 2
#define NUM_DIGITAL_OUTPUTSB 8
#define NUM_OUTPUT_ROWSB 2
#define NUM_FLED_OUTPUTS 0
#define NUM_FLED_CHANNELS 0  // 30/3
#define NUM_FLED_ADDLEDS 0  // Total number of LEDs (10 per dial)
#define NUM_FLED_ADDLEDS2 0 // total for around the room.
#define NUM_RXTX_PORTS 0

#define RX_UART0 3
#define TX_UART0 1
#define RX_UART1 9
#define TX_UART1 10
#define RX_UART2 16
#define TX_UART2 17
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
#define DADIN_PIN4 26 // digital input 1 /// end game
#define DADIN_PIN5 32 // digital input 2 /// start game.... new sort
#define DADIN_PIN6 27 // digital input 3 /// trigger relay 3
#define DADIN_PIN7 33 // dial 2
#define DADIN_PIN8 14 // dial 2
#define DADIN_PIN9 25
#define ADC1_CH0 36
#define ADC1_CH1 37
#define ADC1_CH2 38
#define ADC1_CH3 39
#define ADC1_CH6 34 // dial 3
#define ADC1_CH7 35 // dial 3
