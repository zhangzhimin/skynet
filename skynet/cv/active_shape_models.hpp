#pragma once


///skynet���ҿ�����һ�����ڼ�����Ӿ��ͻ���ѧϰ�Ŀ⣬ ���ڻ���Щ������δ���
///Eigen��һ����ֵ�����⣨����ֵ�������ܣ��� ��Ϊʱ�����ޣ��;߱�һ���Ѷȣ� ������û�й����Լ�����ֵ������
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

		///align�任����仯�� �ñ任�Ƚ�����ת�����ƽ�Ʊ任
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

		///align�任�� �ñ任�Ƚ���ƽ�ƣ�����ת��
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

		//��contour����alig�任�ĺ����� �䷵��һ��align�任�� ���ñ任����contour�����ɻ��align���contour��
		align_transform align_contour(const geometry::contour<point2d, std::vector<point2d>> &contour){
			auto mean_offset = -mean(contour);

			std::vector<double>	rotations(contour.size());
			for (size_t i = 0; i < contour.size(); ++i){
				rotations[i] = std::atan2(contour[i].y, contour[i].x);
			}

			auto mean_angle = -mean(rotations);
			return detail::align_transform(mean_offset, mean_angle);
		}

		///��contour�ı�ǵ����꣬ ת��Ϊһ������������
		geometry::contour<point2d, std::vector<point2d>> feature_vector2contour(const Eigen::VectorXd &v){
			auto size = v.size()/2;
			geometry::contour<point2d, std::vector<point2d>> contour(v.size()/2);
			for (size_t i = 0; i < size; ++i){
				contour[i].x = v[i<<1];
				contour[i].y = v[(i<<1)+1];
			}

			return contour;
		}

		///������������ ת��Ϊcontour
		Eigen::VectorXd	contour2feature_vector(const geometry::contour<point2d, std::vector<point2d>> &contour){
			Eigen::VectorXd  feature_vector(contour.size()<<1);
			for (int i = 0; i < contour.size(); ++i){
				feature_vector[i<<1] = contour[i].x;
				feature_vector[(i<<1)+1] = contour[i].y;
			}

			return feature_vector;
		}

		///��ȡͼ��ľֲ������� imageΪͼ��Դ�� posΪcontour�ϵ�һ����ǵ㣬 normalΪȥ�������� kΪ���ߵİ뾶�� 
		///��������һ��2k+1���ݶ����飬 shared_buffer��һ���ڴ氲ȫ�����飬 ʹ�������ü���
		shared_buffer<point3d> profile_diff(const multi_array<cv::bgra, 2>  &image, const point2d &pos, 
			const point2d &normal, int k){
				shared_buffer<point2d>   profile_pos(2*k + 3);
				for (int i = -(k+1); i <= (k+1); ++i){
					profile_pos[k+1 + i] = pos + (double)i * normal;
				}

				auto d_image = lazy_cast(image, point3d());
				//��ͼ����и�˹�˲��� ������ʱû��ʹ��
				auto filter_image = gaussian_filter(d_image);
				//��ͼ�����ǯλ����ֹ����ʱ��ָ��Խ��
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

		///��������asm model�� �ڼ佫�����contour��image�� ��contour����pca�� ��ȡimage�ľֲ�������
		class shape_model_generator{
		public:

			///contour���ͣ� point2d��ʾcontour�ı�ǵ����ͣ� std::vector<point2d>��ʾ�洢contour������
			typedef geometry::contour<point2d, std::vector<point2d>>		contour_type;
			typedef statistics::metric<Mahalanobis>					metric_type;

			///ͼ�����ͣ� multi_array��ʾһ����ά���飬 �����в�ͬ�����ͺ�ά�ȣ� ������bgra���ͣ� 2ά��
			typedef multi_array<cv::bgra, 2>								image_type;

			shape_model_generator(const extent2 &extent, size_t contour_size) 
				: _extent(extent), _contour_size(contour_size), _threshold(0.9), _profile_radius(5){}

			///����ͼ����ļ��к�contour���ݵ��ļ��У� �ļ�����ʹ����׼��׼���filesystem�� �ú��������ݵ�
			///filename�������� �����������ݡ�
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

			///������ز���image��contour���ݣ� �����ҵĻ����ڴ治��������ȫ�����ء�
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
			
			///��contour���ݽ���pca�������ú���Ϊ�Ƚ���Ҫ�Ĳ��裬 ���н���ϸע��
			statistics::pca_result contour_pca(){

				///��contour���ݣ� ת��Ϊ������pca�����ģ�����������
				Eigen::MatrixXd  feature_matrix(contour_size()<<1, sample_size());
				for (int i = 0; i < feature_matrix.cols(); ++i){
					feature_matrix.col(i) = detail::contour2feature_vector(_align_contours[i]);
				}

				///����Ч�ı�ǵ������ֵ�� ֮ǰ���ǡ�0��0��
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

				///�ع�������ȱʧ�����ݣ� �ò��ִ���Ͳ����ˣ� ��ϸ�ο������ģ����������棩
				///http://sdrv.ms/18rFOsl
				statistics::missing_data_EM data_EM(feature_matrix, _mat_observed);
				feature_matrix = data_EM.em(50);

				///�������ɷַ����� �Ƚϳ��ã� ��������
				_pca_result = statistics::pca(feature_matrix);
				std::cout << "===========================EigenValues====================" << std::endl;
				std::cout << _pca_result.eigen_values << std::endl;

				///����С������ֵ����������Ϊ�������
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

			///���ڻ�ȡ�ֲ����������ߣ��棩�뾶
			int profile_radius() const { return _profile_radius; }
			void profile_radius(int radius) { _profile_radius = radius; }

			///align��ǵ㣬 �Լ���λ�ƺ���ת��Ӱ��
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

			///��ȡasm��apperanceģ�ͣ� Ҳ���ǰ�contour�ı�ǵ㴦�����䷨���������ȡһϵ�е��ݶ����ݣ�����һ��
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

	///��������ڴ洢�������ݵ�ƽ��ģ�ͺ;ֲ������ȣ� ����ӵ�����л��ͷ����л��������� �Է������ݵĴ洢�ͼ���
	///ʹ����boost��serialization�⣬ �ÿ���Ϊ�������c++���л���ܡ�
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

		///��������������ʽ��ȡƽ��ģ��
		void mean_feature(const Eigen::VectorXd &mean_contour)		{  _mean_feature  = mean_contour; }
		Eigen::VectorXd mean_feature() const						{ return _mean_feature; }

		///��76�����Ͼ���Ķ������� ��Ӧ��76����ǵ㡣
		std::vector<metric_type> &metrics()									{ return _metric_types; }
		const std::vector<metric_type> &metrics() const						{ return _metric_types; }

		///contour��״������ֵ
		void eigen_values(const Eigen::VectorXd &eigen_values)		{ _eigen_values = eigen_values; }
		Eigen::VectorXd eigen_values() const						{ return _eigen_values; }

		///����ֵ�ÿ���
		Eigen::VectorXd sqrt_eigen_values() const{
			return _eigen_values.cwiseSqrt();
		}

		///�������������ɵľ���
		void eigen_matrix(const Eigen::MatrixXd &eigen_matrix)		{ _eigen_matrix = eigen_matrix; }
		Eigen::MatrixXd eigen_matrix() const						{ return _eigen_matrix; }

		///��contour����ʽ��ȡƽ��ģ��
		contour_type mean_contour() const{
			return detail::feature_vector2contour(_mean_feature);
		}

		///�ֲ���������İ뾶
		size_t	profile_radius() const{
			return ((_metric_types[0].mean().size()/3)-1)/2;
		}

		///ʵ�����л�����
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

	/// asm�㷨�Ĺؼ����֣� ����ݾֲ��������������� ������ƽ�������������ӿռ���Լ������״
	class active_shape_model{
	public:
		typedef multi_array<cv::bgra, 2>										image_type;
		typedef geometry::contour<point2d, std::vector<point2d>>				contour_type;

		active_shape_model(){}

		active_shape_model(const asm_apperance_model &model) : _apperance_model(model) , _m(2.0f), _search_radius(3){}

		///Ŀ��ͼ������ʼ����
		void attach(const image_type &image, const contour_type &contour){
			_image = image;
			_contour = contour;
			_align_contour = contour_type(contour.size());
		}

		///��ȡ����������  iter_num��ʾ��������
		contour_type contour(size_t  iter_num){
			for (int i = 0; i <iter_num; ++i){
				adjust_contour();
				match_contour();
			}

			return _contour;
		}

		///�ò����������� �����Ĳв��������ӿռ����ֵ
		void eigen_scale(double v) { _m = v; }
		double eigen_scale() const { return _m; }

		/// ��������ʱ�������뾶
		int search_radius() const { return _search_radius; }
		void search_radius(int v) { _search_radius = v; }

	private:
		/// ���ݾֲ�������������
		void adjust_contour(){
			_contour.refresh();
			///��ȡ�����ķ�����
			auto normal = _contour.normal();
			for (int i = 0; i < _contour.size(); ++i){
				int k = _apperance_model.profile_radius();

				///����һϵ���ݶ�
				auto profile_grad = detail::profile_diff(_image, _contour[i], normal[i], search_radius() + k);

				int moment = 0;
				double min_dis= numeric_limits<double>::max();
				Eigen::VectorXd	feature((k*2+1)*3);
				///�����Сֵ�� ��ֹ��һ��ʱΪ0�� ����
				feature.fill(numeric_limits<double>::epsilon());
				for (int j = -search_radius(); j <= search_radius(); ++j){
					for (int n = 0; n < (2*k+1); ++n){
						feature[3*n] += profile_grad[j+search_radius()+n].x;
						feature[3*n+1] += profile_grad[j+search_radius()+n].y;
						feature[3*n+2] += profile_grad[j+search_radius()+n].z;
					}
					feature.normalize();

					///������ʽ���룬 ��ȡ��С����ĵ�
					auto dis = _apperance_model.metrics()[i](feature);
					if (dis < min_dis){
						moment = j;
						min_dis = dis;
					}
				}

				///��������
				_contour[i] += static_cast<double>(moment)*normal[i];
			}
		}
		
		///���������ӿռ������Լ��
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
