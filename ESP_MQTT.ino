
// Node-Red User Login (Device Login - Raspberry Pi)
const char* mqttUserName = "pro1polaris";
const char* mqttPassword = "CVr819P*!";

void connectMQTT() {
  client.setServer(MQTT_SERVER, MQTT_PORT);
  client.setCallback(mqttCallback);  // Set MQTT callback function
  Serial.println("MQTT Callback should be set");

  unsigned long startAttemptTime = millis();
  const unsigned long retryInterval = 5000;  // Retry interval (in milliseconds)
  const unsigned long timeout = 30000;       // Timeout after 30 seconds

  while (!client.connected() && millis() - startAttemptTime < timeout) {
    Serial.print("Connecting to MQTT...");
    if (client.connect(clientId.c_str(), mqttUserName, mqttPassword)) {
      Serial.println("Connected to MQTT.");
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
    StaticJsonDocument<1024> doc;  // Adjust as needed
    doc["mac"] = WiFi.macAddress();
    doc["puzzleName"] = PUZZLE_NAME;
    doc["designer"] = DESIGNER_NAME;
    doc["tech"] = TECH_NAME;
    doc["ip"] = WiFi.localIP().toString();
    //doc["timestamp"] = millis();
    doc["tab"] = NR_TAB;
    doc["group"] = NR_GROUP;
    doc["type"] = NR_TYPE;
    doc["version"] = storedVersion;
    //doc["outputs"] = NUM_OUTPUTS;
    //doc["inputs"] = NUM_INPUTS;
    doc["leds"] = NUM_LEDS;
    doc["chan"] = NUM_CHANNELS;

    String jsonPayload;
    serializeJson(doc, jsonPayload);
    jsonPublished = jsonPayload;

    Serial.print("Payload size: ");
    Serial.println(jsonPayload.length());
    Serial.print("MQTT Buffer Size: ");
    Serial.println(MQTT_MAX_PACKET_SIZE);

    if (jsonPayload.length() > MQTT_MAX_PACKET_SIZE) {
        Serial.println("Error: Payload size exceeds MQTT buffer size.");
        return;
    }
    
    publishDataMQTTPayload_Doc(jsonPayload);
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.println(topic);

  char message[length + 1];
  memcpy(message, payload, length);
  message[length] = '\0';

  StaticJsonDocument<256> doc;
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

    String activityString = String(activity);
    if (activityString == (String(NR_GROUP) + " Button Pressed")) { 
      executeGPIOBatch1();
    } else if (activityString == (String(NR_GROUP) + " Button 2 Pressed")) { 
      executeGPIOBatch2();
    } else if (activityString == (String(NR_GROUP) + " Button 3 Pressed")) {
      executeGPIOBatch3();
    }

    activityString = "";
  } else {
    Serial.println("Key 'activity' not found");
  }

  // Debugging: Print the entire JSON message
  String jsonMessage;
  serializeJson(doc, jsonMessage);
  Serial.println("Message contents:");
  Serial.println(jsonMessage);
}
