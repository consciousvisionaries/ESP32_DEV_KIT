#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <Update.h>
#include <Preferences.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

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

// Pattern variables
String currentPattern = "chase";  // Default pattern
unsigned long patternLastTime = 0;
int chaseIndex = 0;
int reverseChaseIndex = NUM_OUTPUTS - 1;

// OLED Display Setup
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define I2C_ADDRESS 0x3C  // OLED I2C address, common for many displays

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  Serial.begin(115200);

  // Initialize I2C for OLED display with custom SDA and SCL pins
    Wire.begin(21, 22); // SDA = 21, SCL = 22
    
  pinMode(ledPin, OUTPUT);
  for (int i = 0; i < NUM_OUTPUTS; i++) {
    pinMode(outputPins[i], OUTPUT);
    digitalWrite(outputPins[i], LOW);  // Ensure all outputs start LOW
  }

  clientId = "ESP32_" + String(WiFi.macAddress());

  // Initialize OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, I2C_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Infinite loop if OLED fails to initialize
  }
  display.setCursor(0,0);
  display.print("Ready");
  display.display();
  delay(3000);
  display.clearDisplay();

  connectWiFi();
  client.setServer(mqttServer, mqttPort);
  client.setCallback(mqttCallback);  // Set MQTT callback function
  connectMQTT();

  // Initialize Preferences
  preferences.begin("firmware", false);

  // Set the version during initial setup (only if necessary)
  if (!preferences.isKey("version")) {
    preferences.putString("version", "1.3.3");  // Set the initial firmware version
  }

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

  // Handle pattern
  handlePattern(millis());

  // Update OLED with status info
  updateOLED();
}

void handlePattern(unsigned long currentTime) {
   
  if (currentPattern == "static") {
    setAllOutputs(HIGH);  // Turn all outputs ON
  } else if (currentPattern == "off") {
    setAllOutputs(LOW);  // Turn all outputs OFF
  } else if (currentPattern == "blink") {
    if (currentTime - patternLastTime >= 500) {  // 500ms interval
      patternLastTime = currentTime;
      toggleAllOutputs();
    }
  } else if (currentPattern == "chase") {
     

    if (currentTime - patternLastTime >= 200) {  // 200ms interval
      patternLastTime = currentTime;
      setAllOutputs(LOW);  // Turn off all outputs
      digitalWrite(outputPins[chaseIndex], HIGH);  // Light up one output
      chaseIndex = (chaseIndex + 1) % NUM_OUTPUTS;  // Move to the next output
    }
  } else if (currentPattern == "reverseChase") {
    if (currentTime - patternLastTime >= 200) {  // 200ms interval
      patternLastTime = currentTime;
      setAllOutputs(LOW);  // Turn off all outputs
      digitalWrite(outputPins[reverseChaseIndex], HIGH);  // Light up one output in reverse
      reverseChaseIndex = (reverseChaseIndex - 1 + NUM_OUTPUTS) % NUM_OUTPUTS;  // Move to the previous output
    }
  } else if (currentPattern == "randomBlink") {
    if (currentTime - patternLastTime >= 200) {  // 200ms interval
      patternLastTime = currentTime;
      int randomPin = random(0, NUM_OUTPUTS);
      digitalWrite(outputPins[randomPin], !digitalRead(outputPins[randomPin]));  // Toggle a random output
    }
  } else if (currentPattern == "wave") {
    if (currentTime - patternLastTime >= 300) {  // 300ms interval
      patternLastTime = currentTime;
      for (int i = 0; i < NUM_OUTPUTS; i++) {
        if (i % 2 == 0) {
          digitalWrite(outputPins[i], HIGH);  // Turn ON alternating outputs
        } else {
          digitalWrite(outputPins[i], LOW);  // Turn OFF alternating outputs
        }
      }
    }
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
  doc["puzzleName"] = "Outputs Puzzle";
  doc["designer"] = "Paul Hopkins";
  doc["ipAddress"] = WiFi.localIP().toString();
  doc["timestamp"] = millis();
  doc["tab"] = "Christmas Lights";
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

    // Check for the pattern name and update the currentPattern variable
    if (doc.containsKey("pattern")) {
      currentPattern = doc["pattern"].as<String>();
      Serial.printf("Pattern set to: %s\n", currentPattern.c_str());
    }

    // Update output states based on JSON
    for (int i = 0; i < NUM_OUTPUTS; i++) {
      String key = "XmasOutput" + String(i + 1);
      if (doc.containsKey(key)) {
        outputStates[i] = doc[key].as<bool>();
        digitalWrite(outputPins[i], outputStates[i] ? HIGH : LOW);
      }
    }
  }
}

