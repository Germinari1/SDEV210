#ifndef Customer_H
#define Customer_H
#include <string>
#include <ostream>

class Customer {
private: 
	int customer_id;
	std::string fname;
	std::string lname;
	std::string email;
	int points;
public:
	Customer(int customer_id, std::string& fname, std::string& lname, std::string& email, int points) 
		: customer_id(customer_id), fname(fname), lname(lname), email(email), points(points) {}
	
	const int getCustomerID() {
		return customer_id;
	}
	const std::string& getFirstName() {
		return fname;
	}
	const std::string& getLastName() {
		return lname;
	}
	const std::string& getEmail() {
		return email;
	}
	const int getPoints() {
		return points;
	}

	friend std::ostream& operator<<(std::ostream& os, const Customer& customer) {
		os << "<Customer ID(" << customer.customer_id << "), fname(" << customer.fname << "), lname(" << customer.lname << "), email(" << customer.email << "), points(" << customer.points << ")/>";
		return os;
	}
};

#endif