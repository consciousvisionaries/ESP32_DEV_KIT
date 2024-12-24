#include <Preferences.h>

// Function to save WiFi credentials (SSID, password) and version in Preferences
void saveWiFiCredentials(const String& newSSID, const String& newPassword, const String& newtxtVersion) {
  preferences.begin("settings", false); // Open namespace for writing
  preferences.clear(); // Clears all preferences in the "settings" namespace
  preferences.end();
  preferences.begin("settings", false);
  Serial.println("Saving Version: " + newtxtVersion);
  // Store each piece separately
  preferences.putString("ssid", newSSID);
  preferences.putString("password", newPassword);
  preferences.putString("versiontxt", newtxtVersion);
  
  String savedVersion = preferences.getString("versiontxt", "");
  if (savedVersion == newtxtVersion) {
    Serial.println("    Version saved successfully." + savedVersion);
  } else {
    Serial.println("    Version save failed.");
  }

  preferences.end(); // Close namespace
}

// Function to load WiFi credentials and version from Preferences
void loadWiFiCredentials() {
  preferences.begin("settings", true); // Open namespace for reading
  if (ssid == "") {
  ssid = preferences.getString("ssid", "");
  password = preferences.getString("password", "");  
  }
  
  storedVersion = preferences.getString("versiontxt", "");
  
  preferences.end(); // Close namespace
  
  if (ssid.isEmpty() || password.isEmpty()) {
    Serial.println("Loading WiFi... credentials not found.");
  } else {
    Serial.println("Loaded WiFi credentials: SSID=" + ssid + ", Password=" + password + ", Version=" + storedVersion);
  }
}

void connectWiFi() {
  Serial.printf("Connecting to WiFi: %s\n", ssid.c_str());
  Serial.printf("    WiFi Password: %s\n", password.c_str());
  WiFi.begin(ssid.c_str(), password.c_str());

  unsigned long startAttemptTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
    delay(500);
    Serial.print(".");
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("    \nWiFi connected.");
    Serial.print("    IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("    \nWiFi connection failed. Starting Access Point...");
    WiFi.softAP("ESP32_AccessPoint", "12345678");
    Serial.print("    Access Point IP Address: ");
    Serial.println(WiFi.softAPIP());
  }
}

void firmwareUpdates() {
  static unsigned long lastOTA = 0;
  if (millis() - lastOTA > 3600000) {  // Check for updates every hour
    Serial.println("Last OTA > " + lastOTA);
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
    Serial.println("    HTTP Response Code: " + String(httpCode));
    Serial.println("    Payload: " + newVersion);

    if (newVersion != storedVersion) {
      Serial.println("    New version available: " + newVersion);
      Serial.println("    Updating firmware...");
      if (updateFirmware(newVersion)) {
        saveWiFiCredentials(ssid, password, newVersion);
        Serial.println("    Update complete. Rebooting..."+ newVersion);
        delay(5000);
        ESP.restart();
      } else {
        Serial.println("    Firmware update failed.");
      }
    } else {
      Serial.println("    Device firmware is up-to-date.");
    }
  } else {
    Serial.println("    Failed to fetch version info. HTTP code: " + String(httpCode));
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
        Serial.println("    Firmware written successfully.");
        
      } else {
        Serial.println("    Firmware write failed. Written: " + String(written) + ", Expected: " + String(contentLength));
        return false;
      }

      if (Update.end()) {
        if (Update.isFinished()) {
          Serial.println("    Update successfully completed.");
          return true;
        } else {
          Serial.println("    Update not finished. Something went wrong.");
          return false;
        }
      } else {
        Serial.println("    Update failed. Error #: " + String(Update.getError()));
        return false;
      }
    } else {
      Serial.println("     Not enough space for update.");
      return false;
    }
  } else {
    Serial.println("    Failed to download firmware. HTTP code: " + String(httpCode));
    return false;
  }
  http.end();
  return false;
}
