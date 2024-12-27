#define MQTT_TOPIC "/lost"
#define MQTT_SERVER "192.168.0.129" // Replace with your MQTT broker IP
#define MQTT_PORT 1883

// Node-Red User Login (Device Login - Raspberry Pi)
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

    if (client.publish(MQTT_TOPIC, jsonPayload.c_str())) {
        Serial.println("Data sent successfully:");
        Serial.println(jsonPayload);
    } else {
        Serial.println("Failed to send CONFIG data.");
        Serial.print("MQTT Client State: ");
        Serial.println(client.state());
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

    // Extract the value of the 'activity' key
    const char* activity = doc["activity"];

   
    // Access and print the key
    if (doc.containsKey("activity")) {
        const char* activity = doc["activity"];
        Serial.print("Activity: ");
        Serial.println(activity);

        if (String(activity) == (String(NR_GROUP) + " Button Pressed")) { 
                    executeBatch1();
        }
        if (String(activity) == (String(NR_GROUP) + " Button 2 Pressed")) { 
                    executeBatch2();
        }
    } else {
        Serial.println("Key 'activity' not found");
    }

    // Convert the JSON document to a string
    String jsonMessage;
    serializeJson(doc, jsonMessage);
    Serial.println("Message contents:");
    Serial.println(jsonMessage);
  }
}
