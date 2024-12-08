#pragma once
#include "item-observer.h"

class InputSocket : public ItemSubject {
	int _passed;

public:
	InputSocket() : _passed(0) {}

	int passed() const { return _passed; }
	bool send(Item* item) override {
		if (ItemSubject::send(item)) {
			_passed++;
			return true; 
		}
		return false;
	}
};

