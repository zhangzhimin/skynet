#pragma once

#include <iterator>

namespace skynet{

    template <typename T>
    auto begin(T &c){
		return c.begin();
    }

	template <typename T>
	auto begin(const T &c) {
		return c.begin();
	}

	template <typename T>
	auto begin(T *c) {
		return c;
	}

	template <typename T>
	auto end(T &c, size_t length) {
		return c.begin() + length;
	}

	template <typename T>
	auto end(const T &c, size_t length) {
		return c.begin() + length;
	}

	template <typename T>
	auto end(T *c, size_t length) {
		return c + length;
	}
}
