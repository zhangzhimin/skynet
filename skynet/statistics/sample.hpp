#pragma once

#include <skynet/config.hpp>

namespace skynet{ namespace statistics{

	template <typename P, typename >
	class sample : std::vector<P>{
	public:
		typedef P				feature_vector_type;

	};


}}
