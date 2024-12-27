String jsonPublished;

int solutionWin[] = {6,5,9};

// Access Point
#define AP_SSID "BB LOST ESP"
#define AP_PASSWORD "Lost"

#define PUZZLE_NAME "LOST 3 Dial Puzzle"
#define DESIGNER_NAME "Beyond Belief"
#define TECH_NAME "MYSTTECH Creations"

#define NR_TAB "LOST"
#define NR_GROUP "Stage 1"
#define NR_TYPE "LED_MATRIX"

#define NUM_INPUTS 3
#define NUM_OUTPUTS 2
#define NUM_LEDS 30  // Total number of LEDs (10 per dial)
#define NUM_CHANNELS 3

#define LED_PIN 12  // Pin for LEDs
#define PIN_OUTPUT1 5 // to relay
#define PIN_OUTPUT2 19 //to relay

// Dial 1
#define PIN_A1 14
#define PIN_B1 25

// Dial 2
#define PIN_A2 27
#define PIN_B2 33

// Dial 3
#define PIN_A3 26
#define PIN_B3 32

void setup() {
  
  Serial.begin(115200);
  
  setupFirmware();
  connectMQTT();
  sendConfigMQTTPayload(); // Send initial MQTT message when connected
  setupGPIO();
  setupFASTLED();
  setupDashboard();
  
  prepareLEDData(1, 1, 1);

}

void loop() {

  clientMQTTConnected();
  loopFIRMWARE();
  loopGPIO();
  loopFASTLED();
}
