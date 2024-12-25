#define MQTT_TOPIC "/3dialpuzzle"
#define MQTT_SERVER "192.168.0.129" // Replace with your MQTT broker IP
#define MQTT_PORT 1883

const char* mqttUserName = "pro1polaris";
const char* mqttPassword = "CVr819P*!";

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

void sendMQTTPayload() {
  StaticJsonDocument<512> doc;
  doc["mac"] = WiFi.macAddress();
  doc["puzzleName"] = "3 Dial Puzzle";
  doc["designer"] = "Paul Hopkins";
  doc["ipAddress"] = WiFi.localIP().toString();
  doc["timestamp"] = millis();
  doc["tab"] = "Lost";
  doc["group"] = "Stage 2";
  doc["version"] = storedVersion;
  doc["num_outputs"] = NUM_OUTPUTS;
  doc["num_inputs"] = NUM_INPUTS;

  String jsonPayload;
  serializeJson(doc, jsonPayload);

  if (client.publish(MQTT_TOPIC, jsonPayload.c_str())) {
    Serial.println("Puzzle details sent:");
    Serial.println(jsonPayload);
  } else {
    Serial.println("Failed to send puzzle details.");
  }
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

    

   
  }
}
