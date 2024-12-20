

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
    page += ".then(() => refreshPatternButtons()) ";
    page += ".then(() => refreshFirmwareInfo()); }";
    page += "function refreshPatternButtons() { ";
    page += "fetch('/getCurrentPattern').then(response => response.text()).then(currentPattern => { ";
    page += "const patterns = ['static', 'off', 'blink', 'chase', 'reverseChase', 'randomBlink', 'wave']; ";
    page += "patterns.forEach(p => { const btn = document.getElementById(p); if (btn) { btn.style.backgroundColor = (p === currentPattern) ? 'green' : ''; } }); ";
    page += "}); };";

    // Use two setIntervals: one for 10ms (refreshOutputStates), one for 1000ms (refreshFirmwareInfo)
    page += "window.onload = function() { ";
    page += "refreshOutputStates(); refreshPatternButtons(); refreshFirmwareInfo(); ";
    page += "setInterval(function() { refreshOutputStates(); }, 10); ";  // 10ms refresh for output states
    page += "setInterval(function() { refreshFirmwareInfo(); }, 10000); ";  // 1000ms refresh for firmware info
    page += "};";
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
  
    // Quadrant 2: Output States (Two columns in a table)
    page += "<div class='quadrant'><h2>Output States</h2>";
    page += "<div id='outputs'></div>";  // Table will be dynamically injected here
    page += "</div>";

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
        outputHtml += "<div class='dot " + String(outputStates[oddIndex] ? "on" : "off") + "'>Pin " + String(oddIndex + 1) + "</div>";
        outputHtml += "</td>";
      } else {
        outputHtml += "<td></td>";
      }

      // Even column
      int evenIndex = row * 2 + 1;
      if (evenIndex < NUM_OUTPUTS) {
        outputHtml += "<td style='padding: 10px; text-align: center;'>";
        outputHtml += "<div class='dot " + String(outputStates[evenIndex] ? "on" : "off") + "'>Pin " + String(evenIndex + 1) + "</div>";
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
        digitalWrite(outputPins[index], outputStates[index] ? HIGH : LOW);
        Serial.println("Output " + String(index) + " toggled: " + String(outputStates[index]));
      }
    }
    request->send(200, "text/plain", "Toggled");
  });

  // Endpoint for setting patterns
  server.on("/setPattern", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam("pattern")) {
      String newPattern = request->getParam("pattern")->value();
      Serial.println("Pattern Set to " + newPattern);
      if (newPattern.length() > 0) {
        currentPattern = newPattern;
        request->send(200, "text/plain", "Pattern set to " + currentPattern);
      } else {
        request->send(400, "text/plain", "Invalid pattern");
      }
    } else {
      request->send(400, "text/plain", "Pattern parameter missing");
    }
  });

  // Endpoint for getting current pattern
  server.on("/getCurrentPattern", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", currentPattern);
  });

  // Endpoint for getting firmware info
  server.on("/getFirmwareInfo", HTTP_GET, [](AsyncWebServerRequest *request) {
    String info = "Version: " + storedVersion + "<br>";
    info += "IP Address: " + WiFi.localIP().toString() + "<br>";
    info += String("WiFi Status: ") + (WiFi.isConnected() ? "Connected" : "Disconnected") + "<br>";
    info += String("MQTT Status: ") + (client.connected() ? "Connected" : "Disconnected") + "<br>";
    info += "Pattern: " + currentPattern + "<br>";
    request->send(200, "text/html", info);
  });

  server.begin();
}
