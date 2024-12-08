#include "item.h"

int Item::TOTAL = 0;

Item::Item(int process_time)
{
	_id = TOTAL;
	_process_time = process_time;

	++TOTAL;
}

int Item::id() const
{
	return _id;
}

int Item::process_time() const
{
	return _process_time;
}
