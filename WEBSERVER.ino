#include <ESPAsyncWebServer.h>

// Create an AsyncWebServer object on port 80
AsyncWebServer server(80);


// Function to generate the web page
String generatePage() {
  String page = "<html><head>";
  
  // Include styles
  page += "<style>";
  page += "body { background-color: black; color: white; text-align: center; font-family: Arial, sans-serif; }";
  page += "#inputsSection { margin: 20px auto; text-align: center; }";
  page += ".input { font-size: 24px; margin: 10px; padding: 10px; border: 2px solid #fff; border-radius: 10px; }";
  page += "#led-matrix-container { padding: 15px; border: 2px solid #888; border-radius: 8px; background-color: #f9f9f9; box-shadow: 0px 4px 8px rgba(0, 0, 0, 0.1); max-width: 400px; margin: auto; }";
  page += "#led-matrix-container h3 { text-align: center; color: #333; font-family: Arial, sans-serif; }";
  page += "#led-matrix-container p { margin: 5px 0; color: #555; font-size: 14px; font-family: Arial, sans-serif; }";
  page += ".indicator { display: inline-block; width: 20px; height: 20px; border-radius: 50%; margin: 3px; box-shadow: 0px 2px 4px rgba(0, 0, 0, 0.2); }";
  page += ".green { background-color: green; }";
  page += ".red { background-color: red; }";
  page += "#led-matrix { display: flex; flex-direction: column; gap: 8px; align-items: center; }";
  page += "</style>";

  // Include JavaScript
  page += "<script>";
  page += "function refreshInputs() {";
  page += "  fetch('/getAnalogInputs') ";
  page += "    .then(response => response.text()) ";
  page += "    .then(data => { document.getElementById('inputs').innerHTML = data; });";
  page += "}";
  page += "function refreshLEDMatrix() {";
  page += "  fetch('/getLEDMatrix') ";
  page += "    .then(response => response.json()) ";
  page += "    .then(payload => { renderLEDMatrix(payload); });";
  page += "}";
  page += "function renderLEDMatrix(payload) {";
  page += "  document.getElementById('puzzle-name').textContent = payload.puzzleName || 'Unnamed Puzzle';";
  page += "  document.getElementById('group').textContent = payload.group || 'Unknown Group';";
  page += "  document.getElementById('tab').textContent = payload.tab || 'Unknown Tab';";
  page += "  document.getElementById('ip-address').textContent = payload.ipAddress || 'Unknown IP';";
  page += "  const ledMatrix = document.getElementById('led-matrix');";
  page += "  ledMatrix.innerHTML = '';";
  page += "  for (let c = 0; c < 3; c++) {";
  page += "    let ledStrip = payload[`led_strip_${c+1}`];";
  page += "    if (Array.isArray(ledStrip)) {";
  page += "      const rowDiv = document.createElement('div');";
  page += "      for (let i = 0; i < ledStrip.length; i++) {";
  page += "        const indicator = document.createElement('span');";
  page += "        indicator.className = `indicator ${ledStrip[i] === 'on' ? 'green' : 'red'}`;";
  page += "        rowDiv.appendChild(indicator);";
  page += "      }";
  page += "      ledMatrix.appendChild(rowDiv);";
  page += "    }";
  page += "  }";
  page += "}";
  page += "window.onload = function() {";
  page += "  refreshInputs();";
  page += "  refreshLEDMatrix();";
  page += "  setInterval(refreshInputs, 1000);";
  page += "  setInterval(refreshLEDMatrix, 5000);";
  page += "};";
  page += "</script>";

  // Body
  page += "</head><body>";
  page += "<h1>ESP32 Dashboard</h1>";
  page += "<div id='inputsSection'>";
  page += "<h2>Analog Inputs</h2>";
  page += "<div id='inputs'></div>";
  page += "</div>";
  page += "<div id='led-matrix-container'>";
  page += "<h3 id='puzzle-name'>Loading...</h3>";
  page += "<p><strong>Group:</strong> <span id='group'>N/A</span></p>";
  page += "<p><strong>Tab:</strong> <span id='tab'>N/A</span></p>";
  page += "<p><strong>IP Address:</strong> <span id='ip-address'>N/A</span></p>";
  page += "<div id='led-matrix'></div>";
  page += "</div>";
  page += "</body></html>";
  
  return page;
}

void setupDashboard() {

  // Setup server routes
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/html", generatePage());
  });

  server.on("/getAnalogInputs", HTTP_GET, [](AsyncWebServerRequest *request) {
    String inputs = "<div>";
    inputs += "<div class='input'>Analog 1: " + String(analogRead(34)) + "</div>";
    inputs += "<div class='input'>Analog 2: " + String(analogRead(35)) + "</div>";
    inputs += "</div>";
    request->send(200, "text/html", inputs);
  });

  server.on("/getLEDMatrix", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "application/json", jsonPublished);
  });

  // Start server
  server.begin();
}
