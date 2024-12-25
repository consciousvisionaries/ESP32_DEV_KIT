
void connectMQTT() {

  client.setServer(MQTT_SERVER, MQTT_PORT);
  client.setCallback(mqttCallback);  // Set MQTT callback function
  
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    if (client.connect(clientId.c_str(), mqttUserName, mqttPassword)) {
      Serial.println("Connected to MQTT.");
      client.subscribe(MQTT_TOPIC);  // Subscribe to the topic
    } else {
      Serial.print("Failed (state=");
      Serial.print(client.state());
      Serial.println("). Retrying in 5 seconds...");
      delay(5000);
    }
  }
}

void clientMQTTConnected() {
  
  if (!client.connected()) {
    connectMQTT();
  }
  client.loop();  // Ensure MQTT is being handled
}

void publishDataMQTTPayload_Doc(String jsonPayload) {
   
    // Send the payload via MQTT
    if (client.publish(MQTT_TOPIC, jsonPayload.c_str())) {
        Serial.println("Data sent:");
        Serial.println(jsonPayload);
    } else {
        Serial.println("Failed to send data.");
    }
}

void sendConfigMQTTPayload() {
  
  StaticJsonDocument<512> doc;
    doc["mac"] = WiFi.macAddress();
    doc["puzzleName"] = PUZZLE_NAME;
    doc["designer"] = DESIGNER_NAME;
    doc["ipAddress"] = WiFi.localIP().toString();
    doc["timestamp"] = millis();
    doc["tab"] = NR_TAB;
    doc["group"] = NR_GROUP;
    doc["version"] = storedVersion;
    doc["num_outputs"] = NUM_OUTPUTS;
    doc["num_inputs"] = NUM_INPUTS;
  
  String jsonPayload;
  serializeJson(doc, jsonPayload);
  publishDataMQTTPayload_Doc(jsonPayload);
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.println(topic);

  if (strcmp(topic, MQTT_TOPIC) == 0) {  // Check if the message is for the correct topic
    
    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, payload, length);
    if (error) {
      Serial.print("JSON parse error: ");
      Serial.println(error.c_str());
      return;
    }     

    // Convert the JSON document to a string
    String jsonMessage;
    serializeJson(doc, jsonMessage);
    Serial.println("Message contents:");
    Serial.println(jsonMessage);
  }
}
