#include <iostream>
#include "picojson/picojson.h"

int main()
{
    // Define a JSON string
    std::string json_string = R"({
        "name": "Alice",
        "age": 30,
        "hobbies": ["reading", "music", "hiking"],
        "address": {
            "street": "123 Main St",
            "city": "Anytown",
            "state": "CA",
            "zip": "12345"
        }
    })";

    // Parse the JSON string
    picojson::value json_value;
    std::string error = picojson::parse(json_value, json_string);
    if (!error.empty()) {
        std::cerr << "Error parsing JSON: " << error << std::endl;
        return 1;
    }

    // Access the JSON values
    std::string name = json_value.get("name").to_str();
    int age = static_cast<int>(json_value.get("age").get<double>());
    picojson::array hobbies = json_value.get("hobbies").get<picojson::array>();
    std::string street = json_value.get("address").get("street").to_str();
    std::string city = json_value.get("address").get("city").to_str();
    std::string state = json_value.get("address").get("state").to_str();
    std::string zip = json_value.get("address").get("zip").to_str();

    // Print the JSON values
    std::cout << "Name: " << name << std::endl;
    std::cout << "Age: " << age << std::endl;
    std::cout << "Hobbies: ";
    for (const auto& hobby : hobbies) {
        std::cout << hobby.get<std::string>() << " ";
    }
    std::cout << std::endl;
    std::cout << "Address: " << street << ", " << city << ", " << state << " " << zip << std::endl;

    return 0;
}