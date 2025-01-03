#define WEBHTML_VERSION V1.1

String getHTML_headerHTML() {
  return "<html>";
}

String getHead_headerHTML() {
  return "<head>";
}

String getHead_metaHTML() {
   return "<meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">";
}

String getStyle_headerHTML() {
  return "<style>";
}

String getStyle_fullScreenDiv() {
  String scriptHTML = "";
  scriptHTML += "function goFullScreen() {";
  scriptHTML += "var elem = document.getElementById('fullScreenDiv');";
  scriptHTML += "if (elem.requestFullscreen) {";
  scriptHTML += "  elem.requestFullscreen();";      
  scriptHTML += "} else if (elem.mozRequestFullScreen) { // Firefox";
  scriptHTML += "  elem.mozRequestFullScreen();";
  scriptHTML += "} else if (elem.webkitRequestFullscreen) { // Chrome, Safari";
  scriptHTML += "  elem.webkitRequestFullscreen();";
  scriptHTML += "} else if (elem.msRequestFullscreen) { // IE/Edge";
  scriptHTML += "elem.msRequestFullscreen();";
  scriptHTML += "}}";

  scriptHTML += "function exitFullScreen() {";
  scriptHTML += "      if (document.exitFullscreen) {";
  scriptHTML += "          document.exitFullscreen();";
  scriptHTML += "      } else if (document.mozCancelFullScreen) { // Firefox";
  scriptHTML += "          document.mozCancelFullScreen();";
  scriptHTML += "      } else if (document.webkitExitFullscreen) { // Chrome, Safari";
  scriptHTML += "          document.webkitExitFullscreen();";
  scriptHTML += "      } else if (document.msExitFullscreen) { // IE/Edge";
  scriptHTML += "          document.msExitFullscreen();";
  scriptHTML += "      }}";
  
  String bodyDivHTML = "<div id=\"fullScreenDiv\">";
  bodyDivHTML += "<button onclick=\"goFullScreen()\">Go Full Screen</button>";
  bodyDivHTML += "</div>";
  
  return "#fullScreenDiv { width: 300px; height: 200px; background-color: lightblue; margin: 20px; }";
}

String getStyle_body_backgroundBlack(bool black) {
  if (!black) {
    return "body { background-color: white; color: black; text-align: center; font-family: Arial, sans-serif; margin: 20px; padding: 10px; }";
  } else {
    return "body { background-color: black; color: white; text-align: center; font-family: Arial, sans-serif; margin: 20px; padding: 10px; }";
  }
}

String getStyle_headings() {
  String style = "h1 { font-size: 36px; margin-bottom: 20px; }";
  style += "h2 { font-size: 28px; font-weight: bold; margin-bottom: 15px; }";
  style += "h3 { font-size: 20px; font-weight: bold; margin-bottom: 15px; }";
  return style;
}

String getStyle_buttons(String backgroundColor, String color) {
   String style = "button { background-color: " + backgroundColor + "; color: " + color + "; padding: 10px 20px; border: none; border-radius: 5px; cursor: pointer; }";
   style += "button:hover { background-color: #45a049; }";
   return style;
}

String getStyle_footerHTML() {
  return "</style>";
}

String getScript_headerHTML() {
  return "<script>";
}

String getScript_footerHTML () {
  return "</script>";
}

String getHead_footerHTML() {
  return "</head>";
}

String getBody_headerHTML() {
  return "<body>";
}

String getBody_TitleHTML() {
  return "<h2>" + String(TECH_NAME) + " - " + String(MYSTTECH_MODEL) + "</h2>";
}

String getBody_ButtonFullscreenHTML() {
  return "<div id=\'fullScreenDiv\"><button onclick=\"goFullScreen()\">Go Full Screen</button></div>";
}

String getBody_ButtonExitScreenHTML() {
  return "<div id=\"fullScreenDiv\"><button onclick=\"exitFullScreen()\">Exit Full Screen</button></div>";
}

String getBody_footerHTML() {
  return "</body>";
}

String getHTML_footerHTML() {
  return "</html>";
}

