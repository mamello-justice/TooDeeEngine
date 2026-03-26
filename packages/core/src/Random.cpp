#include "Random.h"

#include <cmath>
#include <cstdlib>

float Random::between(const float& min, const float& hi) {
    return min + fmodf((float)std::rand(), hi - min + 1);
}

int Random::between(const int& min, const int& max) {
    return min + std::rand() % (max - min + 1);
}

int Random::sign() {
    return ((double)std::rand() / (RAND_MAX)) > 0.5 ? 1 : -1;
}
