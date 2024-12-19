
void saveWiFiCredentials(const String& newSSID, const String& newPassword, const String& newVersion) {
  preferences.begin("wifi-config", false); // Initialize namespace
  preferences.putString("ssid", newSSID);
  preferences.putString("password", newPassword);
  preferences.putString("version", newVersion);
  preferences.end();
  Serial.println("WiFi credentials saved.");
}

void loadWiFiCredentials() {
  preferences.begin("wifi-config", true); // Initialize namespace
  if (ssid.isEmpty() || password.isEmpty()) {
    ssid = preferences.getString("ssid", "");
    password = preferences.getString("password", "");
    storedVersion = preferences.getString("version", "");
  }
  preferences.end(); // Close preferences
  if (ssid.isEmpty() || password.isEmpty()) {
    Serial.println("WiFi credentials not found.");
  } else {
    Serial.println("Loaded WiFi credentials: SSID=" + ssid + ", Password=" + password);
  }
}

void connectWiFi() {
  Serial.printf("Connecting to WiFi: %s\n", ssid);
    Serial.printf(" WiFi Password: %s\n", password);
  WiFi.begin(ssid, password);
  unsigned long startAttemptTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
    delay(500);
    Serial.print(".");
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected.");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nWiFi connection failed. Starting Access Point...");
    WiFi.softAP("ESP32_AccessPoint", "12345678");
    Serial.print("Access Point IP Address: ");
    Serial.println(WiFi.softAPIP());
  }
}

void firmwareUpdates() {
  static unsigned long lastOTA = 0;
  if (millis() - lastOTA > 3600000) {  // Check for updates every hour
    lastOTA = millis();
    checkForUpdates();
  }
}

void checkForUpdates() {
  
  HTTPClient http;
  String firmwareURL = "https://raw.githubusercontent.com/" + String(githubUser) + "/" + String(githubRepo) + "/" + String(branch) + "/" + String(firmwareFile);
  String versionURL = "https://raw.githubusercontent.com/" + String(githubUser) + "/" + String(githubRepo) + "/" + String(branch) + "/version.txt";  
  http.begin(versionURL);
  int httpCode = http.GET();
  if (httpCode == HTTP_CODE_OK) {
    String payload = http.getString();
    String newFWVersion = extractVersionFromPayload(payload);

    Serial.print("Fetching version from URL: ");
Serial.println(versionURL);
Serial.print("HTTP Response Code: ");
Serial.println(httpCode);
Serial.print("Payload: ");
Serial.println(payload);

    // Get stored version from Preferences
    Serial.print("Current stored version: ");
    Serial.println(storedVersion);
    Serial.print("New version available: ");
    Serial.println(newFWVersion);

    if (newFWVersion != storedVersion) {
      Serial.println("New firmware update found. Updating...");
      performFirmwareUpdate(firmwareURL,newFWVersion);  // Perform the update
    } else {
      Serial.println("Firmware is up to date.");
    }
  } else {
    Serial.println("Failed to fetch update information.");
  }

  http.end();
}


String extractVersionFromPayload(String payload) {
    Serial.print("Raw payload: ");
    Serial.println(payload);
    // Adjust parsing logic here if needed
    return payload; // Assuming the version is directly the payload
}

void performFirmwareUpdate(String firmwareUrl, String newversion) {
  HTTPClient http;
  http.begin(firmwareUrl);
  int httpCode = http.GET();
  
  if (httpCode == HTTP_CODE_OK) {
    int contentLength = http.getSize();
    
    if (Update.begin(contentLength)) {
      WiFiClient *client = http.getStreamPtr();
      size_t written = Update.writeStream(*client);
      
      if (written == contentLength) {
        Serial.println("Update complete. Rebooting...");
        Update.end();

        // Store the new version in Preferences
        saveWiFiCredentials(ssid, password, newversion);
        delay(2000);
        ESP.restart();
        
      } else {
        
        Serial.println("Update failed.");
        Update.end();
      }
    } else {
      Serial.println("Not enough space for update.");
    }
  } else {
    Serial.println("Failed to fetch firmware update.");
  }
  http.end();
}

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
