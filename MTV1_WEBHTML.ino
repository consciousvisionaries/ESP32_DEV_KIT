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

String refreshNavigationButtons_dataHTML() {
  // Add navigation styles to styleHTML
  styleHTML += "#navMenu { margin-top: 20px; text-align: center; }";
  styleHTML += "#navMenu button { padding: 10px 20px; font-size: 18px; color: white; background-color: #007BFF; border: 2px solid #fff; border-radius: 8px; cursor: pointer; margin: 10px; transition: background-color 0.3s ease, transform 0.2s ease; }";
  styleHTML += "#navMenu button:hover { transform: scale(1.1); background-color: #0056b3; }";
  
  // Add HTML for navigation buttons
  String navHTML = "<div id='navMenu'>";
  navHTML += globalHyperlinks.adminButtonHTML;
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
