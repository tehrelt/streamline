#pragma once
#include <ostream>
class Item {
	int _id;
	int _process_time;
	static int TOTAL;

public:
	Item(int process_time);

	int id() const;
	int process_time() const;


	friend std::ostream& operator<<(std::ostream& s, const Item& item) {
		return s << "Item(id: " << item._id << " pt: " << item._process_time << ")";
	}

	friend std::ostream& operator<<(std::ostream& s, const Item* item) {
		return s << "Item(id: " << item->_id << " pt: " << item->_process_time << ")";
	}
};

