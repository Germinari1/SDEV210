#ifndef SupplierManager_H
#define SupplierManager_H
#include <string>
#include <vector>
#include "DBConn.h"
#include "Supplier.h"
#include "SupplierNameManager.h"
#include "SupplierName.h"



/*
+ SupplierManager: Manager that encapsulates all operations for 'Supplier' table and 'SupplierName' table.
	Now, operations for 'supplier name' table are still in 'SupplierNameManager' class, it's just that the SupplierManager
	class will decide when to call certain exposed methods. As a result, we can keep the logic somewhat separate, but 
	still encapsulate the logic related to interacting with suppliers encapsulated in this SupplierManager class.

	Now this is a pretty unique case since 's_name' was a transitive dependency that was removed, so this can be a good
	solution

*/

class SupplierManager {
private:
	DBConn& dbConn;
	std::string tableName;
	SupplierNameManager& supplierNameManager;


	// Constants for maximum lengths for varchar columns (for both tables)
	static const int MAX_S_NAME_LENGTH = 50;
	static const int MAX_DESCRIPTION_LENGTH = 2000;
	static const int MAX_EMAIL_LENGTH = 50;
	static const int MAX_ADDRESS_LENGTH = 50;


	// Creates supplier object from data obtained from SQL row
	Supplier createSupplierFromRow(SQLINTEGER supplier_id, SQLCHAR* description, SQLCHAR* email, SQLCHAR* address, SQLCHAR* s_name) {
		// Null terminate the strings; needed since we convert the datatypes
		description[MAX_DESCRIPTION_LENGTH] = '\0';
		email[MAX_EMAIL_LENGTH] = '\0';
		address[MAX_ADDRESS_LENGTH] = '\0';
		s_name[MAX_S_NAME_LENGTH] = '\0';

		// Convert datatypes 
		int intID = static_cast<int>(supplier_id);
		std::string descriptionStr(reinterpret_cast<char*>(description));
		std::string emailStr(reinterpret_cast<char*>(email));
		std::string addressStr(reinterpret_cast<char*>(address));
		std::string s_name_str(reinterpret_cast<char*>(s_name));

		// Create and return supplier object
		Supplier supplier(intID, s_name_str, descriptionStr, emailStr, addressStr);
		return supplier;
	}

public:
	SupplierManager(
		DBConn& dbConn, 
		std::string& tableName,
		SupplierNameManager& supplierNameManager
	) : dbConn(dbConn),
		tableName(tableName), 
		supplierNameManager(supplierNameManager) {}

	// Initializes 'suppliers' table
	void initTable() {
		std::string query = "CREATE TABLE " + tableName + " ( "
			"supplier_id INT NOT NULL IDENTITY PRIMARY KEY, "
			"description VARCHAR(" + std::to_string(MAX_DESCRIPTION_LENGTH) + ") NOT NULL, "
			"email VARCHAR(" + std::to_string(MAX_EMAIL_LENGTH) + ") NOT NULL, "
			"address VARCHAR(" + std::to_string(MAX_ADDRESS_LENGTH) + ") NOT NULL"
			");";

		if (!dbConn.executeSQL(query)) {
			throw std::runtime_error("Failed to initialize '" + tableName + "' table!");
		}
	}

	// Ensure that the supplier_id links to an actual supplier if not, then we throw an error 
	bool isValidSupplierID(int supplier_id) {
		bool isValidID = dbConn.isValidRow(tableName, "supplier_id", supplier_id);
		return isValidID;
	}


	// Checks that supplier name is within length constraints
	void validateSupplierName(std::string& s_name) {
		if (s_name.length() > MAX_S_NAME_LENGTH) {
			throw std::runtime_error("Supplier name exceeds maximum length of " + std::to_string(MAX_S_NAME_LENGTH) + " characters!");
		}
	}

	// Checks that supplier description is within length constraints
	void validateDescription(std::string& description) {
		if (description.length() > MAX_DESCRIPTION_LENGTH) {
			throw std::runtime_error("Supplier description exceeds maximum length of " + std::to_string(MAX_DESCRIPTION_LENGTH) + " characters!");
		}
	}

	// Checks that supplier email is within length constraints
	void validateEmail(std::string& email) {
		if (email.length() > MAX_EMAIL_LENGTH) {
			throw std::runtime_error("Supplier email exceeds maximum length of " + std::to_string(MAX_EMAIL_LENGTH) + " characters!");
		}
	}

	// Checks that supplier address is within length constraints
	void validateAddress(std::string& address) {
		if (address.length() > MAX_ADDRESS_LENGTH) {
			throw std::runtime_error("Suppiler address exceeds maximum length of " + std::to_string(MAX_ADDRESS_LENGTH) + " characters!");
		}
	}


