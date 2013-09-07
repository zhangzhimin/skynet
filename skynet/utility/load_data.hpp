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
#include <skynet/core/array.hpp>

namespace skynet{namespace utility{
    
    namespace detail{
        
        int read_inverse_int(std::ifstream &fs){
            int temp = 0;
            char *p = reinterpret_cast<char *>(&temp);
            fs.read(p+3, 1);
            fs.read(p+2, 1);
            fs.read(p+1, 1);
            fs.read(p, 1);
            return temp;
        }
        
    }


	std::array<ublas::matrix<double>, 3> get_iris(std::string filename){
		std::ifstream fs(filename);
        ASSERT(!fs.bad() && fs.good(), "Failed to open the file.");
		std::array<ublas::matrix<double>, 3> datas;
		for (int i = 0; i < 3; ++i){
			datas[i] = ublas::matrix<double>(4, 50);
			for (int j = 0; j < 50; ++j){
				char *line = new char[100];
				fs.getline(line, 100);
				std::vector<string> nums;
				string s_line(line);
				boost::split(nums, s_line, boost::is_any_of(","));
				delete[] line;
                ASSERT(nums.size() == 5, "Failed to parse the date.");
				for (int k = 0; k < 4; ++k){
					datas[i](k, j) = boost::lexical_cast<double, string>(nums[k]);
				}
			}
		}

		return datas;
	}

	
	std::vector<byte> get_digit_labels(const string &filename){
		std::ifstream fs(filename, std::ios::binary);
		int  magic_num = 0;
		fs.read((char *)&magic_num, 4);
		ASSERT(magic_num == 0x01080000, "The farmat error.");
		//int items_num = 0;
		//fs.read((char *)&items_num, 4);
        auto items_num = detail::read_inverse_int(fs);
		std::vector<byte> labels(items_num);

		fs.read((char *)&(labels[0]), items_num);
		fs.close();
		return labels;
	}


	std::vector<array2b> get_digit_images(const string &filename){
		std::ifstream fs(filename, std::ios::binary);
		int magic_num = 0;
		fs.read((char *)&magic_num, 4);
		ASSERT(magic_num == 0x03080000, "The farmat error.");
		//
		//int items_num = 0;
        auto items_num = detail::read_inverse_int(fs);
		//fs.read((char *)&items_num, 4);
		//int row_size = 0;
        auto row_size = detail::read_inverse_int(fs);
        auto col_size = detail::read_inverse_int(fs);
		//int col_size = 0;
		//fs.read((char *)&row_size, 4);
		//fs.read((char *)&col_size, 4);
		
		std::vector<array2b> images;
		for (size_t i = 0; i < items_num; ++i){
			array2b image(col_size, row_size);
			fs.read((char *)&(image[0]), col_size * row_size);
			images.push_back(image);
		}

		return images;
	}

}}
