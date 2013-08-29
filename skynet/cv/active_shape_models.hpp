#pragma once


///skynet是我开发的一个用于计算机视觉和机器学习的库， 现在还有些功能尚未完成
///Eigen是一个数值分析库（仅数值分析功能）， 因为时间有限，和具备一定难度， 所以我没有构建自己的数值分析库
///
///
#include <skynet/core/point.hpp>
#include <skynet/core/array.hpp>
#include <skynet/core/io.hpp>
#include <skynet/core/shared_buffer.hpp>
#include <skynet/cv/pixel.hpp>
#include <skynet/cv/field.hpp>
#include <skynet/cv/spatial_based.hpp>
#include <skynet/utility/algorithm.hpp>
#include <skynet/utility/serialization.hpp>
#include <skynet/geometry/contour.hpp>
#include <skynet/statistics/pca.hpp>
#include <skynet/statistics/distribution.hpp>
#include <skynet/statistics/missing_data.hpp>
#include <skynet/statistics/metric.hpp>

#include <Eigen/Core>
#include <Eigen/SVD>

#include <filesystem>
#include <random>


namespace skynet{ namespace cv{ namespace advanced{

	namespace detail{

		///align变换的逆变化， 该变换先进行旋转后进行平移变换
		class inverse_align_transform : public std::unary_function<point2d, point2d>{
		public:
			inverse_align_transform() : _offset(0), _rotation(0){}

			inverse_align_transform(point2d offset, double rotation) : _offset(offset), _rotation(rotation){}

			point2d operator()(const point2d &p) const{
				point2d re(p);
				auto angle = std::atan2(re.y, re.x);
				angle += _rotation;
				auto length = norm(re);
				re.x = length * cos(angle);
				re.y = length * sin(angle);

				re += _offset;

				return re;
			}

		private:
			point2d _offset;
			double  _rotation;
		};

		///align变换， 该变换先进行平移，后旋转。
		class align_transform : public std::unary_function<point2d, point2d>{
		public:
			align_transform () : _offset(0), _rotation(0){}

			align_transform(point2d offset, double rotation): _offset(offset), _rotation(rotation){}

			point2d operator()(const point2d &p) const {
				point2d re(p);
				re += _offset;

				auto length = norm(re);
				auto angle = std::atan2(re.y, re.x);
				angle += _rotation;
				re.x = length * cos(angle);
				re.y = length * sin(angle);

				return re;
			}

			inverse_align_transform inverse() const {
				return inverse_align_transform(-_offset, -_rotation);
			}

		private:
			point2d _offset;
			double	_rotation;
		};

		//对contour进行alig变换的函数， 其返回一个align变换， 将该变换用于contour，即可获得align后的contour。
		align_transform align_contour(const geometry::contour<point2d, std::vector<point2d>> &contour){
			auto mean_offset = -mean(contour);

			std::vector<double>	rotations(contour.size());
			for (size_t i = 0; i < contour.size(); ++i){
				rotations[i] = std::atan2(contour[i].y, contour[i].x);
			}

			auto mean_angle = -mean(rotations);
			return detail::align_transform(mean_offset, mean_angle);
		}

		///将contour的标记点坐标， 转换为一个特征向量。
		geometry::contour<point2d, std::vector<point2d>> feature_vector2contour(const Eigen::VectorXd &v){
			auto size = v.size()/2;
			geometry::contour<point2d, std::vector<point2d>> contour(v.size()/2);
			for (size_t i = 0; i < size; ++i){
				contour[i].x = v[i<<1];
				contour[i].y = v[(i<<1)+1];
			}

			return contour;
		}

		///将特征向量， 转换为contour
		Eigen::VectorXd	contour2feature_vector(const geometry::contour<point2d, std::vector<point2d>> &contour){
			Eigen::VectorXd  feature_vector(contour.size()<<1);
			for (int i = 0; i < contour.size(); ++i){
				feature_vector[i<<1] = contour[i].x;
				feature_vector[(i<<1)+1] = contour[i].y;
			}

			return feature_vector;
		}

