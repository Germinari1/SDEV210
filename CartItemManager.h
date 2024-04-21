#ifndef CartItemManager_H
#define CartItemManager_H
#include <string>
#include <vector>
#include "DBConn.h"
#include "CartItem.h"


/*
+ CartItemManager:
Manages the 'CartItem' table. Using this table we can having a shopping cart for every customer. Each row in this table basically represents 
a product, and how much of that product a customer currently has in their shopping cart.

1. customer_id: References which customer the cart item belongs to.
2. product_id: The item that's in the customer's shopping cart.
	- NOTE: Neither can be null because combined, they make up the primary key and allow
	us to unique identify any row in the database. As well as this, we should note that 
	if a customer gets deleted, then all of their cart items are deleted. As well as this, 
	if a product is deleted, all cart items referencing that product should be deleted.
	We will expose functions such as deleteByCustomerID and deleteByProductID to make this 
	happen, and we'll use these functions in our RetailApp. Right now this will definitely
	be used in functions that handle deleting a product, supplier, and customer. 

	Now you may be asking, but for deleting a supplier, we don't know the product_id values of the 
	products linked to that supplier. You're correct, however we can definitely find those values out.
	Before deleting the products associated with a supplier, we should call something like 'getProductsBySupplierID()'
	and get back a vector of the products. Then just convert that to an array of product_id values. You would 
	attempt to delete CartItems first since they reference products, then delete the products, and finally
	you'd delete the suppliers! Deleting them in this order maintains data integrity.

	Or we could somehow do some subquery magic and make it so we pass in a supplier_id instead of a 
	product_id when we'er deleting a supplier

3. qty: The quantity or amount of that item that is in the customer's shopping cart.

+ Usage: We can rely on the database to get all of cart items in a user's shopping cart, rather than relying on some in-memory 
	method of storing such as an array, which has some holes in data integrity. For example, when deleting a supplier, all of the 
	products referencing that supplier should be deleted. It'd be pretty difficult, to try to find a way to get the IDs of the 
	deleted products, and try to synchronize our in-memory state.


*/
class CartItemManager {
private:
	DBConn& dbConn;
	std::string tableName;
	std::string customerTableName;
	std::string productTableName;


	static const int MAX_P_NAME_LENGTH = 50;

	// Helper function to create customer object from row data.
	CartItem createCartItemFromRow(SQLINTEGER customer_id, SQLINTEGER product_id, SQLINTEGER qty, SQLCHAR* p_name, SQLFLOAT price) {

		// Null terminate the string
		p_name[MAX_P_NAME_LENGTH] = '\0';

		// Convert data-types and add the cart item to the vector of cart items
		int intCustomerID = static_cast<int>(customer_id);
		int intProductID = static_cast<int>(product_id);
		int intQty = static_cast<int>(qty);
		std::string p_name_str(reinterpret_cast<char*>(p_name));
		float floatPrice = static_cast<float>(price);

		// Return cart item as object
		CartItem cartItem(intCustomerID, intProductID, intQty, p_name_str, floatPrice);
		return cartItem;
	}

public:
	CartItemManager(DBConn& dbConn, std::string tableName, std::string customerTableName, std::string productTableName) : dbConn(dbConn), tableName(tableName), customerTableName(customerTableName), productTableName(productTableName) {}

	void initTable() {
		std::string query = "CREATE TABLE " + tableName + " ( "
			"customer_id INT NOT NULL, "
			"product_id INT NOT NULL, "
			"qty INT NOT NULL, "
			"PRIMARY KEY(customer_id, product_id), "
			"FOREIGN KEY (customer_id) REFERENCES " + customerTableName + " (customer_id), "
			"FOREIGN KEY (product_id) REFERENCES " + productTableName + " (product_id)"
			");";
		if (!dbConn.executeSQL(query)) {
			throw std::runtime_error("Failed to initialize '" + tableName + "' table!");
		}
	}


	std::vector<CartItem> fetchCartItems(const std::string query) {
		std::vector<CartItem> cartItems;

		// Execute query to fetch customers
		if (!dbConn.executeSQL(query)) {
			throw std::runtime_error("Failed to query cart items from the database!");
		}

		// Create variables that will get data fro
		SQLINTEGER customer_id = 0;
		SQLINTEGER product_id = 0;
		SQLINTEGER qty = 0;
		SQLCHAR p_name[MAX_P_NAME_LENGTH + 1] = {};
		SQLFLOAT price = 0;


		// Bind/prepare columns to get the data
		dbConn.bindColumn(1, SQL_INTEGER, &customer_id, sizeof(customer_id));
		dbConn.bindColumn(2, SQL_INTEGER, &product_id, sizeof(product_id));
		dbConn.bindColumn(3, SQL_INTEGER, &qty, sizeof(qty));
		dbConn.bindColumn(4, SQL_C_CHAR, &p_name, sizeof(p_name));
		dbConn.bindColumn(5, SQL_C_DOUBLE, &price, sizeof(price));

		while (true) {
			SQLRETURN retcode = dbConn.fetchRow();
			if (retcode == SQL_NO_DATA) {
				break;
			}
			else if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) {
				dbConn.closeCursor();
				throw std::runtime_error("Failed to fetch customer's cart items!");
			}

			CartItem cartItem = createCartItemFromRow(customer_id, product_id, qty, p_name, price);
			cartItems.push_back(cartItem);
		}

		dbConn.closeCursor();

