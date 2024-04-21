#ifndef CartItem_H
#define CartItem_H
#include <string>
#include <ostream>

class CartItem {
private:
	// Currently not using customer_id or product_id, but since it's a representation of a row in CartItem 
	// table, we should include them for consistency. Then We also include extra columns such as p_name and price 
	// since those are useful when actually displaying a shopping cart.
	int customer_id;
	int product_id;
	int qty;
	std::string p_name;
	float price;

public:

	// Default constructor
	CartItem() : customer_id(0), product_id(0), qty(0), p_name(""), price(0.0f) {}

	CartItem(
		int customer_id,
		int product_id,
		int qty,
		std::string p_name,
		float price
	) : customer_id(customer_id), product_id(product_id), qty(qty), p_name(p_name), price(price) {}

	const int getCustomerID() {
		return customer_id;
	}

	const int getProductID() {
		return product_id;
	}

	const int getQty() {
		return qty;
	}

	friend std::ostream& operator<<(std::ostream& os, const CartItem& cartItem) {
		os << "<Product: " << cartItem.p_name << ", Price: $" << cartItem.price << ", Num in Cart: " << cartItem.qty << "/>";
		return os;
	}

	/*
	- customer_id is always a positive integer, so if it's zero it just lets us know it's not a valid cart item.

	NOTE: customer_id, product_id, and any other ID values are supposed to be positive integers, so if they're not then we know that they don't represent a valid row in a table. To keep things short we just check 
	  customer_id here, but you could also check product_id, etc.
	*/
	bool operator!() const {
		return customer_id == 0;
	}
};


#endif 