String refreshInputs_dataHTML() {

  styleHTML += "#inputsSection, #outputsSection { margin: 20px auto; text-align: center; }";
  styleHTML += ".input, .output { font-size: 28px; margin: 10px; padding: 12px; border: 2px solid #fff; border-radius: 12px; transition: background-color 0.3s ease; }";
  styleHTML += ".input:hover, .output:hover { background-color: #333; }";
  
  onloadHTML += "refreshInputs_data();";
  onloadHTML += "updateInputIndicators();";
  setIntervalHTML += "setInterval(updateInputIndicators, 500);";

  bodyDivHTML += "<div id='inputsSection'>";
  bodyDivHTML += "<h3>" + String(bodyInputTitle) + "</h3>";
  bodyDivHTML += "<div id='inputs'>Waiting for inputs...</div>";
  bodyDivHTML += "</div>";

  String script = "function refreshInputs_data() {";
  script += "fetch('/refreshInputs_dataHTML') ";
  script += ".then(response => response.text()) ";
  script += ".then(data => { document.getElementById('inputs').innerHTML = data; });";
  script += "}";
  return script;
}

// Function to update indicator states (green/red based on input status)
String updateInputIndicatorsFunctionality() {

  styleHTML += ".indicator { display: inline-block; width: 40px; height: 40px; border-radius: 50%; margin: 5px; box-shadow: 0px 2px 4px rgba(0, 0, 0, 0.2); transition: background-color 0.3s ease; }";
  styleHTML += ".indicator.green { background-color: green; }";
  styleHTML += ".indicator.red { background-color: red; }";
  

  String script = "function updateInputIndicators() {";
  script += "  const inputs = document.querySelectorAll('[id^=\"inputIndicator\"]');";  // Select all input indicators
  script += "  inputs.forEach(indicator => {";
  script += "    const inputNumber = indicator.id.replace('inputIndicator', '');";  // Extract input number from indicator ID
  script += "    fetch('/getInputState?input=' + inputNumber)";
  script += "    .then(response => response.text())";
  script += "    .then(stateInput => {";
  script += "      indicator.className = (stateInput === 'low') ? 'indicator green' : 'indicator red';";  // Set indicator color based on state
  script += "    });";
  script += "  });";
  script += "}";
  return script;
}

// 6. refreshOutputs_dataHTML() Function
String refreshOutputs_dataHTML() {

  styleHTML += "#buttonsSection { margin-top: 80px; }";
  styleHTML += "button { padding: 10px 20px; font-size: 15px; color: white; background-color: red; border: 2px solid #fff; border-radius: 8px; cursor: pointer; margin: 10px; transition: background-color 0.3s ease, transform 0.2s ease; }";
  styleHTML += "button:hover { transform: scale(1.1); }";
  styleHTML += ".red { background-color: red; }";
  styleHTML += ".green { background-color: green; }";

  onloadHTML += "refreshOutputs_data();";

  setIntervalHTML += "setInterval(refreshOutputs_data, 250);";

  bodyDivHTML += "<div id='outputsSection'>";
  bodyDivHTML += "<h2>" + String(bodyOutputTitle) + "</h2>";
  bodyDivHTML += "<div id='outputs'>Waiting for outputs...</div>";
  bodyDivHTML += "</div>";

  bodyDivHTML += generateOutputButtonsHTML(); // Add buttons section

  String script = "function refreshOutputs_data() {";
  script += "fetch('/refreshOutputs_dataHTML') ";
  script += ".then(response => response.text()) ";
  script += ".then(data => { document.getElementById('outputs').innerHTML = data; })";
  script += ".then(() => { updateButtonColor(outputNumber); });";
  script += "}";

  script += "function toggleOutput(outputNumber) {";
  script += "fetch('/toggleOutputState?output=' + outputNumber) ";
  script += ".then(() => { updateButtonColor(outputNumber); })";
  script += ".catch(error => console.error('Error toggling output:', error));";
  script += "}";

  script += "function updateButtonColor(outputNumber) {";
  script += "const button = document.getElementById('outputButton' + outputNumber);";
  script += "fetch('/getOutputState?output=' + outputNumber)";
  script += ".then(response => response.text())";
  script += ".then(state => { button.className = (state === 'low') ? 'green' : 'red'; });";
  script += "}";

  return script;
}

