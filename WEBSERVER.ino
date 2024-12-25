
// AsyncWebServer server(80);  // ESP32 web server instance on port 80
AsyncWebServer server(80);

void setupDashboard() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    String page = "<html><head>";
    page += "<style> body { background-color: black; color: white; text-align: center; } ";
    page += ".dot { display: inline-block; width: 60px; height: 60px; margin: 5px; border-radius: 50%; background-color: grey; } ";
    page += ".on { background-color: green; } .off { background-color: red; } ";
    page += "button { padding: 10px 20px; margin: 10px; font-size: 32px; background-color: #333; color: white; border: none; cursor: pointer; } ";
    page += "button:hover { background-color: #555; } ";
    page += "#dashboard { display: flex; flex-direction: row; justify-content: space-between; width: 100%; height: 100vh; } ";
    page += "#leftSide { width: 50%; display: flex; flex-direction: column; align-items: center; padding: 10px; } ";
    page += "#rightSide { width: 50%; display: flex; flex-direction: column; align-items: center; padding: 10px; } ";
    page += "#outputsSection { width: 100%; } ";
    page += "#patternSection { width: 100%; } ";
    page += "#audioSection { width: 100%; padding: 20px; text-align: center; } ";
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
    page += ".then(() => refreshPatternButtons()) ";
    page += ".then(() => refreshFirmwareInfo()); }";
    page += "function refreshPatternButtons() { ";
    page += "fetch('/getCurrentPattern').then(response => response.text()).then(currentPattern => { ";
    page += "const patterns = ['static', 'off', 'blink', 'chase', 'reverseChase', 'randomBlink', 'wave']; ";
    page += "patterns.forEach(p => { const btn = document.getElementById(p); if (btn) { btn.style.backgroundColor = (p === currentPattern) ? 'green' : ''; } }); ";
    page += "}); };";
    page += "function getAudioBeats() { fetch('/getAudioBeats') ";
    page += ".then(response => response.text()) ";
    page += ".then(data => { document.getElementById('audioBeats').innerHTML = data; }); }";
    page += "window.onload = function() { ";
    page += "refreshOutputStates(); refreshPatternButtons(); refreshFirmwareInfo(); getAudioBeats(); ";
    page += "setInterval(function() { refreshOutputStates(); }, 10); ";  // Refresh output states every 10ms
    page += "setInterval(function() { refreshFirmwareInfo(); }, 10000); ";  // Refresh firmware info every 10s
    page += "setInterval(function() { getAudioBeats(); }, 1000); ";  // Refresh audio beats every second
    page += "};";
    page += "</script></head><body>";
    page += "<h1>ESP32 Dashboard</h1>";
    page += "<div id='dashboard'>";
