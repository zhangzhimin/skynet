#pragma once

#include <iostream>
#include <skynet\core\array.hpp>
#include <skynet\utility\exception.hpp>

namespace skynet{
	
	template <typename T, typename size_t D>
	std::ostream &operator<<(std::ostream &os, const std::array<T, D> &mat){
		os.write(static_cast<char *>(&(*mat.begin())), mat.size());
		return os;
	}

	template <typename T, typename size_t D>
	std::istream &operator>>(std::istream &is, typename std::array<T, D> &mat){
		is.read(static_cast<char *>(&(*mat.begin())), mat.size());
		return is;
	}

	
    template <typename C>
    void write2raw(const C &mat, const string &file){
        std::ofstream os(file, std::ios::binary);
		
		for(int i = 0; i < mat.size(); ++i){
			auto temp = mat[i];
			os.write((char *)&temp, sizeof(typename C::value_type));
		}
    }

	template <typename C>
	void read4raw(C &container, const string &filepath){
		std::ifstream is(filepath, std::ios:: binary);

		for (int i = 0; i < container.size(); ++i){
			is.read((char *)&(container[i]), sizeof(typename C::value_type));
		}
	}

	template <typename T, typename size_t D>
	void writemtx(const multi_array<T, D> &mat, const string &file){
		std::ofstream os(file, std::ios::binary);
		if (!os.good()){
			throw bad_stream(file);
		}

		os.write((char *)(mat.buffer().get()), mat.size() * sizeof(T));
	}

	template <typename T, typename size_t D>
	void readmtx(multi_array<T, D> &mat, const string &file){
		std::ifstream is(file, std::ios::binary);
		if (!is.good()) {
			throw bad_stream(file);
		}

		is.read((char *)(mat.buffer().get()), mat.size() * sizeof(T));
	}
}