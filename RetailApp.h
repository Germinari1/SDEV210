#ifndef RetailApp_H
#define RetailApp_H
#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include <map>

// Include managers for managing different tables
#include "CustomerManager.h"
#include "SupplierManager.h"
#include "ProductManager.h"
#include "CartItemManager.h"
#include "TransactionManager.h"
#include "OrderItemManager.h"

// Include object representations of rows in our database
#include "Customer.h"
#include "Supplier.h"
#include "Product.h"
#include "CartItem.h"
#include "Transaction.h"

// Include utility function for getting numeric input
#include "utils.h"


class RetailApp {
private:
	CustomerManager& customerManager;
	SupplierManager& supplierManager;
	ProductManager& productManager;
	CartItemManager& cartItemManager;
	TransactionManager& transactionManager;
	OrderItemManager& orderItemManager;


	/*
	- Customer ID of the currently selected customer. We'll use this to 
	know which shopping cart we are managing.

	- currentCustomer: Customer object that's displayed on the cart item menu. We'll
	use a Customer object since, in the cart items screen, we'll need to be able 
	to change the 'points' attribute so it's value is in-sync with the database. Right now this is only used to display the current customer in the cart items menu.
	*/
	int currentCustomerID = 0;
	Customer currentCustomer;

public:
	RetailApp(
		CustomerManager& customerManager, 
		SupplierManager& supplierManager,
		ProductManager& productManager,
		CartItemManager& cartItemManager,
		TransactionManager& transactionManager,
		OrderItemManager& orderItemManager
		) : 
		customerManager(customerManager),
		supplierManager(supplierManager), 
		productManager(productManager),
		cartItemManager(cartItemManager),
		transactionManager(transactionManager),
		orderItemManager(orderItemManager) {}


	// ********** Functions for customer related operations ********** 	

	// Displays and starts the customer menu
	void handleCustomerMenu() {
		int choice;

		do {
			try {
				// Display main menu and prompt input
				std::cout << "Customer Menu: " << std::endl;
				std::cout << "1. Create Customer" << std::endl;
				std::cout << "2. Update Customer" << std::endl;
				std::cout << "3. Delete Customer" << std::endl;
				std::cout << "4. Get Customer By ID" << std::endl;
				std::cout << "5. Display all customers" << std::endl;
				std::cout << "6. Select a current customer" << std::endl;
				std::cout << "7. Exit Customer Menu" << std::endl;
				std::cout << "Please enter a number to continue: ";
				std::cin >> choice;

				if (std::cin.fail()) {
					std::cout << "Invalid input. Please enter a number!" << std::endl;
					std::cin.clear(); // Clear the error flag
					std::cin.ignore(64, '\n'); // Clear the input buffer
					continue; // Restart the loop
				}

				switch (choice) {
				case 1:
					handleCreateCustomer();
					break;
				case 2:
					handleUpdateCustomer();
					break;
				case 3:
					handleDeleteCustomer();
					break;
				case 4:
					handleGetCustomerByID();
					break;
				case 5:
					displayAllCustomers();
					break;
				case 6:
					handleSelectCustomer();
					break;
				case 7:
					std::cout << "Exiting Customer Menu..." << std::endl;
					break;
				default:
					std::cout << "Customer Menu: Invalid choice. Please enter a number between 1 and 6." << std::endl;
				}
			}
			catch (const std::exception& ex) {
				std::cerr << "Customer Menu Error: " << ex.what() << std::endl;
			}
		} while (choice != 7);
	}

	// Prompts input for creating a customer 
	void handleCreateCustomer() {		
		std::string fname, lname, email;
		int points = 0;
		
		// Ignore any new line characters in the buffer so that getline works.
		std::cin.ignore();
		std::cout << "Enter customer's first name: ";
		std::getline(std::cin, fname);

		std::cout << "Enter customer's last name: ";
		std::getline(std::cin, lname);
			
		std::cout << "Enter customer's email: ";
		std::getline(std::cin, email);

		// Do database operation to create customer
		Customer customer = customerManager.createCustomer(fname, lname, email, points);

		std::cout << "Success, new customer: " << customer << std::endl;
	}
	