String refreshDialsLEDs_dataHTML() {
  
  styleHTML += "#led-matrix-container { padding: 15px; border: 2px solid #888; border-radius: 8px; background-color: #f9f9f9; box-shadow: 0px 4px 8px rgba(0, 0, 0, 0.1); max-width: 400px; margin: auto; }";
  styleHTML += "#led-matrix-container h3 { text-align: center; color: #333; font-family: Arial, sans-serif; }";
  styleHTML += "#led-matrix-container p { margin: 5px 0; color: #555; font-size: 14px; font-family: Arial, sans-serif; }";

  String script = "function refreshInputs() {";
  script += "  fetch('/getAnalogInputs') ";
  script += "    .then(response => response.text()) ";
  script += "    .then(data => { document.getElementById('inputs').innerHTML = data; });";
  script += "}";
  script += "function refreshLEDMatrix() {";
  script += "  fetch('/getLEDMatrix') ";
  script += "    .then(response => response.json()) ";
  script += "    .then(payload => { renderLEDMatrix(payload); });";
  script += "}";
  script += "function renderLEDMatrix(payload) {";
  script += "  document.getElementById('puzzle-name').textContent = payload.puzzleName || 'Unnamed Puzzle';";
  script += "  document.getElementById('group').textContent = payload.group || 'Unknown Group';";
  script += "  document.getElementById('tab').textContent = payload.tab || 'Unknown Tab';";
  script += "  document.getElementById('ip-address').textContent = payload.ipAddress || 'Unknown IP';";
  script += "  const ledMatrix = document.getElementById('led-matrix');";
  script += "  ledMatrix.innerHTML = '';";
  script += "  for (let c = 0; c < 3; c++) {";
  script += "    let ledStrip = payload[`led_strip_${c+1}`];";
  script += "    if (Array.isArray(ledStrip)) {";
  script += "      const rowDiv = document.createElement('div');";
  script += "      for (let i = 0; i < ledStrip.length; i++) {";
  script += "        const indicator = document.createElement('span');";
  script += "        indicator.className = `indicator ${ledStrip[i] === 'on' ? 'green' : 'red'}`;";
  script += "        rowDiv.appendChild(indicator);";
  script += "      }";
  script += "      ledMatrix.appendChild(rowDiv);";
  script += "    }";
  script += "  }";
  script += "}";
  
  onloadHTML += "  refreshInputs();";
  onloadHTML += "  refreshLEDMatrix();";
  
  setIntervalHTML += "  setInterval(refreshInputs, 1000);";
  setIntervalHTML += "  setInterval(refreshLEDMatrix, 5000);";
  
  bodyDivHTML += "<h1>ESP32 Dashboard</h1>";
  bodyDivHTML += "<div id='inputsSection'>";
  bodyDivHTML += "<h2>Analog Inputs</h2>";
  bodyDivHTML += "<div id='inputs'></div>";
  bodyDivHTML += "</div>";
  bodyDivHTML += "<div id='led-matrix-container'>";
  bodyDivHTML += "<h3 id='puzzle-name'>Loading...</h3>";
  bodyDivHTML += "<p><strong>Group:</strong> <span id='group'>N/A</span></p>";
  bodyDivHTML += "<p><strong>Tab:</strong> <span id='tab'>N/A</span></p>";
  bodyDivHTML += "<p><strong>IP Address:</strong> <span id='ip-address'>N/A</span></p>";
  bodyDivHTML += "<div id='led-matrix'></div>";
  bodyDivHTML += "</div>";
  
  return script;
}


