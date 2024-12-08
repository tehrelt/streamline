#pragma once
#include "item-receiver.h"
#include "item-producer.h"
#include "queue.h"
#include "runnable.h"
#include "observer.h"
#include "model.h"

class Streamline : public ItemReceiver, public ItemProducer, public Runnable, public Model {
private:
	Queue<Item*> _queue;

protected:
	void do_tick() override;

public:
	Streamline(int capacity);

	void set_cap(int new_cap) {
		_queue.set_cap(new_cap);
		on_change.notify(_queue);
	}

	Queue<Item*>& queue() { return _queue; }

	Subject<Queue<Item*>&> on_change;
	Subject<Item*> on_recv;
	Subject<Item*> on_push;

	// Унаследовано через ItemReceiver
	void recv(Item* item) override;
	bool is_overflow() override;
	static int CREATED;
};

