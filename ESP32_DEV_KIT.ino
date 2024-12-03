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
const char* branch = "ESP32_WROVER2"; // Branch where the firmware file is located

WiFiClient espClient;
PubSubClient client(espClient);
String clientId = "";
Preferences preferences;

bool allServicesActive = false;

// Define the pins
const int ledPin = 2; // GPIO2 is commonly used for the onboard LED on ESP32 boards

// Define the lever pins as a macro holding the values
#define LEVER_PINS 12, 14, 27, 26, 33, 32, 34, 35  // Values, not an array definition

// Define the lever pins array based on the macro
int leverPins[] = { LEVER_PINS };  // Using the macro to initialize the array

// Define the lever states based on the number of pins
const int NUM_LEVER_PINS = 8; // Define NUM_LEVER_PINS as a constant
bool previousLeverStates[NUM_LEVER_PINS]; // Array size is now known at compile time

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
  
  sendMQTTPayload();  // Send MQTT message when connected

  // Set all lever pins as input with pull-up resistors
  for (int i = 0; i < NUM_LEVER_PINS; i++) {
    pinMode(leverPins[i], INPUT_PULLUP);  // Enable pull-up resistors
    previousLeverStates[i] = digitalRead(leverPins[i]) == LOW;  // Initialize previous state
  }
}

void loop() {
  bool leverStatusChanged = false;

  // Check the lever states and detect if there has been any change
  for (int i = 0; i < NUM_LEVER_PINS; i++) {
    bool currentState = digitalRead(leverPins[i]) == LOW;  // Lever is considered active when LOW
    if (currentState != previousLeverStates[i]) {  // Check if state has changed
      Serial.print("Lever ");
      Serial.print(i);
      Serial.print(" changed. New state: ");
      Serial.println(currentState ? "ACTIVE" : "INACTIVE");

      previousLeverStates[i] = currentState;  // Update previous state
      leverStatusChanged = true;  // Mark that a change occurred
    }
  }

  // Only send MQTT payload if any lever status changed
  if (leverStatusChanged) {
    // Create JSON payload for lever pin statuses
    StaticJsonDocument<512> leverDoc;
    for (int i = 0; i < NUM_LEVER_PINS; i++) {
      leverDoc["lever" + String(i)] = previousLeverStates[i] ? "ACTIVE" : "INACTIVE"; // Store pin status as "ACTIVE" or "INACTIVE"
    }

    // Convert JSON object to string
    String jsonLeverPinStatus;
    serializeJson(leverDoc, jsonLeverPinStatus);

    // Publish lever status to MQTT
    if (client.publish("/topic", jsonLeverPinStatus.c_str())) {
      Serial.println("Puzzle details sent:");
      Serial.println(jsonLeverPinStatus); // Print the JSON payload to Serial Monitor for debugging
    } else {
      Serial.println("Failed to send puzzle details.");
    }
  }

  if (!client.connected()) {
    connectMQTT();
  }

  static unsigned long lastOTA = 0;
  
  if (millis() - lastOTA > 3600000) {  // Check for updates every hour
    lastOTA = millis();
    checkForUpdates();
  }

  // Flash the onboard LED 5 times per second until all services are active
  if (allServicesActive) {
    digitalWrite(ledPin, HIGH);
    delay(100);  // LED ON for 100ms
    digitalWrite(ledPin, LOW);
    delay(100);  // LED OFF for 100ms
  } else {
    digitalWrite(ledPin, LOW);  // Keep LED OFF when services are inactive
  }
}
  

void connectWiFi() {
  Serial.println("Connecting to WiFi...");
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
    Serial.print("Connecting to MQTT...");
    if (client.connect(clientId.c_str(), mqttUserName, mqttPassword)) {
      Serial.println("Connected to MQTT.");
      client.subscribe("/topic");  // Example topic subscription
    } else {
      Serial.print("Failed (state=");
      Serial.print(client.state());
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
  doc["group"] = "Stage 4";
  doc["version"] = getStoredVersion();

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
  sendFirmwareOverMQTT();
}

void sendFirmwareOverMQTT() {
  // Send firmware update status over MQTT
  StaticJsonDocument<256> firmwareDoc;
  firmwareDoc["mac"] = WiFi.macAddress();
  firmwareDoc["firmwareStatus"] = "Checking for updates...";
  String firmwareStatus;
  serializeJson(firmwareDoc, firmwareStatus);
  
  if (client.publish("/topic", firmwareStatus.c_str())) {
    Serial.println("Firmware update status sent.");
  } else {
    Serial.println("Failed to send firmware status.");
  }
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
  Serial.println("Checking for firmware updates...");

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

    Serial.print("Stored firmware version: ");
    Serial.println(currentVersion);
    Serial.print("Available firmware version: ");
    Serial.println(newVersion);

    if (newVersion != currentVersion) {
      Serial.println("New firmware available. Starting OTA...");
      
      // Send MQTT message about new firmware
      StaticJsonDocument<256> updateDoc;
      updateDoc["mac"] = WiFi.macAddress();
      updateDoc["firmwareStatus"] = "New firmware available. Starting OTA...";
      String updateStatus;
      serializeJson(updateDoc, updateStatus);
      
      if (client.publish("/topic", updateStatus.c_str())) {
        Serial.println("Firmware update status sent over MQTT.");
      }

      String firmwareURL = getFirmwareURL();
      Serial.println("Downloading firmware...");
      http.begin(firmwareURL);
      int firmwareCode = http.GET();

      if (firmwareCode == HTTP_CODE_OK) {
        WiFiClient* client = http.getStreamPtr();
        if (Update.begin(http.getSize())) {
          size_t written = Update.writeStream(*client);
          if (written == http.getSize()) {
            Serial.println("Written " + String(written) + " successfully.");
            if (Update.end()) {
              Serial.println("OTA firmware update successful.");
              storeVersion(newVersion);
              ESP.restart();
            } else {
              Serial.println("Error ending OTA update.");
            }
          } else {
            Serial.println("Error writing firmware to flash.");
          }
        } else {
          Serial.println("Failed to start OTA update.");
        }
      } else {
        Serial.println("Error downloading firmware.");
      }
      http.end();
    }
  } else {
    Serial.println("Error fetching version from GitHub.");
  }
  http.end();
}
