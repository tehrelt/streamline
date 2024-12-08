#include "configuration.h"
#include <sstream>

std::string Configuration::list() {
	std::stringstream ss;
	for (auto pair : settings) {
		ss << pair.first << " = " << pair.second << "\n";
	}
	return ss.str();
}
