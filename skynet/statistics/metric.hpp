#pragma once

#include <skynet/config.hpp>
#include <skynet/ublas.hpp>
#include <skynet/utility/tag.hpp>


#include <Eigen/Core>

namespace skynet{namespace statistics{



	template <typename M>
	class metric;

	template <>
	class metric<Mahalanobis> : public unary_function<ublas::vector<double>, double>{
	public:
		typedef ublas::matrix<double>					matrix_type;
		typedef ublas::vector<double>					vector_type;
		
		metric(){}

		metric(const metric &rhs) 
			: _mean(rhs._mean), _covariance(rhs._covariance), _inv_cov(rhs._inv_cov){}

		void attach(matrix_type  feature_matrix){
			_mean = ublas::zero_vector<double>(feature_matrix.size1());
			for (int i = 0; i < feature_matrix.size2(); ++i){
				_mean += ublas::column(feature_matrix, i);
			}

			_mean /= feature_matrix.size2();

			for (int i = 0; i < feature_matrix.size2(); ++i){
				ublas::column(feature_matrix, i) -= _mean;
			}
		
			_covariance = ublas::prod(feature_matrix, ublas::trans(feature_matrix));
			_covariance /= feature_matrix.size2();
			_inv_cov = 
		}

		result_type	 operator()(const argument_type &x) const{
			auto x_sub_mean = x - _mean;
			auto dis = ublas::trans(x_sub_mean) * _inv_cov * x_sub_mean;
			return dis;
		}

		void mean(const vector_type &mean)		{ _mean = mean; }
		vector_type mean() const				{ return _mean; }

		template <typename Archive>
		void serialize(Archive &ar, const unsigned int &){
			ar & boost::serialization::make_nvp("mean_feature", _mean);
			ar & boost::serialization::make_nvp("covariance", _covariance);
			ar & boost::serialization::make_nvp("inv_covariance", _inv_cov);
		}

	private:
		vector_type						_mean;
		matrix_type						_covariance;	
		matrix_type						_inv_cov;
	};


}}
