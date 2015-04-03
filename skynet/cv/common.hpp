#pragma  once

#include <functional>
#include <skynet/config.hpp>

namespace skynet {
	namespace cv {
		
		template <typename Array, typename Neighbor, typename Func>
		auto extreme(const Array &data,  Neighbor neighbor, Func fun) {
			neighbor.attach(data.extent());
			return make_lazy_array(data.extent(), [neighbor, data, fun](size_t pos)->bool{
				if ((pos < -neighbor.min_offset()) || (pos >= data.size() - neighbor.max_offset())) {
					return false;
				}

				for (auto offset : neighbor) {
					if (!fun(data[pos], data[pos + offset]))	return false;
				}

				return true;
			});
		}
	}
}