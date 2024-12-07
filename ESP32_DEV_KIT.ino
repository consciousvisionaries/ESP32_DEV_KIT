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
const char* branch = "ESPDEVKIT_8-OUTPUT-DRIVER"; // Branch where the firmware file is located
const char* topicData = "/christmaslights";
const int NUM_OUTPUTS = 8;

WiFiClient espClient;
PubSubClient client(espClient);
String clientId = "";
Preferences preferences;

bool allServicesActive = false;

// Define the pins
const int ledPin = 2; // GPIO2 is commonly used for the onboard LED on ESP32 boards
int outputPins[] = {12, 14, 27, 26, 33, 32, 5, 18};  // Define the GPIO pins for the 8 outputs
bool outputStates[NUM_OUTPUTS] = {false};  // Track the state of each output

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  for (int i = 0; i < NUM_OUTPUTS; i++) {
    pinMode(outputPins[i], OUTPUT);
    digitalWrite(outputPins[i], LOW);  // Ensure all outputs start LOW
  }

  clientId = "ESP32_" + String(WiFi.macAddress());

  connectWiFi();
  client.setServer(mqttServer, mqttPort);
  client.setCallback(mqttCallback);  // Set MQTT callback function
  connectMQTT();

  preferences.begin("firmware", false);

  delay(3000);
  checkForUpdates();

  sendMQTTPayload();  // Send initial MQTT message when connected
}

void loop() {
  if (!client.connected()) {
    connectMQTT();
  }
  client.loop();  // Ensure MQTT is being handled

  static unsigned long lastOTA = 0;
  if (millis() - lastOTA > 3600000) {  // Check for updates every hour
    lastOTA = millis();
    checkForUpdates();
  }

  // Flash the onboard LED when services are active
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
      client.subscribe(topicData);  // Subscribe to the topic
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
  doc["group"] = "Stage 2";
  doc["version"] = getStoredVersion();
  doc["num_outputs"] = NUM_OUTPUTS;

  String jsonPayload;
  serializeJson(doc, jsonPayload);

  if (client.publish(topicData, jsonPayload.c_str())) {
    Serial.println("Puzzle details sent:");
    Serial.println(jsonPayload);
  } else {
    Serial.println("Failed to send puzzle details.");
  }
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.println(topic);

  if (strcmp(topic, topicData) == 0) {  // Check if the message is for the correct topic
    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, payload, length);

    if (error) {
      Serial.print("JSON parse error: ");
      Serial.println(error.c_str());
      return;
    }

    // Update output states based on JSON
    for (int i = 0; i < NUM_OUTPUTS; i++) {
      String key = "XmasOutput" + String(i + 1);
      if (doc.containsKey(key)) {
        outputStates[i] = doc[key];
        digitalWrite(outputPins[i], outputStates[i] ? HIGH : LOW);
        Serial.printf("Output %d set to %s\n", i + 1, outputStates[i] ? "HIGH" : "LOW");
      }
    }
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
  return preferences.getString("version", "");
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
  if (httpCode == HTTP_CODE_OK) {
    String newVersion = http.getString();
    newVersion.trim();
    String currentVersion = getStoredVersion();

    if (newVersion != currentVersion) {
      String firmwareURL = getFirmwareURL();
      http.begin(firmwareURL);
      int firmwareCode = http.GET();
      if (firmwareCode == HTTP_CODE_OK) {
        WiFiClient* client = http.getStreamPtr();
        if (Update.begin(http.getSize())) {
          size_t written = Update.writeStream(*client);
          if (written == http.getSize() && Update.end()) {
            storeVersion(newVersion);
            ESP.restart();
          }
        }
      }
      http.end();
    }
  }
  http.end();
}
