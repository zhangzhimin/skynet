/******************************************************************************
Created By : Zhang Zhimin
Created On : 2012/9/11
Purpose    :  
********************************************************************************/
#pragma once

#include <skynet/utility/type_traits.hpp>
#include <skynet/core/array_express.hpp>
#include <skynet/core/point.hpp>

namespace skynet{

	template <typename M1, typename M2, typename Func>
	inline void transform(const M1 &lhs, M2 &rhs, Func func){
		std::transform(lhs.begin(), lhs.end(), rhs.begin(), func);
	}

	template <typename M1, typename M2>
	inline void deep_copy(const M1 &mat_src, M2 &mat_des){
		//mat_des.resize(mat_src.extent());
		ASSERT(mat_src.extent() == mat_des.extent(), "the extent should be the same.");
		for (size_t i = 0; i < mat_des.size(); ++i){
			mat_des[i] = mat_src[i];
		}
		// std::copy(mat_src.begin(), mat_src.end(), mat_des.begin());
	}  

	template <typename M>
	inline void fill(M &mat, const typename M::value_type &v){
		std::fill(mat.begin(), mat.end(), v);
	}

	template <typename M1, typename M2>
	inline void copy(const M1 &mat1, M2 &mat2){
		std::copy(mat1.begin(), mat1.end(), mat2.begin());
	}

	template <typename C>
	inline void bipolarize(const C &c){
		for (auto &e: c){
			if (e > 0)
				e = 1;
			if (e < 0)
				e = -1;
		}
	}

	namespace detail{
		template <typename C, typename Func>
		inline typename C::iterator order_push_front(C &list, const typename C::value_type &value, Func fun){
			auto it = list.begin();
			while (it != list.end()){
				if (fun(value) < fun(*it)) break;
				++it;
			}

			return list.insert(it, value);
		}
	}

	template <typename C>
	inline typename C::value_type sum(const C &c){
		return std::accumulate(c.begin(), c.end(), typename C::value_type(0));
	}

	template <typename C>
	inline typename C::value_type mean(const C &c){
		return sum(c)/c.size();
	}

	template <typename C>
	inline auto max_element(C &c)->decltype(c.begin()){
		return std::max_element(c.begin(), c.end());
	}

	template <typename C>
	inline auto min_element(C &c)->decltype(c.begin()){
		return std::min_element(c.begin(), c.end());
	}

	template <typename A, typename Fun>
	inline void for_each_index2(const array_express<A> &range, Fun fun){
		static_assert(A::dim == 2, "The dimension should be 2.");
		static_assert(is_same<typename unary_function_traits<Fun>::argument_type, index2>::value, 
			"The argument type must be index2.");

		for (int row = 0; row < range().height(); ++row){
			for (int col = 0; col < range().width(); ++col){
				fun(index2(col, row));
			}
		}
	}


}
