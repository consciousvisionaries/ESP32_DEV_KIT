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
    newFWVersion.trim();
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
  int startIndex = payload.indexOf("version\": \"");
  int endIndex = payload.indexOf("\"", startIndex + 11);
  return payload.substring(startIndex + 11, endIndex);
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
