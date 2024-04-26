#ifndef OrderItemManager_H
#define OrderItemManager_H
#include <string>
#include <vector>
#include <tuple>

#include "DBConn.h"
#include "OrderItem.h"


class OrderItemManager {
private:
	DBConn& dbConn;
	std::string tableName;
	std::string transactionTableName;
	std::string productTableName;

	OrderItem createOrderItemFromRow(SQLINTEGER order_item_id, SQLINTEGER transaction_id, SQLINTEGER product_id, SQLINTEGER qty) {
		int intOrderItemID = static_cast<int>(order_item_id);
		int intTransactionID = static_cast<int>(transaction_id);
		int intProductID = static_cast<int>(product_id);
		int intQty = static_cast<int>(qty);

		OrderItem orderItem(intOrderItemID, intTransactionID, intProductID, intQty);
		return orderItem;
	}

public:
	OrderItemManager(
		DBConn& dbConn,
		std::string tableName,
		std::string transactionTableName,
		std::string productTableName
	) : dbConn(dbConn),
		tableName(tableName),
		transactionTableName(transactionTableName),
		productTableName(productTableName) {}

	void initTable() {
		std::string query = "CREATE TABLE " + tableName + " ( "
			"order_item_id INT NOT NULL IDENTITY PRIMARY KEY, "
			"transaction_id INT NOT NULL, "
			"product_id INT, "
			"qty INT NOT NULL, "
			"FOREIGN KEY (transaction_id) REFERENCES " + transactionTableName + " (transaction_id), "
			"FOREIGN KEY (product_id) REFERENCES " + productTableName + " (product_id)"
			");";
		if (!dbConn.executeSQL(query)) {
			throw std::runtime_error("Failed to initialize '" + tableName + "' table!");
		}
	}

	std::vector<OrderItem> fetchOrderItems(std::string query) {
		std::vector<OrderItem> orderItems;

		// Execute query to fetch order items
		if (!dbConn.executeSQL(query)) {
			throw std::runtime_error("Failed to query order items!");
		}

		// Create buffers to get the row data
		SQLINTEGER order_item_id = 0;
		SQLINTEGER transaction_id = 0;

		/*
		- product_id can be null, so let's ensure we handle binding the column properly in that case by using an indicator. By doing this we check if the column is null, if it 
		is, then we don't feed product_id the data from that column because we don't want to feed an integer a null value. This would cause problems with how we feed data which 
		is explained in our TransactionManager.
		
		
		*/
		SQLINTEGER product_id = 0;
		SQLLEN product_id_indicator = 0;

		SQLINTEGER qty = 0;

		// Bind columns so that the buffers get the data when we do dbConn.fetchRow()
		dbConn.bindColumn(1, SQL_INTEGER, &order_item_id, sizeof(order_item_id));
		dbConn.bindColumn(2, SQL_INTEGER, &transaction_id, sizeof(transaction_id));
		dbConn.bindColumn(3, SQL_INTEGER, &product_id, sizeof(product_id), &product_id_indicator);
		dbConn.bindColumn(4, SQL_INTEGER, &qty, sizeof(qty));

		while (true) {
			SQLRETURN retcode = dbConn.fetchRow();

			// If no more rows to be fetched, exit the loop
			if (retcode == SQL_NO_DATA) {
				break;
			} else if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO && product_id != 0) {
				dbConn.closeCursor(); // ensure we close cursor before throwing an error 
				throw std::runtime_error("Failed to fetch a given customer!");
			}

			// Create product object using row data
			OrderItem orderItem = createOrderItemFromRow(order_item_id, transaction_id, product_id, qty);

			// Put product object into array
			orderItems.push_back(orderItem);
		}

		dbConn.closeCursor();
		return orderItems;
	}



	/*
	- Create an order item for an existing transaction row.
	*/
	OrderItem createOrderItem(int transaction_id, int product_id, int qty) {
		std::string query = "INSERT INTO " + tableName + " (transaction_id, product_id, qty) VALUES(" + std::to_string(transaction_id) + "," + std::to_string(product_id) + "," + std::to_string(qty) + ");";
		
		if (!dbConn.executeSQL(query)) {
			throw std::runtime_error("Failed to insert order item!");
		}

		int order_item_id = dbConn.getLastInsertedID();
		OrderItem orderItem(order_item_id, transaction_id, product_id, qty);

		return orderItem;
	}


	// Gets all order items for a specific transaction
	std::vector<OrderItem> getOrderItems(int transaction_id) {
		std::string query = "SELECT * FROM " + tableName + " WHERE transaction_id=" + std::to_string(transaction_id) + ";";
		return fetchOrderItems(query);
	}


	/*
	+ Handles creating/inserting multiple order item rows.
	*/
	void batchCreateOrderItem(std::vector<std::tuple<int, int, int>> orderItems) {
		if (orderItems.empty()) {
			return; // No items to insert
		}

		// Build the query string with multiple INSERT statements
		std::string query = "INSERT INTO " + tableName + " (transaction_id, product_id, qty) VALUES ";
		for (size_t i = 0; i < orderItems.size(); ++i) {
			query += "(" + std::to_string(std::get<0>(orderItems[i])) + ", "
				+ std::to_string(std::get<1>(orderItems[i])) + ", "
				+ std::to_string(std::get<2>(orderItems[i])) + ")";
			if (i != orderItems.size() - 1) {
				query += ",";
			}
		}

		// Execute the query
		if (!dbConn.executeSQL(query)) {
			throw std::runtime_error("Failed to insert order items!");
		}
	}

	// Nullifies product_id column for all order items that have a given product_id; good when a single product is deleted
	void nullifyProductID(int product_id) {
		std::string query = "UPDATE " + tableName + " SET product_id = NULL WHERE product_id=" + std::to_string(product_id) + ";";

		if (!dbConn.executeSQL(query)) {
			throw std::runtime_error("Failed to update order items and nullify product_id!");
		}
	}

	// Nullifies product_id column for all products that have a given supplier; good when supplier is deleted and we need to nullify all product_id values that were associated with it
	void nullifyProductIDBySupplierID(int supplier_id) {
		
		std::string query = "UPDATE " + tableName + " SET " + tableName + ".product_id = NULL "
			"WHERE product_id IN (SELECT " + productTableName + ".product_id FROM " + productTableName + " WHERE supplier_id=" + std::to_string(supplier_id) + ");";

		if (!dbConn.executeSQL(query)) {
			throw std::runtime_error("Failed to update order items and nullify product_id via supplier_id");
		}

	}

};

#endif