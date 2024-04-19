#ifndef ProductManager_H
#define ProductManager_H

#include <string>
#include <vector>
#include "DBConn.h"
#include "Product.h"

class ProductManager {
private:
	DBConn& dbConn;
	std::string tableName; // Table name for products, such as 'products' table
	std::string supplierTableName; // Table name for the 'suppliers' table in which products reference with suppiler_id
	static const int MAX_P_NAME_LENGTH = 50;
	static const int MAX_DESCRIPTION_LENGTH = 2000;

public:
	ProductManager(
		DBConn& dbConn,
		std::string& tableName,
		std::string& supplierTableName
	) : dbConn(dbConn),
		tableName(tableName),
		supplierTableName(supplierTableName) {}

	// Initialize table for holding products
	void initTable() {
		std::string query = "CREATE TABLE " + tableName + " ( "
			"product_id INT NOT NULL IDENTITY PRIMARY KEY, "
			"supplier_id INT NOT NULL, "
			"p_name VARCHAR(" + std::to_string(MAX_P_NAME_LENGTH) + ") NOT NULL, "
			"description VARCHAR(" + std::to_string(MAX_DESCRIPTION_LENGTH) + ") NOT NULL, "
			"price DECIMAL(8, 2) NOT NULL CHECK (price >= 0), "
			"qty INT NOT NULL CHECK (qty >= 0), "
			"FOREIGN KEY (supplier_id) REFERENCES " + supplierTableName + " (supplier_id)"
			");";
		if (!dbConn.executeSQL(query)) {
			throw std::runtime_error("Failed to initialize '" + tableName + "' table!");
		}
	}

	// Validates p_name is within length constraints
	void validateProductName(std::string& p_name) {
		if (p_name.length() > MAX_P_NAME_LENGTH) {
			throw std::runtime_error("Product p_name exceeds maximum length of " + std::to_string(MAX_P_NAME_LENGTH) + " characters!");
		}
	}

	// Validates description is within length constraints
	void validateDescription(std::string& description) {
		if (description.length() > MAX_DESCRIPTION_LENGTH) {
			throw std::runtime_error("Product description exceeds maximum length of " + std::to_string(MAX_DESCRIPTION_LENGTH) + " characters!");
		}
	}

	// Validates price isn't negative
	void validatePrice(float price) {
		if (price < 0) {
			throw std::runtime_error("Product price can't be negative!");
		}
	}

	// Validates quantity isn't negative
	void validateQty(int qty) {
		if (qty < 0) {
			throw std::runtime_error("Product quantity can't be negative!");
		}
	}

	// Returns a vector of Products
	std::vector<Product> getAllProducts() {
		std::vector<Product> products;

		// Execute query to get all products
		std::string query = "SELECT * FROM " + tableName + ";";
		if (!dbConn.executeSQL(query)) {
			throw std::runtime_error("Failed to fetch customers from the database!");
		}

		// Create buffers/variables that will capture row data
		SQLINTEGER product_id = 0;
		SQLINTEGER supplier_id = 0;
		SQLCHAR p_name[MAX_P_NAME_LENGTH + 1] = {};
		SQLCHAR description[MAX_DESCRIPTION_LENGTH + 1] = {};
		SQLFLOAT price = 0;
		SQLINTEGER qty = 0;

		// Bind columns, allowing them to get data
		dbConn.bindColumn(1, SQL_INTEGER, &product_id, sizeof(product_id));
		dbConn.bindColumn(2, SQL_INTEGER, &supplier_id, sizeof(supplier_id));
		dbConn.bindColumn(3, SQL_C_CHAR, p_name, sizeof(p_name));
		dbConn.bindColumn(4, SQL_C_CHAR, description, sizeof(description));
		dbConn.bindColumn(5, SQL_C_DOUBLE, &price, sizeof(price));
		dbConn.bindColumn(6, SQL_INTEGER, &qty, sizeof(qty));

		// Loop through all rows we got
		while (true) {
			SQLRETURN retcode = dbConn.fetchRow();

			// If no more rows to be fetched, exit the loop
			if (retcode == SQL_NO_DATA) {
				break;
			}
			// Else if we failed to fetch data
			else if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) {
				dbConn.closeCursor(); // ensure we close cursor before throwing an error 
				throw std::runtime_error("Failed to fetch all customers from the database!");
			}

			// Null terminate the strings; prepares them to be converted to std::string
			p_name[MAX_P_NAME_LENGTH] = '\0';
			description[MAX_DESCRIPTION_LENGTH] = '\0';

			// Convert SQL data-types to regular data-types
			int intProductID = static_cast<int>(product_id);
			int intSupplierID = static_cast<int>(supplier_id);
			std::string p_name_str(reinterpret_cast<char*>(p_name));
			std::string descriptionStr(reinterpret_cast<char*>(description));
			float floatPrice = static_cast<float>(price);
			int intQty = static_cast<int>(qty);

			products.emplace_back(Product(intProductID, intSupplierID, p_name_str, descriptionStr, floatPrice, intQty));
		}

