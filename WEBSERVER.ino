
// Set up the server on port 80
AsyncWebServer server(80);

void setupDashboard() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    String page = "<html><head>";
    page += "<style> body { background-color: black; color: white; text-align: center; } ";
    page += ".dot { display: inline-block; width: 40px; height: 40px; margin: 5px; border-radius: 50%; background-color: grey; } ";
    page += ".on { background-color: green; } .off { background-color: red; } ";
    page += "button { padding: 10px 20px; margin: 10px; font-size: 16px; background-color: #333; color: white; border: none; cursor: pointer; } ";
    page += "button:hover { background-color: #555; } ";
    page += "#dashboard { display: grid; grid-template-columns: 1fr 1fr; grid-template-rows: 1fr 1fr; gap: 10px; } ";
    page += "</style>";
    page += "<script>";
   
    page += "function refreshFirmwareInfo() { fetch('/getFirmwareInfo') ";
    page += ".then(response => response.text()) ";
    page += ".then(data => { document.getElementById('firmwareInfo').innerHTML = data; }); }";
    page += "function refreshOutputStates() { fetch('/getOutputStates') ";
    page += ".then(response => response.text()) ";
    page += ".then(data => { document.getElementById('outputs').innerHTML = data; }); }";
    page += "function toggleOutput(index) { fetch('/toggleOutput?output=' + index) ";
    page += ".then(() => refreshOutputStates()); }";
    page += "function setPattern(pattern) { fetch('/setPattern?pattern=' + pattern) ";
    page += ".then(() => refreshPatternButtons()); }";
    page += "function refreshPatternButtons() { ";
    page += "fetch('/getCurrentPattern').then(response => response.text()).then(currentPattern => { ";
    page += "const patterns = ['static', 'off', 'blink', 'chase', 'reverseChase', 'randomBlink', 'wave']; ";
    page += "patterns.forEach(p => { const btn = document.getElementById(p); if (btn) { btn.style.backgroundColor = (p === currentPattern) ? 'green' : ''; } }); ";
    page += "}); };";
    page += "window.onload = function() { refreshOutputStates(); refreshPatternButtons(); refreshFirmwareInfo(); setInterval(function() { refreshOutputStates(); refreshFirmwareInfo(); }, 200); };";
    page += "</script></head><body>";
    page += "<h1>ESP32 Dashboard</h1>";
    page += "<div id='dashboard'>";

    // Quadrant 1: Firmware Info
    page += "<div><h2>Firmware Info</h2><div id='firmwareInfo'>";
    page += "Version: " + storedVersion + "<br>";
    page += "IP Address: " + WiFi.localIP().toString() + "<br>";
    page += String("WiFi Status: ") + (WiFi.isConnected() ? "Connected" : "Disconnected") + "<br>";
    page += String("MQTT Status: ") + (client.connected() ? "Connected" : "Disconnected") + "<br>";
    page += "Pattern: " + currentPattern + "<br>";
    page += "</div></div>";
  
    // Quadrant 2: Output States
    page += "<div><h2>Output States</h2><div id='outputs'>";
    for (int i = 0; i < NUM_OUTPUTS; i++) {
      page += "<div class='dot " + String(outputStates[i] ? "on" : "off") + "'>Pin " + String(i + 1) + "</div>";
    }
    page += "</div></div>";

    // Quadrant 3: Toggle Buttons
    page += "<div><h2>Toggle Outputs</h2>";
    for (int i = 0; i < NUM_OUTPUTS; i++) {
      page += "<button onclick='toggleOutput(" + String(i) + ")'>Toggle Output " + String(i + 1) + "</button>";
    }
    page += "</div>";

    // Quadrant 4: Pattern Control
    page += "<div><h2>Pattern Control</h2>";
    String patterns[] = {"static", "off", "blink", "chase", "reverseChase", "randomBlink", "wave"};
    for (String pattern : patterns) {
      page += "<button id='" + pattern + "' onclick='setPattern(\"" + pattern + "\")'>" + pattern + "</button>";
    }
    page += "</div>";
    

    page += "</div></body></html>";

    request->send(200, "text/html", page);
  });

  // Define /getOutputStates endpoint
  server.on("/getOutputStates", HTTP_GET, [](AsyncWebServerRequest *request) {
    String outputHTML = "";
    for (int i = 0; i < NUM_OUTPUTS; i++) {
      outputHTML += "<div class='dot " + String(outputStates[i] ? "on" : "off") + "'>Pin " + String(i + 1) + "</div>";
    }
    request->send(200, "text/html", outputHTML);  // Send updated states of outputs
  });

  // Toggle output state handler
  server.on("/toggleOutput", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam("output")) {
      int index = request->getParam("output")->value().toInt();
      if (index >= 0 && index < NUM_OUTPUTS) {
        outputStates[index] = !outputStates[index];
        digitalWrite(outputPins[index], outputStates[index] ? HIGH : LOW);
        Serial.println("Output " + String(index) + " toggled: " + String(outputStates[index]));
      }
    }
    request->send(200, "text/plain", "Toggled");
  });

  // Set pattern handler
  server.on("/setPattern", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam("pattern")) {
      String newPattern = request->getParam("pattern")->value();
      Serial.println("Pattern Set to " + newPattern);

      if (newPattern.length() > 0) {
        currentPattern = newPattern;  // Update the pattern variable
        request->send(200, "text/plain", "Pattern set to " + currentPattern);  // Respond to the client
        Serial.println("Pattern Set to " + currentPattern);
      } else {
        request->send(400, "text/plain", "Invalid pattern");  // Handle invalid pattern input
        Serial.println("Invalid Pattern");
      }
    } else {
      request->send(400, "text/plain", "Pattern parameter missing");
    }
  });

  // Get current pattern
  server.on("/getCurrentPattern", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", currentPattern);
  });

  // Define /getFirmwareInfo endpoint
  server.on("/getFirmwareInfo", HTTP_GET, [](AsyncWebServerRequest *request) {
    String info = "Version: " + storedVersion + "<br>";
    info += "IP Address: " + WiFi.localIP().toString() + "<br>";
    info += String("WiFi Status: ") + (WiFi.isConnected() ? "Connected" : "Disconnected") + "<br>";
    info += String("MQTT Status: ") + (client.connected() ? "Connected" : "Disconnected") + "<br>";
    info += "Pattern: " + currentPattern + "<br>";
    request->send(200, "text/html", info);  // Send updated firmware info
  });

  
 


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
