
void setup() {
  
  Serial.begin(115200);

  pinMode(ledPin, OUTPUT);
  for (int i = 0; i < NUM_OUTPUTS; i++) {
    pinMode(outputPins[i], OUTPUT);
    digitalWrite(outputPins[i], LOW);  // Ensure all outputs start LOW
  }

  clientId = "ESP32_" + String(WiFi.macAddress());
  //saveWiFiCredentials(ssid,password,storedVersion);
  loadWiFiCredentials();

  // Print the loaded credentials
  if (ssid.isEmpty() || password.isEmpty()) {
    Serial.println("WiFi credentials not found. Setting default values.");
    ssid = "yourSSID";
    password = "yourPassword";
  }
  
  Serial.println("Loaded WiFi credentials: SSID=" + ssid + ", Password=" + password);
  
  // Simulate the version checking and saving
  checkForVersionUpdate();
  connectWiFi();
  setupDashboard();
  connectMQTT();

  preferences.begin("firmware", false);
  delay(3000);
  checkForUpdates();
  sendMQTTPayload();  // Send initial MQTT message when connected
}

void loop() {

  clientMQTTConnected();
  firmwareUpdates();
  handlePattern(millis());
}