String refreshAdmin_dataHTML() {

    styleHTML += getStyle_body_backgroundBlack(false);
    styleHTML += getStyle_headings();
    styleHTML += getStyle_buttons("#4CAF50", "white");

    styleHTML += "form { display: inline-block; text-align: left; background: white; border: 1px solid #ccc; padding: 20px; border-radius: 10px; box-shadow: 0 0 10px rgba(0, 0, 0, 0.1); }";
    styleHTML += "input[type='text'] { width: 100%; padding: 8px; margin: 10px 0; border: 1px solid #ccc; border-radius: 5px; }";
   
    bodyDivHTML += "<h1>Admin Panel</h1>";

    // Module Information
    bodyDivHTML += "<form action='/editConfig' method='POST'>";
    bodyDivHTML += "<h3>Module Information</h3>";
    bodyDivHTML += "<p><strong>Module:</strong> " + String(MYSTTECH_MODEL) + "</p>";
    bodyDivHTML += "<p><strong>Puzzle Name:</strong> " + String(PUZZLE_NAME) + "</p>";
    bodyDivHTML += "<p><strong>Designer:</strong> " + String(DESIGNER_NAME) + "</p>";
    bodyDivHTML += "<p><strong>Technician:</strong> " + String(TECH_NAME) + "</p>";
    bodyDivHTML += "<p><strong>Model:</strong> " + String(MYSTTECH_MODEL) + "</p>";
    bodyDivHTML += "<p><strong>Version:</strong> " + wifiSettings.storedVersion + "</p>";
    bodyDivHTML += "<p><strong>IP Address:</strong> " + wifiSettings.ipaddress + "</p>";
    bodyDivHTML += "<p><strong>Version:</strong> " + wifiSettings.storedVersion + "</p>";
    bodyDivHTML += "<button type='submit' name='action' value='CONFIG'>Edit GPIO Config</button>";
    bodyDivHTML += "<h3>Node-Red Configurations</h3>";
    bodyDivHTML += "<p><strong>MQTT Server:</strong> " + mqttSettings.mqttServer + "</p>";
    bodyDivHTML += "<p><strong>MQTT Username:</strong> " + mqttSettings.mqttUsername + "</p>";
    bodyDivHTML += "<button type='submit' name='action' value='NODERED'>Edit Node-Red Config</button>";
    bodyDivHTML += "<h3>Network Settings</h3>";
    bodyDivHTML += "<p><strong>WiFi SSID:</strong> " + wifiSettings.ssid + "</p>";
    bodyDivHTML += "<p><strong>WiFi Password:</strong> " + wifiSettings.password + "</p>";
    bodyDivHTML += "<button type='submit' name='action' value='WIFI'>Edit WiFi Config</button>";
    bodyDivHTML += "</form>";
    
    String script = "";
    return script;
}

String refreshConfig_dataHTML() {

    styleHTML += getStyle_body_backgroundBlack(false);
    styleHTML += getStyle_headings();
    styleHTML += getStyle_buttons("#4CAF50", "white");

    styleHTML += "form { display: inline-block; text-align: left; background: white; border: 1px solid #ccc; padding: 20px; border-radius: 10px; box-shadow: 0 0 10px rgba(0, 0, 0, 0.1); }";
    styleHTML += "input[type='text'] { width: 100%; padding: 8px; margin: 10px 0; border: 1px solid #ccc; border-radius: 5px; }";
   
    bodyDivHTML += "<h1>GPIO Configuration Panel</h1>";
   
  return "";
}

