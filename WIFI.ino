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
