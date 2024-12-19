#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <Update.h>
#include <Preferences.h>
#include <ESPAsyncWebServer.h>


String version = "";  // Default version


String ssid = "TELUSDE0875_2.4G";   // Replace with your WiFi SSID
String password = "3X3K22832E";     // Replace with your WiFi password

const char* mqttServer = "192.168.0.129"; // Replace with your MQTT broker IP
const int mqttPort = 1883;
const char* mqttUserName = "pro1polaris";
const char* mqttPassword = "CVr819P*!";

const char* githubUser = "consciousvisionaries";
const char* githubRepo = "ESP32_DEV_KIT";
const char* firmwareFile = "ESP32_DEV_KIT2.ino.esp32.bin";
const char* branch = "ESPDEVKIT_8-OUTPUT-DRIVER"; // Branch where the firmware file is located
const char* topicData = "/christmaslights";
const int NUM_OUTPUTS = 8;

WiFiClient espClient;
PubSubClient client(espClient);
String clientId = "";
Preferences preferences;

bool allServicesActive = false;

// Define the pins
const int ledPin = 2; // GPIO2 is commonly used for the onboard LED on ESP32 boards
int outputPins[] = {12, 14, 27, 26, 33, 32, 5, 18};  // Define the GPIO pins for the 8 outputs
bool outputStates[NUM_OUTPUTS] = {false};  // Track the state of each output

// Pattern variables
String currentPattern = "chase";  // Default pattern
unsigned long patternLastTime = 0;
unsigned long lastUpdateTime = 0;
unsigned long blinkInterval = 500; 
int chaseIndex = 0;
int reverseChaseIndex = NUM_OUTPUTS - 1;

// Example output states
int waveIndex = 0;  // Declare waveIndex (or change to chaseIndex if that was the intention)

// Set up the server on port 80
AsyncWebServer server(80);

void saveVersion(const String& newVersion) {
  preferences.begin("general-config", false); // Initialize namespace
  preferences.putString("version", newVersion);
  preferences.end(); // Ensure changes are saved
  Serial.println("Version saved: " + newVersion);
}

String loadVersion() {
  preferences.begin("general-config", true); // Initialize namespace
  String newVersion = preferences.getString("version", "Not Found");
  preferences.end(); // Close preferences
  Serial.println("Loaded version: " + newVersion);
  return newVersion;
}

void saveWiFiCredentials(const String& newSSID, const String& newPassword) {
  preferences.begin("wifi-config", false); // Initialize namespace
  preferences.putString("ssid", newSSID);
  preferences.putString("password", newPassword);
  preferences.end();
  Serial.println("WiFi credentials saved.");
}

void loadWiFiCredentials() {
  preferences.begin("wifi-config", true); // Initialize namespace
  if (ssid.isEmpty() || password.isEmpty()) {
    ssid = preferences.getString("ssid", "");
    password = preferences.getString("password", "");
    preferences.end(); // Close preferences
  }
  if (ssid.isEmpty() || password.isEmpty()) {
    Serial.println("WiFi credentials not found.");
  } else {
    Serial.println("Loaded WiFi credentials: SSID=" + ssid + ", Password=" + password);
  }
}




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
      setupDashboard();
    setupServer();  

}

void setupServer() {
  
  server.on("/admin", HTTP_GET, [](AsyncWebServerRequest* request) {
    String html = R"rawliteral(
     <!DOCTYPE html>
<html>
<head>
  <title>WiFi Config</title>
  <style>
    .logo-container {
      text-align: center;
      margin-bottom: 20px;
    }
    .form-container {
      text-align: center;
    }
  </style>
</head>
<body>
  <div class="logo-container">
    <img src="mc_logo.jpg" alt="MC Logo" >
  </div>
  <div class="form-container">
    <h2>WiFi Configuration</h2>
    <form action="/setWiFi" method="POST">
      <label for="ssid">SSID:</label><br>
      <input type="text" id="ssid" name="ssid" value=""><br><br>
      <label for="password">Password:</label><br>
      <input type="text" id="password" name="password" value=""><br><br>
      <input type="submit" value="Save">
    </form>
  </div>
</body>
</html>

    )rawliteral";
    request->send(200, "text/html", html);
  });

  server.on("/setWiFi", HTTP_POST, [](AsyncWebServerRequest* request) {
    String newSSID;
    String newPassword;
   

    if (request->hasParam("ssid", true)) {
      newSSID = request->getParam("ssid", true)->value();
    }
    if (request->hasParam("password", true)) {
      newPassword = request->getParam("password", true)->value();
    }

    if (!newSSID.isEmpty() && !newPassword.isEmpty()) {
     
      saveWiFiCredentials(newSSID, newPassword);
      request->send(200, "text/html", "<p>WiFi credentials saved! Restarting...</p>");
      delay(2000);
      ESP.restart();
    } else {
      request->send(200, "text/html", "<p>Invalid input. Please try again.</p>");
    }
  });

  server.begin();
}

