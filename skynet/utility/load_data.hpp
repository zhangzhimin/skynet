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

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include <skynet/config.hpp>

namespace skynet{


	std::array<ublas::matrix<double>, 3> get_iris(std::string filename){
		std::ifstream fs(filename, std::ios::binary);
		std::array<ublas::matrix<double>, 3> datas;
		for (int i = 0; i < 3; ++i){
			datas[i] = ublas::matrix<double>(4, 50);
			for (int j = 0; j < 50; ++j){
				char *line = new char[100];
				fs.getline(line, 100);
				std::vector<string> nums;
				boost::split(nums, string(line), boost::is_any_of(","));
				delete[] line;
				for (int k = 0; k < 4; ++k){
					datas[i](k, j) = boost::lexical_cast<double, string>(nums[k]);
				}
			}
		}

		return datas;
	}

}
