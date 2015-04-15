#pragma once

#include <skynet/core/adaptor_types.hpp>

namespace skynet {

	template <typename ArrayType>
	class interpolate_array {
	public:
		static_assert(ArrayType::dim == 2, "not support 3d array, needs completation");

		typedef typename ArrayType::value_type			value_type;
		static const size_t dim = ArrayType::dim;
		//typedef value_type 								reference;
		//typedef value_type 								const_reference;

		typedef point<ptrdiff_t, dim>					index_type;
		typedef point<size_t, dim>						extent_type;

		interpolate_array(ArrayType data) : _data(data) {}

		template <typename T>
		value_type operator()(point<T, dim> index)	const {
			auto x = index[0];
			auto y = index[1];
			auto x0 = std::floor(x);
			auto y0 = std::floor(y);
			auto f00 = _data(x0, y0);
			auto f01 = _data(x0, y0+1);
			auto f10 = _data(x0 + 1, y0);
			auto f11 = _data(x0 + 1, y0 + 1);

			auto ff0 = f00 + (x - x0) * (f01 - f00);
			auto ff1 = f10 + (x - x0) * (f11 - f10);

			return ff0 + (y - y0) * (ff1 - ff0);
		}

		extent_type extent() const { return _data.extent(); }

	private:
		ArrayType			_data;
	};


	template <typename Array>
	auto interpolate(Array data) {
		return interpolate_array<Array>(data);
	}


}