void setupDashboard() {
server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
  String page = "<html><head>";
  page += "<style> body { background-color: black; color: white; text-align: center; } ";
  page += ".dot { display: inline-block; width: 60px; height: 60px; margin: 5px; border-radius: 50%; background-color: grey; } ";
  page += ".on { background-color: green; } .off { background-color: red; } ";
  page += "button { padding: 10px 20px; margin: 10px; font-size: 16px; background-color: #333; color: white; border: none; cursor: pointer; } ";
  page += "button:hover { background-color: #555; } ";
  page += "</style>";
  
  // AJAX for automatic updates
  page += "<script>";
  page += "function refreshOutputStates() {";
  page += "  fetch('/getOutputStates').then(response => response.text()).then(data => {";
  page += "    document.getElementById('outputStates').innerHTML = data;";
  page += "  });";
  page += "  setTimeout(refreshOutputStates, 10);"; // Update output every second
  page += "}";
  
  // Update pattern status and buttons dynamically
  page += "function setPattern(pattern) {";
  page += "  fetch('/setPattern?pattern=' + pattern).then(response => response.text()).then(data => {";
  page += "    document.getElementById('patternStatus').innerHTML = data;";
  page += "    refreshPatternButtons();";  // Update pattern buttons
  page += "  });";
  page += "  setTimeout(refreshPatternButtons, 1000);";  // Ensure button refresh
  page += "}";
  
  page += "function refreshPatternButtons() {";
  page += "  const patterns = ['static', 'off', 'blink', 'chase', 'reverseChase', 'randomBlink', 'wave'];";
  page += "  patterns.forEach(pattern => {";
  page += "    const btn = document.getElementById(pattern);";
  page += "    if (pattern === '" + currentPattern + "') {";
  page += "      btn.classList.add('on');";
  page += "    } else {";
  page += "      btn.classList.remove('on');";
  page += "    }";
  page += "  });";
  page += "}";
  
  page += "window.onload = function() { refreshOutputStates(); refreshPatternButtons(); };";  // Initial load
  page += "</script>";
  
  page += "</head><body>";
  page += "<h1>Output Status</h1>";
  
  // Container for output states that will be updated with AJAX
  page += "<div id='outputStates'>";
  page += "<div style='display: flex; justify-content: center;'>";
  
  // Loop through the outputs and display dots with pin numbers
  for (int i = 0; i < NUM_OUTPUTS; i++) {
    page += "<div style='text-align: center; margin: 10px;'>";
    page += "<div class=\"dot " + String(outputStates[i] ? "on" : "off") + "\">Pin " + String(i+1)+"</div>";  // Dot based on output state
    page += "</div>";
  }

  page += "</div>";
  page += "</div>";

  // Buttons to toggle outputs
  page += "<br><br>";
  for (int i = 0; i < NUM_OUTPUTS; i++) {
    page += "<button onclick=\"toggleOutput(" + String(i) + ")\">Toggle Output " + String(i + 1) + "</button>";
  }

  // Pattern control section
  page += "<h2>Pattern Control</h2>";
  page += "<div id='patternStatus'>";
  page += "<button id='static' onclick=\"setPattern('static')\">Static</button>";
  page += "<button id='off' onclick=\"setPattern('off')\">Off</button>";
  page += "<button id='blink' onclick=\"setPattern('blink')\">Blink</button>";
  page += "<button id='chase' onclick=\"setPattern('chase')\">Chase</button>";
  page += "<button id='reverseChase' onclick=\"setPattern('reverseChase')\">Reverse Chase</button>";
  page += "<button id='randomBlink' onclick=\"setPattern('randomBlink')\">Random Blink</button>";
  page += "<button id='wave' onclick=\"setPattern('wave')\">Wave</button>";
  page += "</div>";
  
  page += "<br><br><label for='version'>Version:</label><br>";
  page += "<input type='text' id='version' name='version' value=''><br><br>";

  page += "</body></html>";
  request->send(200, "text/html", page);
});


