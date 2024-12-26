#include <ESPAsyncWebServer.h>

AsyncWebServer server(80);


// Function to generate the web page
String generatePage() {
  String page = "<html><head>";
  
  // Include styles for both the dashboard and LED matrix
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

  // Include JavaScript for dynamic updates
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
  page += "  document.getElementById('puzzle-designer').textContent = payload.designer || 'Unknown Designer';";
  page += "  document.getElementById('puzzle-tech').textContent = payload.tachName || 'Unknown Tech';";
  page += "  document.getElementById('ip-address').textContent = payload.ipAddress || 'Unknown IP';";
  page += "  const ledMatrix = document.getElementById('led-matrix');";
  page += "  ledMatrix.innerHTML = '';"; // Clear previous content
page += "  for (let c = 0; c < 3; c++) {"; // Loop through each LED strip (3 strips in total)
page += "    let ledStrip = payload[`led_strip_${c+1}`];"; // Get the LED data for the current strip
page += "    if (Array.isArray(ledStrip)) {"; // Check if the strip contains LED data
page += "      const rowDiv = document.createElement('div');"; // Create a new row for the strip
page += "      for (let i = 0; i < ledStrip.length; i++) {"; // Loop through each LED in the current strip
page += "        const indicator = document.createElement('span');"; // Create a new indicator for the LED
page += "        // Set the LED color based on its status (on/off)";
page += "        indicator.className = `indicator ${ledStrip[i] === 'on' ? 'green' : 'red'}`;"; // Green if 'on', red if 'off'
page += "        rowDiv.appendChild(indicator);"; // Append the indicator to the row
page += "      }";
page += "      ledMatrix.appendChild(rowDiv);"; // Append the row to the LED matrix
page += "    }";
page += "  }";

  page += "}";
  page += "window.onload = function() {";
  page += "  refreshInputs();";
  page += "  refreshLEDMatrix();";
  page += "  setInterval(refreshInputs, 1000);"; // Refresh inputs every second
  page += "  setInterval(refreshLEDMatrix, 5000);"; // Refresh LED matrix every 5 seconds
  page += "};";
  page += "</script>";

  // Begin body
  page += "</head><body>";
  page += "<h1>ESP32 Dashboard</h1>";

  // Section for analog inputs
  page += "<div id='inputsSection'>";
  page += "<h2>Analog Inputs</h2>";
  page += "<div id='inputs'></div>"; // Placeholder for input values
  page += "</div>";

  // Section for LED matrix
  page += "<div id='led-matrix-container'>";
  page += "<h3 id='puzzle-name'>Loading...</h3>";
  page += "<p><strong>Group:</strong> <span id='group'>N/A</span></p>";
  page += "<p><strong>Tab:</strong> <span id='tab'>N/A</span></p>";
  page += "<p><strong>IP Address:</strong> <span id='ip-address'>N/A</span></p>";
  page += "<div id='led-matrix'></div>";
  page += "</div>";

  // Close body and html
  page += "</body></html>";
  
  return page;
}

// Setup the server and JSON endpoint
void setupDashboard() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/html", generatePage());
  });

  server.on("/getAnalogInputs", HTTP_GET, [](AsyncWebServerRequest *request) {
    String inputs = "<div>";
    inputs += "<div class='input'>Analog 1: 123</div>"; // Example values
    inputs += "<div class='input'>Analog 2: 456</div>";
    inputs += "<div class='input'>Analog 3: 789</div>";
    inputs += "</div>";
    request->send(200, "text/html", inputs);
  });

  server.on("/getLEDMatrix", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "application/json", jsonPublished);
  });

  server.begin();
}
