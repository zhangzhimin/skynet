#pragma once

#include <skynet\core\point.hpp>
#include <skynet\core\detail.hpp>
#include <skynet\core\neighbor.hpp>
#include <skynet\core\lazy_evaluation.hpp>
#include <skynet\geometry\region.hpp>
#include <skynet\utility\tag.hpp>
#include <skynet\utility\cycle_num.hpp>

#include <exception>
#include <functional>
#include <queue>

#pragma message("Warning:  when you use the morphology operation, \
                the infomation of the boundary is invalid.(by zzm)")

namespace skynet{ namespace cv{
	using std::function;

	template <typename M, typename N>
	auto erode(const M &mat_src, N n)->lazy_array<M::dim, function<byte (const size_t &)>>{
		n.attach(mat_src.extent());
		function<byte (const size_t &)> fun = [=](const size_t &i)->byte{
			if (i >= (mat_src.size() - n.max_offset()) || i < -n.min_offset())   return 0;

			for (auto it = n.begin(); it != n.end(); ++it){
				auto offset = i + *it;
				if (!mat_src[offset]){
					return 0;
				}
			}

			return 1;
		};

		return make_lazy_array(mat_src.extent(), fun);
	}

	template <typename M, typename N>
	auto dilate(const M &mat_src, N n)->lazy_array<M::dim, function<byte (const size_t &)>>{
		n.attach(mat_src.extent());
		function<byte (const size_t &)> fun = [=](const size_t &i)->byte{
			if (i >= (mat_src.size() - n.max_offset()) || i < -n.min_offset())    return 0;

			for (auto it = n.begin(); it != n.end(); ++it){
				auto offset = i + *it;
				if (mat_src[offset]){
					return 1;
				}
			}

			return 0;
		};

		return make_lazy_array(mat_src.extent(), fun);
	}

	template <typename M, typename N>
	auto open(const M &mat_src, N n)->lazy_array<M::dim, function<byte (const size_t &)>>{
		return dilate(cache4lazy(erode(mat_src, n)), n);
	}

	template <typename M, typename N>
	auto close(const M &mat_src, N n)->lazy_array<M::dim, function<byte (const size_t &)>>{
		return erode(cache4lazy(dilate(mat_src, n)), n);
	}

	template <typename M>
	size_t nuclete(const M &in_image, const size_t &init_pos) {
		auto size = in_image.extent();
		auto nucleus = skynet::size_t2index(init_pos, size);
		decltype(nucleus) pre_index(nucleus);
		//make the pre_index and nucleus are not the same.
		pre_index[0] += 5;

		int count = 100;//Max iteration num

		cycle_num<0, M::dim - 1> cyc_num(0);
		while(count > 0){
			++cyc_num;
			--count;
			auto sec_index = nucleus;
			auto fir_index = nucleus;
			while (sec_index[cyc_num] < size[cyc_num]){
				++(sec_index[cyc_num]);

				if (!in_image.get_value(sec_index))	break;
			}

			while (fir_index[cyc_num] > 0){
				--(fir_index[cyc_num]);

				if (!in_image.get_value(fir_index))	break;
			}

			nucleus = std::move(center_point(fir_index, sec_index));
			if (distance(pre_index, nucleus, street_tag()) < 3){
				return skynet::index2size_t(nucleus, size);
			}

			pre_index = nucleus;
		}	

		return skynet::index2size_t(nucleus, size);
	}