		///获取图像的局部特征， image为图像源， pos为contour上的一个标记点， normal为去法向量， k为剖线的半径， 
		///函数返回一个2k+1的梯度数组， shared_buffer是一个内存安全的数组， 使用了引用计数
		shared_buffer<point3d> profile_diff(const multi_array<cv::bgra, 2>  &image, const point2d &pos, 
			const point2d &normal, int k){
				shared_buffer<point2d>   profile_pos(2*k + 3);
				for (int i = -(k+1); i <= (k+1); ++i){
					profile_pos[k+1 + i] = pos + (double)i * normal;
				}

				auto d_image = lazy_cast(image, point3d());
				//对图像进行高斯滤波， 现在暂时没有使用
				auto filter_image = gaussian_filter(d_image);
				//对图像进行钳位，防止访问时，指针越界
				auto clamp_image = make_clamp_array(d_image);
				shared_buffer<point3d> grad(2*k+1);
				for (int i = -k; i <= k; ++i){
					point2d pre =	profile_pos[i+k+2];
					point2d next =	profile_pos[i+k];
					auto pre_pixel = clamp_image.get_value(nearest_neighbor(pre));
					auto next_pixel = clamp_image.get_value(nearest_neighbor(next));

					grad[k+i] = pre_pixel - next_pixel;
				}

				return grad;
		}

		namespace sys = std::tr2::sys;

		///用于生成asm model， 期间将会加载contour和image， 对contour进行pca， 获取image的局部特征等
		class shape_model_generator{
		public:

			///contour类型， point2d表示contour的标记点类型， std::vector<point2d>表示存储contour的容器
			typedef geometry::contour<point2d, std::vector<point2d>>		contour_type;
			typedef statistics::metric<Mahalanobis>					metric_type;

			///图像类型， multi_array表示一个多维数组， 可以有不同的类型和维度， 这里是bgra类型， 2维的
			typedef multi_array<cv::bgra, 2>								image_type;

			shape_model_generator(const extent2 &extent, size_t contour_size) 
				: _extent(extent), _contour_size(contour_size), _threshold(0.9), _profile_radius(5){}

			///设置图像的文件夹和contour数据的文件夹， 文件遍历使用了准标准库的filesystem， 该函数将数据的
			///filename存起来， 但不加载数据。
			void set_data_dir(const string &image_dir, const string &contour_dir){
				std::tr2::sys::path image_dir_path(image_dir);
				std::tr2::sys::path contour_dir_path(contour_dir);
				ASSERT(std::tr2::sys::is_directory(image_dir_path) && 
					std::tr2::sys::is_directory(contour_dir_path), "The dir is not valid directory path.");


				_image_pathes.clear();
				_contour_pathes.clear();
				std::tr2::sys::recursive_directory_iterator end_iter;
				std::tr2::sys::recursive_directory_iterator iter(image_dir_path);
				for (; iter != end_iter; ++iter){
					auto path = iter->path();
					if (path.extension() != ".img") continue;
					_image_pathes.push_back(path);
				}

				for (sys::recursive_directory_iterator cnt_iter(contour_dir_path); cnt_iter != end_iter; ++cnt_iter){
					auto path = cnt_iter->path();
					if (path.extension() != ".cnt") continue;
					_contour_pathes.push_back(path);
				}

				ASSERT(_image_pathes.size() == _contour_pathes.size(), "The size is not match");
				for (size_t i = 0; i < _image_pathes.size(); ++i){
					ASSERT(_image_pathes[i].basename() == _contour_pathes[i].basename(), "The id not match.");
				}
			}

			///随机加载部分image和contour数据， 由于我的机器内存不够，不能全部加载。
			void random_load_data(size_t num = 500){
				ASSERT(num < _image_pathes.size(), "The num is out range.");

				_images.resize(num);
				_contours.resize(num);

				std::vector<bool> flags(_image_pathes.size());
				fill(flags, true);

				std::uniform_int_distribution<int> uniform_int(0, _image_pathes.size());
				std::mt19937 mt;
				mt.seed(time(nullptr));
				for (int i = 0; i < num;){
					auto index = uniform_int(mt);

					if (!flags[index]) continue;

					flags[index] = false;
					_images[i] = image_type(_extent);
					readmtx(_images[i], _image_pathes[index].string());
					_contours[i] = contour_type(_contour_size);
					read4raw(_contours[i], _contour_pathes[index].string());

					++i;
				}

				set_observed_matrix();

				std::cout << "=========Data Loaded=============" << std::endl;
			}
			
