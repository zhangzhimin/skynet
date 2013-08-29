******************************************************************************
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

	template <typename M1, typename M2, typename Func>
	class lazy_binary_array: array_express<lazy_binary_array<M1, M2, Func> >{
	private:
		typedef skynet::binary_function_traits<Func>		functor_type_traits;
	public:
		IS_SAME_DIMENTION(M1, M2);
		typedef lazy_binary_array										type;

		typedef Func											functor_type;
		typedef typename functor_type_traits::result_type		result_type;

		static_assert(!std::is_reference<result_type>::value,
			"the functor return value cloundn't be reference type");

		typedef lazy_binary_array                              type;
		typedef M1												array1_type;
		typedef M2												array2_type;

		static const size_t                                     dim = array1_type::dim;
		typedef result_type										value_type;
		typedef value_type 									    reference;
		typedef value_type  									const_reference;
		typedef typename M1::extent_type						extent_type;
		typedef typename M1::index_type							index_type;
		typedef index_iterator<type>                            iterator;
		typedef index_iterator<const type>                      const_iterator;

		extent_type extent() const{
			return _lhs.extent();
		}

		size_t size() const{
			return _lhs.size();
		}

		value_type get_value(const index_type &index) const{
			ASSERT(detail::bound_check(index, extent()), "the index is out side");
			return _func(_lhs.get_value(index), _rhs.get_value(index));
		}		

		value_type operator[](const size_t &i) const{
			return _func(_lhs[i], _rhs[i]);
		}

		value_type operator[](const index_type &index) const {
			return this->operator[](index2size_t(index, extent()));
		}

		typename std::enable_if<dim == 2, reference>::type operator()(int x, int y){
			return _func(lhs(x,y), rhs(x,y));
		}

		typename std::enable_if<dim == 2, size_t>::type width() const{
			return extent().x;
		}

		typename std::enable_if<dim == 2, size_t>::type height() const{
			return extent().y;
		}

		typename std::enable_if<dim == 2, size_t>::type col_size() const{
			return extent().x;
		}

		typename std::enable_if<dim == 2, size_t>::type row_size() const{
			return extent().y;
		}

		iterator begin()                                    { return iterator(this, 0); }
		iterator end()                                      { return iterator(this, size()); }
		const_iterator begin() const            { return const_iterator(const_cast<type *>(this), 0); }
		const_iterator end() const              { return const_iterator(const_cast<type *>(this), size()); }


	public:
		lazy_binary_array(const M1 &lhs, const M2 &rhs, const Func &func)
			: _lhs(lhs), _rhs(rhs), _func(func) {
				ASSERT( _lhs.size() == _rhs.size(), "the size is not the same.");
		}

		const M1            _lhs;
		const M2            _rhs;
		Func				_func;
	};


	template <typename M, typename Func>
	class lazy_unary_array: public array_express<lazy_unary_array<M, Func>>{
	private:
		typedef skynet::unary_function_traits<Func>		functor_type_traits;
	public:
		typedef lazy_unary_array										type;
		static const size_t										dim = M::dim;
		typedef Func											functor_type;
		typedef typename functor_type_traits::result_type		result_type;
		typedef typename functor_type_traits::argument_type     argument_type;

		static_assert(!std::is_reference<result_type>::value, 
			"the functor return value cloundn't be reference type");	

		typedef M												array_type;

		typedef result_type										value_type;
		typedef value_type  									reference;
		typedef value_type									    const_reference;
		typedef typename M::extent_type							extent_type;
		typedef typename M::index_type							index_type;
		typedef index_iterator<type>                            iterator;
		typedef index_iterator<const type>                      const_iterator;

		extent_type extent() const{
			return _mat->extent();
		}

		value_type get_value(const index_type &index) const{
			return _func(_mat->get_value(index));
		}		

		value_type operator[](const size_t &offset) const{
			return _func((*_mat)[offset]);
		}

		typename std::enable_if<dim == 2, const_reference>::type operator()(int x, int y) const{
			return _func((*_mat)(x,y));
		}

		typename std::enable_if<dim == 2, size_t>::type width() const{
			return extent().x;
		}

		typename std::enable_if<dim == 2, size_t>::type height() const{
			return extent().y;
		}
		
		const_iterator begin() const     { return const_iterator(const_cast<type *>(this), 0); }
		const_iterator end() const       { return const_iterator(const_cast<type *>(this), size()); }

		size_t size() const{
			return _mat->size();
		}

		lazy_unary_array(const M &mat, const Func &func)
			: _mat(&mat), _func(func) {}

	private:
		const M *const	                                        _mat;
		functor_type		_func;
	};


	template <typename size_t dim_, typename Func>
	class lazy_array: public array_express<lazy_array<dim_, Func>>{
	private:
		typedef unary_function_traits<Func>						functor_type_traits;
		static_assert(std::is_same<typename functor_type_traits::argument_type, size_t>::value, 
			"The func should has only a size_t type parameters");
	public:
		typedef	lazy_array											type;
		static const size_t									        dim = dim_;
		typedef typename functor_type_traits::result_type			value_type;
		typedef typename functor_type_traits::argument_type         argument_type;
		typedef point<int, dim>										extent_type;
		typedef point<int, dim>										index_type;
		typedef index_iterator<type>                                iterator;
		typedef index_iterator<const type>                          const_iterator;
		typedef value_type                                          reference;
		typedef value_type                                          const_reference;

		lazy_array(const extent_type &extent, Func fun): _extent(extent), _fun(fun)	{}

		value_type get_value(const index_type &index) const{
			auto offset = index2size_t(index, _extent);
			return _fun(offset);
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
		Func																_fun;
	};


	template <typename size_t dim_, typename Func>
	class cache_array: public array_express<lazy_array<dim_, Func>>{
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
		typedef index_iterator<type>                            iterator;
		typedef index_iterator<const type>                      const_iterator;

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

	template <typename M1, typename M2, typename Func>
	const lazy_binary_array<M1, M2, Func> apply(const M1 &lhs, const M2 &rhs, Func func){
		return lazy_binary_array<M1, M2, Func>(lhs, rhs, func);
	}

	template <typename M, typename Func>
	const lazy_unary_array<M, Func> apply(const M &mat, Func func){
		return lazy_unary_array<M, Func>(mat, func);
	}


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



	template <typename M1, typename M2, typename Func>
	auto lazy_binary_operate(const M1 &mat1, const M2 &mat2, Func fun)
		->lazy_array<M1::dim, function<typename M2::value_type (const size_t &)>>
	{
		ASSERT(mat1.extent() == mat2.extent(), "The mat1 and mat2 should have the same extent.");
		function<typename M2::value_type (const size_t &)> func = [=](const size_t &i){
			return fun(mat1[i],mat2[i]);
		};

		return make_lazy_array(mat1.extent(), func);
	}

	template <typename M1, typename M2>
	auto operator+(const array_express<M1> &mat_exp1, const array_express<M2> &mat_exp2)
		->lazy_array<M1::dim, function<typename M2::value_type (const size_t &)>>
	{
		return lazy_binary_operate(mat_exp1(), mat_exp2(), std::plus<typename M2::value_type>());
	}

	template <typename M1, typename M2>
	auto operator-(const array_express<M1> &mat_exp1, const array_express<M2> &mat_exp2)
		->lazy_array<M1::dim, function<typename M2::value_type (const size_t &)>>
	{
		return lazy_binary_operate(mat_exp1(), mat_exp2(), std::minus<typename M2::value_type>());
	}

	//the * operation support the scale value *  point..
	/*template <typename M1, typename M2>
	auto operator*(const array_express<M1> &mat_exp1, const array_express<M2> &mat_exp2)
	->lazy_array<M1::dim, function<typename M2::value_type (const size_t &)>>
	{
	return lazy_binary_operate(mat_exp1(), mat_exp2(), std::multiplies<typename M1::value_type>());
	}*/

	/*template <typename M>
	auto operator*(const float &scale, const array_express<M> &mat_exp)
	->lazy_array<M::dim, function<typename M::value_type(const size_t &)>>
	{
	auto mat = mat_exp();

	function<typename M::value_type(const size_t &)> fun =
	[mat, scale](const size_t &i){
	return scale * mat[i];
	};

	return make_lazy_array(mat_exp().extent(), fun);
	}
	*/

	template <typename size_t D, typename R_FUN, typename W_FUN>
	class event_array : public array_express<event_array<D, R_FUN, W_FUN>>{
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
		typedef index_iterator<type>                                    iterator;
		typedef index_iterator<const type>                              const_iterator;
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


	template <typename M,typename V>
	const lazy_unary_array<M, detail::type_cast<typename M::value_type, V>> lazy_cast(const M &mat, V){
		return apply(mat, detail::type_cast<typename M::value_type, V>());
	}
}
