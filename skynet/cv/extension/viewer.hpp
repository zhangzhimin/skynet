/*=============================================================================
The MIT License (MIT)

Copyright @ 2013 Zhang Zhimin

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

#include <skynet\cv\extension\bmp.hpp>

#include <boost\geometry\geometries\point.hpp>

namespace skynet{namespace viewer{

	template <typename C>
	cv::image_bgr render_points(const cv::image_bgr &image, const C &points, cv::bgr &v){
	//	static_assert(std::is_same<typename C::value_type, point2i>::value, "The point type is invalid");
		cv::image_bgr image2(image.width(), image.height());
		copy(image, image2);

		for (auto p: points){
			image2.set_value(nearest_neighbor(p), v);
		}

		return image2;
	}

	template <typename C>
	cv::image_bgr render_points(const cv::image_gray &image,  const C &points, cv::bgr &v){
		auto im_bgr = cv::gray2bgr(image);

		return render_points(im_bgr, points, v);
	}

}}