/*
- Step 1:
1. Get the customer_id (customer checking out).
2. Then get the cart items they are checking out. We are passed this in.
3. Here we can then calculate the total of their cart, and also the order_date using sql query constant.
4. At this point we have enough data to actually create the transaction (customer_id, total, order_date), however we need to do some extra checks
- Step 2:
1. Using productManager, check that quantities are
	within limits. So you'd get an array of products
	that are being used in the cart items. Compare their
	quantites over a loop. If one iteration fails, we
	tell the user which failed, and item they need to remove or update.
2. If the user made it past this phase, quantities that the user has in their cart are valid, so we can start creating the transaction and order items, updating the product quantities, and finally deleting the cartItems associated with the user (cart is cleared
- Step 3:
1.
*/

#ifndef TransactionManager_H
#define TransactionManager_H
#include <string>
#include <vector>
#include <sstream>
#include "DBConn.h"
#include "Transaction.h"
#include "CartItem.h"



class TransactionManager {
private:
	DBConn& dbConn;
	std::string tableName;
	std::string customerTableName;

	Transaction createTransactionFromRow(SQLINTEGER transaction_id, SQLINTEGER customer_id, SQLFLOAT total, DATE_STRUCT order_date) {
		// Convert your SQL data types to regular ones; still need to convert order_date
		int intTransactionID = static_cast<int>(transaction_id);
		int intCustomerID = static_cast<int>(customer_id);
		float floatTotal = static_cast<float>(total);

		// Convert date into string
		std::stringstream ss;
		ss << order_date.year << "-" << order_date.month << "-" << order_date.day;
		std::string strOrderDate = ss.str();
		
		// Create transaction object and return it
		Transaction transaction(intTransactionID, intCustomerID, floatTotal, strOrderDate);
		return transaction;
	}

public:
	TransactionManager(
		DBConn& dbConn,
		std::string tableName,
		std::string customerTableName
	) : dbConn(dbConn),
		tableName(tableName),
		customerTableName(customerTableName) {}

	void initTable() {
		std::string query = "CREATE TABLE " + tableName + " ( "
			"transaction_id INT NOT NULL IDENTITY PRIMARY KEY, "
			"customer_id INT, "
			"total DECIMAL(8,2) NOT NULL, "
			"order_date DATE NOT NULL, "
			"FOREIGN KEY (customer_id) REFERENCES " + customerTableName + " (customer_id)"
			");";
		if (!dbConn.executeSQL(query)) {
			throw std::runtime_error("Failed to initialize '" + tableName + "' table!");
		}
	}

	std::vector<Transaction> fetchTransactions(std::string query) {
		std::vector<Transaction> transactions;

		// Execute query to fetch customers
		if (!dbConn.executeSQL(query)) {
			throw std::runtime_error("Failed to query transactions!");
		}

		// Create buffers to get the row data
		SQLINTEGER transaction_id = 0;
		SQLINTEGER customer_id = 0;
		SQLFLOAT total = 0;
		DATE_STRUCT order_date = { 0 };

		// Bind columns so that the buffers get the data when we do dbConn.fetchRow()
		dbConn.bindColumn(1, SQL_INTEGER, &transaction_id, sizeof(transaction_id));
		dbConn.bindColumn(2, SQL_INTEGER, &customer_id, sizeof(customer_id));
		dbConn.bindColumn(3, SQL_C_DOUBLE, &total, sizeof(total));
		dbConn.bindColumn(4, SQL_C_DATE, &order_date, sizeof(order_date));

		while (true) {
			SQLRETURN retcode = dbConn.fetchRow();

			// If no more rows to be fetched, exit the loop
			if (retcode == SQL_NO_DATA) {
				break;
			}
			// Else if we failed to fetch data
			else if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) {
				dbConn.closeCursor(); // ensure we close cursor before throwing an error 
				throw std::runtime_error("Failed to fetch a given customer!");
			}

			// Create product object using row data
			Transaction product = createTransactionFromRow(transaction_id, customer_id, total, order_date);

			// Put product object into array
			transactions.push_back(product);
		}
		
		dbConn.closeCursor();
		return transactions;
	}

	/*
	- Creates new transaction row in teh databaes and returns object representation
	of transaction.
	

	NOTE: getCurrentDate returns date in yyyy-mm-dd form, which matches how the DATE column stores the dates.
	*/
	Transaction createTransaction(int customer_id, float total) {
		std::string insertQuery = "INSERT INTO " + tableName + " (customer_id, total, order_date) VALUES("
			+ std::to_string(customer_id) + "," + std::to_string(total) + ",GETDATE()"
			");";
		if (!dbConn.executeSQL(insertQuery)) {
			throw std::runtime_error("Failed to insert new transaction!");
		}

		// Get the ID of the transaction or row that we just inserted
		int transaction_id = dbConn.getLastInsertedID();

		// Create transaction object
		Transaction transaction(transaction_id, customer_id, total, dbConn.getCurrentDate());

		return transaction;
	}

	// Nullifies customer_id column for all transactions; good when customer is deleted
	void nullifyCustomerID(int customer_id) {
		std::string query = "UPDATE " + tableName + " SET customer_id = NULL WHERE customer_id=" + std::to_string(customer_id) + ";";
		if (!dbConn.executeSQL(query)) {
			throw std::runtime_error("Failed to update transaction and nullify customer_id!");
		}
	}
};

#endif