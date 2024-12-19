
// Set up the server on port 80
AsyncWebServer server(80);

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
     
      saveWiFiCredentials(newSSID, newPassword, version);
      request->send(200, "text/html", "<p>WiFi credentials saved! Restarting...</p>");
      delay(2000);
      ESP.restart();
    } else {
      request->send(200, "text/html", "<p>Invalid input. Please try again.</p>");
    }
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
