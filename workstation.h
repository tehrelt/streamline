#pragma once
#include "inputable.h"
#include "outputable.h"
#include "model.h"
#include "runnable.h"
#include "observer.h"
#include "mutex"

enum WorkstationState {
	WS_Stop, WS_Idle, WS_Process
};

enum ProgressState {
	PS_Empty,
	PS_Processing,
	PS_Ready,
	PS_Gone	
};

typedef struct {
	ProgressState state;
	int item_id;
	int tick_elapsed;
	int tick_estimate;
} WorkstationProgress;

class Workstation : public Model, public ItemReceiver, public Outputable, public Runnable {
	Item* _item;
	WorkstationState _state;
	int _multiplier;
	int _remaining_time;
	std::mutex _m;
	int tick_passed = 0;


	void set_remaining_time(int rt) {
		_remaining_time = rt;
	}

	void set_state(WorkstationState new_state);

protected:
	void do_tick() override;

public:
	Workstation() : Model(CREATED) {
		_remaining_time = 0;
		_item = nullptr;
		_state = WorkstationState::WS_Stop;
		_multiplier = 1;
		CREATED++;
	}

	Workstation(int multiplier) : Workstation() {
		_multiplier = multiplier;
	}

	Subject<WorkstationState> on_state_change;
	Subject<WorkstationProgress> on_progress;

	int remaining_time() { return _remaining_time; }
	void start();
	void stop();


	static int CREATED;

	// Унаследовано через ItemReceiver
	void recv(Item* item) override;
	bool is_overflow() override;


	friend std::ostream& operator<<(std::ostream& s, const Workstation& ws) {
		return s << "[workstation#" << ws._id << " " << ws._state << "]";
	}


	friend std::ostream& operator<<(std::ostream& s, const Workstation* ws) {
		return s << "[workstation#" << ws->_id << " " << ws->_state << "]";
	}
};

