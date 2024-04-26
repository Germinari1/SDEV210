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

		// Execute query to fetch transactions
		if (!dbConn.executeSQL(query)) {
			throw std::runtime_error("Failed to query transactions!");
		}

		// Create buffers to get the row data
		SQLINTEGER transaction_id = 0;
		
		/*
		+ Dealing with NULL Values:
		For a transaction row, the customer_id column could be NULL in the database. In this case, we'll use an 'indicator'. An indicator is a variable that's used to check whether
		a value that we're fetching for a column is NULL (SQL_NULL_DATA) or not. If so, then we let the program know that the column's value should not be used or fed into our 'customer_id'.
		So if customer_id in the row is null, then we ignore the value and keep our '0' value.

		If we didn't have the customer_id_indicator then it would cause issues. When there would be a NULL value for customer_id in a row, then we'd be feeding a null value to 
		an SQLINTEGER. As a result, it would cause the following columns, total and order_date, to be 'nullified' as well when we're feeding them the row values. As a result 
		when customer_id = NULL, the program would also incorrectly fetch the total and order_date values, setting them to a zero-like or null value as well.


		+ In the while loop:
		When doing dbConn.fetchRow(), when a column such as customer_id is NULL, then our retcode != SQL_SUCCESS, yeah it's indicated as a failure. So it's still able to fetch the row's data, but 
		if a column is seen as NULL, it's indicated as a failure. Of course for us this isn't a failure, it's intentional, our customer_id column for our transactions can be null. So to prevent us 
		from throwing an error and stopping fetchTransactions for this reason, we ensure that we only throw an error when retcode != SQL_SUCCESS AND the customer_id != 0. As a result, we only throw 
		an error when it doesn't involve customer_id being 0. Because we know the customer_id from the row was null when customer_id is 0, because if wasn't null, then customer_id would have been assigned a positive integer that represents a valid ID value.

		*/
		SQLINTEGER customer_id = 0;
		SQLLEN customer_id_indicator = 0;

		SQLFLOAT total = 0;
		DATE_STRUCT order_date = { 0 };

		// Bind columns so that the buffers get the data when we do dbConn.fetchRow()
		dbConn.bindColumn(1, SQL_INTEGER, &transaction_id, sizeof(transaction_id));
		dbConn.bindColumn(2, SQL_INTEGER, &customer_id, sizeof(customer_id), &customer_id_indicator);
		dbConn.bindColumn(3, SQL_C_DOUBLE, &total, sizeof(total));
		dbConn.bindColumn(4, SQL_C_DATE, &order_date, sizeof(order_date));

		while (true) {
			SQLRETURN retcode = dbConn.fetchRow();

			// If no more rows to be fetched, exit the loop
			if (retcode == SQL_NO_DATA) {
				break;
			} else if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO && customer_id != 0) {
				dbConn.closeCursor(); // ensure we close cursor before throwing an error 
				throw std::runtime_error("Failed to fetch a given transaction!");
			}

			// Create product object using row data
			Transaction transaction = createTransactionFromRow(transaction_id, customer_id, total, order_date);

			// Put product object into array
			transactions.push_back(transaction);
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


	// Returns a vector of all transactions in the table
	std::vector<Transaction> getAllTransactions() {
		std::string query = "SELECT * FROM " + tableName + ";";
		std::vector<Transaction> transactions = fetchTransactions(query);
		return transactions;
	}

	Transaction getTransactionByID(int transaction_id) {
		std::string query = "SELECT * FROM " + tableName + " WHERE transaction_id=" + std::to_string(transaction_id) + ";";
		std::vector<Transaction> transactions = fetchTransactions(query);
		if (transactions.size() == 0) {
			throw std::runtime_error("Transaction with ID(" + std::to_string(transaction_id) + ") wasn't found!");
		}

		return transactions[0];
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