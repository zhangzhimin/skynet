/*=============================================================================
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


namespace skynet{


	template <typename T>
	multi_array<T, 2> crop(const multi_array<T, 2> &source, index2  top_left, extent2 extent){
		multi_array<T, 2> target(extent);

		for (size_t row = 0; row < target.height(); ++row){
			for (size_t col = 0; col < target.width(); ++col){
				target(col, row) = source(top_left.x+col, top_left.y+row);
			}
		}

		return target;
	}

	template <typename T>
	std::vector<multi_array<T, 2>> grid_crop(const multi_array<T, 2> &source, extent2 extent){
		auto result_size = source.extent() / extent;
		
		std::vector<multi_array<T, 2>> images;
		for (size_t row = 0; row < result_size.y; ++row){
			for (size_t col = 0; col < result_size.x; ++col){
				index2 top_left = index2(col, row) * extent;
				auto crop_image = crop(source, top_left, extent);
				images.push_back(crop_image);
			}
		}

		return images;
	}

}
