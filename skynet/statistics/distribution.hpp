/******************************************************************************
Created By : Zhang Zhimin
Created On : 2012/11/22
Purpose    :  
********************************************************************************/

#pragma once

#include <skynet\config.hpp>
#include <skynet\utility\math.hpp>
#include <skynet\utility\serialization.hpp>
#include <skynet\utility\eigen_lib.hpp>

#include <Eigen\Core>
#include <Eigen\LU>

namespace skynet{namespace statistics{


	template <typename T>
	class gauss_function;

	//template <typename T>
	template <>
	class gauss_function<float> : public unary_function<float, float>{
		static_assert(std::is_floating_point<float>::value, "the normal function template T should be float point type.");
	public:
		typedef float			value_type;
		typedef value_type		point_type;

		gauss_function() {}

		gauss_function(value_type mean, value_type sigma): m_mean(mean), m_sigma(sigma) {
			init();
		}

		void mean(value_type mean)			{ m_mean = mean; init(); }
		value_type mean() const				{ return m_mean; }
		void sigma(value_type sigma)		{ m_sigma = sigma; init(); }
		value_type sigma() const			{ return m_sigma; }
		void covariance(value_type cov)		{ m_sigma = sqrt(cov); init(); }
		value_type covariance() const		{ return sqr(m_sigma); }

		value_type operator()(const value_type & x) const{
			return	exp(-sqr(x-m_mean)/m_2sqr_sigma) / m_sigma_sqrt_2pi;
		}

		template <typename Archive>
		void serialize(Archive &ar, unsigned int){
			ar & boost::serialization::make_nvp("mean", m_mean);
			ar & boost::serialization::make_nvp("sigma", m_sigma);

			init();
		}

	protected:
		void init() {
			m_sigma_sqrt_2pi = m_sigma * sqrt(2 * PI);
			m_2sqr_sigma = 2 * sqr(m_sigma);
		}

	private:
		value_type		m_mean;
		value_type		m_sigma;

		value_type		m_sigma_sqrt_2pi;
		value_type		m_2sqr_sigma;
	};

	template <>
	class gauss_function<double> : public unary_function<double, double>{
		static_assert(std::is_floating_point<double>::value, "the normal function template T should be double point type.");
	public:
		typedef double			value_type;
		typedef value_type		point_type;

		gauss_function(value_type mean, value_type sigma): m_mean(mean), m_sigma(sigma) {
			init();
		}

		void mean(value_type mean)			{ m_mean = mean; init(); }
		value_type mean() const				{ return m_mean; }
		void sigma(value_type sigma)		{ m_sigma = sigma; init(); }
		value_type sigma() const			{ return m_sigma; }
		value_type covariance() const		{ return sqr(m_sigma); }



		value_type operator()(const value_type & x) const{
			return	exp(-sqr(x-m_mean)/m_2sqr_sigma) / m_sigma_sqrt_2pi;
		}

	protected:
		void init() {
			m_sigma_sqrt_2pi = m_sigma * sqrt(2 * PI);
			m_2sqr_sigma = 2 * sqr(m_sigma);
		}

	private:
		value_type		m_mean;
		value_type		m_sigma;

		value_type		m_sigma_sqrt_2pi;
		value_type		m_2sqr_sigma;
	};

	template <>
	class gauss_function<Eigen::VectorXd> : public std::unary_function<Eigen::VectorXd, double>{
	public:
		typedef Eigen::VectorXd			value_type;
		typedef Eigen::MatrixXd			matrix_type;

		gauss_function(){}

		gauss_function(const gauss_function & g): m_mean(g.m_mean), m_covariance(g.m_covariance), m_inv_cov(g.m_inv_cov), m_scale(g.m_scale){}

		void attach(matrix_type  feature_matrix){
			m_mean.resize(feature_matrix.rows());
			m_mean.setZero();
			for (int i = 0; i < feature_matrix.cols(); ++i){
				m_mean += feature_matrix.col(i);
			}

			m_mean /= feature_matrix.cols();

			for (int i = 0; i < feature_matrix.cols(); ++i){
				feature_matrix.col(i) -= m_mean;
			}

			m_covariance = feature_matrix * feature_matrix.transpose();
			m_covariance /= feature_matrix.cols();
			m_inv_cov = m_covariance.inverse();

			m_scale = sqrt(m_covariance.determinant()) * sqrt(std::pow((2*PI), m_mean.rows()));
		}

		result_type	 operator()(const argument_type &x) const{
			auto x_sub_mean = x - m_mean;
			auto dis = -0.5 * (x_sub_mean.transpose() * m_inv_cov * x_sub_mean);
			double dis_d = dis.value();
			return std::exp(dis_d)/m_scale;
		}

		void mean(const value_type &mean)		{ m_mean = mean; }
		value_type mean() const					{ return m_mean; }

		template <typename Archive>
		void serialize(Archive &ar, const unsigned int &){
			 ar & boost::serialization::make_nvp("mean_feature", m_mean);
			 ar & boost::serialization::make_nvp("covariance", m_covariance);
			 ar & boost::serialization::make_nvp("inv_covariance", m_inv_cov);
			 ar & boost::serialization::make_nvp("scale", m_scale);
		}

	private:
		value_type						m_mean;

		matrix_type						m_covariance;	
		matrix_type						m_inv_cov;
		double							m_scale;

	};


}}