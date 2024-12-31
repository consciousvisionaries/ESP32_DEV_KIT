// 4. getStyle_cssHTML() Function
String getStyle_cssHTML() {
  String page = "<style>";
  page += "body { background-color: black; color: white; text-align: center; font-family: Arial, sans-serif; margin: 20px; padding: 10px; }";
  page += "#inputsSection, #outputsSection { margin: 20px auto; text-align: center; }";
  page += ".input, .output { font-size: 28px; margin: 10px; padding: 12px; border: 2px solid #fff; border-radius: 12px; transition: background-color 0.3s ease; }";
  page += ".input:hover, .output:hover { background-color: #333; }";
  page += "#led-matrix-container { padding: 20px; border: 2px solid #888; border-radius: 8px; background-color: #f9f9f9; box-shadow: 0px 4px 8px rgba(0, 0, 0, 0.1); max-width: 400px; margin: auto; transition: border-color 0.3s ease; }";
  page += "#led-matrix-container:hover { border-color: #4CAF50; }";
  page += ".indicator { display: inline-block; width: 25px; height: 25px; border-radius: 50%; margin: 5px; box-shadow: 0px 2px 4px rgba(0, 0, 0, 0.2); transition: background-color 0.3s ease; }";
  page += ".indicator.green { background-color: green; }";
  page += ".indicator.red { background-color: red; }";
  page += "h1 { font-size: 36px; margin-bottom: 20px; }";
  page += "h2, h3 { font-size: 28px; font-weight: bold; margin-bottom: 15px; }";
  page += "#led-matrix { display: flex; flex-direction: column; gap: 8px; align-items: center; }";
  page += "#buttonsSection { margin-top: 20px; }";
  page += "button { padding: 10px 20px; font-size: 20px; color: white; background-color: red; border: 2px solid #fff; border-radius: 8px; cursor: pointer; margin: 10px; transition: background-color 0.3s ease, transform 0.2s ease; }";
  page += "button:hover { transform: scale(1.1); }";
  page += ".red { background-color: red; }";
  page += ".green { background-color: green; }";
  page += "@media (max-width: 600px) { body { font-size: 16px; } .input, .output { font-size: 20px; } #led-matrix-container { max-width: 100%; } button { font-size: 18px; } }";
  page += "</style>";
  return page;
}

// 5. refreshInputs_dataHTML() Function
String refreshInputs_dataHTML() {
  onloadHTML += "refreshInputs_data();";
  onloadHTML += "updateInputIndicators();";
  setIntervalHTML += "setInterval(updateInputIndicators, 1000);";
  
  bodyDivHTML += "<div id='inputsSection'>";
  bodyDivHTML += "<h2>Inputs</h2>";
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
  onloadHTML += "refreshOutputs_data();";
  
  setIntervalHTML += "setInterval(refreshOutputs_data, 500);";
  setIntervalHTML += "setInterval(updateButtonColor(outputNumber),500);";

  bodyDivHTML += "<div id='outputsSection'>";
  bodyDivHTML += "<h2>Outputs</h2>";
  bodyDivHTML += "<div id='outputs'>Waiting for outputs...</div>";
  bodyDivHTML += "</div>";

  bodyDivHTML += generateOutputButtonsHTML(); // Add buttons section

  String script = "function refreshOutputs_data() {";
  script += "fetch('/refreshOutputs_dataHTML') ";
  script += ".then(response => response.text()) ";
  script += ".then(data => { document.getElementById('outputs').innerHTML = data; });";
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
  script += ".then(state => {";
  script += "button.className = (state === 'low') ? 'green' : 'red';";
  script += "});";
  script += "}";

  

  return script;
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
  return "<h2>Interactive Dashboard</h2>";
}
