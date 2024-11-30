#include <WiFi.h>
#include <PubSubClient.h>

// Replace with your network credentials
const char* ssid = "TELUSDE0875_2.4G";
const char* password = "3X3K22832E";

// MQTT server details
const char* mqttServer = "192.168.0.129"; // Use your local MQTT broker IP
const int mqttPort = 1883;
const char* mqttUserName = "pro1polaris";
const char* mqttPassword = "CVr819P*!";

WiFiClient espClient;
PubSubClient client(espClient);
String clientId = "ESP32_";  // Unique client ID


void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  clientId += String(WiFi.macAddress());
  Serial.print("Connected to WiFi! IP: ");
  Serial.println(WiFi.localIP());

  // Set up MQTT server
  client.setServer(mqttServer, mqttPort);

  // Try connecting to MQTT broker
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(clientId.c_str()),mqttUserName, mqttPassword) {
      Serial.println("Connected to MQTT broker!");
            delay(2000);  // Retry every 2 seconds

    } else {
      Serial.print("Failed to connect, state: ");
      Serial.println(client.state());
      delay(2000);  // Retry every 2 seconds
    }
  }
  // Subscribe to a topic
  client.subscribe("/topic");
  delay(2000);  // Retry every 2 seconds


  client.publish("/topic", (clientId + " IP Address " + WiFi.localIP() + " is now online").c_str());
}

void loop() {
  
  client.loop();

  sendNodeRed(" Hello from ESP32");

}

void sendNodeRed(String message) {

// Send a message to Node-RED every 5 seconds
  static unsigned long lastMsg = 0;
  if (millis() - lastMsg > 60000) {
    lastMsg = millis();
    client.publish("/topic", message.c_str());
    Serial.println("Message sent to /topic");
  }

}
