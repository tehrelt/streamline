#include "workstation.h"
#include "chrono"
#include "thread"
#include "sstream"
#include "logger.h"
#include "configuration.h"

int Workstation::CREATED = 0;

void Workstation::do_tick() {
    if (_state == WS_Stop) {
        return;
    }

    if (_item == nullptr) {
        set_state(WS_Idle);
        on_progress.notify(WorkstationProgress{
            PS_Empty,
            0,
            0,
            0
        });
        return;
    }

    if (_remaining_time == 0) {
        if (_output.push(_item)) {
            on_progress.notify(WorkstationProgress{
                PS_Gone,
                _item->id(),
                0,
                _item->process_time() * _multiplier,
            });
            _item = nullptr;
            set_state(WS_Idle);
        }
        else {
            on_progress.notify(WorkstationProgress{
                PS_Ready,
                _item->id(),
                0,
                _item->process_time() * _multiplier,
            });
        }

        return;
    }

    auto required_ticks = _item->process_time() * _multiplier;
    on_progress.notify(WorkstationProgress{
        PS_Processing,
        _item->id(),
        required_ticks - _remaining_time,
        required_ticks,
	});

    set_remaining_time(_remaining_time - 1);
}

void Workstation::set_state(WorkstationState new_state) {
    if (_state != new_state) {
        on_state_change.notify(new_state);
    }
    _state = new_state;
}

void Workstation::start() {
    set_state(WS_Idle);
}

void Workstation::stop()
{
    set_state(WS_Stop);
}

void Workstation::recv(Item* item) {

    std::lock_guard<std::mutex> lock(_m);

    logger << this << " receives an " << item << std::endl;

    set_state(WS_Process);
    _item = item;
    _remaining_time = item->process_time() * _multiplier;
}

bool Workstation::is_overflow() {
    if (_item != nullptr) {
        ++_ticks_in_wait;
        return true;
    }
    return false;
}
