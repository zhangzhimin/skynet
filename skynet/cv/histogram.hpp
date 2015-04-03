#pragma once

#include <skynet/config.hpp>


namespace skynet {
	namespace cv {

		//class histogram {
		//public:
		//	histogram(size_t length): _data(length){}


		//private:
		//	std::vector<size_t>	_data;
		//};

		template <size_t length, typename Data>
		auto simple_histogram(const Data &data) {
			array<size_t, length> his;
			std::fill(his.begin(), his.end(), 0);
			
			for (auto &v : data) {
				++his[size_t(v)];
			}

			return his;
		}
	}
}