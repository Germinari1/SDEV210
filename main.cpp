#include <iostream>
#include <string>
#include "SQLServerConn.h";
#include "CustomerManager.h";
#include "Customer.h";
#include "DBConn.h";




int main() {
    try {
        // Connect to SQL Server instance on computer
        SQLServerConn connector;
        std::string connectionString = "DRIVER={SQL Server};SERVER=KN2\\SQLEXPRESS;Trusted_Connection=yes;";
        connector.connect(connectionString);

        // At this point SQL Server has connected so create a DBConn instance and select a database to do operations on
        DBConn dbConn(connector.getHDBC());
        std::string dbName = "sample_store";
        if (!dbConn.dbExists(dbName)) {
            dbConn.createDatabase(dbName);
        }
        dbConn.useDatabase(dbName);



        /*
        - Create our managers. Create tables for them if those tables don't 
            already exist
        */
        CustomerManager customerManager(dbConn, "customers");
        if (!dbConn.tableExists("customers")) {
            customerManager.initTable();
        } 


        int id = 2;
        Customer customer = customerManager.getCustomerByID(id);

        std::cout << customer << std::endl;

        


        connector.disconnect();
        std::cout << "End" << std::endl;
    }
    catch (const std::exception& ex) {
        std::cerr << "Exception caught: " << ex.what() << std::endl;
    }
    return 0; // Don't forget to return a value from main
}
