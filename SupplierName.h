#ifndef SupplierName_H
#define SupplierName_H
#include <string>
#include <ostream>

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

	const int getID() {
		return supplier_id;
	}

	const std::string& getName() {
		return s_name;
	}

	friend std::ostream& operator<<(std::ostream& os, const SupplierName& supplierName) {
		os << "<SupplierName supplier_id(" << supplierName.supplier_id << "), s_name(" << supplierName.s_name << ")/>";
		return os;
	}
};

#endif