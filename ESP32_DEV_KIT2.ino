#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <Update.h>
#include <ESPAsyncWebServer.h>
#include <Preferences.h>

Preferences preferences;

String storedVersion = "";  // Default to "0.0.0" if no version is stored
String version = "";  // Default version


String ssid = "TELUSDE0875_2.4G";   // Replace with your WiFi SSID
String password = "3X3K22832E";     // Replace with your WiFi password

const char* mqttServer = "192.168.0.129"; // Replace with your MQTT broker IP
const int mqttPort = 1883;
const char* mqttUserName = "pro1polaris";
const char* mqttPassword = "CVr819P*!";

const char* githubUser = "consciousvisionaries";
const char* githubRepo = "ESP32_DEV_KIT";
const char* firmwareFile = "ESP32_DEV_KIT2.ino.esp32.bin";
const char* branch = "ESPDEVKIT_8-OUTPUT-DRIVER"; // Branch where the firmware file is located
const char* topicData = "/christmaslights";
const int NUM_OUTPUTS = 8;

WiFiClient espClient;
PubSubClient client(espClient);
String clientId = "";

bool allServicesActive = false;

// Define the pins
const int ledPin = 2; // GPIO2 is commonly used for the onboard LED on ESP32 boards
int outputPins[] = {12, 14, 27, 26, 33, 32, 5, 18};  // Define the GPIO pins for the 8 outputs
bool outputStates[NUM_OUTPUTS] = {false};  // Track the state of each output

// Pattern variables
String currentPattern = "chase";  // Default pattern
unsigned long patternLastTime = 0;
unsigned long lastUpdateTime = 0;
unsigned long blinkInterval = 500; 
int chaseIndex = 0;
int reverseChaseIndex = NUM_OUTPUTS - 1;

// Example output states
int waveIndex = 0;  // Declare waveIndex (or change to chaseIndex if that was the intention)

// Set up the server on port 80
AsyncWebServer server(80);

void setup() {
  Serial.begin(115200);

  pinMode(ledPin, OUTPUT);
  for (int i = 0; i < NUM_OUTPUTS; i++) {
    pinMode(outputPins[i], OUTPUT);
    digitalWrite(outputPins[i], LOW);  // Ensure all outputs start LOW
  }

  clientId = "ESP32_" + String(WiFi.macAddress());

  loadWiFiCredentials();
  connectWiFi();
  setupDashboard();
  connectMQTT();

  preferences.begin("firmware", false);
  delay(3000);
  checkForUpdates();
  sendMQTTPayload();  // Send initial MQTT message when connected
}

void loop() {

  clientMQTTConnected();
  firmwareUpdates();
  handlePattern(millis());
}
