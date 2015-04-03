#pragma once

#include <skynet/core/neighbor.hpp>
#include <skynet/core/lazy_evaluation.hpp>

namespace skynet {namespace cv {

	template <typename M, typename ValueType>
	auto harris_score(const M &mat, ValueType kappa){
		auto trace = apply(mat, [](auto v) { return v[0][0] + v[1][1]; });
		auto determinant = apply(mat, [](auto v) { return v[0][0] * v[1][1] - v[0][1] * v[1][0]; });
		return determinant - kappa * (trace * trace);
	}
	
	template<size_t dim>
	struct sift_keypoint {
		size_t				postion;
		size_t				scale;
		point<double, dim>	orientation;
	};

	template <typename Gradient, typename LocalizedPoints>
	auto get_sift_keypoints(const Gradient &gradient, LocalizedPoints points){
		
	}

	template<size_t dim>
	struct sift_discriptor {
		
	};

	auto describe_sift(){

	}
}}