			///对contour数据进行pca分析，该函数为比较重要的步骤， 内有较详细注释
			statistics::pca_result contour_pca(){

				///将contour数据， 转换为可用于pca分析的，特征向量。
				Eigen::MatrixXd  feature_matrix(contour_size()<<1, sample_size());
				for (int i = 0; i < feature_matrix.cols(); ++i){
					feature_matrix.col(i) = detail::contour2feature_vector(_align_contours[i]);
				}

				///将无效的标记点随机赋值， 之前都是“0，0”
				std::uniform_int_distribution<int> uniform_dis(-100, 100);
				std::mt19937 mt;
				mt.seed(time(nullptr));
				for (int row = 0; row < feature_matrix.rows(); ++row){
					for (int col = 0; col < feature_matrix.cols(); ++col){
						if (!_mat_observed(row, col)){
							feature_matrix(row, col) = uniform_dis(mt);
						}
					}
				}

				///重构有特征缺失的数据， 该部分代码就不发了， 详细参考此论文（链接在下面）
				///http://sdrv.ms/18rFOsl
				statistics::missing_data_EM data_EM(feature_matrix, _mat_observed);
				feature_matrix = data_EM.em(50);

				///进行主成分分析， 比较常用， 不做详述
				_pca_result = statistics::pca(feature_matrix);
				std::cout << "===========================EigenValues====================" << std::endl;
				std::cout << _pca_result.eigen_values << std::endl;

				///将较小的特征值及其向量作为噪声祛除
				Eigen::VectorXd cumsum_energy = _pca_result.eigen_values;
				for (int i = 1; i < cumsum_energy.size(); ++i){
					cumsum_energy[i] += cumsum_energy[i-1];
				}

				size_t num = 0;
				for (num = 0; num < cumsum_energy.size(); ++num){
					cumsum_energy[num] /= cumsum_energy[cumsum_energy.size()-1];

					if (cumsum_energy[num] > _threshold) break;
				}
				_eigen_values.resize(num+1);
				_eigen_matrix.resize(_pca_result.eigen_vectors.rows(), num + 1);
				for (int i = 0; i < _eigen_values.size(); ++i){
					_eigen_values[i] = _pca_result.eigen_values[i];
					_eigen_matrix.col(i) = _pca_result.eigen_vectors.col(i);
				}

				std::cout << "==================Valid EigenValues=========================" << std::endl;
				std::cout << _eigen_values << std::endl;
			
				return _pca_result;
			}

			Eigen::VectorXd eigen_values() const {
				return _eigen_values;
			}

			Eigen::MatrixXd eigen_matrix() const{
				return _eigen_matrix;
			}

			size_t contour_size() const {
				return _contour_size;
			}

			size_t sample_size() const {
				return _contours.size();
			}

			///用于获取局部特征的剖线（面）半径
			int profile_radius() const { return _profile_radius; }
			void profile_radius(int radius) { _profile_radius = radius; }

			///align标记点， 以减少位移和旋转的影响
			void align(){
				_align_contours.resize(_contours.size());
				_align_trans.resize(_contours.size());
				_inverse_align_trans.resize(_contours.size());
				for (size_t i = 0; i < _contours.size(); ++i){
					_align_trans[i] = detail::align_contour(_contours[i]);
					_inverse_align_trans[i] = _align_trans[i].inverse();
					_align_contours[i] = contour_type(_contours[i].size());
					transform(_contours[i], _align_contours[i], _align_trans[i]);
				}
			}

			contour_type mean_contour()  {
				ASSERT(_pca_result.mean.size() != 0,"");
				return detail::feature_vector2contour(_pca_result.mean);
			}

			Eigen::VectorXd	mean_feature() const{
				return _pca_result.mean;
			}

			///获取asm的apperance模型， 也就是按contour的标记点处，按其法向量剖面获取一系列的梯度数据，并归一化
			void apperance_model_excute(){
				for (auto &cnt : _contours){
					cnt.refresh();
				}
				for (int feature_i = 0; feature_i < contour_size(); ++feature_i){
					Eigen::MatrixXd  feature_matrix(((_profile_radius<<1)+1) * 3, sample_size());
					feature_matrix.fill(numeric_limits<double>::epsilon());
					for (int cnt_index = 0; cnt_index < sample_size(); ++cnt_index){
						auto profile_grad = detail::profile_diff(_images[cnt_index], _contours[cnt_index][feature_i],
							_contours[cnt_index].normal()[feature_i], _profile_radius);

						for (int j = 0; j < profile_grad.size(); ++j){
							feature_matrix(j*3, cnt_index) += profile_grad[j].x;
							feature_matrix(j*3 +1, cnt_index) += profile_grad[j].y;
							feature_matrix(j*3+2, cnt_index) += profile_grad[j].z;
						}
					}

					for (size_t i = 0;  i < feature_matrix.cols(); ++i){
						feature_matrix.col(i).normalize();
					}

					metric_type gs;
					gs.attach(feature_matrix);

					_metric_types.push_back(gs);
				}

				std::cout << "=======================apperance mode finished=============" << std::endl;
			}

