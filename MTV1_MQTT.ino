#define MQTT_MAX_PACKET_SIZE 225  // Adjust size as needed
#define MQTT_PORT 1883

#define MQTT_VERSION V1.1

//#define MQTT_TOPIC "/edna"
//#define MQTT_SERVER "192.168.0.129" // Replace with your MQTT broker IP

// Node-Red User Login (Device Login - Raspberry Pi)
//const char* mqttUserName = "pro1polaris";
//const char* mqttPassword = "CVr819P*!";

void connectMQTT() {
  client.setServer(mqttSettings.mqttServer.c_str(), MQTT_PORT);
  client.setCallback(mqttCallback);  // Set MQTT callback function
  Serial.println("MQTT Callback should be set");

  unsigned long startAttemptTime = millis();
  const unsigned long retryInterval = 5000;  // Retry interval (in milliseconds)
  const unsigned long timeout = 10000;       // Timeout after 30 seconds

  while (!client.connected() && millis() - startAttemptTime < timeout) {
    Serial.print("Connecting to MQTT...");
    if (client.connect(MQTT_CLIENT_ID, mqttSettings.mqttUserName.c_str(), mqttSettings.mqttPassword.c_str())) {
      Serial.println("Connected to SERVER");
      client.subscribe(MQTT_TOPIC);  // Subscribe to the topic
    } else {
      Serial.print("Failed (state=");
      Serial.print(client.state());
      Serial.println("). Retrying in 5 seconds...");
      unsigned long retryStart = millis();
      while (millis() - retryStart < retryInterval && !client.connected()) {
        delay(100);  // Non-blocking alternative, can handle tasks here
      }
    }
  }

  if (!client.connected()) {
    Serial.println("MQTT connection failed.");
    connectBrokerMQTT();
  }
}

void connectBrokerMQTT() {
  client.setServer(MQTT_BROKER, MQTT_PORT);
  client.setCallback(mqttCallback);  // Set MQTT callback function
  Serial.println("MQTT Callback should be set");

  unsigned long startAttemptTime = millis();
  const unsigned long retryInterval = 5000;  // Retry interval (in milliseconds)
  const unsigned long timeout = 30000;       // Timeout after 30 seconds

  while (!client.connected() && millis() - startAttemptTime < timeout) {
    Serial.print("Connecting to MQTT...");
    if (client.connect(MQTT_CLIENT_ID)) {
      Serial.println("Connected to BROKER");
      client.subscribe(MQTT_TOPIC);  // Subscribe to the topic
    } else {
      Serial.print("Failed (state=");
      Serial.print(client.state());
      Serial.println("). Retrying in 5 seconds...");
      unsigned long retryStart = millis();
      while (millis() - retryStart < retryInterval && !client.connected()) {
        delay(100);  // Non-blocking alternative, can handle tasks here
      }
    }
  }

  if (!client.connected()) {
    Serial.println("MQTT connection failed. Check credentials and server availability.");
  }
}

void clientMQTTConnected() {
  
  if (!client.connected()) {
    connectMQTT();
  }
  client.loop();  // Ensure MQTT is being handled
}


void publishDataMQTTPayload_Doc(String jsonPayload) {

    Serial.print("Payload size: ");
    Serial.println(jsonPayload.length());
    Serial.print("MQTT Buffer Size: ");
    Serial.println(MQTT_MAX_PACKET_SIZE);

    if (jsonPayload.length() > MQTT_MAX_PACKET_SIZE) {
        Serial.println("Error: Payload size exceeds MQTT buffer size.");
        return;
    }
   
    // Send the payload via MQTT
    if (client.publish(MQTT_TOPIC, jsonPayload.c_str())) {
        Serial.println("Data sent:");
        Serial.println(jsonPayload);
        jsonPublished = jsonPayload;
    } else {
        Serial.println("Failed to send data.");
    }
}



void sendConfigMQTTPayload() {
    DynamicJsonDocument doc(512);
    doc["mac"] = WiFi.macAddress();
    doc["puzzleName"] = PUZZLE_NAME;
    doc["designer"] = DESIGNER_NAME;
    doc["tech"] = TECH_NAME;
    doc["ip"] = WiFi.localIP().toString();
    doc["timestamp"] = millis();
    doc["tab"] = globalSettings.nrTab;
    doc["group"] = globalSettings.nrGroup;
    
    DynamicJsonDocument doc1(512);
    doc1["mac"] = WiFi.macAddress();
    doc1["puzzleName"] = PUZZLE_NAME;
    doc1["type"] = NR_TYPE;
    doc1["version"] = wifiSettings.storedVersion;
    doc1["leds"] = NUM_LEDS;
    doc1["chan"] = NUM_CHANNELS;
    doc1["inputs"] = NUM_INPUTS;
    doc1["outputs"] = NUM_OUTPUTS;

    String jsonPayload;
    serializeJson(doc, jsonPayload);
    jsonPublished = jsonPayload;
    publishDataMQTTPayload_Doc(jsonPayload);
    delay(50);
    
    jsonPayload = "";
    serializeJson(doc1, jsonPayload);
    jsonPublished = jsonPayload;
    publishDataMQTTPayload_Doc(jsonPayload);
    delay(50);

}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.println(topic);

  char message[length + 1];
  memcpy(message, payload, length);
  message[length] = '\0';

  DynamicJsonDocument doc(512);  // Use DynamicJsonDocument with appropriate size

  DeserializationError error = deserializeJson(doc, message);
  if (error) {
    Serial.print("JSON parse error: ");
    Serial.println(error.c_str());
    return;
  }

  if (doc.containsKey("activity")) {
    const char* activity = doc["activity"];
    Serial.print("Activity: ");
    Serial.println(activity);

    
    
  } else {
    Serial.println("Key 'activity' not found");
  }

  // Debugging: Print the entire JSON message
  String jsonMessage;
  serializeJson(doc, jsonMessage);
  Serial.println("Message contents:");
  Serial.println(jsonMessage);
}
