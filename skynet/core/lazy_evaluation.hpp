/******************************************************************************
Created By : Zhang Zhimin
Created On : 2012/9/18
Purpose    : implemte the lazy caculate
********************************************************************************/

#pragma once

#include <skynet/core/array.hpp>
#include <skynet/utility/type_traits.hpp>
#include <skynet/utility/iterator_facade.hpp>

#pragma message("The std::function<T>  has the performance problem, if neccessary, refector it")

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
		typedef point<int, dim>										extent_type;
		typedef point<int, dim>										index_type;
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

		const_reference operator()(const index_type &index) const {
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


	template <typename size_t dim_, typename Func>
	class cache_array: public array_expression<lazy_array<dim_, Func>>{
	private:
		typedef unary_function_traits<Func>                        functor_type_traits;
		static_assert(std::is_same<typename functor_type_traits::argument_type, size_t>::value,
			"The Func should has only a size_t type parameters");
	public:
		typedef cache_array                                    type;
		static const size_t                                     dim = dim_;
		typedef typename functor_type_traits::result_type       value_type;
		typedef const value_type &                              reference;
		typedef const value_type &                              const_reference;
		typedef point<int, dim>                                 extent_type;
		typedef point<int, dim>                                 index_type;
		typedef detail::index_iterator<type>                            iterator;
		typedef detail::index_iterator<const type>                      const_iterator;

		cache_array() {}

		cache_array(const extent_type &extent, const Func &fun): _extent(extent), _fun(fun){
			_flag.resize(extent);
			_flag.set_all_zero();
			_buffer.resize(extent);
		}

		const_reference operator[](const size_t &i) const{
			if (_flag[i]){
				return _buffer[i];
			}

			_buffer[i] = _fun(i);
			_flag[i] = 1;
			return _buffer[i];
		}

		extent_type extent() const                          { return _extent; }

		const_iterator begin() const            { return const_iterator(const_cast<type *>(this), 0); }
		const_iterator end() const              { return const_iterator(const_cast<type *>(this), size()); }
		iterator begin()                        { return iterator(this, 0); }
		iterator end()                          { return iterator(this, size()); }

		size_t size() const      { 
			return std::accumulate(_extent.begin(), _extent.end(), 1, std::multiplies<int>());
		}

	private:
		extent_type                                                     _extent;
		Func                                                            _fun;
		std::array<byte, dim>												_flag;
		std::array<value_type, dim>											_buffer;
	};


	template <typename size_t dim, typename Func>
	auto make_lazy_array(const point<int, dim> &extent, Func fun)->const lazy_array<dim, Func>{
		return lazy_array<dim, Func>(extent, fun);
	}

	template <typename size_t dim, typename Func>
	auto make_cache_array(const point<int, dim> &extent, Func fun)->cache_array<dim, Func>{
		return cache_array<dim, Func>(extent, fun);
	}

	template <typename LM>
	auto cache4lazy(const LM &lazy_mat)->cache_array<LM::dim, function<typename LM::value_type(const size_t &)>>{
		function<typename LM::value_type(const size_t &)> fun =[=](const size_t &i){
			return lazy_mat[i];
		};
		return make_cache_array(lazy_mat.extent(), fun);
	}


	template <typename M, typename Func>
	auto apply(const M &mat, Func func) {
		return make_lazy_array(mat.extent(), [=](size_t i) {
			return func(mat[i]);
		});
	}

	template <typename M1, typename M2, typename Func>
	auto apply(const M1 &lhs, const M2 &rhs, Func func) {
		ASSERT_SAME_EXTENT(lhs, rhs);
		return make_lazy_array(lhs.extent(), [=](const size_t &i) {
			return func(lhs[i], rhs[i]);
		});
	}

	template <typename M1, typename M2>
	auto operator+(const array_expression<M1> &mat_exp1, const array_expression<M2> &mat_exp2){
		auto mat1 = mat_exp1();
		auto mat2 = mat_exp2();
		return make_lazy_array(mat1.extent(), [mat1, mat2](const size_t &i) {
			return mat1[i] + mat2[i];
		});
	}

	template <typename M1, typename M2>
	auto operator-(const array_expression<M1> &mat_exp1, const array_expression<M2> &mat_exp2){
		auto mat1 = mat_exp1();
		auto mat2 = mat_exp2();
		return make_lazy_array(mat1.extent(), [mat1, mat2](const size_t &i) {
			return mat1[i] - mat2[i];
		});
	}

	template <typename m1, typename m2>
	auto operator*(const array_expression<m1> &mat_exp1, const array_expression<m2> &mat_exp2)
	{
		auto mat1 = mat_exp1();
		auto mat2 = mat_exp2();
		return make_lazy_array(mat1.extent(), [mat1, mat2](const size_t &i) {
			return mat1[i] * mat2[i];
		});
	}

	template <typename M>
	auto operator*(float scale, const array_expression<M> &mat_exp){
		auto mat = mat_exp();
		return make_lazy_array(mat.extent(), [=](const size_t &i) {
			return scale * mat[i];
		});
	}

	template <typename size_t D, typename R_FUN, typename W_FUN>
	class event_array : public array_expression<event_array<D, R_FUN, W_FUN>>{
	private:
		typedef unary_function_traits<R_FUN>                    read_function_traits;
		binary_function_traits<W_FUN>                           write_function_traits;
	public:
		/*static_assert(std::is_same<typename read_function_traits::argument_type, size_t>::value, 
		"The func should has only a size_t type parameters");*/
		/*       static_assert(std::is_same<typename write_function_traits::argument_type, size_t>::value, 
		"The func should has only a size_t type parameters");
		*/
		typedef event_array                                            type;
		typedef typename read_function_traits::result_type              value_type;
		typedef value_type                                              reference;
		typedef value_type                                              const_reference;
		typedef detail::index_iterator<type>                                    iterator;
		typedef detail::index_iterator<const type>                              const_iterator;
		static const size_t                                             dim = D;
		typedef point<int, dim>                                         extent_type;
		typedef point<int, dim>                                         index_type;

		event_array() : _read_fun(nullptr), _write_fun(nullptr){}

		event_array(const extent_type &extent, R_FUN rf, W_FUN wf) 
			: _extent(extent), _read_fun(rf), _write_fun(wf){}

		class rw_event{
		public:
			rw_event(const size_t &i, const R_FUN &rf, const W_FUN &wf): _i(i), _rf(rf), _wf(wf){}

			operator value_type() const{
				return _rf(_i);
			}

			rw_event &operator=(const value_type &v){
				_wf(_i, v);

				return *this;
			}

		private:
			const size_t &_i;
			const R_FUN  &_rf;
			const W_FUN  &_wf;
		};

		rw_event operator[](const size_t &i){
			return rw_event(i, _read_fun, _write_fun);
		}
		const rw_event operator[](const size_t &i) const {
			return rw_event(i, _read_fun, _write_fun);
		}

		const_iterator begin() const { return const_iterator(const_cast<type *>(this), 0); }
		const_iterator end() const   { return const_iterator(const_cast<type *>(this), size()); }
		iterator begin()             { return iterator(this, 0); }
		iterator end()               { return iterator(this, size()); }

		extent_type extent() const{
			return _extent;
		}
		size_t size() const{
			return std::accumulate(_extent.begin(), _extent.end(), 1, std::multiplies<int>());
		}

	private:
		R_FUN           _read_fun;
		W_FUN           _write_fun;
		point<int, D>   _extent;
	};

	template <typename size_t D, typename RF, typename WF>
	auto make_event_array(const point<int, D> &extent, const RF rf, const WF wf)->event_array<D, RF, WF>{
		return event_array<D, RF, WF>(extent, rf, wf);
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
	auto cast(const ArrayType &mat) {
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
