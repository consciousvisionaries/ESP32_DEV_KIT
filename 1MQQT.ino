#define MQTT_MAX_PACKET_SIZE 225  // Adjust size as needed
#define MQTT_PORT 1883
#define MQTT_VERSION V1.2

#define MAX_PENDING_MESSAGES 10  // Maximum number of sent messages we can store


bool isSubscribed = false;  // Flag to track subscription status
unsigned long lastReconnectAttempt = 0;


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
  
  client.loop();  // Ensure MQTT is being handled
  checkMessageTimeout();  // Check for message timeouts
  handleMQTTConnection();
}


// Array to store messages sent
String sentMessages[MAX_PENDING_MESSAGES];
unsigned long sentTimes[MAX_PENDING_MESSAGES];  // Array to store the time messages were sent
bool messageConfirmed[MAX_PENDING_MESSAGES];     // Flags for confirming message reception

// Function to publish data via MQTT and store the message
void publishDataMQTTPayload_Doc(String jsonPayload) {
  
  Serial.print("Payload size: ");
  Serial.println(jsonPayload.length());
  Serial.print("MQTT Buffer Size: ");
  Serial.println(MQTT_MAX_PACKET_SIZE);

  if (jsonPayload.length() > MQTT_MAX_PACKET_SIZE) {
    Serial.println("Error: Payload size exceeds MQTT buffer size.");
    return;
  }

  // Store the message and timestamp
  for (int i = 0; i < MAX_PENDING_MESSAGES; i++) {
    if (messageConfirmed[i] == true) {
      sentMessages[i] = "";
    }
    if (sentMessages[i] == "") {
      sentMessages[i] = jsonPayload;
      sentTimes[i] = millis();
      messageConfirmed[i] = false;  // Initially, no message is confirmed
      break;
    }
  }

  // Send the payload via MQTT
  if (client.publish(mqttSettings.mqttOneTopic, jsonPayload.c_str())) {

    Serial.println("Data sent topic: " + String(mqttSettings.mqttOneTopic));
    Serial.println(jsonPayload);
  } else {
    Serial.println("Failed to send data.");
  }
}

int unvalidatedMessages;


// Function to check if messages were confirmed within the timeout
void checkMessageTimeout() {
  unsigned long currentMillis = millis();

  // Check for unconfirmed messages that have timed out
  for (int i = 0; i < MAX_PENDING_MESSAGES; i++) {
    if (!messageConfirmed[i] && sentMessages[i] != "") {
      if (currentMillis - sentTimes[i] >= 5000) {  // 5 seconds timeout
        Serial.println("Message timeout: failed count == " + String(unvalidatedMessages) + " " + sentMessages[i]);
        isSubscribed = false;
        unsubscribeFromTopic();
        // Set some flag (e.g., flagTimeout) or take other action
        messageConfirmed[i] = false;  // Set the message as not confirmed
        // You can set a global flag or take other action as needed
        unvalidatedMessages++;
        sentMessages[i] = "";
        
      }
    }
  }
}

// Function to reconnect to the MQTT broker
boolean reconnect() {
  
  if (client.connect("MQTTClient")) {
    Serial.println("Connected to MQTT ONE broker.");
    if (client.subscribe(mqttSettings.mqttOneTopic)) {
      Serial.println("Reconnect... successfully subscribed to topic: " + String(mqttSettings.mqttOneTopic));
            isSubscribed = true;

    }
    return true;
  }
  return false;
}

// Function to unsubscribe from a topic
void unsubscribeFromTopic() {
  Serial.println("Unsubscribing from topic...");

  if (client.unsubscribe(mqttSettings.mqttOneTopic)) {
    Serial.println("Unsubscribe request sent.");
  } else {
    Serial.println("Failed to send unsubscribe request.");
  }
}

// Function to handle MQTT client connection and resubscription
void handleMQTTConnection() {
  
  // Check if the client is connected, if not attempt to reconnect
  if (!client.connected()) {
    if (millis() - lastReconnectAttempt > 5000) {  // Retry every 5 seconds
      Serial.println("Attempting to reconnect to MQTT broker...");
      if (reconnect()) {
        lastReconnectAttempt = millis();  // Reset reconnect attempt time
      }
    }
  } else {
    // The client is connected, handle the subscription status
    if (!isSubscribed) {
      Serial.println("Client is connected but not subscribed. Resubscribing...");
      if (client.subscribe(mqttSettings.mqttOneTopic)) {
        Serial.println("Successfully subscribed to topic: " + String(mqttSettings.mqttOneTopic));
            isSubscribed = true;

      } else {
      Serial.println("Failed to subscribe to topic.");
      }

    }
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
  
  String receivedMessage = String((char*)payload, length);

  // Check if the received message matches any sent message
  for (int i = 0; i < MAX_PENDING_MESSAGES; i++) {
    if (sentMessages[i] == receivedMessage) {
      messageConfirmed[i] = true;  // Mark message as confirmed
      Serial.println("Message server verified: " + receivedMessage);
      return;
    }
  }

  
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
