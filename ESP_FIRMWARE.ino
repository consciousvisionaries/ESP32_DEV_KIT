
// Function to save WiFi credentials (SSID, password) and version in Preferences
void saveWiFiCredentials(const String& newSSID, const String& newPassword, const String& newVersion) {
  preferences.begin("wifi-config", false); // Open namespace for writing
  
  // Store each piece separately
  preferences.putString("ssid", newSSID);
  preferences.putString("password", newPassword);
  preferences.putString("versiontxt", newVersion);
  
  preferences.end();
  
  Serial.println("WiFi credentials and version saved.");
}

// Function to load WiFi credentials and version from Preferences
void loadWiFiCredentials() {
  preferences.begin("wifi-config", true); // Open namespace for reading
  
  ssid = preferences.getString("ssid", "");
  password = preferences.getString("password", "");
  storedVersion = preferences.getString("versiontxt", "");
  
  preferences.end();
  
  if (ssid.isEmpty() || password.isEmpty()) {
    Serial.println("WiFi credentials not found.");
  } else {
    Serial.println("Loaded WiFi credentials: SSID=" + ssid + ", Password=" + password + ", Version=" + storedVersion);
  }
}

// Function to simulate checking for a version update and saving it if a new version is found
void checkForVersionUpdate() {
  String currentVersion = "1.3.4d"; // Assume current version is this
  String newVersion = "1.3.4e";     // For testing, we simulate a version update
  
  if (newVersion != storedVersion) {
    Serial.println("New version available: " + newVersion);
    Serial.println("Updating firmware...");
    
    // Simulate some update time (2 seconds delay for demonstration)
    delay(2000);
    
    // Save the new version
    saveWiFiCredentials(ssid, password, newVersion);
    
    Serial.println("Update complete. Rebooting...");
    delay(1000);
    ESP.restart();
  } else {
    Serial.println("Device firmware is up-to-date.");
  }
}

void connectWiFi() {
  Serial.printf("Connecting to WiFi: %s\n", ssid.c_str());
  Serial.printf("WiFi Password: %s\n", password.c_str());
  WiFi.begin(ssid.c_str(), password.c_str());

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
  String versionURL = "https://raw.githubusercontent.com/" + String(githubUser) + "/" + String(githubRepo) + "/" + String(branch) + "/version.txt";

  Serial.println("Fetching version from URL: " + versionURL);
  http.begin(versionURL);
  int httpCode = http.GET();

  if (httpCode == 200) {
    String newVersion = http.getString();
    newVersion.trim();
    Serial.println("HTTP Response Code: " + String(httpCode));
    Serial.println("Payload: " + newVersion);

    if (newVersion != storedVersion) {
      Serial.println("New version available: " + newVersion);
      Serial.println("Updating firmware...");
      if (updateFirmware(newVersion)) {
        saveWiFiCredentials(ssid, password, newVersion);
        Serial.println("Update complete. Rebooting...");
        ESP.restart();
      } else {
        Serial.println("Firmware update failed.");
      }
    } else {
      Serial.println("Device firmware is up-to-date.");
    }
  } else {
    Serial.println("Failed to fetch version info. HTTP code: " + String(httpCode));
  }
  http.end();
}

bool updateFirmware(const String& newVersion) {
  HTTPClient http;
  String firmwareURL = "https://raw.githubusercontent.com/" + String(githubUser) + "/" + String(githubRepo) + "/" + String(branch) + "/" + String(firmwareFile);
  Serial.println("Fetching firmware from URL: " + firmwareURL);

  http.begin(firmwareURL);
  int httpCode = http.GET();

  if (httpCode == 200) {
    int contentLength = http.getSize();
    bool canBegin = Update.begin(contentLength);

    if (canBegin) {
      WiFiClient* client = http.getStreamPtr();
      size_t written = Update.writeStream(*client);

      if (written == contentLength) {
        Serial.println("Firmware written successfully.");
      } else {
        Serial.println("Firmware write failed. Written: " + String(written) + ", Expected: " + String(contentLength));
        return false;
      }

      if (Update.end()) {
        if (Update.isFinished()) {
          Serial.println("Update successfully completed.");
          return true;
        } else {
          Serial.println("Update not finished. Something went wrong.");
          return false;
        }
      } else {
        Serial.println("Update failed. Error #: " + String(Update.getError()));
        return false;
      }
    } else {
      Serial.println("Not enough space for update.");
      return false;
    }
  } else {
    Serial.println("Failed to download firmware. HTTP code: " + String(httpCode));
    return false;
  }
  http.end();
  return false;
}