	/*
	- Prompt input for updating a customer. 

	NOTE: if customer with customer_id isn't found, then an error is thrown, and then caught 
		in the try/catch defined in handleCustomerMenu, effectively taking the user back to the 
		customer menu if the ID didn't correlate to a customer in the database.
	*/
	void handleUpdateCustomer() {

		// Fetch all customers
		std::vector<Customer> customers = customerManager.getAllCustomers();
		if (customers.size() == 0) {
			std::cout << "No customers available to update!" << std::endl;
			return;
		}

		// Prompt user to pick customer to update from a paginated menu
		Customer customer = selectPaginatedItems<Customer>(customers, 5, "Customer Menu List", "Enter list number for customer we're updating");

		// IF they didn't pick a customer, stop function early
		if (!customer) {
			return;
		}

		// extract customer_id
		const int customer_id = customer.getCustomerID();

		// Prompt input for the attribute they want to update the customer on
		int choice;
		do {
			std::cout << "Selected Customer: " << customer << std::endl;
			std::cout << "1. First Name" << std::endl;
			std::cout << "2. Last Name" << std::endl;
			std::cout << "3. Email" << std::endl;
			std::cout << "Pick an attribute to update: ";
			std::cin >> choice;
			
			if (std::cin.fail()) {
				std::cout << "Invalid choice, please enter a number!" << std::endl;
				std::cin.clear(); // Clear the error flag
				std::cin.ignore(64, '\n'); // Clear input buffer up to 64 characters or until newline is encountered
				continue; // Restart the loop
			}
		} while (choice < 1 || choice > 3);

		// Prompt input for the value whether it be first name, last name, or email.
		std::string value;
		std::cin.ignore(); // clear new line so our getlines work in our switch statement.

		/*
		- Depending on their choice:
		1. Prompt input for value
		2. Then do database operation to update that customer with the new value
		*/
		switch (choice) {
		case 1:
			std::cout << "Enter new first name: ";
			std::getline(std::cin, value);
			customerManager.updateFirstName(customer_id, value);
			break;
		case 2:
			std::cout << "Enter new last name: ";
			std::getline(std::cin, value);
			customerManager.updateLastName(customer_id, value);
			break;
		case 3:
			std::cout << "Enter new email: ";
			std::getline(std::cin, value);
			customerManager.updateEmail(customer_id, value);
			break;
		}

		std::cout << "Successfully, updated customer!" << std::endl;
	}

	// Handles prompting input to delete a customer
	void handleDeleteCustomer() {

		// Fetch all customers
		std::vector<Customer> customers = customerManager.getAllCustomers();
		if (customers.size() == 0) {
			std::cout << "No customers available to delete!" << std::endl;
			return;
		}

		// Prompt user to pick customer to delete from a paginated menu
		Customer customer = selectPaginatedItems<Customer>(customers, 5, "Customer Menu List", "Enter list number for customer we're removing");

		// IF they didn't pick a customer, stop function early
		if (!customer) {
			return;
		}

		// Extra customer_id from 
		const int customer_id = customer.getCustomerID();

		/*
		- If the customer we are deleting is also the currently selected customer
		- reset currentCustomerID back to 0 to indicate no customer is currently selected.
		*/
		if (customer.getCustomerID() == currentCustomerID) {
			currentCustomerID = 0;
		}
		
		// Delete all cart items that reference the customer that's going to be deleted
		cartItemManager.deleteByCustomerID(customer_id);

		// Nullify 'customer_id' column in transactions table for all rows that reference the deleted customer_id
		transactionManager.nullifyCustomerID(customer_id);

		// Delete customer, and on success display that the customer was successfully deleted.
		customerManager.deleteCustomer(customer_id);
		std::cout << "Customer Deleted: " << customer << std::endl;
	}

	// Handles prompting input for customer_id and searching for said customer in database
	void handleGetCustomerByID() {
		// Prompt input on the ID of the customer that we will search for; ensure it's a valid integer. 
		int customer_id = getValidNumericInput<int>("Enter a ID customer you want to see: ");
		
		// Fetch the customer and print them
		Customer customer = customerManager.getCustomerByID(customer_id);
		std::cout << "Found Customer: " << customer << std::endl;
	}

	// Handles displaying all rows in customers table (if any)
	void displayAllCustomers() {
		std::vector<Customer> customers = customerManager.getAllCustomers();

		// If no customers, display message and stop function execution early.
		if (customers.size() == 0) {
			std::cout << "No customers to display!" << std::endl;
			return;
		}

		// Call function to display paginated list of customers
		navigatePaginatedItems<Customer>(customers, 5, "Customer Menu List");
	}

	// Updates the current customer we're managing
	void handleSelectCustomer() {
		std::vector<Customer> customers = customerManager.getAllCustomers();
		if (customers.size() == 0) {
			std::cout << "No customers available to select!" << std::endl;
			return;
		}

		// Prompt user to pick customer to select from a paginated menu
		Customer customer = selectPaginatedItems<Customer>(customers, 5, "Customer Menu List", "Enter list number for customer we're selecting");

		// IF they didn't pick a customer, stop function early
		if (!customer) {
			return;
		}

		// Extra customer_id from 
		const int customer_id = customer.getCustomerID();

		// Update currentCustomerID
		currentCustomerID = customer_id;

		// Print out message displaying the currently selected customer
		std::cout << "Current customer: " << customer << std::endl;
	}

