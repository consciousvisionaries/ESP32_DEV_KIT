#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <Update.h>
#include <Preferences.h>

const char* ssid = "TELUSDE0875_2.4G";   // Replace with your WiFi SSID
const char* password = "3X3K22832E";     // Replace with your WiFi password

const char* mqttServer = "192.168.0.129"; // Replace with your MQTT broker IP
const int mqttPort = 1883;
const char* mqttUserName = "pro1polaris";
const char* mqttPassword = "CVr819P*!";

const char* githubUser = "consciousvisionaries";
const char* githubRepo = "ESP32_DEV_KIT";
const char* firmwareFile = "ESP32_DEV_KIT.ino.esp32.bin";
const char* branch = "ESP32_WROVER1"; // Branch where the firmware file is located

WiFiClient espClient;
PubSubClient client(espClient);
String clientId = "";
Preferences preferences;

bool allServicesActive = false;

// Define the LED pin
const int ledPin = 2; // GPIO2 is commonly used for the onboard LED on ESP32 boards

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);  // Set onboard LED as output
  clientId = "ESP32_" + String(WiFi.macAddress());

  connectWiFi();
  
  client.setServer(mqttServer, mqttPort);
  connectMQTT();

  preferences.begin("firmware", false);

  delay(3000);
  checkForUpdates();

  setup_Puzzle();
}

void loop() {
  if (!client.connected()) {
    connectMQTT();
  }
  //client.loop();

  static unsigned long lastOTA = 0;
  if (millis() - lastOTA > 3600000) {  // Check for updates every hour
    lastOTA = millis();
    checkForUpdates();
  }

//  Flash the onboard LED 5 times per second until all services are active
  if (allServicesActive) {
   digitalWrite(ledPin, HIGH);
   delay(100);  // LED ON for 100ms
   digitalWrite(ledPin, LOW);
   delay(100);  // LED OFF for 100ms
  } else {
    // Pause the LED when all services are active
    digitalWrite(ledPin, LOW);  // Keep LED OFF
  }

  loop_Puzzle();
}

void connectWiFi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected.");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  allServicesActive = true;  // Set to true when WiFi is connected
}

void connectMQTT() {
  while (!client.connected()) {
    if (client.connect(clientId.c_str(), mqttUserName, mqttPassword)) {
      Serial.println("Connected to MQTT.");
      sendMQTTPayload();  // Send MQTT message when connected
      client.subscribe("/topic");  // Example topic subscription
      allServicesActive = true;  // Set to true when MQTT is connected
    } else {
      Serial.println("). Retrying in 5 seconds...");
      delay(5000);
    }
  }
}

void sendMQTTPayload() {
  StaticJsonDocument<512> doc;
  doc["mac"] = WiFi.macAddress();
  doc["puzzleName"] = "Levers Puzzle";
  doc["designer"] = "Paul Hopkins";
  doc["ipAddress"] = WiFi.localIP().toString();
  doc["timestamp"] = millis();
  doc["tab"] = "Presidents Big Mistake";
  doc["group"] = "Stage 1";

  

  String jsonPayload;
  serializeJson(doc, jsonPayload);

  // Debugging payload size
  Serial.println("Payload size: " + String(jsonPayload.length()));

  if (client.publish("/topic", jsonPayload.c_str())) {
    Serial.println("Puzzle details sent:");
    Serial.println(jsonPayload);
  } else {
    Serial.println("Failed to send puzzle details.");
  }

  // Send firmware update status over MQTT
  StaticJsonDocument<256> firmwareDoc;
  firmwareDoc["mac"] = WiFi.macAddress();
  firmwareDoc["firmwareStatus"] = "Checking for updates...";
  String firmwareStatus;
  serializeJson(firmwareDoc, firmwareStatus);
  
 
}

String getFirmwareURL() {
  String url = "https://raw.githubusercontent.com/";
  url += githubUser;
  url += "/";
  url += githubRepo;
  url += "/";
  url += branch;
  url += "/";
  url += firmwareFile;
  return url;
}

String getStoredVersion() {
  String storedVersion = preferences.getString("version", "");
  return storedVersion;
}