	std::vector<Supplier> fetchSuppliers(const std::string query) {
		// Create vector of Supplier objects
		std::vector<Supplier> suppliers;
		
		// Execute sql query; and check if it was successful
		if (!dbConn.executeSQL(query)) {
			throw std::runtime_error("Failed to query supplier and supplierName tables!");
		}

		// Create buffers/variables
		SQLINTEGER supplier_id = 0;
		SQLCHAR description[MAX_DESCRIPTION_LENGTH + 1] = {};
		SQLCHAR email[MAX_EMAIL_LENGTH + 1] = {};
		SQLCHAR address[MAX_ADDRESS_LENGTH + 1] = {};
		SQLCHAR s_name[MAX_S_NAME_LENGTH + 1] = {};

		// Bind columns
		dbConn.bindColumn(1, SQL_INTEGER, &supplier_id, sizeof(supplier_id));
		dbConn.bindColumn(2, SQL_C_CHAR, description, sizeof(description));
		dbConn.bindColumn(3, SQL_C_CHAR, email, sizeof(email));
		dbConn.bindColumn(4, SQL_C_CHAR, address, sizeof(address));
		dbConn.bindColumn(5, SQL_C_CHAR, s_name, sizeof(s_name));

		// Fetch all rows we got
		while (true) {
			// Fetch the row
			SQLRETURN retcode = dbConn.fetchRow();
			if (retcode == SQL_NO_DATA) {
				// No more rows to fetch, exit the loop
				break;
			}
			else if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) {
				dbConn.closeCursor(); // we fetched a row, close cursor before throwing error
				throw std::runtime_error("Failed to fetch all suppliers from the database!");
			}

			// Create Supplier instance and put it into our suppliers vector
			Supplier supplier = createSupplierFromRow(supplier_id, description, email, address, s_name);
			suppliers.push_back(supplier);
		}

		// Close cursor
		dbConn.closeCursor();

