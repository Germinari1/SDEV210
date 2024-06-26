#ifndef Product_H
#define Product_H
#include <string>
#include <ostream>

class Product {
private:
	int product_id;
	int supplier_id;
	std::string p_name;
	std::string description;
	float price;
	int qty;
public:

	// Default constructor
	Product() : product_id(0), supplier_id(0), p_name(""), description(""), price(0.0f), qty(0) {}


	Product(
		int product_id,
		int supplier_id,
		std::string& p_name,
		std::string& description,
		float price,
		int qty
	) : product_id(product_id),
		supplier_id(supplier_id),
		p_name(p_name),
		description(description),
		price(price),
		qty(qty) {}

	const int getProductID() {
		return product_id;
	}

	const int getSupplierID() {
		return supplier_id;
	}

	const std::string& getName() {
		return p_name;
	}

	const std::string& getDescription() {
		return description;
	}

	const float getPrice() {
		return price;
	}

	const int getQuantity() {
		return qty;
	}

	friend std::ostream& operator<<(std::ostream& os, const Product& product) {
		os << "<Product ID(" << product.product_id << "), supplier_id(" << product.supplier_id << "), name(" << product.p_name << "), Price(" << product.price << "), Qty in stock(" << product.qty << ")/>";
		return os;
	}


	// product_id is always a positive integer for references that actually work 
	// so if it's zero, return true for if (!product)
	bool operator!() const {
		return product_id == 0;
	}

};

#endif