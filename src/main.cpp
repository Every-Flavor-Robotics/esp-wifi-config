#include "configurable.h"
#include "web_server.h"

// Define some global variables
int int_val = 10;
float float_val = 20.5f;
String string_val = "Hello, ESP!";

// Create Configurable objects for each variable
ESPWifiConfig::Configurable<int> int_conf(int_val, "/int_val",
                                          "An integer value");
ESPWifiConfig::Configurable<float> float_conf(float_val, "/float_val",
                                              "A float value");
// ESPWifiConfig::Configurable<String> string_conf(string_val, "/string_val",
//                                                 "A string value");

void setup()
{
  Serial.begin(115200);
  delay(2000);

  // Set callbacks
  int_conf.set_get_callback(
      []() { Serial.println("GET request received for int_val"); });

  int_conf.set_post_callback(
      [](int received_val)
      {
        Serial.print("POST request received for int_val with value: ");
        Serial.println(received_val);
      });

  float_conf.set_get_callback(
      []() { Serial.println("GET request received for float_val"); });

  float_conf.set_post_callback(
      [](float received_val)
      {
        Serial.print("POST request received for float_val with value: ");
        Serial.println(received_val, 4);  // Print up to 4 decimal places
      });

  //   string_conf.set_get_callback(
  //       []() { Serial.println("GET request received for string_val"); });

  //   string_conf.set_post_callback(
  //       [](String received_val)
  //       {
  //         Serial.print("POST request received for string_val with value: ");
  //         Serial.println(received_val);
  //       });

  // Start the web server
  ESPWifiConfig::WebServer::getInstance().start();
}

void loop()
{
  // Nothing to do in the main loop for this example
}
