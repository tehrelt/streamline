#pragma once
#include "item.h"

class ItemReceiver {
public:
	virtual void recv(Item* item) = 0;
	virtual bool is_overflow() = 0;
};