	// ********** Functions for supplier related operations ********** 	

	// Displays and starts the supplier menu
	void handleSupplierMenu() {
		int choice;

		do {
			try {
				// Display main menu and prompt input
				std::cout << "Supplier Menu: " << std::endl;
				std::cout << "1. Create Supplier" << std::endl;
				std::cout << "2. Update Supplier" << std::endl;
				std::cout << "3. Delete Supplier" << std::endl;
				std::cout << "4. Get Supplier By ID" << std::endl;
				std::cout << "5. Display all Suppliers" << std::endl;
				std::cout << "6. Exit Supplier Menu" << std::endl;
				std::cout << "Please enter a number to continue: ";
				std::cin >> choice;

				if (std::cin.fail()) {
					std::cout << "Invalid input. Please enter a number!" << std::endl;
					std::cin.clear(); // Clear the error flag
					std::cin.ignore(64, '\n'); // Clear the input buffer
					continue; // Restart the loop
				}

				switch (choice) {
				case 1:
					handleCreateSupplier();
					break;
				case 2:
					handleUpdateSupplier();
					break;
				case 3:
					handleDeleteSupplier();
					break;
				case 4:
					handleGetSupplierByID();
					break;
				case 5:
					displayAllSuppliers();
					break;
				case 6:
					std::cout << "Exiting Supplier Menu..." << std::endl;
					break;
				default:
					std::cout << "Supplier Menu: Invalid choice. Please enter a number between 1 and 6." << std::endl;
				}
			}
			// Here you'll catch all of the errors thrown by the managers' methods
			catch (const std::exception& ex) {
				std::cerr << "Supplier Menu Error: " << ex.what() << std::endl;
			}
		} while (choice != 6);
	}
	
	// Handles prompting input for creating a supplier
	void handleCreateSupplier() {		
		// Ignore to newline
		std::cin.ignore();

		// Prompt input for supplier information
		std::string s_name, description, email, address;
		std::cout << "Enter supplier name: ";
		std::getline(std::cin, s_name);

		std::cout << "Enter supplier description: ";
		std::getline(std::cin, description);

		std::cout << "Enter supplier email: ";
		std::getline(std::cin, email);

		std::cout << "Enter supplier's address: ";
		std::getline(std::cin, address);

		// Attempt to save supplier to the database
		Supplier supplier = supplierManager.createSupplier(s_name, description, email, address);

		std::cout << "Success, new supplier: " << supplier << std::endl;
	}

	// Handles prompting input for updating the attributes of a supplier
	void handleUpdateSupplier() {

		// Fetch all suppliers
		std::vector<Supplier> suppliers = supplierManager.getAllSuppliers();
		if (suppliers.size() == 0) {
			std::cout << "No suppliers available to update!" << std::endl;
			return;
		}

		// Prompt user to pick supplier to update from a paginated menu
		Supplier supplier = selectPaginatedItems<Supplier>(suppliers, 5, "Supplier Menu List", "Enter list number for supplier we're updating");

		// IF they didn't pick a supplier, stop function early
		if (!supplier) {
			return;
		}

		// extract supplier_id
		const int supplier_id = supplier.getSupplierID();

		// Prompt input for attribute to be updated
		int choice;
		do {
			std::cout << "Selected Supplier: " << supplier << std::endl;
			std::cout << "1. Supplier Name" << std::endl;
			std::cout << "2. Description" << std::endl;
			std::cout << "3. Email" << std::endl;
			std::cout << "4. Address" << std::endl;
			std::cout << "Pick an attribute to update: ";
			std::cin >> choice;

			if (std::cin.fail()) {
				std::cout << "Invalid choice, please enter a number!" << std::endl;
				std::cin.clear(); // Clear the error flag
				std::cin.ignore(64, '\n'); // Clear input buffer up to 64 characters or until newline is encountered
				continue; // Restart the loop
			}
		} while (choice < 1 || choice > 4);


		// Prompt input for the value, whether it be the supplier name, description etc.
		std::string value;
		std::cin.ignore(); // clear new line so our getlines work in our switch statement.

		/*
		- Depending on their choice:
		1. Prompt input for value
		2. Then do database operation to update that customer with the new value
		3. Use a setter on updatedCustomer to reflect new change. Then we can print updatedCustomer
			out to the console to show the user their new changes.
		*/
		switch (choice) {
		case 1:
			std::cout << "Enter new supplier name: ";
			std::getline(std::cin, value);
			supplierManager.updateName(supplier_id, value);
			break;
		case 2:
			std::cout << "Enter new description: ";
			std::getline(std::cin, value);
			supplierManager.updateDescription(supplier_id, value);
			break;
		case 3:
			std::cout << "Enter new email: ";
			std::getline(std::cin, value);
			supplierManager.updateEmail(supplier_id, value);
			break;
		case 4:
			std::cout << "Enter new address: ";
			std::getline(std::cin, value);
			supplierManager.updateAddress(supplier_id, value);
			break;
		}

		std::cout << "Successfully, updated supplier!" << std::endl;
	}

