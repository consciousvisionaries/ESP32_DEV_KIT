//#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <Update.h>
#include <ESPAsyncWebServer.h>
#include <Preferences.h>

Preferences preferences;
WiFiClient espClient;
PubSubClient client(espClient);

String clientId = "";
bool allServicesActive = false;

String storedVersion; //"1.3.4d";  // Default to "0.0.0" if no version is stord
String ssid = "TELUSDE0875_2.4G";   // Replace with your WiFi SSID
String password = "3X3K22832E";     // Replace with your WiFi password

const char* mqttServer = "192.168.0.78"; // Replace with your MQTT broker IP
const int mqttPort = 1883;
const char* mqttUserName = "pro1polaris";
const char* mqttPassword = "CVr819P*!";

const char* githubUser = "consciousvisionaries";
const char* githubRepo = "ESP32_DEV_KIT";
const char* firmwareFile = "ESP32_DEV_KIT2.ino.esp32.bin";
const char* branch = "ESPDEVKIT_8-OUTPUT-DRIVER"; // Branch where the firmware file is located
const char* topicData = "/christmaslights";
const int NUM_OUTPUTS = 8;

// Define the pins
const int ledPin = 2; // GPIO2 is commonly used for the onboard LED on ESP32 boards
int outputPins[] = {12, 14, 27, 26, 33, 32, 5, 18};  // Define the GPIO pins for the 8 outputs
bool outputStates[NUM_OUTPUTS] = {false, false, false, false, false, false, false, false};

// Pattern variables
String currentPattern = "chase";  // Default pattern
unsigned long patternLastTime = 0;
unsigned long lastUpdateTime = 0;
unsigned long blinkInterval = 500; 
int chaseIndex = 0;
int reverseChaseIndex = NUM_OUTPUTS - 1;
static unsigned long lastMillis = 0;
String audioPinStateText;
bool beatStatus[3] = {false,false,false};
String beatName[3] = {"LOW","MID","HIGH"};

// Example output states
int waveIndex = 0;  // Declare waveIndex (or change to chaseIndex if that was the intention)