			void extent(const extent2 &extent)	{ _extent = extent; }
			extent2 extent() const { return _extent; }

			std::vector<metric_type> metrics() const{
				return _metric_types;
			}

			std::vector<contour_type> contours() const{
				return _contours;
			}

			std::vector<image_type> images() const{
				return _images;
			}

			std::vector<contour_type> align_contours() const{
				return _align_contours;
			}

			double eigen_values_threshold() const { return _threshold; }
			void eigen_values_threshold(double v) { _threshold = v; }

		protected:

			void set_observed_matrix(){
				_mat_observed.resize(contour_size()*2, sample_size());
				_mat_observed.setOnes();
				for (int i = 0; i < sample_size(); ++i){
					for (int j = 0; j < contour_size(); ++j){
						if (_contours[i][j] == point2d::zero){
							_mat_observed(2*j, i) = 0;
							_mat_observed(2*j+1, i) = 0;
						}
					}
				}
			}

		private:
			std::vector<sys::path>				_image_pathes;
			std::vector<sys::path>				_contour_pathes;

			std::vector<contour_type>			_contours;
			std::vector<image_type>				_images;
			extent2							_extent; 
			size_t							_contour_size;

			std::vector<contour_type>			_align_contours;
			std::vector<detail::align_transform>	_align_trans;
			std::vector<detail::inverse_align_transform> _inverse_align_trans;

			Eigen::MatrixXd					_mat_feature;
			statistics::pca_result          _pca_result;

			contour_type					_mean_contour;

			Eigen::VectorXd					_eigen_values;
			Eigen::MatrixXd					_eigen_matrix;

			std::vector<std::vector<std::vector<point3d>>> _gradient3d;

			std::vector<metric_type>					_metric_types;

			int								_profile_radius;

			double							_threshold;

			Eigen::Matrix<byte,-1,-1,0,-1,-1>				_mat_observed;
		};

	}

	///该类仅用于存储样本数据的平均模型和局部特征等， 此类拥有序列化和反序列化的能力， 以方便数据的存储和加载
	///使用了boost的serialization库， 该库是为数不多的c++序列化框架。
	class asm_apperance_model {
	public:
		typedef geometry::contour<point2d, std::vector<point2d>>			contour_type;
		typedef statistics::metric<Mahalanobis>						metric_type;

		asm_apperance_model(){}

		asm_apperance_model(const asm_apperance_model &rhs){
			_mean_feature = rhs._mean_feature;
			_metric_types = rhs._metric_types;
			_eigen_values = rhs._eigen_values;
			_eigen_matrix = rhs._eigen_matrix;
		}

		///以特征向量的形式获取平均模型
		void mean_feature(const Eigen::VectorXd &mean_contour)		{  _mean_feature  = mean_contour; }
		Eigen::VectorXd mean_feature() const						{ return _mean_feature; }

		///有76个马氏距离的度量器， 对应着76个标记点。
		std::vector<metric_type> &metrics()									{ return _metric_types; }
		const std::vector<metric_type> &metrics() const						{ return _metric_types; }

		///contour形状的特征值
		void eigen_values(const Eigen::VectorXd &eigen_values)		{ _eigen_values = eigen_values; }
		Eigen::VectorXd eigen_values() const						{ return _eigen_values; }

		///特征值得开方
		Eigen::VectorXd sqrt_eigen_values() const{
			return _eigen_values.cwiseSqrt();
		}

		///由特征向量构成的矩阵
		void eigen_matrix(const Eigen::MatrixXd &eigen_matrix)		{ _eigen_matrix = eigen_matrix; }
		Eigen::MatrixXd eigen_matrix() const						{ return _eigen_matrix; }

		///以contour的形式获取平均模型
		contour_type mean_contour() const{
			return detail::feature_vector2contour(_mean_feature);
		}

		///局部特征剖面的半径
		size_t	profile_radius() const{
			return ((_metric_types[0].mean().size()/3)-1)/2;
		}

