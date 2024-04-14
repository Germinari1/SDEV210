#ifndef SupplierName_H
#define SupplierName_H
#include <string>
#include <ostream>

/*
- Since Supplier_Name and Supplier_Info are going to separate tables, we should
	treat them as such. Meaning they should have different managers that will 
	handle the respective table operations.
*/

class SupplierName {
private:
	int supplier_id;
	std::string s_name;
public:
	SupplierName(
		int supplier_id, 
		std::string& s_name
	) : supplier_id(supplier_id), 
		s_name(s_name) {}

	const int getSupplierID() {
		return supplier_id;
	}

	const std::string& getSupplierName() {
		return s_name;
	}

	friend std::ostream& operator<<(std::ostream& os, const SupplierName& supplierName) {
		os << "<Supplier ID(" << supplierName.supplier_id << "), s_name(" << supplierName.s_name << ")/>";
		return os;
	}
};






#endif