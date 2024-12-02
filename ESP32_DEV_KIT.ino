#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// Replace with your network credentials
const char* ssid = "TELUSDE0875_2.4G";
const char* password = "3X3K22832E";

// MQTT server details
const char* mqttServer = "192.168.0.129"; // Replace with your MQTT broker IP
const int mqttPort = 1883;
const char* mqttUserName = "pro1polaris";
const char* mqttPassword = "CVr819P*!";

// MQTT and Wi-Fi clients
WiFiClient espClient;
PubSubClient client(espClient);

// MQTT Client ID
String clientId = "";

// Function prototypes
void connectWiFi();
void connectMQTT();
void sendMQTTPayload();

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
    doc["timestamp"] = DateTime(); // Replace with dynamic timestamp if needed
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
