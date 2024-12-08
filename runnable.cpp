#include "runnable.h"
#include "thread"
#include "configuration.h"

void Runnable::run() {
    while (true) {
		do_tick();
		double dur = 1 / (double)sl_config.tickrate();
		std::this_thread::sleep_for(std::chrono::duration<double>(dur));
    }
}
