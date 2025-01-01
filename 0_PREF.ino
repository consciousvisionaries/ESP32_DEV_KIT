#include <Preferences.h>
Preferences preferences;

// New settings structures
struct WiFiSettings {
  String ssid = "";
  String password = "";
  String storedVersion = "";
  String ipaddress = "";
  String bup_ssid[2] = { "TELUSDE0875_2.4G", "Beyond Belief Entertainment" };
  String bup_password[2] = { "3X3K22832E", "Gary2019" };
};

WiFiSettings wifiSettings;


struct MQTTSettings {
  String mqttUserName = "pro1polaris";
  String mqttPassword = "CVr819P*!";
  String mqttServer = "192.168.0.129";
  String mqttStatus = "";
};

MQTTSettings mqttSettings;


struct GlobalSettings {
  String nrTab = "PRESIDENTS BIG MISTAKE";
  String nrGroup = "Stage 1";
  
  String inputNames[NUM_INPUTS] = { "Override Levers", "Override Dials", "Override Doors", "Override Patch Panel", 
                            "Input 5", "Input 6", "Input 7", "Input 8" };
  String outputNames[NUM_OUTPUTS] = { "Override Levers", "Override Dials", "Override Doors", "Override Patch Panel", 
                            "Output 5", "Output 6", "Output 7", "Output 8" };
  
};

GlobalSettings globalSettings;

struct GlobalHyperlinks {
  
    String buttonHTML = "";
    
};

GlobalHyperlinks globalHyperlinks;

// Function to save WiFi credentials, version, and expanded settings to Preferences
void saveWiFiCredentials(const String& newSSID, const String& newPassword, const String& newtxtVersion) {
  preferences.begin("wifi", false); // Open namespace for writing
  preferences.clear(); // Clears all preferences in the "settings" namespace
  preferences.end();
  
  preferences.begin("wifi", false);
  Serial.println("Saving Version: " + newtxtVersion);
  
  // Store each piece separately
  preferences.putString("ssid", newSSID);
  preferences.putString("password", newPassword);
  preferences.putString("versiontxt", newtxtVersion);
  preferences.putString("ipaddress", wifiSettings.ipaddress);
  preferences.end(); // Close namespace

  Serial.println("Saved:     ssid " + newSSID);
  Serial.println("Saved: password " + newPassword);

  preferences.begin("wifi", true); // Open namespace for reading

  String savedVersion = preferences.getString("versiontxt", "");
  if (savedVersion == newtxtVersion) {
    Serial.println("    Version saved successfully: " + savedVersion);
  } else {
    Serial.println("    Version save failed.");
  }

  preferences.end();
}

// Function to load WiFi credentials and version from Preferences
void loadWiFiCredentials() {
  
  preferences.begin("wifi", true); // Open namespace for reading
  wifiSettings.ssid = preferences.getString("ssid", wifiSettings.ssid);
  wifiSettings.password = preferences.getString("password", wifiSettings.password);  
  wifiSettings.storedVersion = preferences.getString("versiontxt", wifiSettings.storedVersion);
  preferences.end(); 
  
  if (wifiSettings.ssid.isEmpty() || wifiSettings.password.isEmpty()) {
    Serial.println("Loading WiFi... credentials not found.");
  } else {
    Serial.println("Loaded WiFi credentials: SSID=" + wifiSettings.ssid + ", Password=" + wifiSettings.password + ", Version=" + wifiSettings.storedVersion);
  }
}

// Function to save MQTT settings to Preferences
void saveMQTTSettings() {
  
  preferences.begin("mqtt", false); // Open namespace for MQTT settings
  preferences.putString("mqttUserName", mqttSettings.mqttUserName);
  preferences.putString("mqttPassword", mqttSettings.mqttPassword);
  preferences.putString("mqttServer", mqttSettings.mqttServer);
  preferences.end();
  Serial.println("MQTT settings saved.");
}

// Function to load MQTT settings from Preferences
void loadMQTTSettings() {
  
  preferences.begin("mqtt", true); // Open namespace for MQTT settings
  mqttSettings.mqttUserName = preferences.getString("mqttUserName", mqttSettings.mqttUserName);
  mqttSettings.mqttPassword = preferences.getString("mqttPassword", mqttSettings.mqttPassword);
  mqttSettings.mqttServer = preferences.getString("mqttServer", mqttSettings.mqttServer);
  preferences.end();
  Serial.println("MQTT settings loaded.");
}

// Function to save Global settings to Preferences
void saveGlobalSettings() {
  
  preferences.begin("global", false); // Open namespace for Global settings
  preferences.putString("nrTab", globalSettings.nrTab);
  preferences.putString("nrGroup", globalSettings.nrGroup);
  
  // Save input and output names
  for (int i = 0; i < 8; i++) {
    String pheader = "inputName" + String(i);
    preferences.putString(pheader.c_str(), globalSettings.inputNames[i].c_str());
    pheader = "outputName" + String(i);
    preferences.putString(pheader.c_str(), globalSettings.outputNames[i].c_str());
  }

  preferences.end();
  Serial.println("Global settings saved.");
}

// Function to load Global settings from Preferences
void loadGlobalSettings() {
  
  preferences.begin("global", true); // Open namespace for Global settings
  globalSettings.nrTab = preferences.getString("nrTab", globalSettings.nrTab);
  globalSettings.nrGroup = preferences.getString("nrGroup", globalSettings.nrGroup);
  
  // Load input and output names
  for (int i = 0; i < 8; i++) {
    String pheader = "inputName_" + String(i);
    globalSettings.inputNames[i] = preferences.getString(pheader.c_str(), globalSettings.inputNames[i].c_str());
    pheader = "outputName_" + String(i);
    globalSettings.outputNames[i] = preferences.getString(pheader.c_str(), globalSettings.outputNames[i].c_str());
  }

  preferences.end();
  Serial.println("Global settings loaded.");
}

void loadGlobalHyperlinks() {
    
  globalHyperlinks.buttonHTML = "<a href='http://" + wifiSettings.ipaddress + "/')\">Home on " + wifiSettings.ipaddress + "</button><p> <p><a href='http://" + wifiSettings.ipaddress + "/admin')\">Admin</a>";               

}

// Function to load all settings
void loadAllSettings() {
  
  loadMQTTSettings();
  loadGlobalSettings();
  loadGlobalHyperlinks();
  Serial.println("All settings have been loaded.");
}
