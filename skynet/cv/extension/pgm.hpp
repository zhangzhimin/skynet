#pragma once

#include <filesystem>
#include <boost\algorithm\string.hpp>
#include <boost\lexical_cast.hpp>

#include <skynet\utility\exception.hpp>
#include <skynet\cv\extension\image.hpp>

namespace skynet{namespace cv{
	namespace sys = std::tr2::sys;

	namespace io{

		image_gray read_pgm(const string &filepath){
			sys::path path(filepath);
			ASSERT(sys::is_regular_file(path), "not regular file");

			ASSERT(path.extension() == ".pgm", "The read pgm should has .pgm extension.");
			std::ifstream  is(filepath, std::ios::binary);
			char *style = new char[10];
			is.getline(style, 10);

			char *size = new char[100];
			while(true){
				is.getline(size, 100);
				if (size[0] != '#')	break;
			}
			std::vector<string> strings;
			boost::split(strings, string(size), boost::is_any_of(" "));
			ASSERT(strings.size() == 2, "Failed, there is comment in pgm, solve it.");
			image_gray image(boost::lexical_cast<int, string>(strings[0]), boost::lexical_cast<int, string>(strings[1])); 
			if (string(style) == "P5"){
				auto buffer = image.buffer();
				//
				int32_t	max_value = 0;
				is.getline((char *)&max_value, 4); // the max gray value, unusefull

				is.read((char *)(buffer.get()), image.size() * sizeof(image_type::gray8));
				is.close();

				//free memory
				delete[] style;
				delete[] size;
				return image;
			}else{

				is.close();
				delete[] style;
				delete[] size;
				THROW_EXCEPTION(not_implement("Just P5 style is supported"));
			}


		}

		image_rgb read_ppm(const string &filepath){
			sys::path path(filepath);
			ASSERT(sys::is_regular_file(path), "not regular file");

			ASSERT(path.extension() == ".pgm", "The read pgm should has .pgm extension.");
			std::ifstream  is(filepath, std::ios::binary);
			char *style = new char[10];
			is.getline(style, 10);

			char *size = new char[100];
			while(true){
				is.getline(size, 100);
				if (size[0] != '#')	break;
			}
			std::vector<string> strings;
			boost::split(strings, string(size), boost::is_any_of(" "));
			ASSERT(strings.size() == 2, "Failed, there is comment in pgm, solve it.");
			image_rgb image(boost::lexical_cast<int, string>(strings[0]), boost::lexical_cast<int, string>(strings[1])); 
			if (string(style) == "P6"){
				int32_t	max_value = 0;
				is.getline((char *)&max_value, 4); // the max gray value, unusefull

				auto buffer = image.buffer();
				is.read((char *)(buffer.get()), image.size() * sizeof(image_type::rgb));
				is.close();

				//free memory
				delete[] style;
				delete[] size;
				return image;
			}

			if (string(style) == "P3"){
				int32_t max_value = 0;
				is.getline((char *)&max_value, 4);

				auto buffer = image.buffer();


				is.close();
				delete[] style;
				delete[] size;
				THROW_EXCEPTION(not_implement("Just P6 style is supported"));
			}

			delete[] style;
			delete[] size;
			THROW_EXCEPTION(std::runtime_error("The file may has problem."));
		}
	}

}}