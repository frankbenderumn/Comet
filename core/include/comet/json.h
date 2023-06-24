// #include <stdio.h>
// #include <stdlib.h>
// #include "cJSON.h"

// int main() {
//     // Sample JSON string
//     const char* json_data = "{\"name\":\"John\",\"age\":30,\"city\":\"New York\"}";

//     // Parse the JSON string
//     cJSON* root = cJSON_Parse(json_data);
//     if (root == NULL) {
//         printf("Failed to parse JSON: %s\n", cJSON_GetErrorPtr());
//         return 1;
//     }

//     // Access JSON elements
//     cJSON* name = cJSON_GetObjectItem(root, "name");
//     cJSON* age = cJSON_GetObjectItem(root, "age");
//     cJSON* city = cJSON_GetObjectItem(root, "city");

//     if (name && age && city) {
//         // Print the values
//         printf("Name: %s\n", name->valuestring);
//         printf("Age: %d\n", age->valueint);
//         printf("City: %s\n", city->valuestring);

//         // Modify the values
//         cJSON_SetValuestring(name, "Alice");
//         cJSON_SetNumberValue(age, 25);
//         cJSON_SetValuestring(city, "London");

//         // Convert the modified JSON back to a string
//         char* modified_json = cJSON_Print(root);
//         printf("Modified JSON: %s\n", modified_json);
//         free(modified_json);
//     }

//     // Clean up
//     cJSON_Delete(root);

//     return 0;
// }