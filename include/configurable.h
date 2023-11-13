#ifndef ESP_WIFI_CONFIG_CONFIGURABLE_H
#define ESP_WIFI_CONFIG_CONFIGURABLE_H

#include <Arduino_JSON.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>
#include <conversion_utils.h>

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "configurable_base.h"
#include "variable_manager.h"

namespace ESPWifiConfig
{
template <typename T>
class Configurable : public ConfigurableBase
{
 private:
  std::shared_ptr<T> value_ptr;
  String endpoint;
  String description;

  // Callback for GET
  std::function<void()> get_callback;
  // Callback for POST with template type argument
  std::function<void(T)> post_callback;

 public:
  Configurable(T& value, const String& endpoint_path, const String& desc = "")
      : value_ptr(std::make_shared<T>(value)),  // Construct a shared_ptr
        endpoint(endpoint_path),
        description(desc)
  {
    VariableManager<ConfigurableBase>::get_instance().register_variable(this);
  }

  String get_endpoint() const override { return endpoint; }

  T get_value() const { return *value_ptr; }
  void set_value(const T& val) { *value_ptr = val; }

  // Setters for the callbacks
  void set_get_callback(std::function<void()> cb) { get_callback = cb; }
  void set_post_callback(std::function<void(T)> cb) { post_callback = cb; }

  void handle_get(AsyncWebServerRequest* request) override
  {
    // Serial.println("Handling GET request");
    // JSON response
    AsyncResponseStream* response =
        request->beginResponseStream("application/json");
    response->addHeader("Access-Control-Allow-Origin", "*");
    response->addHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    response->addHeader("Access-Control-Allow-Headers",
                        "Origin, X-Requested-With, Content-Type, Accept");
    response->setCode(200);
    // Construct a JSON object with the value
    JSONVar json_object;
    json_object["value"] = *value_ptr;
    // Serial.println(*value_ptr);
    response->print(JSON.stringify(json_object));

    request->send(response);

    if (get_callback)
    {
      get_callback();
    }
  }

  void handle_post(AsyncWebServerRequest* request, uint8_t* data,
                   size_t len) override
  {
    // Serial.println("Handling POST request");
    String payload = String((char*)data);
    // Serial.print("Payload: ");
    // Serial.println(payload);
    JSONVar json_object = JSON.parse(payload);
    Serial.print("JSON object: ");
    Serial.println(json_object["value"]);

    AsyncResponseStream* response = request->beginResponseStream("text/plain");
    response->addHeader("Access-Control-Allow-Origin", "*");
    response->addHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    response->addHeader("Access-Control-Allow-Headers",
                        "Origin, X-Requested-With, Content-Type, Accept");

    try
    {
      T received_value = JsonConverter<T>::convert(json_object["value"]);
      *value_ptr = received_value;
      response->setCode(200);
      response->print("Success");
      if (post_callback) post_callback(received_value);
    }
    catch (const std::exception& e)
    {
      // Handle conversion errors here (invalid JSON format or type mismatch)
      response->setCode(400);
      response->print(e.what());
    }

    request->send(response);
  }
};

}  // namespace ESPWifiConfig

#endif  // ESP_WIFI_CONFIG_CONFIGURABLE_H