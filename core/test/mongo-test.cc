// #include "bsoncxx/json.hpp"
// #include "mongocxx/client.hpp"
// #include "mongocxx/instance.hpp"
// #include "mongocxx/uri.hpp"

int main() {
//   // Initialize the MongoDB C++ driver
//   mongocxx::instance inst{};

//   // Set up the connection URI
//   mongocxx::uri uri("mongodb://localhost:27017");

//   // Connect to the MongoDB server
//   mongocxx::client conn(uri);

//   // Get the database and collection objects
//   mongocxx::database db = conn["mydb"];
//   mongocxx::collection coll = db["mycoll"];

//   // Insert a document into the collection
//   bsoncxx::document::value doc_value =
//       bsoncxx::builder::stream::document{} << "name" << "Alice" << "age" << 30
//                                             << bsoncxx::builder::stream::finalize;
//   coll.insert_one(std::move(doc_value));

//   // Find all documents in the collection
//   mongocxx::cursor cursor = coll.find({});
//   for (auto&& doc : cursor) {
//     std::cout << bsoncxx::to_json(doc) << std::endl;
//   }

//   // Update a document in the collection
//   coll.update_one(
//       bsoncxx::builder::stream::document{} << "name" << "Alice"
//                                             << bsoncxx::builder::stream::finalize,
//       bsoncxx::builder::stream::document{} << "$set" << bsoncxx::builder::stream::open_document
//                                             << "age" << 31 << bsoncxx::builder::stream::close_document
//                                             << bsoncxx::builder::stream::finalize);

//   // Delete a document from the collection
//   coll.delete_one(bsoncxx::builder::stream::document{} << "name" << "Alice"
//                                                         << bsoncxx::builder::stream::finalize);

  return 0;
}