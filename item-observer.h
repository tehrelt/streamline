#pragma once
#include <vector>
#include "item.h"

class ItemObserver {
public:
	virtual bool accept(Item* item) = 0;
};

class ItemSubject {
private:
	ItemObserver* _observer;

public:
	void attach(ItemObserver* observer) {
		_observer = observer;
	}
	void detach(ItemObserver* observer) {
		_observer = nullptr;
	}

	virtual bool send(Item* item) {
		return _observer->accept(item);
	};
};