void storeVersion(String version) {
  preferences.putString("version", version);
}

void checkForUpdates() {

  HTTPClient http;
  String versionURL = "https://raw.githubusercontent.com/" + String(githubUser) + "/" + String(githubRepo) + "/" + String(branch) + "/version.txt";
  http.begin(versionURL);

  int httpCode = http.GET();
  Serial.print("HTTP Code: ");
  Serial.println(httpCode);

  if (httpCode == HTTP_CODE_OK) {
    String newVersion = http.getString();
    newVersion.trim();
    String currentVersion = getStoredVersion();

      if (newVersion != currentVersion) {
      Serial.println("New firmware available. Starting OTA...");
      
      // Send MQTT message about new firmware
      StaticJsonDocument<256> updateDoc;
      updateDoc["mac"] = WiFi.macAddress();
      updateDoc["firmwareStatus"] = "New firmware available. Starting OTA...";
      String updateStatus;
      serializeJson(updateDoc, updateStatus);
      
      String firmwareURL = getFirmwareURL();
      Serial.println("Downloading firmware...");
      http.begin(firmwareURL);
      int firmwareCode = http.GET();

      if (firmwareCode == HTTP_CODE_OK) {
        WiFiClient* client = http.getStreamPtr();
        uint32_t firmwareSize = http.getSize();

        if (Update.begin(firmwareSize)) {
          Serial.println("Starting OTA update...");
          size_t written = Update.writeStream(*client);
          
          // Track OTA progress
          if (written == firmwareSize) {
            if (Update.end()) {
              storeVersion(newVersion);
              ESP.restart();  // Reboot after update
            } else {
              Serial.println("Failed to end the update.");
            }
          } else {
            Serial.print("Failed to write all data to flash. Written: ");
            Serial.print(written);
            Serial.print(", Expected: ");
            Serial.println(firmwareSize);
          }
        } else {
          Serial.println("Not enough space for OTA update.");
        }
      } else {
        Serial.println("Failed to download the firmware.");
      }
      http.end();
    } else {
      Serial.println("Firmware is up to date.");
      // Send MQTT message indicating firmware is up to date
      StaticJsonDocument<256> upToDateDoc;
      upToDateDoc["mac"] = WiFi.macAddress();
      upToDateDoc["firmwareStatus"] = "Firmware is up to date.";
      String upToDateStatus;
      serializeJson(upToDateDoc, upToDateStatus);

    }
  } else {
    Serial.printf("HTTP request failed with error code: %d\n", httpCode);
    String errorResponse = http.getString();
    Serial.println("Error response: ");
    Serial.println(errorResponse);
  }

  http.end();
}

#include <FastLED.h>

// Number of LEDs (8 bits)
#define NUM_LEDS 8
#define LED_PIN 4        // Pin connected to the data input of the LED strip

// Define pins for the levers (input ports)
#define LEVER_PINS {12, 14, 27, 26, 33, 32, 34, 35} // Selected available pins

// Array to store the lever states
int leverPins[] = LEVER_PINS;
bool leverStates[NUM_LEDS];

// FastLED setup
CRGB leds[NUM_LEDS];

void setup_Puzzle() {
  // Initialize serial communication for debugging
  Serial.begin(9600);

  // Initialize LED strip
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
  
  // Set all lever pins as input
  for (int i = 0; i < NUM_LEDS; i++) {
    pinMode(leverPins[i], INPUT_PULLUP);
  }

  // Initialize lever states to false (lever is not pressed)
  for (int i = 0; i < NUM_LEDS; i++) {
    leverStates[i] = false;
  }

  // Turn off LEDs at the start
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Black;
  }
  FastLED.show();
}

void loop_Puzzle() {
  // Check for lever states and update LEDs accordingly
  for (int i = 0; i < NUM_LEDS; i++) {
    leverStates[i] = digitalRead(leverPins[i]);

    if (leverStates[i]) {
      leds[i] = CRGB::Green;  // Turn on LED when lever is pressed
    } else {
      leds[i] = CRGB::Black;  // Turn off LED when lever is not pressed
    }
  }

  // Update the LED strip
  FastLED.show();
}
