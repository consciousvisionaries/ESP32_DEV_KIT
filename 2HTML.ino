#define WEBHTML_VERSION V1.1

String getHTML_headerHTML() {
  return "<html>";
}

String getHead_headerHTML() {
  return "<head>";
}

String getStyle_headerHTML() {
  return "<style>";
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

String getBody_footerHTML() {
  return "</body>";
}

String getHTML_footerHTML() {
  return "</html>";
}

// 5. refreshInputs_dataHTML() Function
String refreshInputs_dataHTML_retScript() {
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

    styleHTML += getStyle_body_backgroundBlack(false);
    styleHTML += getStyle_headings();
    styleHTML += getStyle_buttons("#4CAF50", "white");

    styleHTML += "form { display: inline-block; text-align: left; background: white; border: 1px solid #ccc; padding: 20px; border-radius: 10px; box-shadow: 0 0 10px rgba(0, 0, 0, 0.1); }";
    styleHTML += "input[type='text'] { width: 100%; padding: 8px; margin: 10px 0; border: 1px solid #ccc; border-radius: 5px; }";
   
    bodyDivHTML += "<h1>Admin Panel</h1>";

    // Module Information
    bodyDivHTML += "<form action='/editConfig' method='POST'>";
    bodyDivHTML += "<h3>Module Information</h3>";
    bodyDivHTML += "<p><strong>Module:</strong> " + String(MODULE) + "</p>";
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
  if (form != "ADMIN") {
    navHTML += globalHyperlinks.adminButtonHTML;
  } 
  
  if (form != "HOME") {
    navHTML += globalHyperlinks.homeButtonHTML;
  }
  navHTML += "</div>";

  return navHTML;
}
