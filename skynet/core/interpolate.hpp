*=============================================================================
The MIT License (MIT)

Copyright @ 2013 by Zhang Zhimin 
p3.1415@qq.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
=============================================================================*/

#pragma once


#include <skynet/core/array.hpp>
#include <skynet/utility/algorithm.hpp>

namespace skynet{


	template <typename M>
	multi_array<typename M::value_type, 2> bilinear_interpolate(const M &mat, const extent2 &new_size){
		static_assert(M::dim == 2, "The image should be 2D.");
		typedef typename M::value_type					value_type;

		multi_array<typename M::value_type, 2> mat_re(new_size);
		auto resolution = static_cast<point2d>(mat.extent()) / static_cast<point2d>(new_size);
		resolution -= point2d(numeric_limits<double>::epsilon()); // prevent out range

		for_each_index2(mat_re, [&](const index2 &index){
			point2d pos = resolution * static_cast<point2d>(index);
			index2 top_left  = static_cast<index2>(pos);
			index2 top_right = top_left;
			top_right.x += 1;
			index2 bottom_right = top_left + index2::unit;
			index2 bottom_left = top_left;
			bottom_left.y += 1;

			point2d shift = pos - static_cast<point2d>(top_left);

			auto x_top= (1.0-shift.x)*mat.get_value(top_left) + shift.x*mat.get_value(top_right);
			auto x_bottom = (1.0-shift.x)*mat.get_value(bottom_left) + shift.x*mat.get_value(bottom_right);
			auto cur_value = (1.0-shift.y)*x_top + shift.y*x_bottom;
			mat_re.set_value(index, value_type(cur_value));
		});

		return mat_re;
	}


}
