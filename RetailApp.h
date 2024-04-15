#ifndef RetailApp_H
#define RetailApp_H
#include <iostream>
#include <string>
#include <vector>

// Include managers for managing different tables
#include "CustomerManager.h"
#include "SupplierManager.h"
#include "SupplierNameManager.h"

// Include object representations of rows in our database
#include "Customer.h"
#include "Supplier.h"
#include "SupplierName.h"


class RetailApp {
private:
	CustomerManager& customerManager;
	SupplierManager& supplierManager;
	SupplierNameManager& supplierNameManager;


public:
	RetailApp(
		CustomerManager& customerManager, 
		SupplierManager& supplierManager,
		SupplierNameManager& supplierNameManager
		) : 
		customerManager(customerManager),
		supplierManager(supplierManager), 
		supplierNameManager(supplierNameManager) {}


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
		int customer_id;
		do {
			std::cout << "Enter a ID customer you want to update: ";
			std::cin >> customer_id;
			if (std::cin.fail()) {
				std::cout << "Invalid choice, please enter a number!" << std::endl;
				std::cin.clear(); // Clear the error flag
				std::cin.ignore(64, '\n'); // Clear input buffer up to 64 characters or until newline is encountered
			}
			else {
				break; // Exit the loop if input is valid
			}
		} while (true);


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
		3. Use a setter on updatedCustomer to reflect new change. Then we can print updatedCustomer
			out to the console to show the user their new changes.
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
		int customer_id;
		do {
			std::cout << "Enter a ID customer you want to update: ";
			std::cin >> customer_id;
			if (std::cin.fail()) {
				std::cout << "Invalid choice, please enter a number!" << std::endl;
				std::cin.clear(); // Clear the error flag
				std::cin.ignore(64, '\n'); // Clear input buffer up to 64 characters or until newline is encountered
			}
			else {
				break; // Exit the loop if input is valid
			}
		} while (true);

		customerManager.deleteCustomer(customer_id);

		std::cout << "Customer with ID '" + std::to_string(customer_id) + "' was successfully deleted!" << std::endl;

	}

	// Handles prompting input for customer_id and searching for said customer in database
	void handleGetCustomerByID() {
		// Prompt input on the ID of the customer that we will search for; ensure it's a valid integer. 
		int customer_id;
		do {
			std::cout << "Enter a ID customer you want to update: ";
			std::cin >> customer_id;
			if (std::cin.fail()) {
				std::cout << "Invalid choice, please enter a number!" << std::endl;
				std::cin.clear(); // Clear the error flag
				std::cin.ignore(64, '\n'); // Clear input buffer up to 64 characters or until newline is encountered
			}
			else {
				break; // Exit the loop if input is valid
			}
		} while (true);

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
		int supplier_id;
		do {
			std::cout << "Enter a ID supplier you want to update: ";
			std::cin >> supplier_id;
			if (std::cin.fail()) {
				std::cout << "Invalid choice, please enter a number!" << std::endl;
				std::cin.clear(); // Clear the error flag
				std::cin.ignore(64, '\n'); // Clear input buffer up to 64 characters or until newline is encountered
			}
			else {
				break; // Exit the loop if input is valid
			}
		} while (true);


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

	void handleDeleteSupplier() {
		// Prompt input on the ID of the customer they want to update; ensure it's a valid integer. 
		int supplier_id;
		do {
			std::cout << "Enter the ID of the supplier you want to delete: ";
			std::cin >> supplier_id;
			if (std::cin.fail()) {
				std::cout << "Invalid choice, please enter a number!" << std::endl;
				std::cin.clear(); // Clear the error flag
				std::cin.ignore(64, '\n'); // Clear input buffer up to 64 characters or until newline is encountered
			}
			else {
				break; // Exit the loop if input is valid
			}
		} while (true);

		supplierManager.deleteSupplier(supplier_id);;

		std::cout << "Customer with ID '" + std::to_string(supplier_id) + "' was successfully deleted!" << std::endl;
	}
	

	void handleGetSupplierByID() {
		// Prompt input on the ID of the customer that we will search for; ensure it's a valid integer. 
		int supplier_id;
		do {
			std::cout << "Enter the ID of the supplier you want to display: ";
			std::cin >> supplier_id;
			if (std::cin.fail()) {
				std::cout << "Invalid choice, please enter a number!" << std::endl;
				std::cin.clear(); // Clear the error flag
				std::cin.ignore(64, '\n'); // Clear input buffer up to 64 characters or until newline is encountered
			}
			else {
				break; // Exit the loop if input is valid
			}
		} while (true);

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
};


#endif