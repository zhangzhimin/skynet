

/******************************************************************************
Created By : Zhang Zhimin
Created On : 2012/9/19
Purpose    : Implement the mul-d conv
********************************************************************************/

#pragma once

#include <skynet\core\mask.hpp>
#include <skynet\core\lazy_evaluation.hpp>
#include <skynet\utility\type_traits.hpp>
#include <skynet\core\point.hpp>

#include <algorithm>
#include <functional>
#include <random>

#pragma message("The lambda has bug, the type can't be used")

namespace skynet{

	namespace detail{


		template <typename Mat, typename Mask>
		class conv_functor{
		public:
			typedef typename Mat::value_type			value_type;

			conv_functor(const Mat &mat, const Mask &mask): _mat(mat), _mask(mask),
				_rand(), _mt(std::rand())	{	}

			value_type operator()(size_t i) const{
				if (i >= (_mat.size() - _mask.max_offset()) || i < -_mask.min_offset()){
					return value_type(_rand(_mt));
				}

				value_type temp(0);
				for (int j = 0; j < _mask.size(); ++j){
					auto el = _mask[j];
					temp += el.weight * _mat[i + el.pos] ;
				}

				return temp;
			}

		private:
			Mat			_mat;
			Mask		_mask;
			std::uniform_int_distribution<int>  _rand;
			mutable std::mt19937						_mt;
		};


	}

	template <typename M, typename N>
	auto conv(const M &mat_src, N n)->lazy_array<M::dim, detail::conv_functor<M, N>>{
		static_assert(std::is_signed<typename M::value_type>::value, "The M value_type should be signed.");

		typedef typename M::value_type value_type;
		static const size_t dim = M::dim;
		n.attach(mat_src.extent());

		detail::conv_functor<M, N> fun(mat_src, n);

		return make_lazy_array(mat_src.extent(), fun);
	}

	/*template <typename M, typename N>
	auto conv(const M &mat_src, N n)->lazy_array<M::dim, function<typename M::value_type(const size_t &)>>{
	typedef typename M::value_type value_type;
	static const size_t dim = M::dim;
	n.attach(mat_src.extent());

	function<typename M::value_type(const size_t &)> fun = [=](const size_t &i)->value_type{
	typedef std::remove_const<std::remove_reference<decltype(mat_src[0])>::type>::type value_type;

	if (i >= (mat_src.size() - n.max_offset()) || i < -n.min_offset())  return value_type(0);

	value_type temp(0);
	for (int j = 0; j < n.size(); ++j){
	auto el = n[j];
	temp += el.weight * mat_src[i + el.pos] ;
	}

	return temp;
	};

	return make_lazy_array(mat_src.extent(), fun);
	}*/
};