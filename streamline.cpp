#include "streamline.h"
#include <iostream>
#include <sstream>
#include <thread>
#include "configuration.h"

int Streamline::CREATED = 0;

void Streamline::do_tick() {
    if (_queue.empty()) {
		return;
	}

	if (_out->is_overflow()) {
		_processed_ticks++;
		_ticks_in_wait++;
		on_tick.notify(TickInfo{
			_processed_ticks,
			_ticks_in_wait,
		});
		return;
	}

	try {
		_processed_ticks++;
		on_tick.notify(TickInfo{
			_processed_ticks,
			_ticks_in_wait,
		});

		auto item = _queue.pop();
		_out->recv(item);
		on_change.notify(_queue);
		on_push.notify(item);
	}
	catch (std::exception& e) {
		std::cout << "error with pop queue item" << e.what() << std::endl;
		on_change.notify(_queue);
	}
}

Streamline::Streamline(int capacity) : _queue(Queue<Item*>(capacity)), Runnable(), Model(CREATED) {
	CREATED++;
}

void Streamline::recv(Item* item) {
    _queue.push(item);
	on_change.notify(_queue);
	on_recv.notify(item);
}

bool Streamline::is_overflow() {
    return _queue.full();
}
