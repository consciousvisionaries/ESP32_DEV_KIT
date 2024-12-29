String jsonPublished;

#define MODULE "ESP32WROOM by FREENOVE"

// Access Point
#define AP_SSID "BB EDNA ESP"
#define AP_PASSWORD "Edna"

#define PUZZLE_NAME "EDNA 8 Button MP3 Puzzle"
#define DESIGNER_NAME "Beyond Belief"
#define TECH_NAME "MYSTTECH Creations"

#define NR_TAB "EDNA"
#define NR_GROUP "Stage 1"
#define NR_TYPE "8_BUTTONMP3"

#define NUM_INPUTS 8
#define NUM_OUTPUTS 2
#define NUM_LEDS 10 
#define NUM_CHANNELS 1

#define MQTT_TOPIC "/edna"
#define MQTT_SERVER "192.168.0.129" // Replace with your MQTT broker IP
#define MQTT_PORT 1883

#define MQTT_REMOTE_SERVER "" // remote sandbox
#define MQTT_REPORT_PORT "" // port?



void setup() {
  
  Serial.begin(115200);
  
  setupFirmware();
  setupDashboard();

  connectMQTT();
  sendConfigMQTTPayload(); // Send initial MQTT message when connected
  setupMP3Player();
  setupGPIO();
  setupFASTLED();

  Serial.println("READY.");
}

void loop() {

  clientMQTTConnected();
  loopFIRMWARE();
  loopGPIO();
  loopFASTLED();
}
