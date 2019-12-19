#pragma once

#include <algorithm>
#include <random>
#include <set>
#include <stdexcept>

using std::find;
using std::set;
using std::advance;

namespace common_util {

    template<typename T>
    inline bool contains(const set<T>& container, const T &value) {
        return find(container.begin(), container.end(), value) != container.end();
    }

    template<typename T>
    inline bool not_contains(const set<T>& container, const T &value) {
        return find(container.begin(), container.end(), value) == container.end();
    }

    template<typename T>
    inline T get_random(set<T>& container, random_device random) {
        if (container.empty()) throw out_of_range("Container is empty");

        const auto index = uniform_int_distribution<size_t>(0, container.size() - 1)(random);

        auto iterator = container.begin();
        advance(iterator, index);

        return *iterator;
    }
}