		///实现序列化函数
		template <typename Archive>
		void serialize(Archive &ar, unsigned int){
			ar & boost::serialization::make_nvp("mean_feature",	_mean_feature);
			ar & boost::serialization::make_nvp("metrics", _metric_types);
			ar & boost::serialization::make_nvp("eigen_matrix",	_eigen_matrix);
			ar & boost::serialization::make_nvp("eigen_values", _eigen_values);
		}

	private:
		Eigen::VectorXd			_mean_feature;
		std::vector<metric_type>		_metric_types;
		Eigen::VectorXd		_eigen_values;
		Eigen::MatrixXd     _eigen_matrix;
	};

	/// asm算法的关键部分， 其根据局部特征调整轮廓， 而后用平均轮廓和轮廓子空间来约束其形状
	class active_shape_model{
	public:
		typedef multi_array<cv::bgra, 2>										image_type;
		typedef geometry::contour<point2d, std::vector<point2d>>				contour_type;

		active_shape_model(){}

		active_shape_model(const asm_apperance_model &model) : _apperance_model(model) , _m(2.0f), _search_radius(3){}

		///目标图像和其初始轮廓
		void attach(const image_type &image, const contour_type &contour){
			_image = image;
			_contour = contour;
			_align_contour = contour_type(contour.size());
		}

		///获取最终轮廓，  iter_num表示迭代次数
		contour_type contour(size_t  iter_num){
			for (int i = 0; i <iter_num; ++i){
				adjust_contour();
				match_contour();
			}

			return _contour;
		}

		///该参数用于限制 轮廓的残差在特征子空间里的值
		void eigen_scale(double v) { _m = v; }
		double eigen_scale() const { return _m; }

		/// 调整轮廓时的搜索半径
		int search_radius() const { return _search_radius; }
		void search_radius(int v) { _search_radius = v; }

	private:
		/// 根据局部特征调整轮廓
		void adjust_contour(){
			_contour.refresh();
			///获取轮廓的法向量
			auto normal = _contour.normal();
			for (int i = 0; i < _contour.size(); ++i){
				int k = _apperance_model.profile_radius();

				///返回一系列梯度
				auto profile_grad = detail::profile_diff(_image, _contour[i], normal[i], search_radius() + k);

				int moment = 0;
				double min_dis= numeric_limits<double>::max();
				Eigen::VectorXd	feature((k*2+1)*3);
				///填充最小值， 防止归一化时为0， 出错
				feature.fill(numeric_limits<double>::epsilon());
				for (int j = -search_radius(); j <= search_radius(); ++j){
					for (int n = 0; n < (2*k+1); ++n){
						feature[3*n] += profile_grad[j+search_radius()+n].x;
						feature[3*n+1] += profile_grad[j+search_radius()+n].y;
						feature[3*n+2] += profile_grad[j+search_radius()+n].z;
					}
					feature.normalize();

					///根据马式距离， 获取最小距离的点
					auto dis = _apperance_model.metrics()[i](feature);
					if (dis < min_dis){
						moment = j;
						min_dis = dis;
					}
				}

				///更新轮廓
				_contour[i] += static_cast<double>(moment)*normal[i];
			}
		}
		
		///将轮廓在子空间里，进行约束
		void match_contour(){
			_align_tran = detail::align_contour(_contour);
			_inv_align_tran = _align_tran.inverse();
			transform(_contour, _align_contour, _align_tran);

			Eigen::VectorXd contour_feature = detail::contour2feature_vector(_align_contour);
			Eigen::VectorXd mean_feature = _apperance_model.mean_feature();
			Eigen::VectorXd x_sub_u = contour_feature - mean_feature;

			Eigen::VectorXd b = _apperance_model.eigen_matrix().transpose() * x_sub_u;
			auto sqrt_eigen_values = _apperance_model.sqrt_eigen_values();
			auto max_b = sqrt_eigen_values * _m;

			for (int i = 0; i < b.size(); ++i){
				b[i] = max(min(b[i], max_b[i]), -max_b[i]);
			}

			contour_feature = _apperance_model.mean_feature() + _apperance_model.eigen_matrix() * b;
			_align_contour = detail::feature_vector2contour(contour_feature);
			transform(_align_contour, _contour, _inv_align_tran);
		}

	private:
		asm_apperance_model			_apperance_model;
		image_type					_image;
		contour_type				_contour;
		contour_type				_align_contour;

		detail::align_transform				_align_tran;
		detail::inverse_align_transform		_inv_align_tran;

		int								_search_radius;

		Eigen::Matrix<byte, -1, -1, 0, -1, -1> _mat_flag;

		double								_m;
	};
}}}
