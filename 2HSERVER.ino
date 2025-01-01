#define WEBSERVER_VERSION V1.1
#include <ESPAsyncWebServer.h>

AsyncWebServer server(80);

void setupDashboard() {
    // Initialize the server routes

    // Main page route
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/html", generatePage());
    });

    // Route to refresh input indicators
    server.on("/refreshInputs_dataHTML", HTTP_GET, [](AsyncWebServerRequest *request) {
        String inputData = generateInputIndicatorsHTML();
        request->send(200, "text/plain", inputData);
    });

    // Route to refresh output data
    server.on("/refreshOutputs_dataHTML", HTTP_GET, [](AsyncWebServerRequest *request) {
        String outputData = generateOutputsPayload();
        request->send(200, "application/json", outputData);
    });

    // Route to toggle output state
    server.on("/toggleOutputState", HTTP_GET, [](AsyncWebServerRequest *request) {
        String outputNumberStr = request->getParam("output")->value();
        int outputNumber = outputNumberStr.toInt();

        Serial.print("Toggle Output: ");
        Serial.println(outputNumber);

        toggleOutputStateGPIO(outputNumber);
        request->send(200, "text/plain", "Output toggled");
    });

    // Route to get output state
    server.on("/getOutputState", HTTP_GET, [](AsyncWebServerRequest *request) {
        String outputNumberStr = request->getParam("output")->value();
        int outputNumber = outputNumberStr.toInt();

        if (outputNumber >= 0 && outputNumber < NUM_OUTPUTS) {
            bool state = digitalRead(outputPins[outputNumber]) == HIGH;
            request->send(200, "text/plain", state ? "high" : "low");
        } else {
            request->send(400, "text/plain", "Invalid output number");
        }
    });

    // Route to get input state
    server.on("/getInputState", HTTP_GET, [](AsyncWebServerRequest *request) {
        String inputNumberStr = request->getParam("input")->value();
        int inputNumber = inputNumberStr.toInt();

        if (inputNumber >= 0 && inputNumber < NUM_INPUTS) {
            int pin = inputPins[inputNumber];
            String state = getInputStateGPIO(pin);
            request->send(200, "text/plain", state);
        } else {
            request->send(400, "text/plain", "Invalid input number");
        }
    });

    // Admin page route
    server.on("/admin", HTTP_GET, [](AsyncWebServerRequest *request) {
        String inputData = generateHTMLPage("ADMIN");
        request->send(200, "text/html", inputData);
    });

    // Route to save WiFi configuration
    server.on("/saveWiFi", HTTP_POST, [](AsyncWebServerRequest *request) {
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

        delay(10000);
        ESP.restart();
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

            request->send(200, "text/plain", "Global settings saved successfully. This browser should take you back to administration!");
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
}
