#include <ESPAsyncWebServer.h>

AsyncWebServer server(80);

void setupDashboard() {
    // Main page route
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        String inputData = generateHTMLPage("/home");
        Serial.println(inputData);
        request->send(200, "text/html", inputData);
    });

    // Route to refresh input indicators
    server.on("/refreshInputs_dataHTML", HTTP_GET, [](AsyncWebServerRequest *request) {
        String inputData = generateInputIndicatorsHTML(NUM_DIGITAL_INPUTSA);
        Serial.println(inputData);
        request->send(200, "text/html", inputData);
    });

    server.on("/getMatrixData", HTTP_GET, [](AsyncWebServerRequest *request) {
        int indicators = (NUM_FLED_ADDLEDS / NUM_FLED_CHANNELS);
        String matrixHTML = generate_inputIndicatorHTML(NUM_FLED_CHANNELS, indicators);
        request->send(200, "text/html", matrixHTML);
    });

    // Route to refresh output data
    server.on("/refreshOutputsA_dataHTML", HTTP_GET, [](AsyncWebServerRequest *request) {
        String payload = generateOutputsA_Payload();
        Serial.println(payload);
        request->send(200, "application/json", payload);
    });

    // Route to refresh output data
    server.on("/refreshOutputsB_dataHTML", HTTP_GET, [](AsyncWebServerRequest *request) {
        String payload = generateOutputsB_Payload();
        Serial.println(payload);
        request->send(200, "application/json", payload);
    });

    server.on("/getAnalogInputs", HTTP_GET, [](AsyncWebServerRequest *request) {
        String inputs = "<div>";
        inputs += "<div class='input'>Analog 1: " + String(analogRead(34)) + "</div>";
        inputs += "</div>";
        request->send(200, "text/html", inputs);
    });

    server.on("/getLEDMatrix", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "application/json", jsonPublished);
    });

    // Route to toggle output state
    server.on("/toggleOutputAState", HTTP_GET, [](AsyncWebServerRequest *request) {
        String outputNumberStr = request->getParam("output")->value();
        int outputNumber = outputNumberStr.toInt();

        Serial.print("Toggle Output: ");
        Serial.println(outputNumber);

        toggleOutputStateGPIOA(outputNumber);
        request->send(200, "text/plain", "Output toggled");
    });

     // Route to toggle output state
    server.on("/toggleOutputBState", HTTP_GET, [](AsyncWebServerRequest *request) {
        String outputNumberStr = request->getParam("output")->value();
        int outputNumber = outputNumberStr.toInt();

        Serial.print("Toggle Output: ");
        Serial.println(outputNumber);

        toggleOutputStateGPIOB(outputNumber);
        request->send(200, "text/plain", "Output toggled");
    });

    // Route to get output state
    server.on("/getOutputAState", HTTP_GET, [](AsyncWebServerRequest *request) {
        String outputNumberStr = request->getParam("output")->value();
        int outputNumber = outputNumberStr.toInt();

        if (outputNumber >= 0 && outputNumber < NUM_DIGITAL_OUTPUTSA) {
            bool state = digitalRead(outputPinsA[outputNumber]) == HIGH;
            request->send(200, "text/plain", state ? "high" : "low");
        } else {
            request->send(400, "text/plain", "Invalid output number");
        }
    });

     // Route to get output state
    server.on("/getOutputBState", HTTP_GET, [](AsyncWebServerRequest *request) {
        String outputNumberStr = request->getParam("output")->value();
        int outputNumber = outputNumberStr.toInt();

        if (outputNumber >= 0 && outputNumber < NUM_DIGITAL_OUTPUTSB) {
            bool state = digitalRead(outputPinsB[outputNumber]) == HIGH;
            request->send(200, "text/plain", state ? "high" : "low");
        } else {
            request->send(400, "text/plain", "Invalid output number");
        }
    });

    // Route to get input state
    server.on("/getInputState", HTTP_GET, [](AsyncWebServerRequest *request) {
        String inputNumberStr = request->getParam("input")->value();
        int inputNumber = inputNumberStr.toInt();

        if (inputNumber >= 0 && inputNumber < NUM_DIGITAL_INPUTSA) {
            int pin = inputDigitalPinsB[inputNumber];
            String state = getDigitalInputStateGPIO(pin) ? "HIGH" : "LOW";
            request->send(200, "text/plain", state);
        } else {
            request->send(400, "text/plain", "Invalid input number");
        }
    });

    // Admin page route
    server.on("/admin", HTTP_GET, [](AsyncWebServerRequest *request) {
        String inputData = generateHTMLPage("/admin");
        request->send(200, "text/html", inputData);
    });

    // Route to save WiFi configuration
    server.on("/saveWiFi", HTTP_POST, [](AsyncWebServerRequest *request) {
        if (request->hasParam("ssid", true) && request->hasParam("password", true)) {
            String newSSID = request->getParam("ssid", true)->value();
            String newPassword = request->getParam("password", true)->value();

            wifiSettings.ssid = newSSID;
            wifiSettings.password = newPassword;
            saveWiFiCredentials(newSSID, newPassword, wifiSettings.storedVersion);

            String confirmationPage = "<html><head><title>Confirmation</title></head><body>";
            confirmationPage += "<h1>Configuration Saved</h1>";
            confirmationPage += "<p>New SSID: " + wifiSettings.ssid + "</p>";
            confirmationPage += "<p>New Password: " + wifiSettings.password + "</p>";
            confirmationPage += "<a href='/admin'>Back to Admin Panel</a>";
            confirmationPage += "<p>Restarting...</p>";
            confirmationPage += "</body></html>";

            request->send(200, "text/html", confirmationPage);

            delay(1000);  // Allow page load before restarting
            ESP.restart();
        } else {
            request->send(400, "text/plain", "Missing parameters.");
        }
    });

    // Route to save RedNode settings
    server.on("/saveRedNode", HTTP_POST, [](AsyncWebServerRequest *request) {
        if (request->hasParam("nrTab", true) &&
            request->hasParam("nrGroup", true) &&
            request->hasParam("mqttPassword", true) &&
            request->hasParam("mqttServer", true) &&
            request->hasParam("mqttUsername", true)) {

            globalSettings.nrTab = request->getParam("nrTab", true)->value();
            globalSettings.nrGroup = request->getParam("nrGroup", true)->value();
            mqttSettings.mqttUsername = request->getParam("mqttUsername", true)->value();
            mqttSettings.mqttServer = request->getParam("mqttServer", true)->value();
            mqttSettings.mqttPassword = request->getParam("mqttPassword", true)->value();

            prefSaveGlobalSettings();
            prefSaveMQTTSettings();

            request->send(200, "text/plain", "Global settings saved successfully.");
        } else {
            request->send(400, "text/plain", "Missing parameters.");
        }
    });

    // Edit config route
    server.on("/editConfig", HTTP_POST, [](AsyncWebServerRequest *request) {
        String action = request->getParam("action", true)->value();
        String inputData = generateHTMLPage(action);
        request->send(200, "text/html", inputData);
    });

    // Start the server
    server.begin();
    Serial.println("Server started");
}
