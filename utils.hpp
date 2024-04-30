#pragma once

#include <random>
#include <cmath>

template <typename container_type>
void initializeWithRandNumbers(container_type& da, size_t size, int b, int e) {
    std::random_device dev;
    std::mt19937 rng(dev());
    size_t maxVal = static_cast<size_t>(abs(b) + abs(e));
    std::uniform_int_distribution<std::mt19937::result_type> dist(0, maxVal);

    for (size_t i = 0; i < size; ++i) {
        da.push_back(static_cast<int>(dist(rng)) - static_cast<int>(maxVal / 2));
    }
}
