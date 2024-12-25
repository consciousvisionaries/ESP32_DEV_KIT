#include <ESPAsyncWebServer.h>

AsyncWebServer server(80);

void setupDashboard() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    String page = "<html><head>";
    page += "<style>";
    page += "body { background-color: black; color: white; text-align: center; font-family: Arial, sans-serif; }";
    page += "#inputsSection { margin: 20px auto; text-align: center; }";
    page += ".input { font-size: 24px; margin: 10px; padding: 10px; border: 2px solid #fff; border-radius: 10px; }";
    page += "</style>";
    page += "<script>";
    page += "function refreshInputs() {";
    page += "  fetch('/getAnalogInputs') ";
    page += "    .then(response => response.text()) ";
    page += "    .then(data => { document.getElementById('inputs').innerHTML = data; });";
    page += "}";
    page += "window.onload = function() {";
    page += "  refreshInputs();";
    page += "  setInterval(refreshInputs, 1000);"; // Refresh every second
    page += "};";
    page += "</script>";
    page += "</head><body>";
    page += "<h1>ESP32 Analog Input Dashboard</h1>";
    page += "<div id='inputsSection'>";
    page += "<h2>Analog Inputs</h2>";
    page += "<div id='inputs'></div>"; // Placeholder for input values
    page += "</div>";
    page += "</body></html>";
    request->send(200, "text/html", page);
  });

  server.on("/getAnalogInputs", HTTP_GET, [](AsyncWebServerRequest *request) {
    String inputs = "<div>";
    inputs += "<div class='input'>Analog 1: " + String(pulseCount1) + "</div>";
    inputs += "<div class='input'>Analog 2: " + String(pulseCount2) + "</div>";
    inputs += "<div class='input'>Analog 3: " + String(pulseCount3) + "</div>";
    inputs += "</div>";
    request->send(200, "text/html", inputs);
  });

  server.begin();
}
