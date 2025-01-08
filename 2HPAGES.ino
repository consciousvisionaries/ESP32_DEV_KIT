

String styleHTML;
String onloadHTML;
String setIntervalHTML;
String bodyDivHTML;
String scriptHTML;
String navButtonHTML;

String generateHTMLPage(String scriptname) {

  styleHTML = "";
  onloadHTML = "";
  setIntervalHTML = "";
  bodyDivHTML = "";
  scriptHTML = "";
  navButtonHTML = "";

  String page = getHTML_headerHTML();
  page += getHead_headerHTML();
  page += getHead_metaHTML();
  page += getStyle_headerHTML();

  bool blackTrue = false;
  String newScriptHTML;
  if (scriptname == "ADMIN") {
    newScriptHTML = refreshAdmin_dataHTML();
  } else if (scriptname == "CONFIG") {  // Fixed this line
    newScriptHTML = refreshConfig_dataHTML();
  } else if (scriptname == "NODERED") {  // Fixed this line
    newScriptHTML = refreshNodeRed_dataHTML();
  } else if (scriptname == "WIFI") {  // Fixed this line
    newScriptHTML = refreshWiFi_dataHTML();
  } else if (scriptname == "HOME") {
    blackTrue = true;
    if (NUM_DIGITAL_INPUTS >= 1) {
        newScriptHTML += refreshInputs_dataHTML(); // Refresh inputs
        newScriptHTML += updateInputIndicatorsFunctionality();
    }
    if (NUM_DIGITAL_OUTPUTS >= 1) {
        newScriptHTML += refreshOutputs_dataHTML(); // Refresh outputs
    }
    if (NUM_ANALOG_INPUTS >= 1) {
      newScriptHTML += refreshDialsLEDs_dataHTML(); // example. lost > 3 dials > addressable led
    }
    if (NUM_FLED_CHANNELS >= 1) {
      newScriptHTML += refreshOutputsMatrixSection_dataHTML();
    }
  }
  
  
  navButtonHTML = refreshNavigationButtons_dataHTML(scriptname);

  page += styleHTML;
  page += getStyle_body_backgroundBlack(blackTrue);
  page += getStyle_footerHTML();
  page += getScript_headerHTML();
  page += scriptHTML;
  page += newScriptHTML;
  page += onloadHTML;
  page += setIntervalHTML;
  page += getScript_footerHTML();
  page += getHead_footerHTML();
  page += getBody_headerHTML();
  page += bodyDivHTML;
  page += navButtonHTML;

  page += getBody_footerHTML();
  page += getHTML_footerHTML();
  return page;
}

String generateAnalogInputDisplayHTML() {

  for (int i = 0; i < NUM_ANALOG_INPUTPAIRS; i++) {
    
  }

  return "";
}

String generate_inputIndicatorHTML(int channels, int indicators) {
  String indicatorsHTML = "<div id='indicatorMatrixGroup'>";

  for (int c = 0; c < channels; c++) {
    for (int i = 0; i < indicators; i++) {
      indicatorsHTML += "<div id='indicatorDot" + String(c*i) + "' class='indicator red'></div>";
    }
  }
  indicatorsHTML += "</div>";
  return indicatorsHTML;
}

String generateInputIndicatorsHTML(int indicators) {
  String indicatorsHTML = "<div id='inputs'>";
  for (int i = 0; i < indicators; i++) {
    
    indicatorsHTML += "<div id='inputIndicator" + String(i) + "' class='indicator red'></div>";
  }
  indicatorsHTML += "</div>";
  return indicatorsHTML;
}

String generateOutputsPayload() {
  StaticJsonDocument<200> doc;
  doc["puzzleName"] = PUZZLE_NAME;    // Example
  doc["mac"] = WiFi.macAddress();     // MAC address of the ESP32

  String outputStatus = "[";
  for (int i = 0; i < NUM_DIGITAL_OUTPUTS; i++) {
    outputStatus += (digitalRead(outputPins[i]) == !outputPins_initState[i]) ? "'green'" : "'red'";
    if (i < NUM_DIGITAL_OUTPUTS - 1) outputStatus += ", ";  // Formatting between items
  }
  outputStatus += "]";
  doc["outputs"] = outputStatus;

  

  String payload;
  serializeJson(doc, payload);       // Serialize JSON to string
  Serial.println(payload);

  return payload;
}

String generateOutputButtonsHTML() {
  
  String buttonsHTML = "<div id='buttonsSection'>";
  buttonsHTML += "<h3>" + String(buttonsHTMLTitle) + "</h3>";
  #define NUM_OUTPUT_ROWS 2

  int numColumns = NUM_DIGITAL_OUTPUTS / NUM_OUTPUT_ROWS;  // Calculate number of columns based on rows

  // Ensure even distribution of buttons across rows
  for (int row = 0; row < NUM_OUTPUT_ROWS; row++) {
    
    buttonsHTML += "<div class='buttonRow'>"; // Start a new row

    for (int col = 0; col < numColumns; col++) {
      int outputIndex = row * numColumns + col;  // Calculate the index of the button
      if (outputIndex < NUM_DIGITAL_OUTPUTS) { // Ensure we do not exceed the number of outputs

        String initialColor = (digitalRead(outputPins[outputIndex]) == LOW) ? "green" : "red";
        
        Serial.print("...creating outputPins... reading pin states... setting color... ");
        
    buttonsHTML += "<button id='outputButton" + String(outputIndex) + "' class='" + initialColor +
                       "' onclick='toggleOutput(" + String(outputIndex) + ")'>" + globalSettings.outputNames[outputIndex] + "</button>";
      }
    }
    
    buttonsHTML += "</div><br>"; // End the row and add line break for clarity
            Serial.println(buttonsHTML);
  }

  buttonsHTML += "</div>";
  return buttonsHTML;
}
