

// Function to save Wi-Fi credentials
void saveWiFiCredentials(const String& newSSID, const String& newPassword, const String& newtxtVersion) {
  preferences.begin("wifi", false);
  preferences.clear();  // Clear previous settings
  preferences.end();

  preferences.begin("wifi", false);
  preferences.putString("ssid", newSSID);
  preferences.putString("password", newPassword);
  preferences.putString("versiontxt", newtxtVersion);
  
  // Save backup SSIDs and passwords
  for (int i = 0; i < 2; i++) {
    preferences.putString("bup_ssid" + String(i), wifiSettings.bup_ssid[i]);
    preferences.putString("bup_password" + String(i), wifiSettings.bup_password[i]);
  }
  
  preferences.end();
}

// Function to load Wi-Fi credentials
void loadWiFiCredentials() {
  preferences.begin("wifi", true);
  
  wifiSettings.ssid = preferences.getString("ssid", wifiSettings.ssid);
  wifiSettings.password = preferences.getString("password", wifiSettings.password);
  wifiSettings.storedVersion = preferences.getString("versiontxt", wifiSettings.storedVersion);
  
  // Load backup SSIDs and passwords
  for (int i = 0; i < 2; i++) {
    wifiSettings.bup_ssid[i] = preferences.getString("bup_ssid" + String(i), wifiSettings.bup_ssid[i]);
    wifiSettings.bup_password[i] = preferences.getString("bup_password" + String(i), wifiSettings.bup_password[i]);
  }
  
  preferences.end();
}

// Function to save MQTT settings
void prefSaveMQTTSettings() {
  preferences.begin("mqtt", false);
  preferences.putString("mqttUsername", mqttSettings.mqttUsername);
  preferences.putString("mqttPassword", mqttSettings.mqttPassword);
  preferences.putString("mqttServer", mqttSettings.mqttServer);
  
  // Save backup MQTT settings
  for (int i = 0; i < 3; i++) {
    preferences.putString("bup_mqttServer" + String(i), mqttSettings.bup_mqttServer[i]);
    preferences.putString("bup_mqttUser" + String(i), mqttSettings.bup_mqttUser[i]);
    preferences.putString("bup_mqttPassword" + String(i), mqttSettings.bup_mqttPassword[i]);
  }

  preferences.end();
}

// Function to load MQTT settings
void prefLoadMQTTSettings() {
  preferences.begin("mqtt", true);
  
  mqttSettings.mqttUsername = preferences.getString("mqttUsername", mqttSettings.mqttUsername);
  mqttSettings.mqttPassword = preferences.getString("mqttPassword", mqttSettings.mqttPassword);
  mqttSettings.mqttServer = preferences.getString("mqttServer", mqttSettings.mqttServer);

  // Load backup MQTT settings
  for (int i = 0; i < 3; i++) {
    mqttSettings.bup_mqttServer[i] = preferences.getString("bup_mqttServer" + String(i), mqttSettings.bup_mqttServer[i]);
    mqttSettings.bup_mqttUser[i] = preferences.getString("bup_mqttUser" + String(i), mqttSettings.bup_mqttUser[i]);
    mqttSettings.bup_mqttPassword[i] = preferences.getString("bup_mqttPassword" + String(i), mqttSettings.bup_mqttPassword[i]);
  }

  preferences.end();
}

// Function to save Global settings
void prefSaveGlobalSettings() {
  preferences.begin("global", false);
  preferences.putString("nrTab", globalSettings.nrTab);
  preferences.putString("nrGroup", globalSettings.nrGroup);

  // Save input and output names
  for (int i = 0; i < 8; i++) {
    preferences.putString("inputName" + String(i), globalSettings.inputNames[i]);
    preferences.putString("outputName" + String(i), globalSettings.outputNames[i]);
  }

  preferences.end();
}

// Function to load Global settings
void prefLoadGlobalSettings() {
  preferences.begin("global", true);
  
  globalSettings.nrTab = preferences.getString("nrTab", globalSettings.nrTab);
  globalSettings.nrGroup = preferences.getString("nrGroup", globalSettings.nrGroup);

  // Load input and output names
  for (int i = 0; i < 8; i++) {
    globalSettings.inputNames[i] = preferences.getString("inputName" + String(i), globalSettings.inputNames[i]);
    globalSettings.outputNames[i] = preferences.getString("outputName" + String(i), globalSettings.outputNames[i]);
  }

  preferences.end();
}

// Function to load Hyperlinks based on Wi-Fi IP address
void prefLoadGlobalHyperlinks() {
  globalHyperlinks.homeButtonHTML = "<a href='http://" + wifiSettings.ipaddress + "/'>Home on " + wifiSettings.ipaddress + "</a>";
  globalHyperlinks.adminButtonHTML = "<a href='http://" + wifiSettings.ipaddress + "/admin'>Admin</a>";
}

// Function to load all settings at once
void prefLoadAllSettings() {
  prefLoadMQTTSettings();
  prefLoadGlobalSettings();
  prefLoadGlobalHyperlinks();
}

// Setup function to initialize preferences and load settings
void setupPREF() {

  Serial.begin(115200);

  // Initialize Preferences
  preferences.begin("wifi", true);
  preferences.end();

  preferences.begin("mqtt", true);
  preferences.end();

  preferences.begin("global", true);
  preferences.end();

  // Load all settings after initialization
  prefLoadAllSettings();

  // Now you can use the loaded settings, e.g., connect to Wi-Fi, MQTT, etc.
  Serial.println("WiFi SSID: " + wifiSettings.ssid);
  Serial.println("MQTT Server: " + mqttSettings.mqttServer);
  Serial.println("Global Tab: " + globalSettings.nrTab);
}