server.on("/setPattern", HTTP_GET, [](AsyncWebServerRequest *request){
    String pattern = request->getParam("pattern")->value();
    currentPattern = pattern;  // Update the pattern

    // Send response with JavaScript to update the pattern buttons dynamically
    String page = "<html><head>";
    page += "<script>";
    page += "function refreshPatternButtons() {";
    page += "  const patterns = ['static', 'off', 'blink', 'chase', 'reverseChase', 'randomBlink', 'wave'];";
    page += "  patterns.forEach(pattern => {";
    page += "    const btn = document.getElementById(pattern);";
    page += "    if (pattern === '" + currentPattern + "') {";
    page += "      btn.classList.add('on');";
    page += "    } else {";
    page += "      btn.classList.remove('on');";
    page += "    }";
    page += "  });";
    page += "  setTimeout(function() { window.location.href = '/'; }, 2000);";
    page += "}";
    page += "refreshPatternButtons();"; // Call the function to refresh the buttons
    page += "</script>"; // JavaScript to update buttons dynamically
    page += "</head><body><h1>Pattern Set to " + pattern + "</h1><a href='/'>Go Back</a></body></html>";

    request->send(200, "text/html", page);
});


// Toggle output states without leaving the dashboard
server.on("/toggleOutput", HTTP_GET, [](AsyncWebServerRequest *request){
  String outputIndexStr = request->getParam("output")->value();
  int outputIndex = outputIndexStr.toInt();

  // Toggle the selected output
  if (outputIndex >= 0 && outputIndex < 8) {
    outputStates[outputIndex] = !outputStates[outputIndex];
  }

  // Send response back without leaving the page
  request->send(200, "text/html", "<h1>Output " + String(outputIndex + 1) + " Toggled</h1>");
});

  server.begin();

  
  client.setServer(mqttServer, mqttPort);
  client.setCallback(mqttCallback);  // Set MQTT callback function
  connectMQTT();

  // Initialize Preferences
  preferences.begin("firmware", false);

 
  delay(3000);
  checkForUpdates();

  sendMQTTPayload();  // Send initial MQTT message when connected
  


}

void setup() {
  Serial.begin(115200);

    
  pinMode(ledPin, OUTPUT);
  for (int i = 0; i < NUM_OUTPUTS; i++) {
    pinMode(outputPins[i], OUTPUT);
    digitalWrite(outputPins[i], LOW);  // Ensure all outputs start LOW
  }

  clientId = "ESP32_" + String(WiFi.macAddress());

  loadWiFiCredentials();
  connectWiFi();
  
}

void getOutputStates() {

  // Get current output states via AJAX
server.on("/getOutputStates", HTTP_GET, [](AsyncWebServerRequest *request){
  String outputHtml = "<div style='text-align: center; margin: 10px;'>";
  for (int i = 0; i < 8; i++) {
    digitalWrite(outputPins[i], outputStates[i] ? HIGH : LOW);
    outputHtml += "<div class=\"dot " + String(outputStates[i] ? "on" : "off") + "\">Pin " + String(i+1)+"</div>";
    outputHtml += "</div>";
  }
  request->send(200, "text/html", outputHtml);
});

}


void loop() {
  if (!client.connected()) {
    connectMQTT();
  }
  client.loop();  // Ensure MQTT is being handled

  static unsigned long lastOTA = 0;
  if (millis() - lastOTA > 3600000) {  // Check for updates every hour
    lastOTA = millis();
    checkForUpdates();
  }

  // Flash the onboard LED when services are active
  if (allServicesActive) {
    digitalWrite(ledPin, HIGH);
    delay(100);  // LED ON for 100ms
    digitalWrite(ledPin, LOW);
    delay(100);  // LED OFF for 100ms
  } else {
    digitalWrite(ledPin, LOW);  // Keep LED OFF when services are inactive
  }

  // Handle pattern
  handlePattern(millis());
}

