
#include "web_server.h"

#include "configurable_base.h"
#include "configurable_manager.h"

// #include "readable.h"

namespace ESPWifiConfig
{

std::vector<ConfigurableBase*> global_configurables;
// std::vector<ReadableBase*> global_readables;

WebServer::WebServer(int port) : server(port) {}

WebServer& WebServer::getInstance()
{
  static WebServer instance;
  return instance;
}

// void WebServer::register_configurable(ConfigurableBase* configurable)
// {
//   configurables.push_back(configurable);
// }

void WebServer::connect_to_wifi()
{
  // Connect to Wi-Fi
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Establishing connection to WiFi...");
    // Print Credentials
    // Serial.println(WIFI_SSID);
    // Serial.println(WIFI_PASSWORD);
  }

  Serial.println("Connected to WiFi");
  // Print IP
  Serial.println(WiFi.localIP());
}

void WebServer::setup_session_endpoints()
{
  // Endpoint for POST to set session_id
  server.on(
      "/session", HTTP_POST, [](AsyncWebServerRequest* request) {}, nullptr,
      [this](AsyncWebServerRequest* request, uint8_t* data, size_t len,
             size_t index, size_t total)
      {
        // Check content type
        if (request->contentType() != "application/json")
        {
          request->send(400, "text/plain",
                        "Content-Type must be application/json");
          return;
        }

        // Convert received data into a String
        String request_body = String((char*)data);

        // Parse the JSON body
        JSONVar json_object = JSON.parse(request_body);

        // Check if parsing succeeded
        if (JSON.typeof(json_object) == "undefined")
        {
          request->send(500, "text/plain", "Failed to parse JSON");
          return;
        }

        // Check if the JSON contains the session_id field
        if (json_object.hasOwnProperty("session_id"))
        {
          this->session_id = (const char*)json_object["session_id"];
          request->send(200, "text/plain", "Session ID received");
        }
        else
        {
          request->send(400, "text/plain",
                        "No session_id provided in JSON body");
        }
      });

  // Endpoint for GET to check session_id
  server.on(
      "/session", HTTP_GET,
      [this](AsyncWebServerRequest* request)  // Capture 'this' pointer
      {
        if (request->hasHeader("session_id"))
        {
          String received_session_id = request->header("session_id");
          if (received_session_id ==
              this->session_id)  // Use 'this' to access member variable
          {
            request->send(200, "application/json", "{\"status\": \"success\"}");
            // Blink GPIO 8
            digitalWrite(8, HIGH);
            delayMicroseconds(100);
            digitalWrite(8, LOW);
          }
          else
          {
            request->send(403, "text/plain", "Forbidden: Incorrect session ID");
          }
        }
        else
        {
          request->send(400, "text/plain", "No session_id provided");
        }
      });
}

void WebServer::start()
{
  // Setup GPIO pin for Green LED
  pinMode(8, OUTPUT);

  connect_to_wifi();

  setup_session_endpoints();

  // Setup all the GET and POST endpoints for the configurables
  auto configurables = ConfigurableManager::get_instance().get_configurables();
  for (auto& conf : configurables)
  {
    Serial.print("Registering configurable: ");
    Serial.println(conf->get_endpoint());
    // Set up a GET endpoint for each configurable
    server.on(conf->get_endpoint().c_str(), HTTP_GET,
              [conf](AsyncWebServerRequest* request)
              { conf->handle_get(request); });

    // Set up a POST endpoint for each configurable
    server.on(
        conf->get_endpoint().c_str(), HTTP_POST,
        [](AsyncWebServerRequest* request) {}, nullptr,
        [conf](AsyncWebServerRequest* request, uint8_t* data, size_t len,
               size_t index, size_t total)
        { conf->handle_post(request, data, len); });
  }

  //   //   Setup all the GET endpoints for the readables
  //   for (auto& read : global_readables)
  //   {
  //     Serial.print("Registering readable: ");
  //     Serial.println(read->get_endpoint());
  //     // Set up a GET endpoint for each readable
  //     server.on(read->get_endpoint().c_str(), HTTP_GET,
  //               [read](AsyncWebServerRequest* request)
  //               { read->handle_get(request); });
  //   }

  // Finally, start the server
  server.begin();
}

void WebServer::stop() { server.end(); }

}  // namespace ESPWifiConfig