	//http://en.wikipedia.org/wiki/grassfire
	// for each row in image left to right
	//  for each column in image top to bottom
	//    if(pixel is in region){
	//      set pixel to 1 + minimum value of the north and west neighbours
	//    }else{
	//      set pixel to zero
	//    }
	//  }
	//}
	//for each row right to left
	//  for each column bottom to top
	//    if(pixel is in region){
	//      set pixel to min(value of the pixel,1 + minimum value of the south and east neighbours)
	//    }else{
	//      set pixel to zero
	//    }
	//  }
	//}
	template <typename M1>
	multi_array<int, M1::dim> grassfire(const M1 &in_image){
		const diamand_neighbor<M1::dim> neighbors(in_image.extent());
		multi_array<int, M1::dim> out_image(in_image.extent());
		out_image.set_all_zero();
		//top to down
		auto mid_it = neighbors.begin()+M1::dim;
		for (size_t i = -neighbors.min_offset(); i < (in_image.size()-neighbors.max_offset()); ++i){
			if (in_image[i]){
				auto min_dis = std::min_element(mid_it, neighbors.end(), 
					[&out_image, &i](const int &lhs, const int &rhs){
						return out_image[i+lhs] < out_image[i+rhs];
				});
				out_image[i] = out_image[i+*min_dis] + 1;
			}
		}
		//down to top
		for (size_t i = in_image.size()-neighbors.max_offset()-1; i >= -neighbors.min_offset(); --i){
			if (in_image[i]){
				auto min_dis = std::min_element(neighbors.begin(), mid_it, 
					[&out_image, &i](const int &lhs, const int &rhs){
						return out_image[i+lhs] < out_image[i+rhs];
				});
				out_image[i] = std::min(out_image[i+*min_dis] + 1, out_image[i]);
			}
		}

		return out_image;
	}

	template <typename M>
	multi_array<int, M::dim> grassfire_outside(const M &volume){
		const diamand_neighbor<M::dim> neighbors(volume.extent());
		multi_array<int, M::dim> out_volume(volume.extent());
		out_volume.set_all_zero();
		static const int BOUND_VALUE = std::numeric_limits<int>::max()/2;
		skynet::set_boundary(out_volume, BOUND_VALUE);

		auto mid_it = neighbors.begin()+M::dim;
		for (size_t i = -neighbors.min_offset(); i < (volume.size()-neighbors.max_offset()); ++i){
			if (!volume[i]){
				auto min_dis = std::min_element(mid_it, neighbors.end(), 
					[&out_volume, &i](const int &lhs, const int &rhs){
						return out_volume[i+lhs] < out_volume[i+rhs];
				});
				out_volume[i] = out_volume[i+*min_dis] + 1;
			}
		}
		//down to top
		for (size_t i = volume.size()-neighbors.max_offset()-1; i >= -neighbors.min_offset(); --i){
			if (!volume[i]){
				auto min_dis = std::min_element(neighbors.begin(), mid_it, 
					[&out_volume, &i](const int &lhs, const int &rhs){
						return out_volume[i+lhs] < out_volume[i+rhs];
				});
				out_volume[i] = std::min(out_volume[i+*min_dis] + 1, out_volume[i]);
			}
		}

		return out_volume;
	}

	//template <typename M1, typename M2>
	//void ridge(const M1 &in_image, M2 &out_image){
	//	IS_SAME_DIMENTION(M1, M2);
	//	ASSERT(in_image.size() == out_image.size(), "");
	//	ASSERT(out_image[0] == 0, "Not zero image");

	//	const squared_neighbor<M1::dim> neighbors(in_image.extent());
	//	for (size_t i = -neighbors.min_offset(); i < in_image.size()-neighbors.max_offset(); ++i){
	//		if (in_image[i] == 0)	continue;

	//		auto temp = in_image[i] + 1;
	//		bool hitted = true;
	//		for (int n = 0; n < neighbors.size(); ++n){
	//			if (in_image[i+neighbors[n]] >= temp){
	//				hitted = false;
	//				break;
	//			}
	//		}

	//		if (hitted)	
	//			out_image[i] = true;
	//	}
	//}

	//http://sdrv.ms/WozzDy Distance-Ordered Homotopic Thinning: A Skeletonization Algorithm for 3D Digital Images
	//
	template <typename M1, typename M2>
	void thinning3D(const M1 &in_image, M2 &out_image){
		IS_SAME_DIMENTION(M1, M2);
		static_assert(M1::dim == 3,"The diemention should be 3.");
		ASSERT(in_image.size() == out_image.size(), "");
		ASSERT(out_image[0] == 0, "");

		mask3d mat_distance(in_image.extent());
		mat_distance.set_all_zero();
		grassfire(in_image, mat_distance);
		mask3d mat_accessed(in_image.extent());
		mat_accessed.set_all_zero();

		std::queue<size_t> q;
		for (size_t i = 0; i < mat_distance.size(); ++i){
			if (mat_distance[i] == 1){
				q.push(i);
			}
		}

		auto detetable = [&](const size_t &i){

		};

		while(!q.empty()){
			auto offset = q.front();
			q.pop();
			if(){

			}
			else
				out_image[offset] = true;
		}
	}
}}