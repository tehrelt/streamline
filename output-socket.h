#pragma once
#include "item-producer.h"
#include "item-observer.h"
#include <iostream>

class OutputSocket : public ItemProducer {
public:
	OutputSocket() {}

	bool push(Item* item) {
		if (_out == nullptr) {
			std::cout << "receiver is not set" << std::endl;
			return false;
		}

		if (_out ->is_overflow()) {
			std::cout << "receiver is overflowed" << std::endl;
			return false;
		}

		_out->recv(item);
		return true;
	}

	ItemReceiver* receiver() { return _out; }
};

