#pragma once
#include "item-receiver.h"
class ItemProducer
{
protected:
	ItemReceiver* _out;

public:
	void set_output_to(ItemReceiver* receiver) {
		_out = receiver;
	}
};

