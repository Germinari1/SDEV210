#ifndef Supplier_H
#define Supplier_H
#include <string>
#include <ostream>

class Supplier {
private:
	int supplier_id;
	std::string s_name;
	std::string description;
	std::string email;
	std::string address;
public:

	Supplier() : supplier_id(0), s_name(""), description(""), email(""), address("") {}


	Supplier(
		int supplier_id,
		std::string s_name,
		std::string description,
		std::string email,
		std::string address
	) : supplier_id(supplier_id),
		s_name(s_name),
		description(description),
		email(email),
		address(address) {}

	const int getSupplierID() {
		return supplier_id;
	}

	const std::string& getDescription() {
		return description;
	}

	const std::string& getEmail() {
		return email;
	}

	const std::string& getAddress() {
		return address;
	}

	/*
	- Custom print method for printing out a supplier

	NOTE: We omit the description since it can be lengthy.
	*/
	friend std::ostream& operator<<(std::ostream& os, const Supplier& supplier) {
		os << "<Supplier ID(" << supplier.supplier_id << "), name(" << supplier.s_name << "), email(" << supplier.email << "), address(" << supplier.address << ")/>";
		return os;
	}

	// !supplier is true, when supplier_id = 0
	bool operator!() const {
		return supplier_id == 0;
	}

};

#endif