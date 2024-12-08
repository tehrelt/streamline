#pragma once
#include "input-socket.h"

class Inputable : public ItemObserver {
protected:
	InputSocket _input;
};

