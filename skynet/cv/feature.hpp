#pragma once

#include <skynet/core/neighbor.hpp>
#include <skynet/core/lazy_evaluation.hpp>
#include <skynet/utility/math.hpp>

namespace skynet {
	namespace cv {

		//\brief harris�ǵ����㷨
		//\param mat ͼ��Ľṹ��������������
		//\param kappa ����ֵΪ0.04~0.15.
		//\return ���ؽǵ�������ͼ
		template <typename M, typename ValueType>
		inline auto harris_score(const M &tensor, ValueType kappa) {
			//��������ļ�
			auto trace = apply(tensor, [](auto v) { return v[0][0] + v[1][1]; });
			//����ʽֵ
			auto determinant = apply(tensor, [](auto v) { return v[0][0] * v[1][1] - v[0][1] * v[1][0]; });
			//����������
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
				//�����ȿ����ڣ�0��2PI)�Ŀ����䣬ͨ�����ȵĵ�������ֹ�߽��������
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
		//\param gradient ͼ���ݶȳ�
		//\param points ��λ�õ�������λ��
		//\param neigbor ͳ�Ƶ�����
		//\return ���ش������������
		template <typename Gradient, typename LocalizedPoints, typename Neighbor>
		inline auto get_sift_keypoints(const Gradient &gradient, LocalizedPoints points, Neighbor neighbor) {
			typedef sift_keypoint		sift_keypoint_type;

			neighbor.attach(gradient.extent());
			auto sigma = 0.5 * sqrt(neighbor.size());

			//���ݶȱ����ɷ����ǿ��
			auto om_tmp = detail::orientation_magnitude_gradient(gradient);
			//ӳ�䵽0-36�ı�����
			auto om = apply(om_tmp, [](auto x) {
				x.orientation = x.orientation * 36 / (2.0 * PI);
				return x;
			});

			std::vector<sift_keypoint_type> keypoints;
			for (auto pos : points) {
				//�������߽�ĵ��ų�����Ϊ���ﲻ��ȡ�������ľֲ���Ϣ
				if (pos < -neighbor.min_offset() || pos >= gradient.size() - neighbor.max_offset()) {
					continue;
				}

				//ʹ����������
				auto  center = point<double, Gradient::dim>(size_t2index(pos, gradient.extent()));
				//��˹��Ȩ�غ���
				auto distance_weight = gaussian_function<decltype(center)>(sigma, center);
				//36���ķ���ֱ��ͼ
				std::array<double, 36> hist;
				std::fill(hist.begin(), hist.end(), 0);

				//ͳ�������ÿ��������Ϣ
				for (auto shift : neighbor) {
					auto offset = pos + shift;
					auto index = size_t2index(pos, gradient.extent());
					hist[om[offset].orientation] += om[offset].magnitude * distance_weight(decltype(center)(index));
				}

				//�ҳ�ֱ��ͼ������ֵ
				auto max_peak = *std::max_element(hist.begin(), hist.end());
				for (auto i = 0; i < hist.size(); ++i) {
					//��ֱ��ͼ����ֵ�������ֵ��0.8ʱ�������ж����ǲ��Ƿ�ֵ
					if (hist[i] > 0.8 * max_peak) {
						//��ֵ�������������ֵ������Ϊ������ѭ���ģ����Կ��Ǳ߽����
						auto left = i - 1 > 0 ? i - 1 : hist.size() - 1;
						auto right = i + 1 < hist.size() ? i + 1 : 0;
						if (hist[left] > hist[i] || hist[right] > hist[i]) {
							continue;
						}
						//�����µ�������
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

		//\brief ����������������
		//\param gradient ͼ���ݶȳ�
		//\param keypoints ��������
		//\return �������ǵ�������
		template <typename Gradient>
		inline auto describe_sift(const Gradient &gradient, std::vector<sift_keypoint> keypoints) {
			std::vector<point<double, 128>> descriptors;
			auto extent = gradient.extent();
			for (auto keypoint : keypoints) {
				//�������4x4��ֱ��ͼ
				multi_array<point<double, 8>, 2> hists(4, 4);
				for (auto &hist : hists) {
					std::fill(hist.begin(), hist.end(), 0);
				}
				//������������
				auto theta = keypoint.orientation;
				//�����������
				auto center = size_t2index(keypoint.position, extent);
				//˫�����ݶȲ�ֵ
				auto continuity_gradient = interpolate(gradient);
				//������תת��
				auto rotatation_tran = [theta, center](index2 index)->point2d {
					point2d tmp;
					auto index0 = index - center;
					tmp[0] = cos(theta)*index0[0] + sin(theta) * index0[1];
					tmp[1] = -sin(theta)*index0[0] + cos(theta) * index0[1];
					return tmp + point2d(center);
				};

				//����16*16����������
				for (int i = -8; i <= 8; ++i) {
					for (int j = -8; j <= 8; ++j) {
						//���м��ٽ������ų�
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

						//������λ��ӳ�䵽ʵ��ͼ���λ��
						point2d index_tran = rotatation_tran(index);
						//����������ʵ��ͼ��λ���Ƿ��ڱ߽���
						if (!bound_check(index_tran + point2d::unit, extent) || !bound_check(index_tran, extent)) continue;
						//��ȡ˫���Բ�ֵ����ݶ�ֵ
						auto current_gradient = continuity_gradient(index_tran);
						//���ݶ�תΪ�����ǿ�ȵı�ʾ
						auto current_om = detail::gradient2om(current_gradient);
						//orientation based on keypoint main orientation
						//�ݶȷ���������㷽������ص�
						current_om.orientation -= keypoint.orientation;
						//���ݶȷ����ٴ�ӳ�䵽0~2PI֮��
						current_om.orientation = current_om.orientation < 0 ? current_om.orientation + 2 * PI : current_om.orientation;
						//���ݶȷ���ӳ�䵽0~8֮��
						auto o = current_om.orientation * 8 / (2 * PI);
						//��ȡ������ֱ��ͼ���Ͻ�
						auto up_o = std::round(o);
						//������ֱ��ͼ���½�
						auto down_o = up_o - 1;
						//������ֱ��ͼ�������ӵ����Լ�Ȩϵ��
						auto w_o = (o - down_o - 0.5);
						//������λ���Ͻ�
						point2d up_id(std::round(id[0]), std::round(id[1]));
						//������λ���½�
						point2d down_id = up_id - point2d::unit;
						//������λ�����Լ�Ȩϵ��
						auto w_id = (id - down_id - point2d(0.5, 0.5));

						//ֱ��ͼ�����Լ�Ȩ
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

				//��16������ֱ��ͼ��ת��Ϊ128����������
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