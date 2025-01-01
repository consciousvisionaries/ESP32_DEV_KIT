#include <WiFi.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <Update.h>
#include <PubSubClient.h>
#include <esp_task_wdt.h>

#define FIRMWARE_VERSION V1.1
WiFiClient espClient;
PubSubClient client(espClient);

//const char* GITHUB_USER = "consciousvisionaries";
//const char* GITHUB_REPO = "ESP32_DEV_KIT";
//const char* GITHUB_BIN = "ESP32_DEV_KIT.ino.xxxxx.bin";
//const char* GITHUB_BRANCH = "ESP32DEVKIT_8ButtonMP3Puzzle";

//String bup_ssid[] = { "TELUSDE0875_2.4G", "Beyond Belief Entertainment" };
//String bup_password[] = { "3X3K22832E", "Gary2019" };



String jsonPublished;

bool allServicesActive = false;

void setup() {
  
  setupFirmware();
  setupDashboard();

 
  //setupMP3Player();
  setupGPIO();
  setupFASTLED();
  //setupESPTask();
  Serial.println("READY.");
}

void loop() {

  clientMQTTConnected();
  loopFIRMWARE();
  loopGPIO();
  loopFASTLED();
  //loopESPTask();
}

void setupESPTask() {
  
  esp_task_wdt_config_t wdt_config = {
        .timeout_ms = 10000,  // Timeout in milliseconds (5 seconds)
        .idle_core_mask = (1 << 0), // Use Core 0
        .trigger_panic = true
    };
    esp_task_wdt_init(&wdt_config);
    esp_task_wdt_add(NULL); // Add current task
}

void loopESPTask() {
  
    esp_task_wdt_reset(); // Feed the watchdog timer
}

void setupFirmware() {
  
  Serial.begin(115200);
  
  loadWiFiCredentials();

  
  // Simulate the version checking and saving
  connectWiFi();
  
  loadAllSettings();

  connectMQTT();
  
  sendConfigMQTTPayload(); // Send initial MQTT message when connected

  delay(3000);
  checkForUpdates();

}


unsigned long startAttemptTime = millis();

void connectWiFi() {
  bool connected = false; // Flag to track connection status

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
    Serial.printf("    WiFi Password: %s\n", wifiSettings.password.c_str());
    
    WiFi.begin(wifiSettings.ssid.c_str(), wifiSettings.password.c_str());

    startAttemptTime = millis();

    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
      delay(1000);
      Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\nWiFi connected.");
      Serial.print("    IP Address: ");
      Serial.println(WiFi.localIP());
      wifiSettings.ipaddress = String(WiFi.localIP());
      Serial.println(">>>>>>>>>>>>>>>>>>>>>>");
      Serial.println(wifiSettings.ipaddress);

      connected = true; // Set connection flag
      break; // Exit the loop as Wi-Fi is successfully connected
    } else {
      Serial.println("\nWiFi connection failed. Trying next network...");
      delay(1000); // Small delay before trying next credentials
    }
  }

  // If still not connected, start Access Point mode
  if (!connected && millis() - startAttemptTime > 10000) {
    Serial.println("No Wi-Fi connection established. Starting Access Point...");
    WiFi.softAP(AP_SSID, AP_PASSWORD);
    Serial.print("    Access Point IP Address: ");
    Serial.println(WiFi.softAPIP());
  }
}



void loopFIRMWARE() {
  
}

void checkForUpdates() {
  
  HTTPClient http;
  String versionURL = String("https://raw.githubusercontent.com/") + 
                      String(GITHUB_USER) + "/" + 
                      String(GITHUB_REPO) + "/" + 
                      String(GITHUB_BRANCH) + "/" + 
                       "version.txt";
  Serial.println(versionURL);
  Serial.println("Fetching version from URL: " + versionURL);
  http.begin(versionURL);
  int httpCode = http.GET();
  if (httpCode == 200) {
    String newVersion = http.getString();
    newVersion.trim();
    Serial.println("    HTTP Response Code: " + String(httpCode));
    Serial.println("    Payload: " + newVersion);
    if (newVersion != wifiSettings.storedVersion) {
      Serial.println("    Stored Version: " + wifiSettings.storedVersion);
      Serial.println("    New version available: " + newVersion);
      Serial.println("    Updating firmware...");
      if (updateFirmware(newVersion)) {
        saveWiFiCredentials(wifiSettings.ssid, wifiSettings.password, newVersion);
        Serial.println("    Update complete. Rebooting..."+ newVersion);
        delay(5000);
        ESP.restart();
       
      } else {
        Serial.println("    Firmware update failed.");
      }
    } else {
      Serial.println("    Stored Version: " + wifiSettings.storedVersion);
      Serial.println("    Device firmware is up-to-date.");
    }
  } else {
    Serial.println("    Failed to fetch version info. HTTP code: " + String(httpCode));
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
