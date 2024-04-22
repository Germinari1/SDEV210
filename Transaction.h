#ifndef Transaction_H
#define Transaction_H
#include <string>

class Transaction {
private:
	int transaction_id;
	int customer_id;
	float total;
	std::string order_date;

public:
	Transaction(
		int transaction_id,
		int customer_id,
		float total,
		std::string order_date
	) : transaction_id(transaction_id),
		customer_id(customer_id),
		total(total),
		order_date(order_date) {}


	int getTransactionID() {
		return transaction_id;
	}
	int getCustomerID() {
		return customer_id;
	}
	float getTotal() {
		return total;
	}
	std::string getOrderDate() {
		return order_date;
	}

	friend std::ostream& operator<<(std::ostream& os, const Transaction& transaction) {
		os << "<Transaction ID(" << transaction.transaction_id << "), customer_id(" << transaction.customer_id << "), total(" << transaction.total << "), order_date(" << transaction.order_date << ")/>";
		return os;
	}
};


#endif