		return cartItems;
	}


	// Get all cart items for a particular customer
	std::vector<CartItem> getCustomerCartItems(int customer_id) {
		// Create JOIN query to get all cart items for a particular customer_id; get all cart item columns, but also p_name and price from products table.
		std::string query = "SELECT " + tableName + ".*, " + productTableName + ".p_name, " + productTableName + ".price "
			"FROM " + tableName + " "
			"JOIN " + productTableName + " ON " + productTableName + ".product_id = " + tableName + ".product_id "
			"WHERE customer_id = " + std::to_string(customer_id) + ";";

		// Run query and get back vector of cart items; then return vector
		std::vector<CartItem> cartItems = fetchCartItems(query);
		return cartItems;
	}

	// Get a specific cart item for a particular customer
	CartItem getCartItem(int customer_id, int product_id) {

		// Create a JOIN query for a cart item with a particular customer_id and product_id
		std::string query = "SELECT " + tableName + ".*, " + productTableName + ".p_name, " + productTableName + ".price "
			"FROM " + tableName + " "
			"JOIN " + productTableName + " ON " + productTableName + ".product_id = " + tableName + ".product_id "
			"WHERE customer_id = " + std::to_string(customer_id) + " AND " + productTableName + ".product_id = " + std::to_string(product_id) + ";";

		// Fetch cart items; we're expecting only one cart item since customer_id and product_id make up the primary key
		std::vector<CartItem> cartItems = fetchCartItems(query);

		// If empty, cart item doesn't exist
		if (cartItems.empty()) {
			throw std::runtime_error("Cart Item for customer_id(" + std::to_string(customer_id) + ") and product_id(" + std::to_string(product_id) + ") doesn't exist!");
		}

		// Not empty, so index out and return the found cart item
		CartItem cartItem = cartItems[0];
		return cartItem;


		return cartItem;
	}

	/*
	- Handles checking if a particular item is already in a given customer's shopping cart.

	To do this, just check the cart items table to see if a row with
	the matching customer_id and product_id already exists.

	*/
	bool isExistingCartItem(int customer_id, int product_id) {
		bool isExists = true;

		// Create and execute query
		std::string query = "SELECT * FROM " + tableName + " WHERE customer_id=" + std::to_string(customer_id) + " AND product_id=" + std::to_string(product_id) + ";";
		if (!dbConn.executeSQL(query)) {
			throw std::runtime_error("Failed to query cart items from the database!");
		}

		// Fetch the row, if it isn't a success, then the row (cart item) doesn't exist so mark the boolean as false
		SQLRETURN retcode = dbConn.fetchRow();
		if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) {
			isExists = false;
		}
		dbConn.closeCursor();
		return isExists;
	}

	/*
	- Add new cart item. Good for adding new product into a customer's cart.
	
	NOTE: Ensure it is not an existing cart item, because if we don't we'd be adding 
		in a customer_id and product_id that already exists, therefore ruining the 
		uniqueness of the table.
	*/
	void createCartItem(int customer_id, int product_id, int qty) {

		// Check if item is already in the given customer's cart, if so stop function execution
		bool isExists = isExistingCartItem(customer_id, product_id);
		if (isExists) {
			throw std::runtime_error("Product with ID (" + std::to_string(product_id) + ") is already in customer's cart!");
		}

		std::string query = "INSERT INTO " + tableName + " (customer_id, product_id, qty) VALUES ("
			+ std::to_string(customer_id) + ", "
			+ std::to_string(product_id) + ", "
			+ std::to_string(qty) + ");";

		if (!dbConn.executeSQL(query)) {
			throw std::runtime_error("Insert cart item into the database!");
		}
	}

	// Updates the quantity for an existing cart item.
	void updateCartItem(int customer_id, int product_id, int qty) {
		
		// Verify that the cart item actually exists.
		bool isExists = isExistingCartItem(customer_id, product_id);
		if (!isExists) {
			throw std::runtime_error("Cart item with customer_id(" + std::to_string(customer_id) + " and product_id(" + std::to_string(product_id) + ") is already in customer's cart!");
		}

		std::string query = "UPDATE " + tableName + " SET qty="
			+ std::to_string(qty) + " WHERE customer_id="
			+ std::to_string(customer_id) + " AND product_id="
			+ std::to_string(product_id) + ";";
		if (!dbConn.executeSQL(query)) {
			throw std::runtime_error("Failed to update cart item!");
		}


	}

	// Delete a cart item from the table using customer_id and product_id; removing item from customer's cart
	void deleteCartItem(int customer_id, int product_id) {
		std::string query = "DELETE FROM " + tableName + " WHERE customer_id=" + std::to_string(customer_id) + " AND product_id=" + std::to_string(product_id) + ";";
		if (!dbConn.executeSQL(query)) {
			throw std::runtime_error("Failed to delete cart item from the database!");
		}
	}

	// Delete cart items via product_id; good when deleting a product
	void deleteByProductID(int product_id) {
		std::string query = "DELETE FROM " + tableName + " WHERE product_id=" + std::to_string(product_id) + ";";
		if (!dbConn.executeSQL(query)) {
			throw std::runtime_error("Failed to delete cart items via product_id!");
		}
	}

	// Delete cart items via customer_id; good when deleting a customer
	void deleteByCustomerID(int customer_id) {
		std::string query = "DELETE FROM " + tableName + " WHERE customer_id=" + std::to_string(customer_id) + ";";
		if (!dbConn.executeSQL(query)) {
			throw std::runtime_error("Failed to delete cart items via customer_id!");
		}
	}

	// Delete all cart items where the product in the cart references a specific supplier
	void deleteBySupplierID(int supplier_id) {
		std::string query = "DELETE FROM " + tableName + " WHERE product_id IN (SELECT product_id FROM " + productTableName + " WHERE supplier_id=" + std::to_string(supplier_id) + ");";

		if (!dbConn.executeSQL(query)) {
			throw std::runtime_error("Failed to delete cart items via supplier_id!");
		}
	}
};

#endif