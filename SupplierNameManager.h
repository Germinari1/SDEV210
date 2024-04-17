#ifndef SupplierNameManager_H
#define SupplierNameManager_H
#include <string>
#include <sstream>
#include "DBConn.h"
#include "SupplierName.h"

/*
+ Manager for handling operations for the SupplierName table.

NOTE: We should note that s_name should be unique, so when creating and 
	updating a SupplierName, we'll check if that supplier name already exists.

Supplier Table
----------------------------------
supplier_id (Primary Key)
description
...


Supplier Name Table
----------------------------------
supplier_id (Foreign Key, also your primary key)
name

+ Store class:



*/

class SupplierNameManager {
private:
	DBConn& dbConn;
	std::string tableName; // name of the 'Supplier Name' table
	std::string supplierTableName; // name of the 'Supplier' Table that we reference from

	static const int MAX_S_NAME_LENGTH = 50; // maximum length for s_name
public:
	SupplierNameManager(DBConn& dbConn, std::string& tableName, std::string& supplierTableName) : dbConn(dbConn), tableName(tableName), supplierTableName(supplierTableName) {}

	const std::string& getTableName() {
		return tableName;
	}

	/*
	- Initializes the supplier name manager.
	
	NOTE: I do [tableName] because my table name "Supplier Names" has a space. In these cases 
	when table names have spaces or special characters, we surround them by square brakcets.
	*/
	
	void initTable() {
		std::string query = "CREATE TABLE " + tableName + " ( "
			"supplier_id INT NOT NULL PRIMARY KEY, "
			"s_name VARCHAR(" + std::to_string(MAX_S_NAME_LENGTH) + ") NOT NULL, "
			"FOREIGN KEY (supplier_id) REFERENCES " + supplierTableName + " (supplier_id)"
			");";

		if (!dbConn.executeSQL(query)) {
			throw std::runtime_error("Failed to initialize '" + tableName + "' table!");
		}
	}

	// Checks if a s_name (Supplier name) is unique in the Supplier Name table
	void checkUniqueSupplierName(std::string& s_name) {
		std::string query = "SELECT * FROM " + tableName + " WHERE s_name='" + s_name + "';";
		if (!dbConn.executeSQL(query)) {
			throw std::runtime_error("Failed to check IF SupplierName with s_name already exists!");
		}

		// Check if we were successful in finding a SupplierName with the input s_name
		SQLRETURN retcode = dbConn.fetchRow();
		if (retcode == SQL_SUCCESS) {
			dbConn.closeCursor(); // close cursor before throwing error
			throw std::runtime_error("SupplierName with s_name '" + s_name + "' already exists!");
		}

		// A unique s_name, close the cursor before function ends.
		dbConn.closeCursor();
	}

	// Creates row in SupplierName table
	void createSupplierName(int supplier_id, std::string& s_name) {
		// Create and execute query
		std::string query = "INSERT INTO " + tableName + " (supplier_id, s_name) VALUES('" + std::to_string(supplier_id) + "', '" + s_name + "');";
		if (!dbConn.executeSQL(query)) {
			throw std::runtime_error("Failed to create SupplierName with s_name(" + s_name + ")!");
		}
	}

	// Updates row in SupplierName table
	void updateSupplierName(int supplier_id, std::string& s_name) {
		// Construct query and do operation on 'Supplier Name' table.
		std::string query = "UPDATE " + tableName + " SET s_name='" + s_name + "' WHERE supplier_id=" + std::to_string(supplier_id) + ";";
		if (!dbConn.executeSQL(query)) {
			throw std::runtime_error("Failed to update SupplierName with id '" + std::to_string(supplier_id) + "'!");
		}
	}

	// Deletes row in SupplierName table
	void deleteSupplierName(int supplier_id) {
		std::string query = "DELETE FROM " + tableName + " WHERE supplier_id=" + std::to_string(supplier_id) + ";";
		if (!dbConn.executeSQL(query)) {
			throw std::runtime_error("Failed to delete SupplierName with supplier_id '" + std::to_string(supplier_id) + "'. Supplier with supplier_id may not exist!");
		}
	}
	
};

#endif