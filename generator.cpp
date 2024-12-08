#include "generator.h"
#include "lib.h"
#include <thread>
#include "sstream"
#include "configuration.h"
#include "logger.h"

int Generator::CREATED = 0;

void Generator::do_tick() {
	if (!is_working()) {
		return;
	}

	if (_state == Work) {
		if (_item == nullptr) {
			int process_time = rand_range(sl_config.min_pt(), sl_config.max_pt());
			_item = new Item(process_time);
			logger << "producing item(id: " << _item->id() << ", pt: " << _item->process_time() << ")" << std::endl;
		}
		else if (_progress >= _rate) {
			set_state(Idle);
		}

		set_progress(_progress + 1);
	}
	else if (_state == Idle) {
		if (_output.push(_item)) {
			_item = nullptr;
			set_progress(0);
			set_state(Work);
			return;
		}
	}
}

void Generator::set_state(GeneratorState new_state) {
	if (_state == Stop && new_state == Idle) { return; }
	_state = new_state;
	on_state_change.notify(new_state);
}

Generator::Generator() : Model(CREATED), Outputable() {
	set_state(Stop);
	set_rate(GENERATOR_RATE);
	CREATED++;
}

Generator::Generator(int rate) : Generator(){
	set_rate(rate);
}

void Generator::set_rate(int new_rate) {
	_rate = new_rate;
	on_rate_change.notify(new_rate);
}

void Generator::set_progress(int new_progress) {
	_progress = new_progress;
	on_progress_change.notify(new_progress);
}

void Generator::start() {
	std::cout << "starting up generator..." << std::endl;
	set_state(Work);
}

void Generator::stop() {
	std::cout << "shutdown generator..." << std::endl;
	set_state(Stop);
}