		// Return the 'suppliers' vector
		return suppliers;
	}


	/*
	+ Gets all suppliers in the database.
	
	NOTE: Merges Supplier and Supplier_Name tables as well.
	*/
	std::vector<Supplier> getAllSuppliers() {
		
		const std::string supplierNameTable = supplierNameManager.getTableName();

		// Construct SQL Query that joins 'suppliers' and 'supplier names' table.
		std::string query =
			"SELECT " +
			tableName + ".supplier_id, " +   // column 1
			tableName + ".description, " +   // column 2
			tableName + ".email, " +         // column 3
			tableName + ".address, " +       // column 4
			supplierNameTable + ".s_name " + // column 5
			"FROM " + tableName + " " +
			"JOIN " + supplierNameTable + " " + 
			"ON " + tableName + ".supplier_id = " + supplierNameTable + ".supplier_id;";

		// Create vector of Supplier objects; also get the name of the 'Supplier Nmae' table which will help us build our JOIN query.
		std::vector<Supplier> suppliers = fetchSuppliers(query);

		// Return the 'suppliers' vector
		return suppliers;
	}

	// Gets all info for a supplier by its ID
	Supplier getSupplierByID(int supplier_id) {
		const std::string supplierNameTable = supplierNameManager.getTableName();
		
		// Construct query to find supplier with supplier_id
		std::string query =
			"SELECT " +
			tableName + ".supplier_id, " +   // column 1
			tableName + ".description, " +   // column 2
			tableName + ".email, " +         // column 3
			tableName + ".address, " +       // column 4
			supplierNameTable + ".s_name " + // column 5
			"FROM " + tableName + " " +
			"JOIN " + supplierNameTable + " " +
			"ON " + tableName + ".supplier_id = " + supplierNameTable + ".supplier_id " + 
			"WHERE " + tableName + ".supplier_id = " + std::to_string(supplier_id) + ";";

		// Check to see if vector is empty, which means supplier_id didn't reference a supplier in the table.
		std::vector<Supplier> suppliers = fetchSuppliers(query);
		if (suppliers.empty()) {
			throw std::runtime_error("Supplier with ID '" + std::to_string(supplier_id) + "' wasn't found!");
		}

		// suppliers vector isn't empty; expect it to contain only one supplier, so index and return it.
		Supplier supplier = suppliers[0];
		return supplier;
	}

	/*
	- Handles creating a supplier; with both create supplier and supplierName rows for both tables


	NOTE: if description="'Inside single quotes' and outside', that throws off our sql query and causes a syntax error.
		The single quote (apostrophe) is a special character in SQL that specifies the beginning and end of string data.
		So to use it as part of your literal string, you'll need to escape. To do this we replace that one single quote 
		with two single quotes. As a result the SQL database will see it as one single quote.
	*/
	Supplier createSupplier(std::string& s_name, std::string& description, std::string& email, std::string& address) {
		
		// Ensure that the input meets syntax constraints
		validateSupplierName(s_name);
		validateDescription(description);
		validateEmail(email);
		validateAddress(address);

		/*
		Escape our input values to prepare them for execution in SQL statements.

		- Escape them after doing syntax and before our database checks and queries. We escape after doing 
		syntax checks to ensure that potentially added escaped characters don't count towards the length. 

		- Then we escape before our queries to ensure that our queries are going to work when the values entered
			contain single quotes. Don't worry as SQL treats two single ('') quotes as a regular single quote (').

		- We'll use the escaped versions in our query, while we'd create our object with the regular versions. As a result 
			when we return supplier, if something was escaped like the s_name, then it wouldn't show two single quotes in 
			places where there'd usually be one.
		*/
		std::string escaped_s_name = dbConn.escapeSQL(s_name);
		std::string escaped_description = dbConn.escapeSQL(description);
		std::string escaped_address = dbConn.escapeSQL(address);
		std::string escaped_email = dbConn.escapeSQL(email);


		// Do database check to verify if s_name isn't already taken by a row in the 'supplier name' table.
		supplierNameManager.checkUniqueSupplierName(escaped_s_name);

		// Input good, first create row in 'suppliers' table
		std::string query = "INSERT INTO " + tableName + " (description, email, address) VALUES ('" + escaped_description + "', '" + escaped_email + "', '" + escaped_address + "');";
		if (!dbConn.executeSQL(query)) {
			throw std::runtime_error("Failed to create supplier email('" + email + "'), address('" + address + "')!");
		}

		// Get the supplier_id from the 'supplier' table.
		const int supplier_id = dbConn.getLastInsertedID();

		// Then create row in the 'supplier name' table
		supplierNameManager.createSupplierName(supplier_id, escaped_s_name);

		// Create and return supplier object that has all info for a supplier
		Supplier supplier(supplier_id, s_name, description, email, address);
		return supplier;
	}

	// Handles updating a supplier's name
	void updateName(int supplier_id, std::string& s_name) {

		// Validate name length
		validateSupplierName(s_name);

		// Escape the supplier name
		std::string escaped_s_name = dbConn.escapeSQL(s_name);

		// Ensure supplier name is unique and not already taken in supplier name table
		supplierNameManager.checkUniqueSupplierName(escaped_s_name);

		// Update the supplier's name in the 'Supplier Name' table; use the supplierNameManager for this.
		supplierNameManager.updateSupplierName(supplier_id, escaped_s_name);
	}

	// Handles updating a supplier's description
	void updateDescription(int supplier_id, std::string& description) {

		// Validate description length
		validateDescription(description);
		// Escape the description; don't need to create 'escaped_description' since we aren't directly creating a supplier object to return
		description = dbConn.escapeSQL(description);

		// Update the row in the 'Supplier' table to alter the description
		std::string query = "UPDATE " + tableName + " SET description='" + description + "' WHERE supplier_id=" + std::to_string(supplier_id) + ";";
		if (!dbConn.executeSQL(query)) {
			throw std::runtime_error("Failed to update 'supplier' with ID " + std::to_string(supplier_id) + ";");
		}
	}

	// Handles updating a supplier's email
	void updateEmail(int supplier_id, std::string& email) {
		validateEmail(email);

		// Escape the email
		email = dbConn.escapeSQL(email);

		// Construct query to update the supplier's email
		std::string query = "UPDATE " + tableName + " SET email='" + email + "' WHERE supplier_id=" + std::to_string(supplier_id) + ";";
		if (!dbConn.executeSQL(query)) {
			throw std::runtime_error("Failed to update 'supplier' with ID " + std::to_string(supplier_id) + ";");
		}
	}

	// Handles updating a supplier's address
	void updateAddress(int supplier_id, std::string& address) {
		validateAddress(address);

		// Escape the address
		address = dbConn.escapeSQL(address);

		// Construct query to update the supplier's address
		std::string query = "UPDATE " + tableName + " SET address='" + address + "' WHERE supplier_id=" + std::to_string(supplier_id) + ";";
		if (!dbConn.executeSQL(query)) {
			throw std::runtime_error("Failed to update supplier's address with ID " + std::to_string(supplier_id) + ";");
		}
	}

	// Handles deleting a supplier
	void deleteSupplier(int supplier_id) {
		// First delete the supplier name entry, this is because it references supplier_id
		supplierNameManager.deleteSupplierName(supplier_id);

		// Delete row from 'suppliers' table
		std::string query = "DELETE FROM " + tableName + " WHERE supplier_id=" + std::to_string(supplier_id) + ";";
		if (!dbConn.executeSQL(query)) {
			throw std::runtime_error("Failed to delete supplier with id '" + std::to_string(supplier_id) + "'!");
		}
	}
};

#endif