	// Prompts input for deleting a supplier 
	void handleDeleteSupplier() {
		// Fetch all suppliers
		std::vector<Supplier> suppliers = supplierManager.getAllSuppliers();
		if (suppliers.size() == 0) {
			std::cout << "No suppliers available to delete!" << std::endl;
			return;
		}

		// Prompt user to pick supplier to update from a paginated menu
		Supplier supplier = selectPaginatedItems<Supplier>(suppliers, 5, "Supplier Menu List", "Enter list number for supplier we're deleting");

		// IF they didn't pick a supplier, stop function early
		if (!supplier) {
			return;
		}

		// extract supplier_id
		const int supplier_id = supplier.getSupplierID();

		// Delete all cart items that reference a product, where the product has a supplier_id of the deleted supplier
		cartItemManager.deleteBySupplierID(supplier_id);

		// Nullify all order items that reference products that the supplier associated with
		orderItemManager.nullifyProductIDBySupplierID(supplier_id);

		// Delete all products associated with supplier
		productManager.deleteBySupplierID(supplier_id);

		// Then delete the supplier, which will also delete the supplier name
		supplierManager.deleteSupplier(supplier_id);
		std::cout << "Supplier Deleted: " << supplier << std::endl;
	}
	
	// Prompts input for supplier ID and then displaying that supplier
	void handleGetSupplierByID() {
		// Prompt input on the ID of the supplier that we will search for; ensure it's a valid integer. 
		int supplier_id = getValidNumericInput<int>("Enter the ID of the supplier you want to display: ");
		
		// Fetch the customer and print them
		Supplier supplier = supplierManager.getSupplierByID(supplier_id);

		/*
		- Print out the found supplier. We've customized the output stream for the Supplier class to exclude the description,
		which could be lengthy. Here, we first display the supplier's details using the custom ostream operator, and then
		print the description separately to ensure all information is visible to the user.
		*/
		std::cout << "Found Supplier: " << std::endl;
		std::cout << supplier << std::endl;
		std::cout << "Description: " << supplier.getDescription() << std::endl;	
	}

	// Display all suppliers in the database
	void displayAllSuppliers() {
		// Get all suppliers from the database
		std::vector<Supplier> suppliers = supplierManager.getAllSuppliers();

		// Check to see if vector is empty; if so then stop execution early
		if (suppliers.size() == 0) {
			std::cout << "No suppliers to display!" << std::endl;
			return;
		}

		navigatePaginatedItems<Supplier>(suppliers, 5, "Supplier Menu List");
		
	}

	// ********** Functions for Product related operations ********** 	

	// Handles displaying and managing the product menu
	void handleProductMenu() {
		int choice;
		do {
			try {
				// Display main menu and prompt input
				std::cout << "Product Menu: " << std::endl;
				std::cout << "1. Create Product" << std::endl;
				std::cout << "2. Update Product" << std::endl;
				std::cout << "3. Delete Product" << std::endl;
				std::cout << "4. Get Product By ID" << std::endl;
				std::cout << "5. Display all Products" << std::endl;
				std::cout << "6. Exit Product Menu" << std::endl;
				std::cout << "Please enter a number to continue: ";
				std::cin >> choice;

				if (std::cin.fail()) {
					std::cout << "Invalid input. Please enter a number!" << std::endl;
					std::cin.clear(); // Clear the error flag
					std::cin.ignore(64, '\n'); // Clear the input buffer
					continue; // Restart the loop
				}

				switch (choice) {
				case 1:
					handleCreateProduct();
					break;
				case 2:
					handleUpdateProduct();
					break;
				case 3:
					handleDeleteProduct();
					break;
				case 4:
					handleGetProductByID();
					break;
				case 5:
					displayAllProducts();
					break;
				case 6:
					std::cout << "Exiting Product Menu..." << std::endl;
					break;
				default:
					std::cout << "Product Menu: Invalid choice. Please enter a number between 1 and 6." << std::endl;
				}
			}
			// Here you'll catch all of the errors thrown by the managers' methods
			catch (const std::exception& ex) {
				std::cerr << "Product Menu Error: " << ex.what() << std::endl;
			}
		} while (choice != 6);
	}

