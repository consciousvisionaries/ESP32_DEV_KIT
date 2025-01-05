#define MQTT_MAX_PACKET_SIZE 225  // Adjust size as needed
#define MQTT_PORT 1883
#define MQTT_VERSION V1.2

void connectMQTT() {
  client.setServer(mqttSettings.mqttOneServer, MQTT_PORT);
  client.setCallback(mqttCallback);  // Set MQTT callback function

  const char* mac = WiFi.macAddress().c_str();

  Serial.println("MQTT Callback should be set");

  unsigned long startAttemptTime = millis();
  const unsigned long retryInterval = 5000;  // Retry interval (in milliseconds)
  const unsigned long timeout = 10000;       // Timeout after 10 seconds

  String MQTT_CLIENT_ID = WiFi.macAddress();

  while (!client.connected() && millis() - startAttemptTime < timeout) {
    Serial.print("Connecting to MQTT...");
    if (client.connect(MQTT_CLIENT_ID.c_str(), mqttSettings.mqttOneUser, mqttSettings.mqttOnePassword)) {
      Serial.println("Connected to MQTT.ONE > SERVER");
    
      
      if (client.subscribe(mqttSettings.mqttOneTopic)) {
      Serial.println("Successfully subscribed to topic: " + String(mqttSettings.mqttOneTopic));
      } else {
      Serial.println("Failed to subscribe to topic.");
      }

    } else {
      Serial.print("Failed (state=");
      Serial.print(client.state());
      Serial.println("). Retrying in 5 seconds...");
      delay(retryInterval);
    }
  }

  if (!client.connected()) {
    Serial.println("MQTT connection failed.");
    connectBrokerMQTT();
  }
}

void connectBrokerMQTT() {
  client.setServer(mqttSettings.mqttBrokerServer.c_str(), MQTT_PORT);
  client.setCallback(mqttCallback);  // Set MQTT callback function
  Serial.println("MQTT Callback should be set");

  unsigned long startAttemptTime = millis();
  const unsigned long retryInterval = 5000;  // Retry interval (in milliseconds)
  const unsigned long timeout = 30000;       // Timeout after 30 seconds

  String MQTT_CLIENT_ID = WiFi.macAddress();

  while (!client.connected() && millis() - startAttemptTime < timeout) {
    Serial.print("Connecting to MQTT...");
    if (client.connect(MQTT_CLIENT_ID.c_str())) {
      Serial.println("Connected to BROKER");
      client.subscribe(mqttSettings.mqttOneTopic);  // Subscribe to the topic
    } else {
      Serial.print("Failed (state=");
      Serial.print(client.state());
      Serial.println("). Retrying in 5 seconds...");
      delay(retryInterval);
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
  if (client.publish(mqttSettings.mqttOneTopic, jsonPayload.c_str())) {
    Serial.println("Data sent topic: " + String(mqttSettings.mqttOneTopic));
    Serial.println(jsonPayload);
  } else {
    Serial.println("Failed to send data.");
  }
}

void sendJsonDocUpdateMQTTPayload(String jsonPayload1) {
  DynamicJsonDocument doc(MQTT_MAX_PACKET_SIZE);

  doc["mac"] = WiFi.macAddress();
  doc["puzzleName"] = PUZZLE_NAME;
  doc["designer"] = DESIGNER_NAME;
  doc["tech"] = TECH_NAME;
  doc["ip"] = WiFi.localIP().toString();
  doc["timestamp"] = millis();
  doc["tab"] = globalSettings.nrTab;
  doc["group"] = globalSettings.nrGroup;
  doc["type"] = NR_TYPE;

  String jsonPayload;
  serializeJson(doc, jsonPayload);
  publishDataMQTTPayload_Doc(jsonPayload);
  delay(50);

  publishDataMQTTPayload_Doc(jsonPayload1);
  delay(50);
}

void sendMessageUpdateMQTTPayload(String message) {
  DynamicJsonDocument doc(512);
  doc["mac"] = WiFi.macAddress();
  doc["puzzleName"] = PUZZLE_NAME;
  doc["message"] = message;

  String jsonPayload;
  serializeJson(doc, jsonPayload);
  sendJsonDocUpdateMQTTPayload(jsonPayload);
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  
  Serial.print("Message arrived on topic: ");
  Serial.println(topic);

  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  DynamicJsonDocument doc(512);
  DeserializationError error = deserializeJson(doc, message);
  if (error) {
    Serial.print("JSON parse error: ");
    Serial.println(error.c_str());
    return;
  }

  if (doc.containsKey("activity")) {
    String activity = doc["activity"].as<String>();
    Serial.print("Activity: ");
    Serial.println(activity);

    if (activity == "getFirmwareUpdate") {
      sendMessageUpdateMQTTPayload("Firmware Status Request Confirmed from: " + String(PUZZLE_NAME));
    } else if (activity == (String(NR_GROUP) + " Button Pressed")) { 
      executeGPIOBatchPin1();
    } else if (activity == (String(NR_GROUP) + " Button 2 Pressed")) { 
      executeGPIOBatchPin2();
    } else {
      executeMQTTBatchBelow(activity);
    }
  } else {
    Serial.println("Key 'activity' not found");
  }
}

void executeMQTTBatchBelow(String activity) {
  Serial.println("Key 'activity': " + activity);
}
