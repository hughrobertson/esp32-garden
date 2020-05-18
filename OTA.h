#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
//#include <TelnetStream.h>
//#include <credentials.h>
#include "BluetoothSerial.h"

//const char* ssid = mySSID;
//const char* password = myPASSWORD;
extern BluetoothSerial SerialBT;
void setupOTA() {
//  WiFi.mode(WIFI_STA);
//  WiFi.begin(ssid, password);
//  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
//    Serial.println("Connection Failed! Rebooting...");
//    delay(5000);
//    ESP.restart();
//  }

  // Port defaults to 3232
  // ArduinoOTA.setPort(3232);

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA
  .onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    SerialBT.println("Start updating " + type);
  })
  .onEnd([]() {
    SerialBT.println("\nEnd");
  })
    .onProgress([](unsigned int progress, unsigned int total) {
      SerialBT.printf(" %u%%", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      SerialBT.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) SerialBT.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) SerialBT.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) SerialBT.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) SerialBT.println("Receive Failed");
      else if (error == OTA_END_ERROR) SerialBT.println("End Failed");
    });

  ArduinoOTA.begin();
  //TelnetStream.begin();

  SerialBT.println("OTA Initialized");
  SerialBT.print("IP address: ");
  SerialBT.println(WiFi.localIP());
}