	// Prompts input for creating a new product
	void handleCreateProduct() {
		// Ignore to newline so our getlines work
		std::cin.ignore();

		// Prompt input for product information
		std::string p_name, description;
		int supplier_id, qty;
		float price;

		std::cout << "Enter product name: ";
		std::getline(std::cin, p_name);

		std::cout << "Enter description: ";
		std::getline(std::cin, description);

		// Prompt input for supplier_id, qty in stock, and price of the product
		supplier_id = getValidNumericInput<int>("Enter the ID of the supplier selling this product: ");
		qty = getValidNumericInput<int>("Enter the qty in stock: ");
		price = getValidNumericInput<float>("Enter price of the product: ");

		/*
		- The reason we validate the input out here is because we want to do input checks before using the 
		resources to query our database. And since your productManager doesn't have a way to check the validity of the supplier_id, we'll lift the logic up to the RetailApp.
		*/
		productManager.validateProductName(p_name);
		productManager.validateDescription(description);
		productManager.validatePrice(price);
		productManager.validateQty(qty);

		// Ensure supplier_id links to an actual supplier
		const bool isValid = supplierManager.isValidSupplierID(supplier_id);
		if (!isValid) {
			throw std::runtime_error("Supplier with supplier_id(" + std::to_string(supplier_id) + ") wasn't found!");
		}

		// Data is good so create product and print it out
		Product product = productManager.createProduct(supplier_id, p_name, description, price, qty);
		std::cout << "Success, new product: " << product << std::endl;
	}

	// Prompts input for updating an existing product 
	void handleUpdateProduct() {

		// Fetch all products
		std::vector<Product> products = productManager.getAllProducts();
		if (products.size() == 0) {
			std::cout << "No products available to update!" << std::endl;
			return;
		}

		// Prompt user to pick product to update from a paginated menu
		Product product = selectPaginatedItems<Product>(products, 5, "Product Menu List", "Enter list number for product we're updating");

		// IF they didn't pick a product, stop function early
		if (!product) {
			return;
		}

		// extract supplier_id
		const int product_id = product.getProductID();

		// Prompt input for attribute to be updated
		int choice;
		do {
			std::cout << "Selected Product: " << product << std::endl;
			std::cout << "1. Product Name" << std::endl;
			std::cout << "2. Description" << std::endl;
			std::cout << "3. Price" << std::endl;
			std::cout << "4. Quantity in stock" << std::endl;
			std::cout << "Pick an attribute to update: ";
			std::cin >> choice;

			if (std::cin.fail()) {
				std::cout << "Invalid choice, please enter a number!" << std::endl;
				std::cin.clear(); // Clear the error flag
				std::cin.ignore(64, '\n'); // Clear input buffer up to 64 characters or until newline is encountered
				continue; // Restart the loop
			}
		} while (choice < 1 || choice > 4);


		// clear new line so our getlines work in some of the switch statement's branches .
		std::cin.ignore(); 

		std::string p_name;
		std::string description;
		float price;
		int qty;
		
		switch (choice) {
		case 1:
			std::cout << "Enter new product name: ";
			std::getline(std::cin, p_name);
			productManager.updateName(product_id, p_name);
			break;
		case 2:
			std::cout << "Enter new description: ";
			std::getline(std::cin, description);
			productManager.updateDescription(product_id, description);
			break;
		case 3:
			price = getValidNumericInput<float>("Enter new price: ");
			productManager.updatePrice(product_id, price);
			break;
		case 4:
			qty = getValidNumericInput<int>("Enter new qty in stock: ");
			productManager.updateQuantity(product_id, qty);
			break;
		}

		std::cout << "Successfully, updated product!" << std::endl;
	}

	// Prompts input for deleting an existing product
	void handleDeleteProduct() {

		// Fetch all products
		std::vector<Product> products = productManager.getAllProducts();
		if (products.size() == 0) {
			std::cout << "No products available to delete!" << std::endl;
			return;
		}

		// Prompt user to pick product to update from a paginated menu
		Product product = selectPaginatedItems<Product>(products, 5, "Product Menu List", "Enter list number for product we're deleting");

		// IF they didn't pick a product, stop function early
		if (!product) {
			return;
		}

		// extract supplier_id
		const int product_id = product.getProductID();

		// Delete all cart items that reference the product being deleted
		cartItemManager.deleteByProductID(product_id);

		// Nullify all order items that reference the product_id
		orderItemManager.nullifyProductID(product_id);

		// Then delete the product
		productManager.deleteProduct(product_id);
		std::cout << "Deleted Product: " << product << std::endl;
	}

