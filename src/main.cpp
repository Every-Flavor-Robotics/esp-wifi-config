#include <Arduino.h>

#include "web_server.h"

ESPWifiConfig::WebServer& server = ESPWifiConfig::WebServer::getInstance();

void setup()
{
  // Start the serial communication
  Serial.begin(115200);

  Serial.println("Starting WebServer...");

  // Start the web server
  server.start();

  Serial.println("WebServer started!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop()
{
  // Nothing here for now. The web server runs in the background.
  delay(1000);  // Just a delay to avoid overwhelming the loop
}