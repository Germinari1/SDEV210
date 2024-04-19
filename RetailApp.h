#ifndef RetailApp_H
#define RetailApp_H
#include <iostream>
#include <string>
#include <vector>

// Include managers for managing different tables
#include "CustomerManager.h"
#include "SupplierManager.h"
#include "ProductManager.h"

// Include object representations of rows in our database
#include "Customer.h"
#include "Supplier.h"
#include "SupplierName.h"
#include "Product.h"

// Include utility function for getting numeric input
#include "getValidNumericInput.h"


class RetailApp {
private:
	CustomerManager& customerManager;
	SupplierManager& supplierManager;
	ProductManager& productManager;

public:
	RetailApp(
		CustomerManager& customerManager, 
		SupplierManager& supplierManager,
		ProductManager& productManager
		) : 
		customerManager(customerManager),
		supplierManager(supplierManager), 
		
		productManager(productManager) {}


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
				std::cout << "6. Exit Customer Menu" << std::endl;
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
					std::cout << "Exiting Customer Menu..." << std::endl;
					break;
				default:
					std::cout << "Customer Menu: Invalid choice. Please enter a number between 1 and 6." << std::endl;
				}
			}
			catch (const std::exception& ex) {
				std::cerr << "Customer Menu Error: " << ex.what() << std::endl;
			}
		} while (choice != 6);
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
		// Prompt input on the ID of the customer they want to update; ensure it's a valid integer. 
		int customer_id = getValidNumericInput<int>("Enter a ID customer you want to update: ");
		
		// Get the customer by ID; if customer not found, hten 
		Customer updatedCustomer = customerManager.getCustomerByID(customer_id);
		
		// Prompt input for the attribute they want to update the customer on
		int choice;
		do {
			std::cout << "Selected Customer: " << updatedCustomer << std::endl;
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
		// Prompt input on the ID of the customer they want to update; ensure it's a valid integer. 
		int customer_id = getValidNumericInput<int>("Enter a ID customer you want to update: ");

		// Fetch customer to see if they exist; if not error will be thrown
		Customer customer = customerManager.getCustomerByID(customer_id);

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

		// Display list of customers
		std::cout << "Available Customers:" << std::endl;
		for (size_t i = 0; i < customers.size(); ++i) {
			std::cout << i + 1 << ". " << customers[i] << std::endl;
		}
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
		// Prompt input on the ID of the supplier they want to update; ensure it's a valid integer. 
		int supplier_id = getValidNumericInput<int>("Enter a ID supplier you want to update: ");
		
		// Get the customer by ID; if customer not found, then error will be thrown
		Supplier updatedSupplier = supplierManager.getSupplierByID(supplier_id);

		// Prompt input for attribute to be updated
		int choice;
		do {
			std::cout << "Selected Supplier: " << updatedSupplier << std::endl;
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
		// Prompt input on the ID of the customer they want to update; ensure it's a valid integer. 
		int supplier_id = getValidNumericInput<int>("Enter the ID of the supplier you want to delete: ");
		
		// Fetch supplier by id; this also serves to check if supplier_id links to an actual supplier. If not it throws error.
		Supplier supplier = supplierManager.getSupplierByID(supplier_id);

		// supplier_id is valid, so delete all products associated with supplier
		productManager.deleteBySupplierID(supplier_id);

		// Then delete the supplier, which will also delete supplier name
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

		// There are suppliers, so display them.
		std::cout << "Available Suppliers:" << std::endl;
		for (size_t i = 0; i < suppliers.size(); ++i) {
			std::cout << i + 1 << ". " << suppliers[i] << std::endl;
		}
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

		// Prompt input on the ID of the product they want to update; ensure it's a valid integer. 		
		int product_id = getValidNumericInput<int>("Enter a ID of the product you want to update: ");

		// Get the product via its ID; if not found an error will be thrown
		Product product = productManager.getProductByID(product_id);

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
		int product_id = getValidNumericInput<int>("Enter ID of product you want to delete: ");

		// Fetch product and verify it exists in the database
		Product product = productManager.getProductByID(product_id);

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
		std::cout << "Available Products:" << std::endl;
		for (size_t i = 0; i < products.size(); ++i) {
			std::cout << i + 1 << ". " << products[i] << std::endl;
		}

	}


	// ********** Functions for Shopping-Cart related operations ********** 
	void handleCartMenu() {
		int choice;
		do {
			try {
				// Display main menu and prompt input
				std::cout << "Cart Menu: " << std::endl;
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
					break;
				case 2:
					break;
				case 3:
					
					break;
				case 4:
					
					break;
				case 5:
					std::cout << "Exiting Cart Menu..." << std::endl;
					break;
				default:
					std::cout << "Cart Menu: Invalid choice. Please enter a number between 1 and 6." << std::endl;
				}
			}
			catch (const std::exception& ex) {
				std::cerr << "Cart Menu Error: " << ex.what() << std::endl;
			}
		} while (choice != 5);
	}

	/*
	+ Handles adding a new product to the shopping cart.

	1. User should be able to give the product_id, it should be an existing product.
	2. If they pick a product that's already in the shopping cart, then we should stop
		execution and tell the user.
	3. User should be able to specify the quantity of the product that is being 
		put in the cart, it has to be within limits of quantity in stock.
	4. If the user picks a product that is out of stock, we should stop 
		the execution there.

	
	
	*/
	void handleAddToCart() {
		const int product_id = getValidNumericInput<int>("Enter id of the product you want to add to cart: ");

		// 

		// Verify that product is valid
		Product product = productManager.getProductByID(product_id);


	}


};

#endif