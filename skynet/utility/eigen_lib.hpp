#pragma once

#include <skynet/utility/serialization.hpp>

#include <Eigen/Core>

namespace boost{namespace serialization{

	using namespace boost::serialization;

	template <typename Archive>
	void serialize(Archive &ar, Eigen::VectorXd &vec, const unsigned int&){
		if (Archive::is_saving::value){
			size_t size = vec.size();
			ar & make_nvp("size", size);

			if (vec.size() == 0)
				return;

			ar & make_array(&(vec[0]), vec.size());
		}
		else{
			size_t size = 0;
			ar & boost::serialization::make_nvp("size", size);
			vec.resize(size);

			if (vec.size() == 0)
				return;

			ar & make_array(&vec[0], vec.size());
		}
	}

	template <typename Archive>
	void serialize(Archive &ar, Eigen::MatrixXd &matrix, const unsigned int&){
		if (Archive::is_saving::value){
			size_t row = matrix.rows();
			size_t col = matrix.cols();
			ar & make_nvp("row_size", row);
			ar & make_nvp("col_size", col);

			ar & make_array(&matrix(0, 0), col * row);
		}
		else{
			size_t row = 0;
			size_t col = 0;
			ar & make_nvp("row_size", row);
			ar & make_nvp("col_size", col);
			matrix.resize(row, col);

			ar & make_array(&matrix(0, 0), col * row);
		}
	}
}}
