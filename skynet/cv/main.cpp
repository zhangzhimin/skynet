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

#include <skynet\ublas.hpp>

using namespace skynet;

int main(){
	//cv::bmp_info_header info;
	//auto image = cv::image_cast<cv::image_gray>(cv::io::read_bmp("D:\\FaceDatabase\\MIT\\faces\\404201.bmp", info));
	
	//auto image_re = bilinear_interpolate(image, extent2(16,16));
	//mask<int, 2> mask = "[1; -1]";
	
	//auto edge = conv(lazy_cast(image, int(0)), mask);
	//auto temp = clone(edge);

	//cv::io::write_bmp(image_re, "D:/temp/resize16.bmp");

	
	//cv::read_bmp("C:\\Users\\÷æ√Ù\\Desktop\\image_0002.bmp", info);
	

	//multi_array<cv::bgr, 2> image(1023,1024);
	//cv::bgr pixel = {0,0,255};
	//cv::bgr pixel2 = {255, 0, 0};
	//std::fill(image.begin(), image.begin()+image.size()/2, pixel2);
	//std::fill(image.begin() + image.size()/2, image.end(), pixel);

	ublas::matrix<double> mat(2,2);
	mat(0, 0) = 2;
	mat(0, 1) = 0;
	mat(1, 0) = 0; 
	mat(1, 1) = 2;

	auto inv_mat = inverse(mat);
	

	return 0;
}