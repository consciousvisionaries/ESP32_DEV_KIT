#include <WiFi.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <Update.h>
#include <PubSubClient.h>
#include <esp_task_wdt.h>

#define FIRMWARE_VERSION "V1.1"

WiFiClient espClient;
PubSubClient client(espClient);

String jsonPublished;
bool allServicesActive = false;

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
  String mqttUsername = "pro1polaris";
  String mqttPassword = "CVr819P*!";
  String mqttServer = "192.168.0.129";
};

MQTTSettings mqttSettings;

struct GlobalSettings {
  String nrTab = "PRESIDENTS BIG MISTAKE";
  String nrGroup = "Stage 1";

  String inputNames[8] = { "LEVERS", "DIALS", "OPEN DOOR", "PATCH PANEL", "MAG BALL (SOUND)", "TELEPHONE", "READY SELF DISTRUCT", "DISTRUCT MISSLE" };
  String outputNames[NUM_OUTPUTS] = { "LEVERS", "DIALS", "OPEN DOOR", "PATCH PANEL", "BALL & MAG", "TELEPHONE", "READY SELF DISTRUCT", "DISTRUCT MISSLE" };
};                                  

GlobalSettings globalSettings;

struct GlobalHyperlinks {
  String adminButtonHTML = "";
  String homeButtonHTML = "";
};

GlobalHyperlinks globalHyperlinks;

void setup() {
  setupFirmware();
  setupDashboard();
  setupGPIO();
  setupFASTLED();
  Serial.println("READY.");
}

void loop() {
  clientMQTTConnected();
  loopFIRMWARE();
  loopGPIO();
  loopFASTLED();
}

void setupESPTask() {
  esp_task_wdt_config_t wdt_config = {
      .timeout_ms = 10000,
      .idle_core_mask = (1 << 0),
      .trigger_panic = true
  };
  esp_task_wdt_init(&wdt_config);
  esp_task_wdt_add(NULL);
}

void loopESPTask() {
  esp_task_wdt_reset();
}

void setupFirmware() {
  Serial.begin(115200);
  loadWiFiCredentials();
  connectWiFi();
  prefLoadAllSettings();
  connectMQTT();
  sendConfigMQTTPayload();
  delay(3000);
  checkForUpdates();
}

void connectWiFi() {
  bool connected = false;

  for (int i = -1; i < 3; i++) {
    Serial.println("Attempting WIFI Loop #: " + String(i));
    if (i != -1) {
      wifiSettings.ssid = wifiSettings.bup_ssid[i];
      wifiSettings.password = wifiSettings.bup_password[i];
      Serial.println("Trying backup credentials...");
    } else {
      Serial.println("Trying stored credentials...");
    }

    Serial.printf("Connecting to WiFi: %s\n", wifiSettings.ssid.c_str());
    Serial.printf("WiFi Password: %s\n", wifiSettings.password.c_str());
    WiFi.begin(wifiSettings.ssid.c_str(), wifiSettings.password.c_str());

    unsigned long startAttemptTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
      delay(1000);
      Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\nWiFi connected.");
      Serial.print("IP Address: ");
      Serial.println(WiFi.localIP());
      wifiSettings.ipaddress = WiFi.localIP().toString();
      connected = true;
      break;
    } else {
      Serial.println("\nWiFi connection failed. Trying next network...");
      delay(1000);
    }
  }

  if (!connected) {
    Serial.println("No Wi-Fi connection established. Starting Access Point...");
    WiFi.softAP(AP_SSID, AP_PASSWORD);
    Serial.print("Access Point IP Address: ");
    Serial.println(WiFi.softAPIP());
  }
}

unsigned long lastUpdateCheck = 0; // Global variable to track the last update check time
const unsigned long updateInterval = 3600000; // 1 hour in milliseconds

void loopFIRMWARE() {
  // Check if it's time to perform a firmware update check
  if (millis() - lastUpdateCheck >= updateInterval) {
    Serial.println("Checking for firmware updates...");
    checkForUpdates();
    lastUpdateCheck = millis(); // Reset the timer
  }
}

void checkForUpdates() {
  HTTPClient http;
  String versionURL = String("https://raw.githubusercontent.com/") +
                      String(GITHUB_USER) + "/" +
                      String(GITHUB_REPO) + "/" +
                      String(GITHUB_BRANCH) + "/" +
                      "version.txt";

  Serial.println("Fetching version from URL: " + versionURL);
  http.begin(versionURL);
  int httpCode = http.GET();
  if (httpCode == 200) {
    String newVersion = http.getString();
    newVersion.trim();

    if (newVersion != wifiSettings.storedVersion) {
      Serial.println("New version available: " + newVersion);
      if (updateFirmware(newVersion)) {
        saveWiFiCredentials(wifiSettings.ssid, wifiSettings.password, newVersion);
        Serial.println("Update complete. Rebooting...");
        delay(5000);
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
  String firmwareURL = String("https://raw.githubusercontent.com/") +
                       String(GITHUB_USER) + "/" +
                       String(GITHUB_REPO) + "/" +
                       String(GITHUB_BRANCH) + "/" +
                       String(GITHUB_BIN);

  Serial.println("Fetching firmware from URL: " + firmwareURL);
  http.begin(firmwareURL);
  int httpCode = http.GET();

  if (httpCode == 200) {
    int contentLength = http.getSize();
    if (Update.begin(contentLength)) {
      WiFiClient* client = http.getStreamPtr();
      size_t written = Update.writeStream(*client);

      if (written == contentLength) {
        Serial.println("Firmware written successfully.");
        if (Update.end() && Update.isFinished()) {
          return true;
        } else {
          Serial.println("Update not finished.");
        }
      } else {
        Serial.println("Firmware write failed.");
      }
    } else {
      Serial.println("Not enough space for update.");
    }
  } else {
    Serial.println("Failed to download firmware. HTTP code: " + String(httpCode));
  }

  http.end();
  return false;
}
