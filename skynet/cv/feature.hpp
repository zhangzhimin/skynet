#pragma once

#include <skynet/core/neighbor.hpp>
#include <skynet/core/lazy_evaluation.hpp>
#include <skynet/utility/math.hpp>

namespace skynet {namespace cv {

	template <typename M, typename ValueType>
	auto harris_score(const M &mat, ValueType kappa){
		auto trace = apply(mat, [](auto v) { return v[0][0] + v[1][1]; });
		auto determinant = apply(mat, [](auto v) { return v[0][0] * v[1][1] - v[0][1] * v[1][0]; });
		return determinant - kappa * (trace * trace);
	}
	
	struct sift_keypoint {
		size_t				position;
		size_t				scale;
		double				orientation;
	};

	namespace detail {
		struct orientation_and_magnitude {
			double	orientation;
			double magnitude;
		};
	
		inline orientation_and_magnitude gradient2om(point2d gradient) {
			orientation_and_magnitude om;
			om.magnitude = norm<Euclidean>(gradient);
			om.orientation = std::atan2(gradient[1], gradient[0]);
			auto epsilon = numeric_limits<double>::epsilon();
			//将弧度控制在（0，2PI)的开区间
			om.orientation = (om.orientation + PI + epsilon) / (1.0 + 2.0 * epsilon);
			return om;
		}
	
		//0-2PI
		template <typename Gradient>
		auto orientation_magnitude_gradient(const Gradient &gradient) {
			return apply(gradient, gradient2om);
		}
	}

	template <typename Gradient, typename LocalizedPoints, typename Neighbor>
	auto get_sift_keypoints(const Gradient &gradient, LocalizedPoints points, Neighbor neighbor){
		typedef sift_keypoint		sift_keypoint_type;	

		neighbor.attach(gradient.extent());
		auto sigma = 0.5 * sqrt(neighbor.size());

		auto om_tmp = detail::orientation_magnitude_gradient(gradient);
		//映射到0-36的闭区间
		auto om = apply(om_tmp, [](auto x) { 
			x.orientation = x.orientation * 36 / (2.0 * PI);
			return x;
		});

		std::vector<sift_keypoint_type> keypoints;
		for (auto pos : points) {
			if (pos < -neighbor.min_offset() || pos >= gradient.size() - neighbor.max_offset()) {
				continue;
			}

			auto  center = point<double, Gradient::dim>(size_t2index(pos, gradient.extent()));
			auto distance_weight = gaussian_function<decltype(center)>(sigma, center);
			std::array<double, 36> hist;
			std::fill(hist.begin(), hist.end(), 0);
			for (auto shift : neighbor) {
				auto offset = pos + shift;
				auto index = size_t2index(pos, gradient.extent());
				hist[om[offset].orientation] += om[offset].magnitude * distance_weight(decltype(center)(index)) ;
			}
			
			auto max_peak = *std::max_element(hist.begin(), hist.end());
			for (auto i = 0; i < hist.size(); ++i){
				if (hist[i] > 0.9 * max_peak){
					auto left = i - 1 > 0 ? i-1: hist.size()-1;
					auto right = i + 1 < hist.size() ? i + 1 : 0;
					if (hist[left] > hist[i] || hist[right] > hist[i]) {
						continue;
					}
					sift_keypoint_type keypoint;
					keypoint.position = pos;
					keypoint.orientation = i;
					keypoints.push_back(keypoint);
				}
			}
		}

		return std::move(keypoints);
	}

	template<size_t dim>
	struct sift_discriptor {
		
	};

	template <typename Gradient>
	auto describe_sift(const Gradient &gradient, std::vector<sift_keypoint> keypoints){
		auto extent = gradient.extent();
		for (auto keypoint : keypoints) {
			multi_array<array<double, 8>, 2> hists(4, 4);
			auto theta = keypoint.orientation;
			auto center = size_t2index(keypoint.position, extent);
			auto continuity_gradient = interpolate_array(gradient);
			auto rotate_tran = [theta, center](index2 index)->point2d {
				point2d tmp;
				auto index0 = index - center;
				tmp[0] = cos(theta)*index0[0] + sin(theta) * index0[1];
				tmp[1] = -sin(theta)*index0[0] + cos(theta) * index0[1];
				return tmp + point2d(center);
			};

			for (int i = -8; i <= 8; ++i) {
				for (int j = -8; j <= 8; ++j) {
					if (i == 8 || j == 8) {
						continue;
					}

					auto offset = keypoint.position + i + j * extent[0];
					auto index = size_t2index(offset, extent);
					//subcell index
					auto id = point2d(index - center) / point2d(index2(4, 4));
					id += point2d(index2(2, 2));
					if (id[0] > 2)		--id[0];
					if (id[1] > 2)		--id[1];

					point2d index_tran = rotation_tran(index); 
					auto current_gradient = continuity_gradient(index_tran);
					auto current_om = detail::gradient2om(current_gradient);
					//orientation based on keypoint main orientation
					current_om.orientation -= keypoint.orientation;
					auto o = current_om.orientation * 8 / (2 * PI);
					auto up_o = std::round(o);
					int down_o = up - 1;
					auto up_i = up_o >= 8 ? 7 : up_o;
					auto down_i = down_o < 0 ? 0 : down_o;			

					auto w_o = (o - down_i + 0.5);
					point2d up_id(std::round(id[0]), std::round(id[1]));
					point2d down_id = up_id - point2d::unit;
					auto w_id = (id - down_id + point2d(0.5, 0.5));

					array<double, 2> X0 = {down_id[0], up_id[0] };
					array<double, 2> X1 = { down_id[1], up_id[1] };
					array<double, 2> X2 = { down_i, up_i };

					for (int i = 0; i < 1; ++i) {
						for (int j = 0; j < 1; ++j) {
							for (int k = 0; k < 1; ++k) {
								array<double, 3> W;
								W[0] = i == 0 ? 1 - w_id[0] :  w_id[0];
								W[1] = j == 0 ? 1 - w_id[1] : w_id[1];
								W[2] = k == 0 ? 1 - w_o : wo;
								hists(index2(X0[i], X1[j]))[X2[k]] += current_om.orientation * current_om.magnitude * W[0] * W[1] * W[2];
							}
						}
					}								
				}
			}
		}
	}
}}