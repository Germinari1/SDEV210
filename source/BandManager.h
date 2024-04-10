#ifndef BandManager_H
#define BandManager_H
#include <string>
#include <sstream>
#include "../source/DBConn.h";
#include "../source/Band.h";

/*
+ BandManager: Class that encapsulates and handles operations with the 'bands' table, or table
	that has objects that represents bands.

*/

class BandManager {
private:
	DBConn& dbConn;
	std::string tableName; 
public:
	BandManager(DBConn& dbConn, const std::string& tableName) : dbConn(dbConn), tableName(tableName) {}


	/*
	+ Creates table for the bands. If a table doesn't exist already in said database, then
	this function is used for creating that table. Then after you can do call the below 
	methods now the table exists.
	*/
	void initTable() {
		std::string query = "CREATE TABLE " + tableName + " ( "
			"id INT NOT NULL IDENTITY PRIMARY KEY, "
			"name VARCHAR(255) NOT NULL "
			");";
		if (!dbConn.executeSQL(query)) {
			throw std::runtime_error("Failed to initialize '" + tableName + "' table!");
		}
	}

	// Creates a band in the database, and returns object representation
	Band createBand(const std::string& name) {
		std::ostringstream queryStream;
		queryStream << "INSERT INTO " + tableName + "(name) VALUES('" << name << "')";
		if (!dbConn.executeSQL(queryStream.str())) {
			throw std::runtime_error("Failed to create band '" + name + "!");
		}
		const int id = dbConn.getLastInsertedID();
		Band band(id, name);
		return band;
	}

	// Other methods for updating and deleting bands
};


#endif