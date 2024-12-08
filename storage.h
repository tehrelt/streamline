#pragma once
#include "item-receiver.h"
#include "observer.h"

class Storage : public ItemReceiver
{
private:
	int _stored;

	void set_stored(int stored);

public:
	Storage() : _stored(0) {}
	int stored() { return _stored; }


	void recv(Item* item) override;
	bool is_overflow() override;

	Subject<int> on_stored_change;
};