	// Handles prompting input for product_id and displaying detailed product information 
	void handleGetProductByID() {
		// Prompt input on the ID of the customer that we will search for; ensure it's a valid integer. 
		int product_id = getValidNumericInput<int>("Enter the ID of the product you want to display: ");

		// Fetch the customer and print them
		Product product = productManager.getProductByID(product_id);

		std::cout << "Found Product: " << std::endl;
		std::cout << product << std::endl;
		std::cout << "Description: " << product.getDescription() << std::endl;
	}

	// Handles displaying all products in the database
	void displayAllProducts() {
		std::vector<Product> products = productManager.getAllProducts();

		// Check to see if vector is empty; if so then stop execution early
		if (products.size() == 0) {
			std::cout << "No products to display!" << std::endl;
			return;
		}

		// There are suppliers, so display them.
		navigatePaginatedItems<Product>(products, 5, "Product Menu List");
	}

	// ********** Functions for Shopping-Cart related operations ********** 
	/*
	+ Handles displaying choices for the cart menu

	NOTE: currentCustomerID has to be defined and reference a currently existing customer before proceeding any further into the cart menu. This is because all of the operations are going to be related to a given customer's shopping cart.
	*/
	void handleCartMenu() {
		int choice;

		// Verify that currentCustomerID is defined and references a customer in the database
		if (!currentCustomerID) {
			std::cout << "Please select a customer first, before managing cart items!" << std::endl;
			return;
		}

		currentCustomer = customerManager.getCustomerByID(currentCustomerID);

		do {
			try {
				// Display main menu and prompt input
				std::cout << "Cart Menu: " << std::endl;
				std::cout << "Current Customer: " << currentCustomer << std::endl;
				std::cout << "1. Add product to cart" << std::endl;
				std::cout << "2. Remove product from cart" << std::endl;
				std::cout << "3. View all cart items" << std::endl;
				std::cout << "4. Update cart item quantity" << std::endl;
				std::cout << "5. Check out current cart" << std::endl;
				std::cout << "6. Exit Cart Menu" << std::endl;
				std::cout << "Please enter a number to continue: ";
				std::cin >> choice;

				if (std::cin.fail()) {
					std::cout << "Invalid input. Please enter a number!" << std::endl;
					std::cin.clear(); // Clear the error flag
					std::cin.ignore(64, '\n'); // Clear the input buffer
					continue; // Restart the loop
				}

				switch (choice) {
				case 1:
					handleAddToCart();
					break;
				case 2:
					handleRemoveFromCart();
					break;
				case 3:
					displayCustomerCart();
					break;
				case 4:	
					handleUpdateCartItem();
					break;
				case 5:
					handleCheckout();
					break;
				case 6:
					std::cout << "Exiting Cart Menu..." << std::endl;
					break;
				default:
					std::cout << "Cart Menu: Invalid choice. Please enter a number between 1 and 6." << std::endl;
				}
			}
			catch (const std::exception& ex) {
				std::cerr << "Cart Menu Error: " << ex.what() << std::endl;
			}
		} while (choice != 6);
	}

	// Handles input for adding a new product to cart
	void handleAddToCart() {
		// Only fetch products are in stock and available; 
		// If there are no items available to be put in the cart, return early
		std::vector<Product> products = productManager.getAvailableProducts();
		if (products.size() == 0) {
			std::cout << "No available items to add to cart!" << std::endl;
			return;
		}

		// prompt user to potentially pick a product to add to cart
		Product product = selectPaginatedItems<Product>(products, 5, "Product Menu", "Enter list number of product you're adding");
		
		// If user didn't pick a product to be added to cart
		if (!product) {
			return;
		}

		/*
		- Get the number in stock
		- At this point we can determine that the product's quantity in stock is greater than 0 (at least 1).
			As well, we'll set a rule that the maximum of a particular product you can add to cart will be 10.
			However if the number in stock (productQty) is less than 10, then the maximum will be set to the 
			amount remaining in stock.
		- For integer quantity within min and max
		*/
		int qty = handleInputCartQty(product);

		/*
		- Good values so attempt to create a new cart item. Now this will also check if the user already has 
		the product in their cart, and if so an error will be thrown.

		NOTE: createCartItem() is different from other create functions in other managers as it doesn't 
			return anything. This is because if we really wanted to return a CartItem object, we'd have to 
			do another database query. Instead of doing that, we can construct the CartItem object here 
			in this function since we have enough information to do that.
		*/
		cartItemManager.createCartItem(currentCustomerID, product.getProductID(), qty);
		CartItem cartItem(currentCustomerID, product.getProductID(), qty, product.getName(), product.getPrice());
		std::cout << "Added to Cart: " << cartItem << std::endl;
	}

