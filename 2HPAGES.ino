

String styleHTML;
String onloadHTML;
String setIntervalHTML;
String bodyDivHTML;
String scriptHTML;
String navButtonHTML;

String generatePage() {

  styleHTML = "";
  onloadHTML = "";
  setIntervalHTML = "";
  bodyDivHTML = "";
  scriptHTML = "";
  navButtonHTML = "";

  // style
  styleHTML += getStyle_headerHTML();
  styleHTML += getStyle_body_backgroundBlack(true);
  styleHTML += getStyle_headings();

  // body
  bodyDivHTML += getBody_headerHTML();
  bodyDivHTML += "<header><h1>" + String(PUZZLE_NAME) + "</h1></header>";
  bodyDivHTML += getBody_TitleHTML();

  // script
  scriptHTML += getScript_headerHTML();        
  if (NUM_INPUTS >= 1) {
    scriptHTML += refreshInputs_dataHTML_retScript(); // Refresh inputs
    scriptHTML += updateInputIndicatorsFunctionality();
  }
  if (NUM_OUTPUTS >= 1) {
    scriptHTML += refreshOutputs_dataHTML(); // Refresh outputs
  }
  scriptHTML += onloadHTML;                
  scriptHTML += setIntervalHTML;   

 
  bodyDivHTML += refreshNavigationButtons_dataHTML();

  // assemble page
  String page = getHTML_headerHTML();
  page += getHead_headerHTML();
  
  styleHTML += getStyle_footerHTML();
  page += styleHTML;
  scriptHTML += getScript_footerHTML();
  page += scriptHTML;         
  page += getHead_footerHTML(); 
  bodyDivHTML += getBody_footerHTML();     
  page += bodyDivHTML;
  page += getHTML_footerHTML();        
  return page;
}

String generateHTMLPage(String script) {

  styleHTML = "";
  onloadHTML = "";
  setIntervalHTML = "";
  bodyDivHTML = "";
  scriptHTML = "";
  navButtonHTML = "";

    String page = getHTML_headerHTML();
    page += getHead_headerHTML();
    page += getStyle_headerHTML();
    
    String newScriptHTML;
    if (script = "ADMIN") {
      newScriptHTML = refreshAdmin_dataHTML();
    } else {
      newScriptHTML = refreshConfig_dataHTML();
    }
    navButtonHTML = refreshNavigationButtons_dataHTML();
    
    page += styleHTML;
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

String generateEditConfigPage() {

  styleHTML = "";
  onloadHTML = "";
  setIntervalHTML = "";
  bodyDivHTML = "";
  scriptHTML = "";
  navButtonHTML = "";

  String page = getHTML_headerHTML();
  page += getHead_headerHTML();
  page += getStyle_headerHTML();

  return page;
}

String generateInputIndicatorsHTML() {
  String indicatorsHTML = "<div id='inputs'>";
  for (int i = 0; i < NUM_INPUTS; i++) {
    
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
  for (int i = 0; i < NUM_OUTPUTS; i++) {
    outputStatus += (digitalRead(outputPins[i]) == HIGH) ? "'red'" : "'green'"; // Output state (red/green)
    if (i < NUM_OUTPUTS - 1) outputStatus += ", ";  // Formatting between items
  }
  outputStatus += "]";
  doc["outputs"] = outputStatus;

  String payload;
  serializeJson(doc, payload);       // Serialize JSON to string
  return payload;
}

String generateOutputButtonsHTML() {
  String buttonsHTML = "<div id='buttonsSection'>";
  buttonsHTML += "<h3>" + String(buttonsHTMLTitle) + "</h3>";

  int numColumns = NUM_OUTPUTS / NUM_OUTPUT_ROWS;  // Calculate number of columns based on rows

  // Ensure even distribution of buttons across rows
  for (int row = 0; row < NUM_OUTPUT_ROWS; row++) {
    buttonsHTML += "<div class='buttonRow'>"; // Start a new row

    for (int col = 0; col < numColumns; col++) {
      int outputIndex = row * numColumns + col;  // Calculate the index of the button

      if (outputIndex < NUM_OUTPUTS) { // Ensure we do not exceed the number of outputs
        String initialColor = (digitalRead(outputPins[outputIndex]) == LOW) ? "green" : "red";
        buttonsHTML += "<button id='outputButton" + String(outputIndex) + "' class='" + initialColor +
                       "' onclick='toggleOutput(" + String(outputIndex) + ")'>" + globalSettings.outputNames[outputIndex] + "</button>";
      }
    }
    buttonsHTML += "</div><br>"; // End the row and add line break for clarity
  }

  buttonsHTML += "</div>";
  return buttonsHTML;
}
