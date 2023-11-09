#ifndef ESP_WIFI_CONFIG_CONFIGURABLE_MANAGER_H
#define ESP_WIFI_CONFIG_CONFIGURABLE_MANAGER_H

#include <vector>

namespace ESPWifiConfig
{

// Forward declaration of ConfigurableBase
class ConfigurableBase;

class ConfigurableManager
{
 private:
  std::vector<ConfigurableBase*> configurables;

  // Private Constructor
  ConfigurableManager() {}

  // Private Copy Constructor to prevent copying
  ConfigurableManager(const ConfigurableManager&);

  // Private assignment operator to prevent assignment
  ConfigurableManager& operator=(const ConfigurableManager&);

 public:
  // Public method to get the instance of the singleton
  static ConfigurableManager& get_instance();

  // Method to add a configurable
  void register_configurable(ConfigurableBase* configurable);

  // Method to get all configurables
  const std::vector<ConfigurableBase*>& get_configurables() const;
};

}  // namespace ESPWifiConfig

#endif  // ESP_WIFI_CONFIG_CONFIGURABLE_MANAGER_H
