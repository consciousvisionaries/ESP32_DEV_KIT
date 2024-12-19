void connectMQTT() {

  client.setServer(mqttServer, mqttPort);
  client.setCallback(mqttCallback);  // Set MQTT callback function
  
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

void clientMQTTConnected() {
  
  if (!client.connected()) {
    connectMQTT();
  }
  client.loop();  // Ensure MQTT is being handled
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
  doc["version"] = storedVersion;
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
