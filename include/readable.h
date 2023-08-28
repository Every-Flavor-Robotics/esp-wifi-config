#ifndef ESP_WIFI_CONFIG_READABLE_H
#define ESP_WIFI_CONFIG_READABLE_H

#include <Arduino_JSON.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>

#include <functional>  // for std::function
#include <string>

namespace ESPWifiConfig
{

class ReadableBase
{
 public:
  virtual ~ReadableBase() {}
  virtual void handle_get(AsyncWebServerRequest* request) = 0;
  virtual String get_endpoint() const = 0;
};

extern std::vector<ReadableBase*> global_readables;

template <typename T>
class Readable : public ReadableBase
{
 private:
  // Callback for GET which retrieves the value
  std::function<T()> get_value_callback;

  String endpoint;
  String description;

 public:
  Readable(std::function<T()> value_func, const String& endpoint_path,
           const String& desc)
      : get_value_callback(value_func),
        endpoint(endpoint_path),
        description(desc)

  {
    global_readables.push_back(this);
  }

  String get_endpoint() const override { return endpoint; }

  void handle_get(AsyncWebServerRequest* request) override
  {
    if (get_value_callback)
    {
      T value = get_value_callback();
      request->send(200, "text/plain", String(value));
    }
    else
    {
      request->send(400, "text/plain", "Failed to retrieve value.");
    }
  }
};

}  // namespace ESPWifiConfig

#endif  // ESP_WIFI_CONFIG_CONFIGURABLE_H