	// Handles input for removing a product from the cart 
	void handleRemoveFromCart() {

		// Get a customer's cart items
		std::vector<CartItem> cartItems = cartItemManager.getCustomerCartItems(currentCustomerID);
		if (cartItems.size() == 0) {
			std::cout << "Nothing to remove, since no items are in cart!" << std::endl;
			return;
		}

		// prompt user to potentially pick a product to remove from cart
		CartItem cartItem = selectPaginatedItems<CartItem>(cartItems, 5, "Cart Item Menu", "Enter list number for cart item you're removing");

		// If user didn't pick a cartItem to be removed, then stop function execution early.
		if (!cartItem) {
			return;
		}

		// Cart item was picked for removal, so remove it from database; then print out the cartItem that the user removed.
		cartItemManager.deleteCartItem(currentCustomerID, cartItem.getProductID());
		std::cout << "Removed Cart Item!" << std::endl;
	}

	// Handles input for updating an item's quantity when in cart
	void handleUpdateCartItem() {
		// Get a customer's cart items
		std::vector<CartItem> cartItems = cartItemManager.getCustomerCartItems(currentCustomerID);
		if (cartItems.size() == 0) {
			std::cout << "Nothing to update, since no items are in cart!" << std::endl;
			return;
		}

		// prompt user to potentially pick a product to remove from cart
		CartItem cartItem = selectPaginatedItems<CartItem>(cartItems, 5, "Cart Item Menu", "Enter list number for cart item you're updating");

		// If user didn't pick a cartItem to be removed, then stop function execution early.
		if (!cartItem) {
			return;
		}

		/*
		- Fetch product associated with cart item
		- Check that the product is still in stock. If it not, this just means the user has something in their cart, it was still in stock when they added it to their cart, but now it's out of stock. Of course this will be addressed at checkout.
		- Now do the same thing and prompt input for the quantity of the product they want. We'll use the we used for adding 
		*/
		Product product = productManager.getProductByID(cartItem.getProductID());
		const int productQty = product.getQuantity();
		if (productQty == 0) {
			std::cout << "Product is actually out of stock!" << std::endl;
			return;
		}
		int qty = handleInputCartQty(product);

		// Now update the quantity for the cart item and display success message
		cartItemManager.updateCartItem(currentCustomerID, cartItem.getProductID(), qty);
		std::cout << "Updated Cart Item!" << std::endl;
	}

	/*
	- Helper function for handling prompting input for the qty of the product that the customer is going to put in the cart.
	Here we decide a user at maximum can only have 10 of one product in their cart, but if the actual quantity in stock for 
	that product is less than 10, the max is the remaining product
	*/
	int handleInputCartQty(Product product) {
		int productQty = product.getQuantity();
		int min = 1;
		int max = 10;
		if (productQty < max) {
			max = productQty;
		}
		int qty = getValidRangeInput<int>("Enter quantity of product you're adding to cart: ", min, max);
		return qty;
	}

	// Handles displaying paginated list of all cart items
	void displayCustomerCart() {
		std::vector<CartItem> cartItems = cartItemManager.getCustomerCartItems(currentCustomerID);

		if (cartItems.size() == 0) {
			std::cout << "No Items in Cart!" << std::endl;
			return;
		}

		navigatePaginatedItems<CartItem>(cartItems, 5, "Customer Cart");
	}