void setAllOutputs(bool state) {
  for (int i = 0; i < NUM_OUTPUTS; i++) {
    digitalWrite(outputPins[i], state ? HIGH : LOW);
    outputStates[i] = state;  // Update the tracked states
  }
}

void toggleAllOutputs() {
  for (int i = 0; i < NUM_OUTPUTS; i++) {
    bool currentState = digitalRead(outputPins[i]);  // Read the current state of the pin
    digitalWrite(outputPins[i], !currentState);     // Write the opposite state
    outputStates[i] = !currentState;                // Update the tracked state
  }
}

void checkForUpdates() {
  HTTPClient http;
  String url = "https://raw.githubusercontent.com/" + String(githubUser) + "/" + String(githubRepo) + "/" + String(branch) + "/" + String(firmwareFile);

  http.begin(url);
  int httpCode = http.GET();

  if (httpCode == HTTP_CODE_OK) {
    String payload = http.getString();
    String newVersion = extractVersionFromPayload(payload);
    
    // Get stored version from Preferences
    String storedVersion = preferences.getString("version", "0.0.0");  // Default to "0.0.0" if no version is stored

    Serial.print("Current stored version: ");
    Serial.println(storedVersion);
    Serial.print("New version available: ");
    Serial.println(newVersion);

    if (newVersion != storedVersion) {
      Serial.println("New firmware update found. Updating...");
      performFirmwareUpdate(url);  // Perform the update
    } else {
      Serial.println("Firmware is up to date.");
    }
  } else {
    Serial.println("Failed to fetch update information.");
  }

  http.end();
}


String extractVersionFromPayload(String payload) {
  int startIndex = payload.indexOf("version\": \"");
  int endIndex = payload.indexOf("\"", startIndex + 11);
  return payload.substring(startIndex + 11, endIndex);
}
void performFirmwareUpdate(String firmwareUrl) {
  HTTPClient http;
  http.begin(firmwareUrl);
  int httpCode = http.GET();
  
  if (httpCode == HTTP_CODE_OK) {
    int contentLength = http.getSize();
    
    if (Update.begin(contentLength)) {
      WiFiClient *client = http.getStreamPtr();
      size_t written = Update.writeStream(*client);
      
      if (written == contentLength) {
        Serial.println("Update complete. Rebooting...");
        Update.end();

        // Store the new version in Preferences
        String newVersion = extractVersionFromPayload(http.getString());
        preferences.putString("version", newVersion);  // Save version to Preferences
        
        ESP.restart();
      } else {
        Serial.println("Update failed.");
        Update.end();
      }
    } else {
      Serial.println("Not enough space for update.");
    }
  } else {
    Serial.println("Failed to fetch firmware update.");
  }
  http.end();
}


String getStoredVersion() {
  return preferences.getString("version", "0.0.0");  // Default version if not found
}

void updateOLED() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("Wi-Fi: ");
  display.print(WiFi.status() == WL_CONNECTED ? "Connected" : "Disconnected");

  display.setCursor(0, 10);
  display.print("Pattern: ");
  display.print(currentPattern);

  display.setCursor(0, 20);
  display.print("Version: ");
  display.print(getStoredVersion());

  display.setCursor(0, 30);
  display.print("MQTT: ");
  display.print(client.connected() ? "Connected" : "Disconnected");

  display.display();
}
