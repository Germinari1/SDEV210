#include <iostream>
#include <string>
#include "../source/DBConn.h";
#include "../source/SQLServerConn.h";
#include "../source/BandManager.h";
#include "../source/Band.h";

int main() {
    try {
        // Connect to SQL Server instance on computer
        SQLServerConn connector;
        std::string connectionString = "DRIVER={SQL Server};SERVER=KN2\\SQLEXPRESS;Trusted_Connection=yes;";
        connector.connect(connectionString);
        
        // At this point sql server has connected so create a DBConn instance and select a database to do operations on
        DBConn dbConn (connector.getHDBC());
        std::string dbName = "record_company";
        dbConn.useDatabase(dbName);

        /*
        - Create managers for each table you want. Ensure you do init table to 
            create a table for that database.

        - NOTE: If you don't have a bands table, then you do initTable().
        */
        
        BandManager bandManager(dbConn, "bands");
        // bandManager.initTable();

        std::string bandName = "Amplification";
        
        Band band = bandManager.createBand(bandName);
        std::cout << band << std::endl;
        connector.disconnect();
    } catch (const std::exception& ex) {
        std::cerr << "Exception caught: " << ex.what() << std::endl;
    }
}