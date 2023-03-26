#include <iostream>
#include <fstream>
#include <string>
#include "rapidyaml/ryml.hpp"

int main() {
  // Open the YAML file
  std::ifstream fin("example.yaml");
  
  // Read the YAML data into a string
  std::string yaml_data((std::istreambuf_iterator<char>(fin)), std::istreambuf_iterator<char>());
  
  // Parse the YAML data from the string into a node tree
  auto doc = ryml::parse(c4::substr(const_cast<char*>(yaml_data.c_str()), yaml_data.length()));
  
  // Access the values in the node tree using their respective keys
  std::cout << "Name: " << doc["name"].val() << std::endl;
  std::cout << "Age: " << doc["age"].val() << std::endl;
  std::cout << "City: " << doc["city"].val() << std::endl;
  
  // Access the hobbies array and loop through its elements
  auto hobbies = doc["hobbies"];
  std::cout << "Hobbies: ";
  for (const auto& hobby : hobbies) {
    std::cout << hobby.val() << " ";
  }
  std::cout << std::endl;
  
  return 0;
}