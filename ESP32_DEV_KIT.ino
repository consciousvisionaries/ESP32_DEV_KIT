
void setup() {
  
  Serial.begin(115200);
  
  setupGPIO();
  setupFASTLED();
  setupFirmware();
  setupDashboard();

  connectMQTT();
  sendConfigMQTTPayload();  // Send initial MQTT message when connected
}

void loop() {

  clientMQTTConnected();
  loopFIRMWARE();
  loopGPIO();
  loopFASTLED();
}
