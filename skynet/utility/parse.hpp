#pragma once

#include <regex>
#include <boost\algorithm\string.hpp>
#include <boost\lexical_cast.hpp>

#include <skynet\ublas.hpp>
#include <skynet\core\array.hpp>

namespace skynet{

	ublas::matrix<double> parse_matrix(const std::string  &s){
		std::regex  e("(\\[)(.*)(\\])");
		std::smatch sm;
		if (!std::regex_match(s, sm, e)){
			THROW_EXCEPTION(std::exception("The matrix format should be as [1 2 3; 1 3 4]."));
		}

		string mid_s = sm[2].str();
		std::vector<string> v_rows;
		//split by ;
		boost::split(v_rows, mid_s, boost::is_any_of(";"));
		//remove space
		std::regex e2("(\\s*)(.+)");
		for (size_t row = 0; row < v_rows.size(); ++row){
			std::smatch sm2;
			if (!std::regex_match(v_rows[row], sm2, e2)){
				THROW_EXCEPTION(std::exception("The matrix format should be as [1 2 3; 1 3 4]."));
			}
			v_rows[row] = sm2[2];
		}

		std::vector<std::vector<string>> vecs2_num(v_rows.size());
		for (size_t row = 0; row < v_rows.size(); ++row){
			boost::split(vecs2_num[row], v_rows[row], boost::is_any_of(" "));
		}

		ublas::matrix<double>  mat(v_rows.size(), vecs2_num.front().size());
		for (size_t row = 0; row < mat.size1(); ++row){
			for (size_t col = 0; col < mat.size2(); ++col){
				if (vecs2_num[row].size() != mat.size2()){
					THROW_EXCEPTION(std::exception("The matrix format should be as [1 2 3; 1 3 4]."));
				}
				auto num_str = vecs2_num[row][col];
				mat(row, col) = boost::lexical_cast<double, string>(num_str);
			}
		}

		return mat;
	};

	template <typename T>
	multi_array<T, 2> parse_array2(const std::string  &s){
		std::regex  e("(\\[)(.*)(\\])");
		std::smatch sm;
		if (!std::regex_match(s, sm, e)){
			THROW_EXCEPTION(std::exception("The matrix format should be as [1 2 3; 1 3 4]."));
		}

		string mid_s = sm[2].str();
		std::vector<string> v_rows;
		//split by ;
		boost::split(v_rows, mid_s, boost::is_any_of(";"));
		//remove space
		std::regex e2("(\\s*)(.+)");
		for (size_t row = 0; row < v_rows.size(); ++row){
			std::smatch sm2;
			if (!std::regex_match(v_rows[row], sm2, e2)){
				THROW_EXCEPTION(std::exception("The matrix format should be as [1 2 3; 1 3 4]."));
			}
			v_rows[row] = sm2[2];
		}

		std::vector<std::vector<string>> vecs2_num(v_rows.size());
		for (size_t row = 0; row < v_rows.size(); ++row){
			boost::split(vecs2_num[row], v_rows[row], boost::is_any_of("\b, \t"));
		}

		multi_array<T, 2> mat(vecs2_num.front().size(), v_rows.size());
		for (size_t row = 0; row < mat.height(); ++row){
			for (size_t col = 0; col < mat.width(); ++col){
				if (vecs2_num[row].size() != mat.width()){
					THROW_EXCEPTION(std::exception("The matrix format should be as [1 2 3; 1 3 4]."));
				}
				auto num_str = vecs2_num[row][col];
				mat(col, row) = boost::lexical_cast<T, string>(num_str);
			}
		}

		return mat;
	};

}