// Firmware Info
    page += "<div><h2>Firmware Info</h2><div id='firmwareInfo'>";
   
    page += "</div>";
    // Left Side: Firmware Info, Toggle Buttons, Pattern Control
    page += "<div id='leftSide'>";

    page += "</div>";

    // Toggle Buttons
    page += "<div><h2>Toggle Outputs</h2>";
    for (int i = 0; i < NUM_OUTPUTS; i++) {
      page += "<button onclick='toggleOutput(" + String(i) + ")'>Toggle Output " + String(i + 1) + "</button>";
    }
    page += "</div>";

    // Pattern Control
    page += "<div id='patternControl'><h2>Pattern Control</h2>";
    String patterns[] = {"static", "off", "blink", "chase", "reverseChase", "randomBlink", "wave"};
    for (int i = 0; i < 7; i++) {
      page += "<button id='" + patterns[i] + "' onclick='setPattern(\"" + patterns[i] + "\")'>" + patterns[i] + "</button>";
    }
    page += "</div>";

    page += "</div>"; // End of leftSide

    // Right Side: Output States, Audio Beats
    page += "<div id='rightSide'>";

    // Output States
    page += "<div id='outputsSection'><h2>Output States</h2><div id='outputs'></div></div>";

    // Audio Beats Section
    page += "<div id='audioSection'><h2>Audio Beats</h2><div id='audioBeats'></div></div>";

    page += "</div>"; // End of rightSide

    

    page += "</div></body></html>";

    request->send(200, "text/html", page);
  });

  

  // Endpoint for getting audio beats
  server.on("/getAudioBeats", HTTP_GET, [](AsyncWebServerRequest *request) {
    String outputHtml = "<table style='width: 100%; border-collapse: collapse;'>";
    
    outputHtml += "<thead><tr><th>Beat Name</th><th>Beat Status</th></tr></thead>";
    outputHtml += "<tbody>";

    // For each beat, display its name and status (on/off)
    for (int i = 0; i < 3; i++) {
      outputHtml += "<tr>";
      outputHtml += "<td style='padding: 10px; text-align: center;'>" + beatName[i] + "</td>";
      outputHtml += "<td style='padding: 10px; text-align: center;'>";
      outputHtml += "<div class='dot " + String(beatStatus[i] ? "on" : "off") + "'></div>";
      outputHtml += "</td>";
      outputHtml += "</tr>";
    }

    outputHtml += "</tbody></table>";
    request->send(200, "text/html", outputHtml);
  });

  // Endpoint for getting current pattern
  server.on("/getCurrentPattern", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", currentPattern);
  });

  // Endpoint for getting output states
  server.on("/getOutputStates", HTTP_GET, [](AsyncWebServerRequest *request) {
    String outputHtml = "<table style='width: 100%; border-collapse: collapse;'>";
    
    outputHtml += "<thead><tr><th>Odd Pins</th><th>Even Pins</th></tr></thead>";
    outputHtml += "<tbody>";

    int maxRows = (NUM_OUTPUTS + 1) / 2;
    for (int row = 0; row < maxRows; row++) {
      outputHtml += "<tr>";
      // Odd column
      int oddIndex = row * 2;
      if (oddIndex < NUM_OUTPUTS) {
        outputHtml += "<td style='padding: 10px; text-align: center;'>";

        outputHtml += "<div style='font-size: 2em; line-height: 1.5;' class='dot " + String(outputStates[oddIndex] ? "on" : "off") + "'>" + String(oddIndex + 1) + "</div>";
        outputHtml += "</td>";
      } else {
        outputHtml += "<td></td>";
      }
      // Even column
      int evenIndex = row * 2 + 1;
      if (evenIndex < NUM_OUTPUTS) {
        outputHtml += "<td style='padding: 10px; text-align: center;'>";
        outputHtml += "<div style='font-size: 2em; line-height: 1.5;' class='dot " + String(outputStates[evenIndex] ? "on" : "off") + "'>" + String(evenIndex + 1) + "</div>";
        outputHtml += "</td>";
      } else {
        outputHtml += "<td></td>";
      }
      outputHtml += "</tr>";
    }
    outputHtml += "</tbody></table>";
    request->send(200, "text/html", outputHtml);
  });

  // Endpoint for toggling outputs
  server.on("/toggleOutput", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam("output")) {
      int index = request->getParam("output")->value().toInt();
      if (index >= 0 && index < NUM_OUTPUTS) {
        outputStates[index] = !outputStates[index];
        digitalWrite(outputPins[index], outputStates[index]);
      }
    }
    request->send(200, "text/plain", "OK");
  });

  // Endpoint for setting patterns
  server.on("/setPattern", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam("pattern")) {
      String pattern = request->getParam("pattern")->value();
      currentPattern = pattern;
      // Logic to apply pattern control
    }
    request->send(200, "text/plain", "Pattern set to: " + currentPattern);
  });

  // Endpoint for getting firmware info
server.on("/getFirmwareInfo", HTTP_GET, [](AsyncWebServerRequest *request) {
  String info = "<div style='font-size: 2em; line-height: 1.5;'>";  // Triple font size, and ensure spacing
  info += "Version: " + storedVersion + "<br>";
  info += "IP Address: " + WiFi.localIP().toString() + "<br>";
  info += String("WiFi Status: ") + (WiFi.isConnected() ? "Connected" : "Disconnected") + "<br>";
  info += String("MQTT Status: ") + (client.connected() ? "Connected" : "Disconnected") + "<br>";
  info += "Pattern: " + currentPattern + "<br>";
  info += "</div>";  // Close the div with custom styling
  request->send(200, "text/html", info);
});


  server.begin();
}
