
void setup() {
  
  Serial.begin(115200);

  pinMode(ledPin, OUTPUT);
  for (int i = 0; i < NUM_OUTPUTS; i++) {
    pinMode(outputPins[i], OUTPUT);
    digitalWrite(outputPins[i], LOW);  // Ensure all outputs start LOW
  }

  clientId = "ESP32_" + String(WiFi.macAddress());
  loadWiFiCredentials();

  // Print the loaded credentials
  if (ssid.isEmpty() || password.isEmpty()) {
    Serial.println("WiFi credentials not found. Setting default values.");
    //ssid = "TELUSDE0875_2.4G";
    //password = "3X3K22832E";
    //storedVersion = "1.3.4g";
  }
  
  Serial.println("Loaded WiFi credentials: SSID=" + ssid + ", Password=" + password + ", Version=" + storedVersion);
  
  // Simulate the version checking and saving
  connectWiFi();
  setupDashboard();
  connectMQTT();

  preferences.begin("firmware", false);
  delay(3000);
  checkForUpdates();
  sendMQTTPayload();  // Send initial MQTT message when connected

  oscilliscopeSetUp();
}

void loop() {

  clientMQTTConnected();
  firmwareUpdates();
  handlePatterns();
  oscilliscopeLoop();
  
  // Monitoring the audio pin
  int audioState = digitalRead(ledPin);
  if (audioState == HIGH) {
    audioPinStateText = "ON";
  } else {
    audioPinStateText = "OFF";
  }
}
