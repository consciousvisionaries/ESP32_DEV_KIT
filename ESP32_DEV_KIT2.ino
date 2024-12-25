
void setup() {
  
  Serial.begin(115200);

  setupGPIO();
  setupFirmware();
  setupDashboard();

  connectMQTT();
  sendMQTTPayload();  // Send initial MQTT message when connected
}

void loop() {

  clientMQTTConnected();
  loopFIRMWARE();
  loopGPIO();
}
