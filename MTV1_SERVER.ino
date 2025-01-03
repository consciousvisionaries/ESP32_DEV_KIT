#include <ESPAsyncWebServer.h>

#define WEBSERVER_VERSION V1.1

AsyncWebServer server(80);

String styleHTML;
String onloadHTML;
String setIntervalHTML;
String bodyDivHTML;
String scriptHTML;

String generatePage() {

  styleHTML = "";
  onloadHTML = "";
  setIntervalHTML = "";
  bodyDivHTML = "";
  scriptHTML = "";

  String page = "<html><head>";
  
  styleHTML += getStyle_header(); // Adds CSS style
  
  scriptHTML += scriptHeader_HTML();        // Adds JavaScript header

  if (NUM_INPUTS >= 1) {
    scriptHTML += refreshInputs_dataHTML(); // Refresh inputs
    scriptHTML += updateInputIndicatorsFunctionality();
  }
  if (NUM_OUTPUTS >= 1) {
    scriptHTML += refreshOutputs_dataHTML(); // Refresh outputs
  }
  page += styleHTML;
  page += getStyle_footer();
  page += scriptHTML;
  page += onloadHTML;                 // Onload script
  page += setIntervalHTML;            // Set interval script
  page += headFooter_HTML();          // Adds head and footer HTML
  page += bodyHeader_HTML();          // Adds body header HTML
  page += bodyTitle_HTML();           // Adds body title HTML
  page += bodyDivHTML;                // Adds body div content

  page += "</body></html>";           // Closing HTML tags
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
                       "' onclick='toggleOutput(" + String(outputIndex) + ")'>" + outputNames[outputIndex] + "</button>";
      }
    }
    buttonsHTML += "</div><br>"; // End the row and add line break for clarity
  }

  buttonsHTML += "</div>";
  return buttonsHTML;
}

void toggleOutputState(int outputNumber) {
  if (outputNumber >= 0 && outputNumber < NUM_OUTPUTS) {
    int pin = outputPins[outputNumber];

    bool currentState = digitalRead(pin);  // Read the current state before toggling
    bool newState = (currentState == HIGH) ? LOW : HIGH;  // Toggle state
    digitalWrite(pin, newState);  // Set the new state

    // Confirm the state change
    Serial.print("Output ");
    Serial.print(outputNumber);
    Serial.print(": Toggled to ");
    Serial.println(newState ? "HIGH" : "LOW");

  } else {
    Serial.println("Error: Invalid output number.");
  }
}

String getInputState(int inputPin) {
  int state = digitalRead(inputPin);
  return (state == HIGH) ? "high" : "low";  // Return "high" or "low"
}

void setupDashboard() {
  // Initialize the server routes

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/html", generatePage());
  });

  server.on("/refreshInputs_dataHTML", HTTP_GET, [](AsyncWebServerRequest *request) {
    String inputData = generateInputIndicatorsHTML();
    request->send(200, "text/plain", inputData);
  });

  // Server route for outputs data
  server.on("/refreshOutputs_dataHTML", HTTP_GET, [](AsyncWebServerRequest *request) {
    // Generate and return the output data payload
    String outputData = generateOutputsPayload();
    request->send(200, "application/json", outputData);
  });

  // Route to toggle output state
  server.on("/toggleOutputState", HTTP_GET, [](AsyncWebServerRequest *request) {
    String outputNumberStr = request->getParam("output")->value();
    int outputNumber = outputNumberStr.toInt();

    // Toggle the output state
    Serial.print("Toggle Output: ");
    Serial.println(outputNumber);

    toggleOutputState(outputNumber);
    request->send(200, "text/plain", "Output toggled");
  });

  // Route to get the state of an output
  server.on("/getOutputState", HTTP_GET, [](AsyncWebServerRequest *request) {
    String outputNumberStr = request->getParam("output")->value();
    int outputNumber = outputNumberStr.toInt();

    if (outputNumber >= 0 && outputNumber < NUM_OUTPUTS) {
      bool state = digitalRead(outputPins[outputNumber]) == HIGH ? true : false;
      request->send(200, "text/plain", state ? "high" : "low");
    } else {
      request->send(400, "text/plain", "Invalid output number");
    }
  });

  // Route to get the state of an input
server.on("/getInputState", HTTP_GET, [](AsyncWebServerRequest *request) {
  String inputNumberStr = request->getParam("input")->value();
  int inputNumber = inputNumberStr.toInt();

  if (inputNumber >= 0 && inputNumber < NUM_INPUTS) {
    int pin = inputPins[inputNumber];  // You should have an array for input pins
    String state = getInputState(pin);  // Get the state of the input pin
    request->send(200, "text/plain", state);  // Send the state back
  } else {
    request->send(400, "text/plain", "Invalid input number");
  }
});

  // Start the server
  server.begin();
}