String refreshNodeRed_dataHTML() {

    styleHTML += getStyle_body_backgroundBlack(false);
    styleHTML += getStyle_headings();
    styleHTML += getStyle_buttons("#4CAF50", "white");

    styleHTML += "form { display: inline-block; text-align: left; background: white; border: 1px solid #ccc; padding: 20px; border-radius: 10px; box-shadow: 0 0 10px rgba(0, 0, 0, 0.1); }";
    styleHTML += "input[type='text'] { width: 100%; padding: 8px; margin: 10px 0; border: 1px solid #ccc; border-radius: 5px; }";

    bodyDivHTML += "<h1>Node-Red Configuration Panel</h1>";
    bodyDivHTML += "<form action='/saveRedNode' method='POST'>";
    bodyDivHTML += "<h3>MQTT Network Details</h3>";
    bodyDivHTML += "<label for='nrTab'>UI Dashboard Tab:</label>";
    bodyDivHTML += "<input type='text' id='nrTab' name='nrTab' value='" + String(globalSettings.nrTab) + "'><br><br>";
    bodyDivHTML += "<label for='nrGroup'>UI Dashboard Group:</label>";
    bodyDivHTML += "<input type='text' id='nrGroup' name='nrGroup' value='" + String(globalSettings.nrGroup) + "'><br><br>";
    bodyDivHTML += "<label for='mqttServer'>MQTT Server:</label>";
    bodyDivHTML += "<input type='text' id='mqttServer' name='mqttServer' value='" + String(mqttSettings.mqttServer) + "'><br><br>";
    bodyDivHTML += "<label for='mqttUsername'>MQTT Username:</label>";
    bodyDivHTML += "<input type='text' id='mqttUsername' name='mqttUsername' value='" + String(mqttSettings.mqttUsername) + "'><br><br>";
    bodyDivHTML += "<label for='mqttPassword'>MQTT Password:</label>";
    bodyDivHTML += "<input type='text' id='mqttPassword' name='mqttPassword' value='" + String(mqttSettings.mqttPassword) + "'><br><br>";
    bodyDivHTML += "<button type='submit'>Save Red Node Changes</button>";
    bodyDivHTML += "</form>";
    return "";
}

String refreshWiFi_dataHTML() {

    styleHTML += getStyle_body_backgroundBlack(false);
    styleHTML += getStyle_headings();
    styleHTML += getStyle_buttons("#4CAF50", "white");

    styleHTML += "form { display: inline-block; text-align: left; background: white; border: 1px solid #ccc; padding: 20px; border-radius: 10px; box-shadow: 0 0 10px rgba(0, 0, 0, 0.1); }";
    styleHTML += "input[type='text'] { width: 100%; padding: 8px; margin: 10px 0; border: 1px solid #ccc; border-radius: 5px; }";
    
    bodyDivHTML += "<h1>WiFi Configuration Panel</h1>";
    bodyDivHTML += "<form action='/saveWiFi' method='POST'>";
    bodyDivHTML += "<h3>Network Settings</h3>";
    bodyDivHTML += "<p><strong>WiFi SSID:</strong> " + wifiSettings.ssid + "</p>";
    bodyDivHTML += "<p><strong>WiFi Password:</strong> " + wifiSettings.password + "</p>";
    bodyDivHTML += "<label for='ssid'>SSID:</label>";
    bodyDivHTML += "<input type='text' id='ssid' name='ssid' value='" + wifiSettings.ssid + "'><br><br>";
    bodyDivHTML += "<label for='password'>Password:</label>";
    bodyDivHTML += "<input type='text' id='password' name='password' value='" + wifiSettings.password + "'><br><br>";
    bodyDivHTML += "<button type='submit'>Save WiFi Changes</button>";
    bodyDivHTML += "</form>";
    
    return "";
}

String refreshNavigationButtons_dataHTML(String form) {
  
  // Add navigation styles to styleHTML
  styleHTML += "#navMenu { margin-top: 20px; text-align: center; }";
  styleHTML += "#navMenu a {";
  styleHTML += "  background-color: #4CAF50; color: white; padding: 10px 20px; border: none; border-radius: 5px; cursor: pointer; }";
  styleHTML += "  transition: background-color 0.3s ease, transform 0.2s ease;";
  styleHTML += "  text-decoration: none;"; // Remove underline
  styleHTML += "}";
  styleHTML += "#navMenu a:hover {";
  styleHTML += "  transform: scale(1.1);"; // Hover effect: Increase size
  styleHTML += "  hover { background-color: #45a049; }";
  styleHTML += "}";
  
  // Add HTML for navigation buttons
  String navHTML = "<div id='navMenu'>";
    
  if (form != "HOME") {
    navHTML += globalHyperlinks.homeButtonHTML;
  } else { navHTML += globalHyperlinks.adminButtonHTML; }

  navHTML += "</div>";

  return navHTML;
}
