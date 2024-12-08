#pragma once
#include "observer.h"
#include "output-socket.h"
#include "outputable.h"
#include "model.h"
#include "runnable.h"
#include "chrono"

enum GeneratorState {
	Work, Idle, Stop
};

class Generator : public Model, public Outputable, public Runnable {
	int _rate;
	GeneratorState _state;

	Item* _item;

	int _progress;

	void set_state(GeneratorState new_state);

protected:
	void do_tick() override;

public:
	Generator();
	Generator(int rate);
	GeneratorState state() const { return _state; }
	int rate() const { return _rate; }
	bool is_working() const { return _state != Stop; }
	void set_rate(int new_rate);
	void set_progress(int new_progress);

	Subject<int> on_rate_change;
	Subject<int> on_progress_change;
	Subject<GeneratorState> on_state_change;

	void start();
	void stop();

	static int CREATED;
};

