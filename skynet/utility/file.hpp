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

//the mac and linux  not support
//#include <filesystem>
#include <boost/filesystem.hpp>

#include <skynet/config.hpp>

namespace skynet{

	namespace sys = boost::filesystem;

	template <typename Fun>
	void load_directory(const string &directory, Fun f){
		sys::path base_path(directory);
		if (!sys::is_directory(base_path)){
			THROW_EXCEPTION(std::runtime_error("It's invalid directory path."));
		}

		sys::recursive_directory_iterator it_end;
		for (sys::recursive_directory_iterator it(base_path); it != it_end; ++it){
			f(it->path().string());
		}
	}
    
    std::vector<string> load_directory(const string &directory){
		sys::path base_path(directory);
		if (!sys::is_directory(base_path)){
			THROW_EXCEPTION(std::runtime_error("It's invalid directory path."));
		}
        
        std::vector<string>  files;
		sys::recursive_directory_iterator it_end;
		for (sys::recursive_directory_iterator it(base_path); it != it_end; ++it){
			files.push_back(it->path().string());
		}
	}


}
