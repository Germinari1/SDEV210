#ifndef OrderItem_H
#define OrderItem_H
#include <ostream>

class OrderItem {
private:
	int order_item_id;
	int transaction_id;
	int product_id;
	int qty;
public:
	OrderItem(
		int order_item_id,
		int transaction_id,
		int product_id,
		int qty
	) : order_item_id(order_item_id),
		transaction_id(transaction_id),
		product_id(product_id),
		qty(qty) {}

	int getOrderItemID() {
		return order_item_id;
	}

	int getTransactionID() {
		return transaction_id;
	}

	int getProductID() {
		return product_id;
	}

	
	friend std::ostream& operator<<(std::ostream& os, const OrderItem& orderItem) {
		os << "<Order Item ID: " << orderItem.order_item_id << ", transaction_id: " << orderItem.transaction_id << ", product_id: " << orderItem.product_id << ", qty:" << orderItem.qty << "/>";
		return os;
	}
};

#endif