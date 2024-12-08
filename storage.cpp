#include "storage.h"
#include <iostream>
#include <sstream>

void Storage::set_stored(int stored) {
    _stored = stored;
    on_stored_change.notify(stored);
}

void Storage::recv(Item* item) {
    std::cout << "storage receiving " << item << std::endl;
    set_stored(_stored + 1);
}

bool Storage::is_overflow() {
    return false;
}
