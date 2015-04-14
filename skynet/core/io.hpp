#pragma once

#include <iostream>
#include <skynet/core/array.hpp>
#include <skynet/utility/exception.hpp>

namespace skynet{namespace io{
	
	template <typename T, size_t D>
	std::ostream &operator<<(std::ostream &os, const std::array<T, D> &mat){
		os.write(static_cast<char *>(&(*mat.begin())), mat.size());
		return os;
	}

	template <typename T, size_t D>
	std::istream &operator>>(std::istream &is, typename std::array<T, D> &mat){
		is.read(static_cast<char *>(&(*mat.begin())), mat.size());
		return is;
	}

	
	template <typename T, size_t D>
	void writemtx(const multi_array<T, D> &mat, const string &file){
		std::ofstream os(file, std::ios::binary);
		if (!os.good()){
			throw std::runtime_error("can't open the file "+ file);
		}

		os.write((char *)(mat.buffer().get()), mat.size() * sizeof(T));
	}

	template <typename M>
	void writemtx(const array_expression<M> &me, const string &file) {
		auto mat = me();
		multi_array<typename M::value_type, M::dim> raw_data(mat.extent());
		copy(mat, raw_data);
		writemtx(raw_data, file);
	}

	template <typename T, size_t D>
	void readmtx(multi_array<T, D> &mat, const string &file){
		std::ifstream is(file, std::ios::binary);
		if (!is.good()) {
			throw std::runtime_error("can't open the file "+ file);
		}

		is.read((char *)(mat.buffer().get()), mat.size() * sizeof(T));
	}
}}