	// Handles checking out the cart
	void handleCheckout() {

		// Fetch cart items for the customer
		std::vector<CartItem> cartItems = cartItemManager.getCustomerCartItems(currentCustomerID);
		if (cartItems.size() == 0) {
			std::cout << "Cannot checkout since no items in Cart!" << std::endl;
			return;
		}

		// Get vector of product ID values for products in the customer's cart.
		std::vector<int> productIDs;
		for (size_t i = 0; i < cartItems.size(); i++) {
			int product_id = cartItems[i].getProductID();
			productIDs.push_back(product_id);
		}

		/*
		- productQuantityMap: Get a map of key product_id and value quantity in stock for that product
		- newProductQuantities: Vector of tuples in form (product_id, qty), where qty is going to be the updated number in stock for the product (num in stock - num in cart)
		*/
		std::map<int, int> productQuantityMap = productManager.getProductQuantities(productIDs);
		std::vector<std::tuple<int, int>> newProductQuantities;


		// Total in your cart.
		float total = 0;

		// Check if each cart item's qty (num in cart) doesn't exceed the product's qty (num in stock)
		for (size_t i = 0; i < cartItems.size(); i++) {
			int product_id = cartItems[i].getProductID();
			int numInCart = cartItems[i].getQty();
			int numInStock = productQuantityMap[product_id];
			if (numInCart > numInStock) {
				std::cout << "Product '" << cartItems[i].getProductName() << "' has a quantity (" << numInCart
					<< ") in your cart that exceeds the available stock (" << numInStock << ")" << std::endl;
				return;
			}

			// Calculate the new product qty 
			int updatedNumInStock = numInStock - numInCart;

			// Add the total for that item
			total += cartItems[i].getTotal();

			// Push a tuple into newProductQuantities
			newProductQuantities.push_back(std::make_tuple(product_id, updatedNumInStock));

		}


		Customer customer = customerManager.getCustomerByID(currentCustomerID);
		int customerPoints = customer.getPoints();
		int usedPoints = 0;


		// Printing out their cart total
		std::cout << "Your Cart Total: $" << total << "!" << std::endl;

		/*
		- If the customer has points we'll prompt them for the amount of points
		they want to spend, 0 if none. Then print out their new cart total.
		*/
		if (customerPoints > 0) {
			std::cout << "You have '" << customerPoints << "' points. One point is one dollar off your total!" << std::endl;
			usedPoints = getValidRangeInput<int>("Enter number of points you're using (0, if none): ", 0, customerPoints);
		}

		/*
		- If they spent points, calculate and display their new total!

		NOTE: If they used enough points to make the total negative, then just make the total 0 dollars. However this does not account for the extra points that they lose due to them making things negative.
		*/
		if (usedPoints > 0) {
			total -= usedPoints;
			if (total < 0) {
				total = 0;
			}
			std::cout << "New Cart Total: $" << total << "!" << std::endl;
		}

		/*
		- Now calculate the earned point of points the customer gets on their 
		final total. For example, if total is $250 and they spent 100 points, the 
		final total is now $150. We'll then use this total to calculate the points.
		As a result, they earn 15 points rather than 25 points, because in reality they only spent $150.
		*/
		int earnedPoints = calculatePointsFromCost(total);



		// Have confirmation that the user wants to checkout their cart
		char choice = promptYesOrNo("Do you want to confirm your checkout? (y/n): ");
		if (choice == 'n') {
			std::cout << "Cancelling checkout, returning to cart item menu..." << std::endl;
			return;
		}

		/*
		- Quantities are within limits, and loop calculated cart total. Shopping cart is ready for checkout. Now create new row in transaction table.
		*/
		Transaction transaction = transactionManager.createTransaction(currentCustomerID, total);
		int transaction_id = transaction.getTransactionID();

		/*
		- Create the individual rows in the order items table that are associated with that transaction. 

		- orderItems: Vector of tuples in form (transaction_id, product_id, qty) that is used to insert the order items in the database.

		
		*/
		std::vector<std::tuple<int, int, int>> orderItems;
		for (size_t i = 0; i < cartItems.size(); i++) {
			int product_id = cartItems[i].getProductID();
			int qty = cartItems[i].getQty();
			orderItems.push_back(std::make_tuple(transaction_id, product_id, qty));
		}
		orderItemManager.batchCreateOrderItem(orderItems);

		// Update the quantities in the products table to reflect new quantities after the customer checked out their cart; it seems batchUpdate could be the issue
		productManager.batchUpdateProductQty(newProductQuantities);

		// Now just clear the customer's cart; just delete all cart items associated with the customer who just checked out their cart.
		cartItemManager.deleteByCustomerID(currentCustomerID);

		
		/*
		- Calculate the total points the customer should have after checking out and update their points column
		
		NOTE: In the most extreme case, the customer spends all of their points, so customerPoints and usedPoints cancel out, making it so updatedCustomerPoints = earnedPoints. Then earnedPoints >= 0, so updatedCustomerPoints won't ever be negative, so you don't need a conditional check here.
		*/
		
		int updatedCustomerPoints = customerPoints + earnedPoints - usedPoints;
		customerManager.updatePoints(currentCustomerID, updatedCustomerPoints);


		/*
		- Update the points value on the currentCustomer object as well to be in sync with the database

		NOTE: This allows us to correctly show off the customer's current point value, without having to fetch the customer from the database.
		*/
		currentCustomer.setPoints(updatedCustomerPoints);

		
		// Display that transaction went successfully
		std::cout << "Successful checkout, transaction: " << transaction << std::endl;
	}


	/*
	- Calculates the points earned by a customer from the total cost of their cart.
	Let's say for every 10 dollars, the customer earns one point

	1. Do total / 10, which will likely yield a decimal. SO 149.99 / 10 is 
		14.99 points.
	2. Now round down to nearest integer to get 14 points earned. Then return it.
	*/
	int calculatePointsFromCost(float total) {
		int points = static_cast<int>(total / 10);
		return points;
	}



};

#endif