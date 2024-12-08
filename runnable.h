#pragma once
#include "observer.h"
#include "mutex"

typedef struct {
	int processed;
	int in_wait;
} TickInfo;

class Runnable {
protected:

	int _processed_ticks = 0;
	int _ticks_in_wait = 0;

	Runnable() {
		_processed_ticks = 0;
		_ticks_in_wait = 0;
	}

	virtual void do_tick() = 0;


public:
	Subject<TickInfo> on_tick;

	void run();

	void reset_stat() {
		_processed_ticks = 0;
		_ticks_in_wait = 0;
	}

	int processed_ticks () const{
		return _processed_ticks;
	}
	int ticks_in_wait() const {
		return _ticks_in_wait;
	}
	double avg_wait_time() const {
		return (double)_ticks_in_wait / (double)_processed_ticks;
	}
};

