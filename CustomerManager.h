#ifndef CustomerManager_H
#define CustomerManager_H
#include <string>
#include <sstream>
#include "DBConn.h";
#include "Customer.h";


/*
+ CustomerManager: Class that encapsulates and handles operations with the 'Customer' table
*/

class CustomerManager {
private:
	DBConn& dbConn;
	std::string tableName;

	/*
	- It'd be a good idea to have constants to show the limits of our varchars.
	  Note that right now, all varchar values are 50. Except for the 'description'
	  attribute for Supplier_info and product tables which are 2000. So having these 
	  static constants

	*/
	static const int MAX_FNAME_LENGTH = 50;
	


public:
	CustomerManager(DBConn& dbConn, std::string tableName) : dbConn(dbConn), tableName(tableName) {}

	void initTable() {
		std::string query = "CREATE TABLE " + tableName + " ( "
			"customer_id INT NOT NULL IDENTITY PRIMARY KEY, "
			"fname VARCHAR(255) NOT NULL, "
			"lname VARCHAR(255) NOT NULL, "
			"email VARCHAR(255) NOT NULL, "
			"points INT NOT NULL "
			");";
		if (!dbConn.executeSQL(query)) {
			throw std::runtime_error("Failed to initialize '" + tableName + "' table!");
		}
	}

	Customer getCustomerByID(int customer_id) {
		std::string query = "SELECT * FROM " + tableName + " WHERE customer_id=" + std::to_string(customer_id) + ";";

		// Execute SQL Statement
		if (!dbConn.executeSQL(query)) {
			throw std::runtime_error("Failed to execute query for customer with ID '" + std::to_string(customer_id) + "'!");
		}

		// Variables to hold column (index 255 is for null terminator character)
		SQLCHAR fname[256] = {};
		SQLCHAR lname[256] = {};
		SQLCHAR email[256] = {};
		SQLINTEGER points = 0;

		// Bind columns
		SQLRETURN retcode = dbConn.bindColumn(2, SQL_C_CHAR, fname, sizeof(fname));
		retcode = dbConn.bindColumn(3, SQL_C_CHAR, lname, sizeof(lname));
		retcode = dbConn.bindColumn(4, SQL_C_CHAR, email, sizeof(email));
		retcode = dbConn.bindColumn(5, SQL_INTEGER, &points, sizeof(points));

		// Fetch the row
		retcode = dbConn.fetchRow();
		if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) {
			throw std::runtime_error("Customer with ID '" + std::to_string(customer_id) + "' wasn't found!");
		}

		/*
		- Null terminate the strings. reinterpret_cast needs that null-terminator 
		because in C/C++ strings end with a null character '\0'. If they don't, then 
		our conversion process could read beyond what was intended, resulting in unexpected behavior.
		
		- Add null characters.
		*/
		fname[sizeof(fname) - 1] = '\0';
		lname[sizeof(lname) - 1] = '\0';
		email[sizeof(email) - 1] = '\0';

		// Convert SQLCHAR to strings and SQLINTEGER to int
		std::string fStr(reinterpret_cast<char*>(fname));
		std::string lStr(reinterpret_cast<char*>(lname));
		std::string eStr(reinterpret_cast<char*>(email));
		int intPoints = static_cast<int>(points);

		// Construct and return a Customer object
		Customer customer(
			customer_id,
			fStr,
			lStr,
			eStr,
			intPoints
		);
		return customer;
	}

	Customer createCustomer(std::string& fname, std::string& lname, std::string& email, int points) {
		std::string query = "INSERT INTO " + tableName + " (fname, lname, email, points) VALUES ('" + fname + "', '" + lname + "', '" + email + "', " + std::to_string(points) + ")";

		if (!dbConn.executeSQL(query)) {
			throw std::runtime_error("Failed to create customer '" + fname + " " + lname + "' with email '" + email + "'!");
		}

		const int id = dbConn.getLastInsertedID();
		Customer customer(id, fname, lname, email, points);
		return customer;
	}

	Customer updateCustomer(int customer_id, std::string& fname, std::string& lname, std::string& email, int points) {
		// Create and run SQL query to update multiple columns.
		std::string query = "UPDATE " + tableName + " SET fname='" + fname + "', lname='" + lname + "', email='" + email + "', points=" + std::to_string(points) + " WHERE customer_id=" + std::to_string(customer_id) + ";";
		if (!dbConn.executeSQL(query)) {
			throw std::runtime_error("Failed to update customer with id '" + std::to_string(customer_id) + "'!");
		}

		// Update was successful, create object representing the updated customer. Then return it.
		Customer customer(customer_id, fname, lname, email, points);
		return customer;
	}

	void deleteCustomer(int customer_id) {
		std::string query = "DELETE FROM " + tableName + " WHERE customer_id=" + std::to_string(customer_id) + ";";
		if (!dbConn.executeSQL(query)) {
			throw std::runtime_error("Failed to delete customer with id '" + std::to_string(customer_id) + "'!");
		}
	}


};


#endif