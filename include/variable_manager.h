#ifndef ESP_WIFI_CONFIG_VARIABLE_MANAGER_H
#define ESP_WIFI_CONFIG_VARIABLE_MANAGER_H

#include <vector>

namespace ESPWifiConfig
{

// Forward declaration of the base class
class ConfigurableBase;
class ReadableBase;

template <typename T>
class VariableManager
{
 private:
  std::vector<T*> items;

  // Private Constructor
  VariableManager() {}

  // Deleted Copy Constructor to prevent copying
  VariableManager(const VariableManager&) = delete;

  // Deleted assignment operator to prevent assignment
  VariableManager& operator=(const VariableManager&) = delete;

 public:
  // Public method to get the instance of the singleton
  static VariableManager& get_instance()
  {
    static VariableManager instance;
    return instance;
  }

  // Method to add an item
  void register_variable(T* item) { items.push_back(item); }

  // Method to get all items
  const std::vector<T*>& get_variables() const { return items; }
};

}  // namespace ESPWifiConfig

#endif  // ESP_WIFI_CONFIG_VARIABLE_MANAGER_H