void handlePattern(unsigned long currentTime) {
   
  // Update outputs based on current pattern
  if (currentPattern == "blink") {
    if (currentTime - lastUpdateTime > blinkInterval) {
      lastUpdateTime = currentTime;
      for (int i = 0; i < NUM_OUTPUTS; i++) {
        outputStates[i] = !outputStates[i];  // Toggle all outputs
      }
      getOutputStates();
    }
  }
  else if (currentPattern == "chase") {
    if (currentTime - lastUpdateTime > blinkInterval) {
      lastUpdateTime = currentTime;
      outputStates[chaseIndex] = true;
      if (chaseIndex > 0) outputStates[chaseIndex - 1] = false;
      chaseIndex = (chaseIndex + 1) % NUM_OUTPUTS;
      getOutputStates();
    }
  }
  else if (currentPattern == "reverseChase") {
    if (currentTime - lastUpdateTime > blinkInterval) {
      lastUpdateTime = currentTime;
      outputStates[chaseIndex] = true;
      if (chaseIndex < (NUM_OUTPUTS-1)) outputStates[chaseIndex + 1] = false;
      chaseIndex = (chaseIndex - 1 + NUM_OUTPUTS) % NUM_OUTPUTS;
      getOutputStates();
    }
  }
  else if (currentPattern == "randomBlink") {
    if (currentTime - lastUpdateTime > blinkInterval) {
      lastUpdateTime = currentTime;
      int randOutput = random(0, NUM_OUTPUTS);
      outputStates[randOutput] = !outputStates[randOutput];  // Toggle random output
      getOutputStates();
    }
  }
  else if (currentPattern == "wave") {
    if (currentTime - lastUpdateTime > blinkInterval) {
      lastUpdateTime = currentTime;
      for (int i = 0; i < NUM_OUTPUTS; i++) {
        outputStates[i] = false;  // Turn off all outputs
      }
      outputStates[waveIndex] = true;  // Turn on the "wave" output
      waveIndex = (waveIndex + 1) % 8;
      getOutputStates();
    }
  }
  else if (currentPattern == "static") {
    // In "static" pattern, you can keep the outputs in a fixed state
    // For example, set all to ON or OFF based on your choice
    for (int i = 0; i < NUM_OUTPUTS; i++) {
      outputStates[i] = true;  // Keep all outputs on in static mode
    }
        getOutputStates();

  }
  else if (currentPattern == "off") {
    // In "off" pattern, turn all outputs off
    for (int i = 0; i < NUM_OUTPUTS; i++) {
      outputStates[i] = false;  // Turn all outputs off
    }
        getOutputStates();

  }

}

void connectMQTT() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    if (client.connect(clientId.c_str(), mqttUserName, mqttPassword)) {
      Serial.println("Connected to MQTT.");
      client.subscribe(topicData);  // Subscribe to the topic
    } else {
      Serial.print("Failed (state=");
      Serial.print(client.state());
      Serial.println("). Retrying in 5 seconds...");
      delay(5000);
    }
  }
}

void sendMQTTPayload() {
  StaticJsonDocument<512> doc;
  doc["mac"] = WiFi.macAddress();
  doc["puzzleName"] = "Outputs Puzzle";
  doc["designer"] = "Paul Hopkins";
  doc["ipAddress"] = WiFi.localIP().toString();
  doc["timestamp"] = millis();
  doc["tab"] = "Christmas Lights";
  doc["group"] = "Stage 2";
  doc["version"] = loadVersion();
  doc["num_outputs"] = NUM_OUTPUTS;

  String jsonPayload;
  serializeJson(doc, jsonPayload);

  if (client.publish(topicData, jsonPayload.c_str())) {
    Serial.println("Puzzle details sent:");
    Serial.println(jsonPayload);
  } else {
    Serial.println("Failed to send puzzle details.");
  }
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.println(topic);

  if (strcmp(topic, topicData) == 0) {  // Check if the message is for the correct topic
    
    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, payload, length);

    if (error) {
      Serial.print("JSON parse error: ");
      Serial.println(error.c_str());
      return;
    }

    // Check for the pattern name and update the currentPattern variable
    if (doc.containsKey("pattern")) {
      currentPattern = doc["pattern"].as<String>();
      Serial.printf("Pattern set to: %s\n", currentPattern.c_str());
    }

    // Update output states based on JSON
    for (int i = 0; i < NUM_OUTPUTS; i++) {
      String key = "XmasOutput" + String(i + 1);
      if (doc.containsKey(key)) {
        outputStates[i] = doc[key].as<bool>();
        digitalWrite(outputPins[i], outputStates[i] ? HIGH : LOW);
      }
    }
  }
}

void setAllOutputs(bool state) {
  for (int i = 0; i < NUM_OUTPUTS; i++) {
    digitalWrite(outputPins[i], state ? HIGH : LOW);
    outputStates[i] = state;  // Update the tracked states
  }
}

void toggleAllOutputs() {
  for (int i = 0; i < NUM_OUTPUTS; i++) {
    bool currentState = digitalRead(outputPins[i]);  // Read the current state of the pin
    digitalWrite(outputPins[i], !currentState);     // Write the opposite state
    outputStates[i] = !currentState;                // Update the tracked state
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
    
    // Get stored version from Preferences
    String storedVersion = loadVersion();  // Default to "0.0.0" if no version is stored

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
        saveVersion(newversion);  
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
