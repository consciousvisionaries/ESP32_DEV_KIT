#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <Update.h>
#include <Preferences.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESPAsyncWebServer.h>

const char* ssid = "TELUSDE0875_2.4G";   // Replace with your WiFi SSID
const char* password = "3X3K22832E";     // Replace with your WiFi password

const char* mqttServer = "192.168.0.129"; // Replace with your MQTT broker IP
const int mqttPort = 1883;
const char* mqttUserName = "pro1polaris";
const char* mqttPassword = "CVr819P*!";

const char* githubUser = "consciousvisionaries";
const char* githubRepo = "ESP32_DEV_KIT";
const char* firmwareFile = "ESP32_DEV_KIT.ino.esp32.bin";
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

// OLED Display Setup
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define I2C_ADDRESS 0x3C  // OLED I2C address, common for many displays

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Example output states
int waveIndex = 0;  // Declare waveIndex (or change to chaseIndex if that was the intention)

// Set up the server on port 80
AsyncWebServer server(80);

void setup() {
  Serial.begin(115200);

  // Initialize I2C for OLED display with custom SDA and SCL pins
    Wire.begin(21, 22); // SDA = 21, SCL = 22
    
  pinMode(ledPin, OUTPUT);
  for (int i = 0; i < NUM_OUTPUTS; i++) {
    pinMode(outputPins[i], OUTPUT);
    digitalWrite(outputPins[i], LOW);  // Ensure all outputs start LOW
  }

  clientId = "ESP32_" + String(WiFi.macAddress());

  // Initialize OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, I2C_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Infinite loop if OLED fails to initialize
  }
  display.setCursor(0,0);
  display.print("Ready");
  display.display();
  delay(3000);
  display.clearDisplay();

  connectWiFi();
  client.setServer(mqttServer, mqttPort);
  client.setCallback(mqttCallback);  // Set MQTT callback function
  connectMQTT();

  // Initialize Preferences
  preferences.begin("firmware", false);

  // Set the version during initial setup (only if necessary)
  if (!preferences.isKey("version")) {
    preferences.putString("version", "0.0.1");  // Set the initial firmware version
  }

  delay(3000);
  checkForUpdates();

  sendMQTTPayload();  // Send initial MQTT message when connected

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




  // Start the web server
  server.begin();


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

  // Update OLED with status info
  updateOLED();
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

void connectWiFi() {
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected.");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  allServicesActive = true;  // Set to true when WiFi is connected
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
  doc["version"] = getStoredVersion();
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
  String url = "https://raw.githubusercontent.com/" + String(githubUser) + "/" + String(githubRepo) + "/" + String(branch) + "/" + String(firmwareFile);

  http.begin(url);
  int httpCode = http.GET();

  if (httpCode == HTTP_CODE_OK) {
    String payload = http.getString();
    String newVersion = extractVersionFromPayload(payload);
    
    // Get stored version from Preferences
    String storedVersion = preferences.getString("version", "0.0.2 (new)");  // Default to "0.0.0" if no version is stored

    Serial.print("Current stored version: ");
    Serial.println(storedVersion);
    Serial.print("New version available: ");
    Serial.println(newVersion);

    if (newVersion != storedVersion) {
      Serial.println("New firmware update found. Updating...");
      performFirmwareUpdate(url);  // Perform the update
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
void performFirmwareUpdate(String firmwareUrl) {
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
        String newVersion = extractVersionFromPayload(http.getString());
        preferences.putString("version", newVersion);  // Save version to Preferences
        
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


String getStoredVersion() {
  return preferences.getString("version", "0.0.0");  // Default version if not found
}

void updateOLED() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("Wi-Fi: ");
  display.print(WiFi.status() == WL_CONNECTED ? "Connected" : "Disconnected");

  display.setCursor(0, 10);
  display.print("Pattern: ");
  display.print(currentPattern);

  display.setCursor(0, 20);
  display.print("Version: ");
  display.print(getStoredVersion());

  display.setCursor(0, 30);
  display.print("MQTT: ");
  display.print(client.connected() ? "Connected" : "Disconnected");

  display.display();
}
