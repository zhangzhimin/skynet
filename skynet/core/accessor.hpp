#pragma once

#include <skynet/core/array.hpp>

namespace skynet {

	template <typename Array>
	class slice_accessor {

	};

	/*template <typename Array>
	class vertexes_accessor{
	public:
		typedef typename Array::value_type				value_type;
		typedef typename Array::reference				reference;
		typedef typename Array::const_reference			const_reference;
		static const size_t dim = Array::dim;

		vertexes_accessor(Array data): _data(data) {}	

		reference operator[](size_t pos) {
			point<int, dim>	index;
			auto extent = _data.extent();

			for (size_t d = 0; d < dim; ++d) {
				size_t tmp = pos & 0x01;
				pos = pos >> 1;
				if (tmp == 0) {
					index[d] = 0;
				}
				else {
					index[d] = extent[d]-1;
				}				
			}

			return _data(index);
		}

		size_t	size() const {
			return size_t(1) << Array::dim;
		}

		

	private:
		Array			_data;
		
	};*/
}