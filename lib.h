#pragma once
#include <random>

int rand_range(int lower, int upper) {
	std::srand(std::time(nullptr));
	return lower + std::rand() % (upper - lower + 1);
}
