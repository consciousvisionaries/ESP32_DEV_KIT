#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <Update.h>
#include <Preferences.h>

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
const char* firmwareFile = "ESP32_DEV_KIT.ino.esp32.bin";
const char* branch = "main"; // Branch where the firmware file is located

// MQTT and Wi-Fi clients
WiFiClient espClient;
PubSubClient client(espClient);

// MQTT Client ID
String clientId = "";

// Preferences object to store the current version
Preferences preferences;

// Function prototypes
void connectWiFi();
void connectMQTT();
void sendMQTTPayload();
void checkForUpdates();
String getFirmwareURL();
String getStoredVersion();
void storeVersion(String version);

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

    // Open the preferences for reading and writing
    preferences.begin("firmware", false);

    delay(3000);
    checkForUpdates();
}

void loop() {
    // Ensure MQTT connection is alive
    if (!client.connected()) {
        Serial.println("MQTT disconnected, attempting to reconnect...");
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
            sendMQTTPayload();  // Send payload immediately upon connection
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
    StaticJsonDocument<512> doc;  // Increase the size of the document for more data

    // Build JSON payload
    doc["mac"] = WiFi.macAddress();
    doc["puzzleName"] = "Tarot Card V5";  // You can update the puzzle name dynamically
    doc["designer"] = "Paul Hopkins";
    doc["ipAddress"] = WiFi.localIP().toString();
    doc["timestamp"] = millis();  // Replace with dynamic timestamp if needed
    doc["tab"] = "Presidents Big Mistake"; // You can update this as well
    doc["group"] = "Stage 3";  // Update based on your group or phase details

    // Add any puzzle-specific data here
    doc["puzzleDetails"]["currentCard"] = "The Fool";  // Example: current card
    doc["puzzleDetails"]["progress"] = 45;  // Example: puzzle progress (as a percentage)

    String jsonPayload;
    serializeJson(doc, jsonPayload);

    // Publish payload only if MQTT is connected
    if (client.connected()) {
        if (client.publish("/topic/puzzleDetails", jsonPayload.c_str())) {  // Use specific topic for puzzle details
            Serial.println("Puzzle details sent:");
            Serial.println(jsonPayload);
        } else {
            Serial.println("Failed to send puzzle details.");
        }
    } else {
        Serial.println("MQTT not connected, skipping publish.");
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

// Function to get the stored version from preferences
String getStoredVersion() {
    String storedVersion = preferences.getString("version", "");
    return storedVersion;
}

// Function to store the version in preferences
void storeVersion(String version) {
    preferences.putString("version", version);
}

// Function to check for OTA updates
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
        newVersion.trim(); // Remove any trailing newline or spaces
        String currentVersion = getStoredVersion();

        Serial.print("Stored firmware version: ");
        Serial.println(currentVersion);
        Serial.print("Available firmware version: ");
        Serial.println(newVersion);

        if (newVersion != currentVersion) {
            Serial.println("New firmware available. Starting OTA...");

            // URL of the firmware bin file
            String firmwareURL = getFirmwareURL();
            Serial.println("Downloading firmware...");

            // Start the HTTP connection to download the firmware
            http.begin(firmwareURL);
            int firmwareCode = http.GET();

            if (firmwareCode == HTTP_CODE_OK) {
                WiFiClient* client = http.getStreamPtr();
                uint32_t firmwareSize = http.getSize();
                if (Update.begin(firmwareSize)) {
                    Serial.println("Starting OTA update...");
                    size_t written = Update.writeStream(*client);
                    if (written == firmwareSize) {
                        Serial.println("OTA update completed. Rebooting...");
                        if (Update.end()) {
                            ESP.restart(); // Reboot after the update
                        } else {
                            Serial.println("Failed to end the update.");
                        }
                    } else {
                        Serial.println("Failed to write all data to flash.");
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
        }
    } else {
        // Log the error response
        Serial.printf("HTTP request failed with error code: %d\n", httpCode);
        String errorResponse = http.getString();
        Serial.println("Error response: ");
        Serial.println(errorResponse);
    }

    http.end();
}
