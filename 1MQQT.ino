#define MQTT_MAX_PACKET_SIZE 225  // Adjust size as needed
#define MQTT_PORT 1883
#define MQTT_VERSION V1.2

void connectMQTT() {
  client.setServer(mqttSettings.mqttServer.c_str(), MQTT_PORT);
  client.setCallback(mqttCallback);  // Set MQTT callback function
  Serial.println("MQTT Callback should be set");

  unsigned long startAttemptTime = millis();
  const unsigned long retryInterval = 5000;  // Retry interval (in milliseconds)
  const unsigned long timeout = 10000;       // Timeout after 10 seconds

  MQTT_CLIENT_ID = String(WiFi.macAddress());

  while (!client.connected() && millis() - startAttemptTime < timeout) {
    Serial.print("Connecting to MQTT...");
    if (client.connect(MQTT_CLIENT_ID.c_str(), mqttSettings.mqttUsername.c_str(), mqttSettings.mqttPassword.c_str())) {
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
    if (client.connect(MQTT_CLIENT_ID.c_str())) {
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


void sendJsonDocUpdateMQTTPayload(String jsonPayload1) {
  
  // Create a new JSON document for the message to send
  DynamicJsonDocument doc(MQTT_MAX_PACKET_SIZE);

  // Add fields to the document
  doc["mac"] = WiFi.macAddress();
  doc["puzzleName"] = PUZZLE_NAME;
  doc["designer"] = DESIGNER_NAME;
  doc["tech"] = TECH_NAME;
  doc["ip"] = WiFi.localIP().toString();
  doc["timestamp"] = millis();
  doc["tab"] = globalSettings.nrTab;
  doc["group"] = globalSettings.nrGroup;
  doc["type"] = NR_TYPE;

  // Convert the doc to a JSON string and publish
  String jsonPayload;
  serializeJson(doc, jsonPayload);
  jsonPublished = jsonPayload;
  publishDataMQTTPayload_Doc(jsonPayload);
  delay(50);  // Delay to ensure message is processed

  publishDataMQTTPayload_Doc(jsonPayload1);
  delay(50);  // Delay to ensure message is processed
}

void sendMessageUpdateMQTTPayload(String message) {
    DynamicJsonDocument doc(512);
    doc["mac"] = WiFi.macAddress();
    doc["puzzleName"] = PUZZLE_NAME;
    doc["message"] = message;
    
    // Convert the doc to a JSON string and publish
    String jsonPayload;
    serializeJson(doc, jsonPayload);
    jsonPublished = jsonPayload;

    sendJsonDocUpdateMQTTPayload(jsonPayload);
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
    if (String(activity) == "getFirmwareUpdate") {
      Serial.println("Command Accepted!");
      sendMessageUpdateMQTTPayload("Firmware Status Request Confirmed from: " + String(PUZZLE_NAME));
    } else if (activity == (String(NR_GROUP) + " Button Pressed")) { 
      executeGPIOBatch1();
    } else if (activity == (String(NR_GROUP) + " Button 2 Pressed")) { 
      executeGPIOBatch2();
    } else {
      executeMQTTBatchBelow(activity);
    }
  } else {
    Serial.println("Key 'activity' not found");
  }

  // Debugging: Print the entire JSON message
  String jsonMessage;
  serializeJson(doc, jsonMessage);
  Serial.println("Message contents:");
  Serial.println(jsonMessage);
}

void executeMQTTBatchBelow(String activity) {

    Serial.println("Key 'activity' : " + activity);
    }
