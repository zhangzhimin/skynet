#pragma once

#include <skynet/core/neighbor.hpp>
#include <skynet/core/lazy_evaluation.hpp>
#include <skynet/utility/math.hpp>

namespace skynet {
	namespace cv {

		//\brief harris角点检测算法
		//\param mat 图像的结构张量（黑塞矩阵）
		//\param kappa 建议值为0.04~0.15.
		//\return 返回角点隶属度图
		template <typename M, typename ValueType>
		inline auto harris_score(const M &tensor, ValueType kappa) {
			//黑塞矩阵的迹
			auto trace = apply(tensor, [](auto v) { return v[0][0] + v[1][1]; });
			//行列式值
			auto determinant = apply(tensor, [](auto v) { return v[0][0] * v[1][1] - v[0][1] * v[1][0]; });
			//计算隶属度
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
				//将弧度控制在（0，2PI)的开区间，通过精度的调整，防止边界情况发生
				om.orientation = (om.orientation + PI + epsilon) / (1.0 + 2.0 * epsilon);
				return om;
			}

			//0-2PI
			template <typename Gradient>
			inline auto orientation_magnitude_gradient(const Gradient &gradient) {
				return apply(gradient, gradient2om);
			}
		}

		//\brief 
		//\param gradient 图像梯度场
		//\param points 定位好的特征点位置
		//\param neigbor 统计的领域
		//\return 返回带方向的特征点
		template <typename Gradient, typename LocalizedPoints, typename Neighbor>
		inline auto get_sift_keypoints(const Gradient &gradient, LocalizedPoints points, Neighbor neighbor) {
			typedef sift_keypoint		sift_keypoint_type;

			neighbor.attach(gradient.extent());
			auto sigma = 0.5 * sqrt(neighbor.size());

			//将梯度表述成方向和强度
			auto om_tmp = detail::orientation_magnitude_gradient(gradient);
			//映射到0-36的闭区间
			auto om = apply(om_tmp, [](auto x) {
				x.orientation = x.orientation * 36 / (2.0 * PI);
				return x;
			});

			std::vector<sift_keypoint_type> keypoints;
			for (auto pos : points) {
				//将靠近边界的点排除，因为那里不能取到完整的局部信息
				if (pos < -neighbor.min_offset() || pos >= gradient.size() - neighbor.max_offset()) {
					continue;
				}

				//使用索引坐标
				auto  center = point<double, Gradient::dim>(size_t2index(pos, gradient.extent()));
				//高斯窗权重函数
				auto distance_weight = gaussian_function<decltype(center)>(sigma, center);
				//36柱的方向直方图
				std::array<double, 36> hist;
				std::fill(hist.begin(), hist.end(), 0);

				//统计领域的每个样点信息
				for (auto shift : neighbor) {
					auto offset = pos + shift;
					auto index = size_t2index(pos, gradient.extent());
					hist[om[offset].orientation] += om[offset].magnitude * distance_weight(decltype(center)(index));
				}

				//找出直方图的最大峰值
				auto max_peak = *std::max_element(hist.begin(), hist.end());
				for (auto i = 0; i < hist.size(); ++i) {
					//当直方图的柱值大于最大值的0.8时，我们判断其是不是峰值
					if (hist[i] > 0.8 * max_peak) {
						//峰值比最近的两个柱值都大，因为方向是循环的，所以考虑边界情况
						auto left = i - 1 > 0 ? i - 1 : hist.size() - 1;
						auto right = i + 1 < hist.size() ? i + 1 : 0;
						if (hist[left] > hist[i] || hist[right] > hist[i]) {
							continue;
						}
						//创建新的特征点
						sift_keypoint_type keypoint;
						keypoint.position = pos;
						//Todo  3-linear  interpolation.
						//keypoint.orientation = i * 2 *PI / 36;
						keypoint.orientation = om_tmp[pos].orientation;
						keypoints.push_back(keypoint);
					}
				}
			}

			return std::move(keypoints);
		}

		template<size_t dim>
		struct sift_discriptor {

		};

