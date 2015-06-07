#pragma once

#include <exception>
#include <skynet/config.hpp>

namespace skynet {
	class notimplement_exception : public std::logic_error {
	public:
		notimplement_exception() :std::logic_error("not implement") {}
	};
}