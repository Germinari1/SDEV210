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