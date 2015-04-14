/******************************************************************************
Created By : Zhang Zhimin
Created On :  
Purpose    :  
********************************************************************************/

#pragma once

#include <skynet/core/array.hpp>
#include <skynet/core/neighbor.hpp>
#include <skynet/utility/math.hpp>


namespace skynet{namespace cv{

	template <typename M1, typename M2>
	void region_grow(const M1 &in_image, M2 &out_image, const size_t &seed){
		//  typedef typename M2::value_type flag_type;
		if (!in_image[seed]){
			LOG_WARNING("the seed value should be non-zero.");
			return;
		}

		auto size = in_image.extent();
		skynet::set_boundary(out_image, 1);
		if (out_image[seed]){
			skynet::set_boundary(out_image, 0);
			LOG_WARNING("the seed is in boundary, it's invalid");
			return;
		}
		else{
			out_image[seed] = true;
		}

		//  auto size = in_image
		std::vector<size_t> roots;
		roots.reserve(in_image.size()>>12);				
		roots.push_back(seed);

		const diamand_neighbor<M1::dim> neighbors(size);

		while (!roots.empty()){
			auto root = roots.back();
			roots.pop_back();	

			for (auto it = neighbors.begin(); it != neighbors.end(); ++it){
				auto offset = root + (*it);

				if(in_image[offset] && !out_image[offset]){
					roots.push_back(offset);
					out_image[offset] = true;
				}
			}
		}	

		skynet::set_boundary(out_image, 0);
	}

	//该版本可能存在效率问题
	template <typename M1, typename M2, typename M3>
	void dilate_grow(const M1 &in_image, const M2 &flag_image, M3 &out_image){
		ASSERT(in_image.extent() == out_image.extent(), "The extent should be the same.");

		copy(flag_image, out_image);
		const diamand_neighbor<M1::dim> neighbors(in_image.extent());
		for (size_t i = -neighbors.min_offset(); i < in_image.size() - neighbors.max_offset(); ++i){
			if (flag_image[i]){
				for (auto it = neighbors.begin(); it != neighbors.end(); ++it){
					if (out_image[i + *it]) continue;

					if (in_image[i + *it]){
						out_image[i + *it] = true;
					}
				}
			}
		}
	}


	template <typename M>
	std::vector<size_t> get_region(const M &in_image, const size_t &seed){
		auto extent = in_image.extent();
		multi_array<bool, M::dim> flag_mat(extent);
		fill(flag_mat, 0);
		set_boundary(flag_mat, 1);

		std::vector<size_t> roots;
		roots.reserve(1000000);	
		roots.push_back(seed);

		std::vector<size_t> region;
		if (!in_image[seed]){
			return region;
		}
		if (flag_mat[seed] != 0){
			return region;
		}

		region.push_back(seed);
		diamand_neighbor<M::dim> neighbors;
		neighbors.attach(extent);

		while (!roots.empty()){
			auto root = roots.back();
			roots.pop_back();	

			for (auto it = neighbors.begin(); it != neighbors.end(); ++it){
				auto offset = root + (*it);

				if(in_image[offset]&& !flag_mat[offset]){
					roots.push_back(offset);
					flag_mat[offset] = true;;
					region.push_back(offset);
				}
			}
		}	

		//set_boundary(flag_mat, 0);
		return region;
	}


	//template <typename M>
	//std::vector<region<M::dim>> get_regions(const M &in_image){
	//	auto extent = in_image.extent();
	//	multi_array<byte, M::dim> flag_mat(extent);
	//	flag_mat.set_all_zero();
	//	skynet::set_boundary(flag_mat, 1);

	//	std::vector<region<M::dim>> regions;

	//	for (uint i = 0; i < in_image.size(); ++i){
	//		if (!in_image[i] || flag_mat[i] != 0)  continue;

	//		std::vector<size_t> roots;
	//		roots.reserve(in_image.size()>>(M::dim<<1));
	//		roots.push_back(i);

	//		region<M::dim> region(extent);
	//		const diamand_neighbor<M::dim> neighbors(extent);

	//		while (!roots.empty()){
	//			auto root = roots.back();
	//			roots.pop_back();	

	//			for (auto it = neighbors.begin(); it != neighbors.end(); ++it){
	//				auto offset = root + (*it);

	//				if(in_image[offset] && flag_mat[offset] == 0){
	//					roots.push_back(offset);
	//					flag_mat[offset] = 1;
	//					region.add(offset);
	//				}
	//			}
	//		}
	//		regions.push_back(region);
	//	}

	//	skynet::set_boundary(flag_mat, 0);
	//	return regions;        
	//}


	template <typename M1,typename M2>
	pair<double, double> confidence_connected(const M1 &in_image, M2 &out_image, const std::vector<size_t> &seeds, const double &mul){
		IS_SAME_DIMENTION(M1, M2);
		ASSERT(in_image.extent() == out_image.extent(), "The extent of the in image and out image is not same.");
		typedef typename M1::value_type		value_type;

		//pre
		skynet::set_boundary(out_image, 1);
		auto it = std::find_if(seeds.begin(), seeds.end(), [&out_image](const size_t &i){ return out_image[i]; });
		if (it != seeds.end()){
			LOG_WARNING("the seed is in boundary, it's invalid");
			skynet::set_boundary(out_image, 0);
			return pair<double, double>(0,0);
		}
		set_value(out_image, seeds, true);				
		std::vector<size_t> roots(seeds.begin(), seeds.end());
		std::vector<size_t> leafs;

		//initial
		double sum = 0.0;
		double sum_squares = 0.0;
		size_t num = 0;
		for (auto it = roots.begin(); it != roots.end(); ++it){
			sum += in_image[*it];
			sum_squares += sqr(in_image[*it]);
			++num;
		}
		double mean = sum/num;
		double variance = (sum_squares - (sqr(sum)/num))/(num-1);
		value_type upper = static_cast<value_type>(mean + mul*sqrt(variance));
		value_type lower = static_cast<value_type>(mean - mul*sqrt(variance));

		const diamand_neighbor<M1::dim> neighbors(in_image.extent());
		while (true){
			if (roots.empty()){
				std::swap(roots, leafs);

				mean = sum/num;
				variance = (sum_squares - (sqr(sum)/num))/(num-1);
				upper = max(static_cast<value_type>(mean + mul*sqrt(variance)), upper);
				lower = min(static_cast<value_type>(mean - mul*sqrt(variance)), lower);
			}
			if(roots.empty())
				break;

			auto root = roots.back();
			roots.pop_back();	

			for (auto it = neighbors.begin(); it != neighbors.end(); ++it){
				auto offset = root + (*it);

				if(!out_image[offset] && in_image[offset]>lower && in_image[offset]<upper){
					sum += in_image[offset];
					sum_squares += sqr(in_image[offset]);
					++num;

					leafs.push_back(offset);
					out_image[offset] = true;
				}
			}
		}	

		skynet::set_boundary(out_image, 0);
		return make_pair(mean, sqrt(variance));
	}

}}
