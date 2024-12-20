
void setup() {

 saveWiFiCredentials("TELUSDE0875_2.4G","3X3K22832E","1.1.1")
 ;
  
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
    ssid = "TELUSDE0875_2.4G";
    password = "3X3K22832E";
  }
  
  Serial.println("Loaded WiFi credentials: SSID=" + ssid + ", Password=" + password);
  
  // Simulate the version checking and saving
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
