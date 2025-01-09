

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
  if (scriptname == "/admin") {
    newScriptHTML = refreshAdmin_dataHTML();
  } else if (scriptname == "/config") {  // Fixed this line
    newScriptHTML = refreshConfig_dataHTML();
  } else if (scriptname == "/nodered") {  // Fixed this line
    newScriptHTML = refreshNodeRed_dataHTML();
  } else if (scriptname == "/wifi") {  // Fixed this line
    newScriptHTML = refreshWiFi_dataHTML();
  } else if (scriptname == "/home") {
    blackTrue = true;

    if (NUM_ANALOG_INPUTPAIRS >= 1) {
      newScriptHTML += refreshDialsLEDs_dataHTML(); // example. lost > 3 dials > addressable led
    } 
    if (NUM_DIGITAL_INPUTSA >= 1) {
        newScriptHTML += refreshInputs_dataHTML(); // Refresh inputs
        newScriptHTML += updateInputIndicatorsFunctionality();
    }
    if (NUM_DIGITAL_INPUTSB >= 1) {
        newScriptHTML += refreshInputs_dataHTML(); // Refresh inputs
        newScriptHTML += updateInputIndicatorsFunctionality();
    }
    if (NUM_DIGITAL_OUTPUTSA >= 1) {
        newScriptHTML += refreshOutputsA_dataHTML(); // Refresh outputs
    }
    if (NUM_DIGITAL_OUTPUTSB >= 1) {
        newScriptHTML += refreshOutputsB_dataHTML(); // Refresh outputs
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

String generateOutputsA_Payload() {
  StaticJsonDocument<200> doc;
  doc["puzzleName"] = PUZZLE_NAME;    // Example
  doc["mac"] = WiFi.macAddress();     // MAC address of the ESP32

  String outputStatus = "[";
  for (int i = 0; i < NUM_DIGITAL_OUTPUTSA; i++) {
    outputStatus += (digitalRead(outputPinsA[i]) != outputPins_initStateA[i]) ? "'green'" : "'red'";
    if (i < NUM_DIGITAL_OUTPUTSA - 1) outputStatus += ", ";  // Formatting between items
  }
  outputStatus += "]";
  doc["outputs"] = outputStatus;

  

  String payload;
  serializeJson(doc, payload);       // Serialize JSON to string
  Serial.println(payload);

  return payload;
}

String generateOutputsB_Payload() {
  StaticJsonDocument<200> doc;
  doc["puzzleName"] = PUZZLE_NAME;    // Example
  doc["mac"] = WiFi.macAddress();     // MAC address of the ESP32

  String outputStatus = "[";
  for (int i = 0; i < NUM_DIGITAL_OUTPUTSB; i++) {
    outputStatus += (digitalRead(outputPinsB[i]) != outputPins_initStateB[i]) ? "'green'" : "'red'";
    if (i < NUM_DIGITAL_OUTPUTSB - 1) outputStatus += ", ";  // Formatting between items
  }
  outputStatus += "]";
  doc["outputs"] = outputStatus;

  

  String payload;
  serializeJson(doc, payload);       // Serialize JSON to string
  Serial.println(payload);

  return payload;
}

String generateOutputButtonsAHTML() {
  String buttonsHTML = "<div id='buttonsSectionA'>";
  buttonsHTML += "<h3>" + String(buttonsHTMLTitle) + "</h3>";

  int numColumns = NUM_DIGITAL_OUTPUTSA / NUM_OUTPUT_ROWSA;  // Calculate number of columns based on rows

  // Ensure even distribution of buttons across rows
  for (int row = 0; row < NUM_OUTPUT_ROWSA; row++) {
    
    buttonsHTML += "<div class='buttonRow'>"; // Start a new row

    for (int col = 0; col < numColumns; col++) {
      int outputIndex = row * numColumns + col;  // Calculate the index of the button
      if (outputIndex < NUM_DIGITAL_OUTPUTSA) { // Ensure we do not exceed the number of outputs

        String initialColor = (digitalRead(outputPinsA[outputIndex]) == outputPins_initStateA[outputIndex]) ? "red" : "green";
        
    buttonsHTML += "<button id='outputButtonA" + String(outputIndex) + "' class='" + initialColor +
                       "' onclick='toggleOutputA(" + String(outputIndex) + ")'>" + globalSettings.outputNames[outputIndex] + "</button>";
      }
    }
    
    buttonsHTML += "</div><br>"; // End the row and add line break for clarity
  }

  buttonsHTML += "</div>";
  return buttonsHTML;
}

String generateOutputButtonsBHTML() {
  String buttonsHTML = "<div id='buttonsSectionB'>";
  buttonsHTML += "<h3>" + String(buttonsHTMLTitle) + "</h3>";

  int numColumns = NUM_DIGITAL_OUTPUTSB / NUM_OUTPUT_ROWSB;  // Calculate number of columns based on rows

  // Ensure even distribution of buttons across rows
  for (int row = 0; row < NUM_OUTPUT_ROWSB; row++) {
    
    buttonsHTML += "<div class='buttonRow'>"; // Start a new row

    for (int col = 0; col < numColumns; col++) {
      int outputIndex = row * numColumns + col;  // Calculate the index of the button
      if (outputIndex < NUM_DIGITAL_OUTPUTSB) { // Ensure we do not exceed the number of outputs

        String initialColor = (digitalRead(outputPinsB[outputIndex]) == outputPins_initStateB[outputIndex]) ? "red" : "green";
        
    buttonsHTML += "<button id='outputButtonB" + String(outputIndex) + "' class='" + initialColor +
                       "' onclick='toggleOutputB(" + String(outputIndex) + ")'>" + globalSettings.outputNames[outputIndex] + "</button>";
      }
    }
    
    buttonsHTML += "</div><br>"; // End the row and add line break for clarity
  }

  buttonsHTML += "</div>";
  return buttonsHTML;
}
