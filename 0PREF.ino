#include <Preferences.h>
Preferences preferences;

void saveWiFiCredentials(const String& newSSID, const String& newPassword, const String& newtxtVersion) {
  preferences.begin("wifi", false);
  preferences.clear();
  preferences.end();

  preferences.begin("wifi", false);

  preferences.putString("ssid", newSSID);
  preferences.putString("password", newPassword);
  preferences.putString("versiontxt", newtxtVersion);
  preferences.end();

  preferences.begin("wifi", true);
  String savedVersion = preferences.getString("versiontxt", "");
  preferences.end();
}

void loadWiFiCredentials() {
  preferences.begin("wifi", true);
  wifiSettings.ssid = preferences.getString("ssid", wifiSettings.ssid);
  wifiSettings.password = preferences.getString("password", wifiSettings.password);
  wifiSettings.storedVersion = preferences.getString("versiontxt", wifiSettings.storedVersion);
  preferences.end();
}

void prefSaveMQTTSettings() {
  preferences.begin("mqtt", false);
  preferences.putString("mqttUsername", mqttSettings.mqttUsername);
  preferences.putString("mqttPassword", mqttSettings.mqttPassword);
  preferences.putString("mqttServer", mqttSettings.mqttServer);
  preferences.end();
}

void prefLoadMQTTSettings() {
  preferences.begin("mqtt", true);
  mqttSettings.mqttUsername = preferences.getString("mqttUsername", mqttSettings.mqttUsername);
  mqttSettings.mqttPassword = preferences.getString("mqttPassword", mqttSettings.mqttPassword);
  mqttSettings.mqttServer = preferences.getString("mqttServer", mqttSettings.mqttServer);
  preferences.end();
}

void prefSaveGlobalSettings() {
  preferences.begin("global", false);
  preferences.putString("nrTab", globalSettings.nrTab);
  preferences.putString("nrGroup", globalSettings.nrGroup);

  for (int i = 0; i < 8; i++) {
    String pheader = "inputName" + String(i);
    preferences.putString(pheader.c_str(), globalSettings.inputNames[i].c_str());
    pheader = "outputName" + String(i);
    preferences.putString(pheader.c_str(), globalSettings.outputNames[i].c_str());
  }
  preferences.end();
}

void prefLoadGlobalSettings() {
  preferences.begin("global", true);
  globalSettings.nrTab = preferences.getString("nrTab", globalSettings.nrTab);
  globalSettings.nrGroup = preferences.getString("nrGroup", globalSettings.nrGroup);

  for (int i = 0; i < 8; i++) {
    String pheader = "inputName_" + String(i);
    globalSettings.inputNames[i] = preferences.getString(pheader.c_str(), globalSettings.inputNames[i].c_str());
    pheader = "outputName_" + String(i);
    globalSettings.outputNames[i] = preferences.getString(pheader.c_str(), globalSettings.outputNames[i].c_str());
  }
  preferences.end();
}

void prefLoadGlobalHyperlinks() {
  globalHyperlinks.homeButtonHTML = "<a href='http://" + wifiSettings.ipaddress + "/'>Home on " + wifiSettings.ipaddress + "</a>";
  globalHyperlinks.adminButtonHTML = "<a href='http://" + wifiSettings.ipaddress + "/admin'>Admin</a>";
}

void prefLoadAllSettings() {
  prefLoadMQTTSettings();
  prefLoadGlobalSettings();
  prefLoadGlobalHyperlinks();
}
