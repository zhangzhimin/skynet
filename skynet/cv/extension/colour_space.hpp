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

#include <skynet\utility\algorithm.hpp>
#include <skynet\cv\extension\image.hpp>

namespace skynet{namespace cv{

	image_gray rgb2gray(const image_rgb &source){
		image_gray target(source.extent());
		transform(source, target, [](const rgb &e) {  return byte(0.27*e.r + 0.71*e.g + 0.07*e.b); });
		return target;
	}

	image_gray bgr2gray(const image_bgr &source){
		image_gray target(source.extent());
		transform(source, target, [](const bgr &e) { return byte(0.27*e.r + 0.71*e.g + 0.07*e.b); });
		return target;
	}

}}