#include "pqxx/pqxx"
#include <iostream>

int main() {
    // Connect to the database
    pqxx::connection conn("dbname=demodb user=fbender hostaddr=127.00.1 port=5432");
    
    // Create a transaction
    pqxx::work txn(conn);
    
    // Execute a query
    pqxx::result result = txn.exec("SELECT name FROM name");
    
    // Print out the results
    for (pqxx::result::const_iterator row = result.begin(); row != result.end(); ++row) {
        std::string name = row[0].as<std::string>();
        int age = row[1].as<int>();
        std::cout << name << " is " << age << " years old." << std::endl;
    }
    
    // Commit the transaction
    txn.commit();
    
    // Disconnect from the database
    conn.disconnect();

    printf("\033[0;32mGracefully terminated!\033[0m\n");
    
    return 0;
}