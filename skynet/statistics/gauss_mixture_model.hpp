/******************************************************************************
Created By : Zhang Zhimin
Created On : 2013/4/10
Purpose    :  
********************************************************************************/

#pragma once

#include <skynet\statistics\k_mean.hpp>
#include <skynet\core\matrix.hpp>


#include <skynet\utility\serialization.hpp>

namespace skynet{ namespace statistics{

	template <typename C>
	class gauss_mixture_model{
	public:
		typedef typename C::value_type value_type;
		typedef typename value_type	   point_type;
		static const size_t point_dim = get_dim<point_type>::value;
		//static_assert(std::is_floating_point<value_type>::value, "should be float point type");
		gauss_mixture_model(){}

		gauss_mixture_model(const C &samples, const std::vector<gauss_function<float>> &gausses, const std::vector<float> &weights)
			: m_samples(samples), m_gausses(gausses), m_weights(weights){ 
				ASSERT(m_gausses.size() == m_weights.size(), "");
				ASSERT(is_equal(std::accumulate(m_weights.begin(), m_weights.end(), 0.01), 1.0f, 1e-1), "");
		}

		gauss_mixture_model(const C &samples) : m_samples(samples), m_gausses(), m_weights(){

		}

	private:
		void exception(){
			for (size_t i = 0; i < m_samples.size(); ++i){

				auto accum = 0.0f;
				for ( size_t k = 0; k < m_gausses.size(); ++k){
					auto density = m_gausses[k](m_samples[i]);
					m_exception.set_value(index2(i, k), density);
					accum += m_weights[k] * density;
				}

				if (accum < numeric_limits<float>::epsilon()){
					accum = numeric_limits<float>::epsilon();
				}

				for (size_t k = 0; k < m_gausses.size(); ++k){
					m_exception.set_value(index2(i, k), 
						m_weights[k]*m_exception.get_value(index2(i, k))/accum);
				}
			}
		}

		void maximization(){
			for (size_t g = 0; g < m_gausses.size(); ++g){
				//weight mean
				float sum = numeric_limits<float>::epsilon();
				float mean(0);
				for (size_t i = 0; i < m_samples.size(); ++i){
					auto density = m_exception.get_value(index2(i, g));
					sum += density;
					mean += density * m_samples[i];
				}
				m_weights[g] = sum / m_samples.size();
				mean /= sum;
				m_gausses[g].mean(mean);

				//cov
				//matrix2f mat_cov(extent2(point_dim, point_dim));
				//mat_cov.set_all_zero();

				float cov = 0.0f;
				for (size_t i = 0; i < m_samples.size(); ++i){
					cov += sqr(m_samples[i]-mean) * m_exception.get_value(index2(i,g));
				}				
				cov /= sum;
				m_gausses[g].covariance(cov);
			}
		}

	public:
		void excute(size_t iter_num){
			m_exception.resize(index2(m_samples.size(), m_gausses.size()));

			//ASSERT(is_equal(std::accumulate(m_weights.begin(), m_weights.end(), 0.0f), 1.0f, 0.1f), "");


			while (iter_num-- > 0){
				exception();
				maximization();
			}
		}

		template <typename Archive>
		void serialize(Archive &ar, unsigned int){
			ar & boost::serialization::make_nvp("gausses", m_gausses);
			ar & boost::serialization::make_nvp("weights", m_weights);
		}
				

	private:
		std::vector<gauss_function<float>>				m_gausses;
		std::vector<float>						m_weights;
		C                                   m_samples;
		matrix2f							m_exception;
	};

}}