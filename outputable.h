#pragma once
#include "output-socket.h"

class Outputable {
protected:
	OutputSocket _output;

public: 

	void set_output_to(ItemReceiver* r) {
		_output.set_output_to(r);
	}
};