		//\brief 特征点描述子生成
		//\param gradient 图像梯度场
		//\param keypoints 特征点们
		//\return 特征点们的描述子
		template <typename Gradient>
		inline auto describe_sift(const Gradient &gradient, std::vector<sift_keypoint> keypoints) {
			std::vector<point<double, 128>> descriptors;
			auto extent = gradient.extent();
			for (auto keypoint : keypoints) {
				//特征点的4x4个直方图
				multi_array<point<double, 8>, 2> hists(4, 4);
				for (auto &hist : hists) {
					std::fill(hist.begin(), hist.end(), 0);
				}
				//特征点主方向
				auto theta = keypoint.orientation;
				//特征点的中心
				auto center = size_t2index(keypoint.position, extent);
				//双线性梯度插值
				auto continuity_gradient = interpolate(gradient);
				//坐标旋转转换
				auto rotatation_tran = [theta, center](index2 index)->point2d {
					point2d tmp;
					auto index0 = index - center;
					tmp[0] = cos(theta)*index0[0] + sin(theta) * index0[1];
					tmp[1] = -sin(theta)*index0[0] + cos(theta) * index0[1];
					return tmp + point2d(center);
				};

				//遍历16*16的样本区间
				for (int i = -8; i <= 8; ++i) {
					for (int j = -8; j <= 8; ++j) {
						//将中间临界样本排除
						if (i == 0 || j == 0) {
							continue;
						}

						auto offset = keypoint.position + i + j * extent[0];
						auto index = size_t2index(offset, extent);
						//subcell index
						auto id = point2d(index - center) / point2d(index2(4, 4));
						id += point2d(index2(2, 2));
						if (id[0] > 2)		--id[0];
						if (id[1] > 2)		--id[1];

						//将样本位置映射到实际图像的位置
						point2d index_tran = rotatation_tran(index);
						//检查样本点的实际图像位置是否在边界内
						if (!bound_check(index_tran + point2d::unit, extent) || !bound_check(index_tran, extent)) continue;
						//获取双线性插值后的梯度值
						auto current_gradient = continuity_gradient(index_tran);
						//将梯度转为方向和强度的表示
						auto current_om = detail::gradient2om(current_gradient);
						//orientation based on keypoint main orientation
						//梯度方向和特征点方向是相关的
						current_om.orientation -= keypoint.orientation;
						//将梯度方向再次映射到0~2PI之间
						current_om.orientation = current_om.orientation < 0 ? current_om.orientation + 2 * PI : current_om.orientation;
						//将梯度方向映射到0~8之间
						auto o = current_om.orientation * 8 / (2 * PI);
						//截取方向在直方图的上界
						auto up_o = std::round(o);
						//方向在直方图的下界
						auto down_o = up_o - 1;
						//方向在直方图两个柱子的线性加权系数
						auto w_o = (o - down_o - 0.5);
						//样本的位置上界
						point2d up_id(std::round(id[0]), std::round(id[1]));
						//样本的位置下界
						point2d down_id = up_id - point2d::unit;
						//样本的位置线性加权系数
						auto w_id = (id - down_id - point2d(0.5, 0.5));

						//直方图三线性加权
						array<double, 2> X0 = { down_id[0], up_id[0] };
						array<double, 2> X1 = { down_id[1], up_id[1] };
						array<double, 2> X2 = { down_o, up_o };
						X2[0] = X2[0] < 0 ? 7 : X2[0];
						X2[1] = X2[1] > 7 ? 0 : X2[1];
						for (int k = 0; k < 2; ++k) {
							for (int m = 0; m < 2; ++m) {
								for (int n = 0; n < 2; ++n) {
									if (std::isless(X0[k], 0) || std::isgreater(X0[k], 3) || std::isless(X1[m], 0) || std::isgreater(X1[m], 3)) {
										continue;
									}

									array<double, 3> W;
									W[0] = k == 0 ? 1 - w_id[0] : w_id[0];
									W[1] = m == 0 ? 1 - w_id[1] : w_id[1];
									W[2] = n == 0 ? 1 - w_o : w_o;
									auto v = current_om.magnitude * W[0] * W[1] * W[2];
									auto &hist = hists(index2(X0[k], X1[m]));
									hist[X2[n]] += v;
								}
							}
						}


					}
				}

				//将16个方向直方图，转换为128的特征向量
				point<double, 128> descriptor;
				auto it = descriptor.begin();
				for (auto hist : hists) {
					for (auto e : hist) {
						*it = e;
						++it;
					}
				}

				descriptors.push_back(std::move(descriptor));
			}

			return std::move(descriptors);
		}

	}
}