namespace ESPWifiConfig
{

#include <Arduino_JSON.h>

// Utility to parse and convert JSON data to specific types
template <typename T>
struct JsonConverter
{
};

// Specialization for float
template <>
struct JsonConverter<float>
{
  static float convert(const JSONVar& obj)
  {
    if (obj.hasOwnProperty("value") && JSON.typeof(obj["value"]) == "number")
    {
      return static_cast<float>((double)obj["value"]);
    }
    throw std::runtime_error("Invalid JSON format or type for float.");
  }
};

// // Specialization for std::string (assuming that we're using std::string)
// template <>
// struct JsonConverter<std::string>
// {
//   static std::string convert(const JSONVar& obj)
//   {
//     if (obj.hasOwnProperty("value") && JSON.typeof(obj["value"]) == "string")
//     {
//       return static_cast<const char*>(obj["value"]);
//     }
//     throw std::runtime_error("Invalid JSON format or type for string.");
//   }
// };

// Specialization for int
template <>
struct JsonConverter<int>
{
  static int convert(const JSONVar& obj)
  {
    if (obj.hasOwnProperty("value") && JSON.typeof(obj["value"]) == "number")
    {
      return static_cast<int>((double)obj["value"]);
    }
    throw std::runtime_error("Invalid JSON format or type for int.");
  }
};

}  // namespace ESPWifiConfig
