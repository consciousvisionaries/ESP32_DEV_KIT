#define WEBHTML_VERSION V1.1

String getStyle_header() {

  String style = "<style>";
  style += "body { background-color: black; color: white; text-align: center; font-family: Arial, sans-serif; margin: 20px; padding: 10px; }";
  style += "h1 { font-size: 36px; margin-bottom: 20px; }";
  style += "h2 { font-size: 28px; font-weight: bold; margin-bottom: 15px; }";
  style += "h3 { font-size: 20px; font-weight: bold; margin-bottom: 15px; }";

  return style;
}

String getStyle_footer() {
  String style = "";
  
  //style += "@media (max-width: 600px) { body { font-size: 16px; } .input, .output { font-size: 20px; } #led-matrix-container { max-width: 100%; } button { font-size: 18px; } }";
  style += "</style>";
  return style;
}

// 5. refreshInputs_dataHTML() Function
String refreshInputs_dataHTML() {

  styleHTML += "#inputsSection, #outputsSection { margin: 20px auto; text-align: center; }";
  styleHTML += ".input, .output { font-size: 28px; margin: 10px; padding: 12px; border: 2px solid #fff; border-radius: 12px; transition: background-color 0.3s ease; }";
  styleHTML += ".input:hover, .output:hover { background-color: #333; }";
  styleHTML += "#led-matrix-container { padding: 20px; border: 2px solid #888; border-radius: 8px; background-color: #f9f9f9; box-shadow: 0px 4px 8px rgba(0, 0, 0, 0.1); max-width: 400px; margin: auto; transition: border-color 0.3s ease; }";
  styleHTML += "#led-matrix-container:hover { border-color: #4CAF50; }";

  onloadHTML += "refreshInputs_data();";
  onloadHTML += "updateInputIndicators();";
  setIntervalHTML += "setInterval(updateInputIndicators, 500);";

  bodyDivHTML += "<div id='inputsSection'>";
  bodyDivHTML += "<p>";
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

  styleHTML += ".indicator { display: inline-block; width: 100px; height: 100px; border-radius: 50%; margin: 5px; box-shadow: 0px 2px 4px rgba(0, 0, 0, 0.2); transition: background-color 0.3s ease; }";
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

  styleHTML += "#led-matrix { display: flex; flex-direction: column; gap: 8px; align-items: center; }";
  styleHTML += "#buttonsSection { margin-top: 20px; }";
  styleHTML += "button { padding: 10px 20px; font-size: 20px; color: white; background-color: red; border: 2px solid #fff; border-radius: 8px; cursor: pointer; margin: 10px; transition: background-color 0.3s ease, transform 0.2s ease; }";
  styleHTML += "button:hover { transform: scale(1.1); }";
  styleHTML += ".red { background-color: red; }";
  styleHTML += ".green { background-color: green; }";

  onloadHTML += "refreshOutputs_data();";

  setIntervalHTML += "setInterval(refreshOutputs_data, 250);";

  bodyDivHTML += "<div id='outputsSection'>";
  bodyDivHTML += "<p>";

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

String refreshAdmin_dataHTML() {

    styleHTML += "body { font-family: Arial, sans-serif; background-color: #f4f4f9; margin: 0; padding: 0; text-align: center; }";
    styleHTML += "h1 { background-color: #4CAF50; color: white; padding: 20px; }";
    styleHTML += "h3 { color: #333; margin-top: 20px; }";
    styleHTML += "form { display: inline-block; text-align: left; background: white; border: 1px solid #ccc; padding: 20px; border-radius: 10px; box-shadow: 0 0 10px rgba(0, 0, 0, 0.1); }";
    styleHTML += "input[type='text'] { width: 100%; padding: 8px; margin: 10px 0; border: 1px solid #ccc; border-radius: 5px; }";
    styleHTML += "button { background-color: #4CAF50; color: white; padding: 10px 20px; border: none; border-radius: 5px; cursor: pointer; }";
    styleHTML += "button:hover { background-color: #45a049; }";

    bodyDivHTML += "<h1>Admin Panel</h1>";

    // Module Information
    bodyDivHTML += "<h3>Module Information</h3>";
    bodyDivHTML += "<p><strong>Module:</strong> " + String(MODULE) + "</p>";
    bodyDivHTML += "<p><strong>Puzzle Name:</strong> " + String(PUZZLE_NAME) + "</p>";
    bodyDivHTML += "<p><strong>Designer:</strong> " + String(DESIGNER_NAME) + "</p>";
    bodyDivHTML += "<p><strong>Technician:</strong> " + String(TECH_NAME) + "</p>";
    bodyDivHTML += "<p><strong>Model:</strong> " + String(MYSTTECH_MODEL) + "</p>";

    // Global Settings Form
    bodyDivHTML += "<h3>Global Settings</h3>";
    bodyDivHTML += "<form action='/saveGlobalSettings' method='POST'>";
    bodyDivHTML += "<label for='nrTab'>Tab:</label>";
    bodyDivHTML += "<input type='text' id='nrTab' name='nrTab' value='" + String(globalSettings.nrTab) + "'><br><br>";
    bodyDivHTML += "<label for='nrGroup'>Group:</label>";
    bodyDivHTML += "<input type='text' id='nrGroup' name='nrGroup' value='" + String(globalSettings.nrGroup) + "'><br><br>";
    bodyDivHTML += "<label for='storedVersion'>Version:</label>";
    bodyDivHTML += "<input type='text' id='storedVersion' name='storedVersion' value='" + String(wifiSettings.storedVersion) + "'><br><br>";
    bodyDivHTML += "<label for='mqttServer'>MQTT Server:</label>";
    bodyDivHTML += "<input type='text' id='mqttServer' name='mqttServer' value='" + String(mqttSettings.mqttServer) + "'><br><br>";
    bodyDivHTML += "<button type='submit'>Save Global Settings</button>";
    bodyDivHTML += "</form>";

    // WiFi Settings
    bodyDivHTML += "<h3>WiFi Settings</h3>";
    bodyDivHTML += "<form action='/saveConfig' method='POST'>";
    bodyDivHTML += "<label for='ssid'>SSID:</label>";
    bodyDivHTML += "<input type='text' id='ssid' name='ssid' value='" + wifiSettings.ssid + "'><br><br>";
    bodyDivHTML += "<label for='password'>Password:</label>";
    bodyDivHTML += "<input type='text' id='password' name='password' value='" + wifiSettings.password + "'><br><br>";
    bodyDivHTML += "<button type='submit'>Save Changes</button>";
    bodyDivHTML += "</form>";
    
    String script = "";
    return script;
}

String refreshNavigationButtons_dataHTML() {
  
  // Add navigation styles to styleHTML
  styleHTML += "#navMenu { margin-top: 20px; text-align: center; }";
  styleHTML += "#navMenu a {";
  styleHTML += "  padding: 10px 20px;"; // Increased size by 50%
  styleHTML += "  font-size: 20px;"; // Increased font size by 50%
  styleHTML += "  color: white;";
  styleHTML += "  background-color: blue;";
  styleHTML += "  border: 2px solid white;";
  styleHTML += "  border-radius: 8px;"; // Increased border-radius for rounded corners
  styleHTML += "  cursor: pointer;";
  styleHTML += "  margin: 15px;"; // Increased margin for better spacing
  styleHTML += "  transition: background-color 0.3s ease, transform 0.2s ease;";
  styleHTML += "  text-decoration: none;"; // Remove underline
  styleHTML += "}";
  styleHTML += "#navMenu a:hover {";
  styleHTML += "  transform: scale(1.1);"; // Hover effect: Increase size
  styleHTML += "  background-color: #0056b3;"; // Hover background color
  styleHTML += "}";
  
  // Add HTML for navigation buttons
  String navHTML = "<div id='navMenu'>";
  navHTML += globalHyperlinks.adminButtonHTML;
  navHTML += "<p></p><p></p><p></p>";
  navHTML += globalHyperlinks.homeButtonHTML;
  navHTML += "</div>";

  return navHTML;
}




// 7. scriptHeader_HTML() Function
String scriptHeader_HTML() {
  return "<script>";
}

// 8. headFooter_HTML() Function
String headFooter_HTML() {
  return "</script></head>";
}

// 9. bodyHeader_HTML() Function
String bodyHeader_HTML() {
  return "<body><header><h1>" + String(PUZZLE_NAME) + "</h1></header>";
}

// 10. bodyTitle_HTML() Function
String bodyTitle_HTML() {
  return "<h2>" + String(TECH_NAME) + " - " + String(MYSTTECH_MODEL) + "</h2>";
}
