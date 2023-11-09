#include "configurable_manager.h"

#include "configurable.h"  // This will include the full definition of ConfigurableBase

namespace ESPWifiConfig
{

// Define the static method to get the instance of the singleton
ConfigurableManager& ConfigurableManager::get_instance()
{
  static ConfigurableManager instance;
  return instance;
}

// Define the method to add a configurable
void ConfigurableManager::register_configurable(ConfigurableBase* configurable)
{
  configurables.push_back(configurable);
}

// Define the method to get all configurables
const std::vector<ConfigurableBase*>& ConfigurableManager::get_configurables()
    const
{
  return configurables;
}

}  // namespace ESPWifiConfig
