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
const char* branch = "main"; // Branch where the firmware file is located

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
      sendMQTTPayload();  // Send MQTT message when connected
      client.subscribe("/topic");  // Example topic subscription
      allServicesActive = true;  // Set to true when MQTT is connected
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
  doc["puzzleName"] = "Tarot Puzzle";
  doc["designer"] = "Paul Hopkins";
  doc["ipAddress"] = WiFi.localIP().toString();
  doc["timestamp"] = millis();
  doc["tab"] = "Presidents Big Mistake";
  doc["group"] = "Stage 2";

  

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
  
  if (client.publish("/topic/firmwareStatus", firmwareStatus.c_str())) {
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
      
      if (client.publish("/topic/firmwareStatus", updateStatus.c_str())) {
        Serial.println("Firmware update status sent over MQTT.");
      }

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
            Serial.println("OTA update completed. Rebooting...");
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

      if (client.publish("/topic/firmwareStatus", upToDateStatus.c_str())) {
        Serial.println("Firmware up-to-date status sent over MQTT.");
      }
    }
  } else {
    Serial.printf("HTTP request failed with error code: %d\n", httpCode);
    String errorResponse = http.getString();
    Serial.println("Error response: ");
    Serial.println(errorResponse);
  }

  http.end();
}