		// Close database cursor since we fetched data AND returned vector of products.
		dbConn.closeCursor();
		return products;
	}

	// Returns a Product object when passed a product_id
	Product getProductByID(int product_id) {

		// Query to select all products from table
		std::string query = "SELECT * FROM " + tableName + " WHERE product_id=" + std::to_string(product_id) + ";";
		if (!dbConn.executeSQL(query)) {
			throw std::runtime_error("Failed to query '" + tableName + "' table!");
		}

		// Create buffers/variables to 
		SQLINTEGER supplier_id = 0;
		SQLCHAR p_name[MAX_P_NAME_LENGTH + 1] = {};
		SQLCHAR description[MAX_DESCRIPTION_LENGTH + 1] = {};
		SQLFLOAT price = 0;
		SQLINTEGER qty = 0;

		// Bind columns, allowing them to get data
		dbConn.bindColumn(2, SQL_INTEGER, &supplier_id, sizeof(supplier_id));
		dbConn.bindColumn(3, SQL_C_CHAR, p_name, sizeof(p_name));
		dbConn.bindColumn(4, SQL_C_CHAR, description, sizeof(description));
		dbConn.bindColumn(5, SQL_C_DOUBLE, &price, sizeof(price));
		dbConn.bindColumn(6, SQL_INTEGER, &qty, sizeof(qty));


		// Fetch the row's data and put it in our buffers/variables
		SQLRETURN retcode = dbConn.fetchRow();
		if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) {
			// Close cursor before throwing exception; prevents cursor from being in an invalid state for the next query
			dbConn.closeCursor();
			throw std::runtime_error("Product with ID '" + std::to_string(product_id) + "' wasn't found!");
		}

		// Close cursor here as well
		dbConn.closeCursor();

		// Null terminate the string values
		p_name[MAX_P_NAME_LENGTH] = '\0';
		description[MAX_DESCRIPTION_LENGTH] = '\0';

		// Convert SQL data-types to regular data-types
		int intProductID = static_cast<int>(product_id);
		int intSupplierID = static_cast<int>(supplier_id);
		std::string p_name_str(reinterpret_cast<char*>(p_name));
		std::string descriptionStr(reinterpret_cast<char*>(description));
		float floatPrice = static_cast<float>(price);
		int intQty = static_cast<int>(qty);


		// Create and return product object
		Product product(intProductID, intSupplierID, p_name_str, descriptionStr, floatPrice, intQty);

		return product;
	}

	// Creates a new product in the database and returns the object representation of that product
	Product createProduct(int supplier_id, std::string p_name, std::string description, float price, int qty) {

		// Escape string related data	
		std::string escaped_p_name = dbConn.escapeSQL(p_name);
		std::string escapedDescription = dbConn.escapeSQL(description);

		// Construct INSERT query for inserting a new product
		std::string query = "INSERT INTO " + tableName + " (supplier_id, p_name, description, price, qty) VALUES ('" + std::to_string(supplier_id) + "', '" + escaped_p_name + "', '" + escapedDescription + "', '" + std::to_string(price) + "', '" + std::to_string(qty) + "');";

		// Attempt to execute insert query
		if (!dbConn.executeSQL(query)) {
			throw std::runtime_error("Failed to create with supplier_id(" + std::to_string(supplier_id) + "), and p_name '" + p_name + "'!");
		}

		// Get the ID of the product, create an object representation, and return the product
		const int product_id = dbConn.getLastInsertedID();
		Product product(product_id, supplier_id, p_name, description, price, qty);
		return product;
	}

	// Updates a product's name
	void updateName(int product_id, std::string p_name) {
		validateProductName(p_name);
		p_name = dbConn.escapeSQL(p_name);
		std::string query = "UPDATE " + tableName + " SET p_name='" + p_name + "' WHERE product_id=" + std::to_string(product_id) + ";";
		if (!dbConn.executeSQL(query)) {
			throw std::runtime_error("Failed to update product with id '" + std::to_string(product_id) + "'!");
		}
	}

	// Updates a product's description
	void updateDescription(int product_id, std::string description) {
		validateDescription(description);
		description = dbConn.escapeSQL(description);
		std::string query = "UPDATE " + tableName + " SET description='" + description + "' WHERE product_id=" + std::to_string(product_id) + ";";
		if (!dbConn.executeSQL(query)) {
			throw std::runtime_error("Failed to update product with id '" + std::to_string(product_id) + "'!");
		}
	}

	// Updates a product's price
	void updatePrice(int product_id, float price) {
		validatePrice(price);
		std::string query = "UPDATE " + tableName + " SET price=" + std::to_string(price) + " WHERE product_id=" + std::to_string(product_id) + ";";
		if (!dbConn.executeSQL(query)) {
			throw std::runtime_error("Failed to update product with id '" + std::to_string(product_id) + "'!");
		}
	}

	// Updates quantity on a product
	void updateQuantity(int product_id, int qty) {
		validateQty(qty);
		std::string query = "UPDATE " + tableName + " SET qty=" + std::to_string(qty) + " WHERE product_id=" + std::to_string(product_id) + ";";
		if (!dbConn.executeSQL(query)) {
			throw std::runtime_error("Failed to update product with id '" + std::to_string(product_id) + "'!");
		}
	}

	// Deletes a product
	void deleteProduct(int product_id) {
		std::string query = "DELETE " + tableName + " WHERE product_id=" + std::to_string(product_id) + ";";
		if (!dbConn.executeSQL(query)) {
			throw std::runtime_error("Failed to delete product with id '" + std::to_string(product_id) + "'. It may not exist!");
		}
	};

	void deleteBySupplierID(int supplier_id) {
		std::string query = "DELETE " + tableName + " WHERE supplier_id=" + std::to_string(supplier_id) + ";";
		if (!dbConn.executeSQL(query)) {
			throw std::runtime_error("Failed to delete product with supplier_id '" + std::to_string(supplier_id) + "'. It may not exist!");
		}
	}
};

#endif