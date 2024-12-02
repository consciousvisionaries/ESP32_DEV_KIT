#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <Update.h>

// Replace with your network credentials
const char* ssid = "TELUSDE0875_2.4G";
const char* password = "3X3K22832E";

// MQTT server details
const char* mqttServer = "192.168.0.129"; // Replace with your MQTT broker IP
const int mqttPort = 1883;
const char* mqttUserName = "pro1polaris";
const char* mqttPassword = "CVr819P*!";

// GitHub repository details
const char* githubUser = "consciousvisionaries";
const char* githubRepo = "ESP32_DEV_KIT";
const char* firmwareFile = "firmware.bin";
const char* branch = "main"; // Branch where the firmware file is located

// MQTT and Wi-Fi clients
WiFiClient espClient;
PubSubClient client(espClient);

// MQTT Client ID
String clientId = "";

// Function prototypes
void connectWiFi();
void connectMQTT();
void sendMQTTPayload();
void checkForUpdates();
String getFirmwareURL();

void setup() {
    Serial.begin(115200);

    // Generate a unique MQTT Client ID based on MAC address
    clientId = "ESP32_" + String(WiFi.macAddress());

    // Connect to Wi-Fi
    connectWiFi();

    // Set up MQTT server
    client.setServer(mqttServer, mqttPort);

    // Connect to MQTT
    connectMQTT();

    delay(3000);
    checkForUpdates();

}

void loop() {
    // Ensure MQTT connection is alive
    if (!client.connected()) {
        connectMQTT();
    }
    client.loop();

    // Periodically send JSON payload
    static unsigned long lastSend = 0;
    if (millis() - lastSend > 60000) { // Every 60 seconds
        lastSend = millis();
        sendMQTTPayload();
    }

    // Periodically check for firmware updates
    static unsigned long lastOTA = 0;
    if (millis() - lastOTA > 3600000) { // Every hour
        lastOTA = millis();
        checkForUpdates();
    }
}

// Function to connect to Wi-Fi
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
}

// Function to connect to MQTT
void connectMQTT() {
    while (!client.connected()) {
        Serial.print("Connecting to MQTT...");
        if (client.connect(clientId.c_str(), mqttUserName, mqttPassword)) {
            Serial.println("Connected to MQTT.");
            sendMQTTPayload();
            client.subscribe("/topic"); // Subscribe to a topic if needed
        } else {
            Serial.print("Failed (state=");
            Serial.print(client.state());
            Serial.println("). Retrying in 5 seconds...");
            delay(5000); // Retry delay
        }
    }
}

// Function to send JSON payload
void sendMQTTPayload() {
    StaticJsonDocument<256> doc;

    // Build JSON payload
    doc["mac"] = WiFi.macAddress();
    doc["puzzleName"] = "Mystery Puzzle";
    doc["designer"] = "Jane Doe";
    doc["ipAddress"] = WiFi.localIP().toString();
    doc["timestamp"] = millis(); // Replace with dynamic timestamp if needed
    doc["tab"] = "Presidents Big Mistake";
    doc["group"] = "Stage 3";

    String jsonPayload;
    serializeJson(doc, jsonPayload);

    // Publish payload
    if (client.publish("/topic", jsonPayload.c_str())) {
        Serial.println("Payload sent:");
        Serial.println(jsonPayload);
    } else {
        Serial.println("Failed to send payload.");
    }
}

// Function to construct the firmware URL
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

// Function to check for OTA updates
void checkForUpdates() {
    Serial.println("Checking for firmware updates...");
    HTTPClient http;
    String firmwareURL = getFirmwareURL();
    http.begin(firmwareURL);

    int httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK) { // File found
        int contentLength = http.getSize();
        bool canBegin = Update.begin(contentLength);

        if (canBegin) {
            WiFiClient& updateClient = http.getStream();
            Serial.println("Starting OTA update...");
            size_t written = Update.writeStream(updateClient);

            if (written == contentLength) {
                Serial.println("Firmware written successfully. Rebooting...");
                if (Update.end()) {
                    if (Update.isFinished()) {
                        Serial.println("OTA update finished successfully!");
                        ESP.restart();
                    } else {
                        Serial.println("OTA update failed!");
                    }
                } else {
                    Serial.printf("OTA Error: %s\n", Update.errorString());
                }
            } else {
                Serial.println("Firmware write failed.");
            }
        } else {
            Serial.println("Not enough space for OTA update.");
        }
    } else {
        Serial.printf("HTTP error: %d\n", httpCode);
    }

    http.end();
}
