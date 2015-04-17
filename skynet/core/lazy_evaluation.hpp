/******************************************************************************
Created By : Zhang Zhimin
Created On : 2012/9/18
Purpose    : implemte the lazy caculate
********************************************************************************/

#pragma once

#include <skynet/core/array.hpp>
#include <skynet/utility/type_traits.hpp>
#include <skynet/utility/iterator_facade.hpp>

namespace skynet{
	using std::function;

	template <typename size_t dim_, typename Func>
	class lazy_array: public array_expression<lazy_array<dim_, Func>>{
	private:
		typedef unary_function_traits<Func>							functor_type_traits;
		static_assert(std::is_same<typename functor_type_traits::argument_type, size_t>::value, 
			"The func should has only a size_t type parameters");
	public:
		typedef	lazy_array											type;
		static const size_t									        dim = dim_;
		typedef typename functor_type_traits::result_type			value_type;
		typedef typename functor_type_traits::argument_type         argument_type;
		typedef point<size_t, dim>										extent_type;
		typedef point<ptrdiff_t, dim>										index_type;
		typedef detail::index_iterator<type>                                iterator;
		typedef detail::index_iterator<const type>                          const_iterator;
		typedef value_type                                          reference;
		typedef value_type                                          const_reference;

		lazy_array(const extent_type &extent, Func fun): _extent(extent), _fun(fun)	{
			_slide[0] = _extent[0];
			for (int i = 1; i < dim; ++i) {
				_slide[i] = _extent[i] * _slide[i - 1];
			}
		}

		value_type operator()(const index_type &index) const {
			size_t pos = index[0];

			for (int i = 1; i < dim; ++i) {
				pos += index[i] * _slide[i - 1];
			}
			return (*this)[pos];
		}


		value_type operator[](const size_t &i) const{
			return _fun(i);
		}

		iterator begin()                    { return iterator(this, 0); }
		iterator end()                      { return iterator(this, size()); }
		const_iterator begin() const        { return const_iterator(const_cast<type *>(this), 0); }
		const_iterator end() const          { return const_iterator(const_cast<type *>(this), size()); }

		extent_type extent() const				{ return _extent; }

		size_t	size() const{
			return std::accumulate(_extent.begin(), _extent.end(), 1, std::multiplies<int>());
		}

	private:
		extent_type						                                    _extent;
		extent_type															_slide;
		Func																_fun;
	};

	template <typename size_t dim, typename Func>
	inline auto make_lazy_array(const point<size_t, dim> &extent, Func fun)->const lazy_array<dim, Func>{
		return lazy_array<dim, Func>(extent, fun);
	}

	template <typename M, typename Func>
	inline auto apply(const M &mat, Func func) {
		return make_lazy_array(mat.extent(), [=](size_t i) {
			return func(mat[i]);
		});
	}

	template <typename M1, typename M2, typename Func>
	inline auto apply(const M1 &lhs, const M2 &rhs, Func func) {
		ASSERT_SAME_EXTENT(lhs, rhs);
		return make_lazy_array(lhs.extent(), [=](const size_t &i) {
			return func(lhs[i], rhs[i]);
		});
	}

	template <typename M1, typename M2>
	inline auto operator+(const array_expression<M1> &mat_exp1, const array_expression<M2> &mat_exp2){
		auto mat1 = mat_exp1();
		auto mat2 = mat_exp2();
		return make_lazy_array(mat1.extent(), [mat1, mat2](const size_t &i) {
			return mat1[i] + mat2[i];
		});
	}

	template <typename M1, typename M2>
	inline auto operator-(const array_expression<M1> &mat_exp1, const array_expression<M2> &mat_exp2){
		auto mat1 = mat_exp1();
		auto mat2 = mat_exp2();
		return make_lazy_array(mat1.extent(), [mat1, mat2](const size_t &i) {
			return mat1[i] - mat2[i];
		});
	}

	template <typename m1, typename m2>
	inline auto operator*(const array_expression<m1> &mat_exp1, const array_expression<m2> &mat_exp2)
	{
		auto mat1 = mat_exp1();
		auto mat2 = mat_exp2();
		return make_lazy_array(mat1.extent(), [mat1, mat2](const size_t &i) {
			return mat1[i] * mat2[i];
		});
	}

	template <typename M>
	inline auto operator*(float scale, const array_expression<M> &mat_exp){
		auto mat = mat_exp();
		return make_lazy_array(mat.extent(), [=](const size_t &i) {
			return scale * mat[i];
		});
	}

	namespace detail{
		template <typename A, typename R>
		struct type_cast: public unary_function<A, R>{
			R operator()(const A &a) const{
				return static_cast<R>(a);
			}
		};
	}

	template <typename ValueType, typename ArrayType>
	inline auto cast(const ArrayType &mat) {
		return apply(mat, detail::type_cast<typename ArrayType::value_type, ValueType>());
	}


	template <typename ArrayType>
	inline auto evalute(const ArrayType &data) {
		multi_array<typename ArrayType::value_type, ArrayType::dim> re(data.extent());
		copy(data, re);
		return re;
	}

	template <typename ArrayType>
	inline auto operator*(const array_expression<ArrayType> &me) {
		return evalute(me());
	}
}
