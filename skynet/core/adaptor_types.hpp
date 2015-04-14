#pragma once

#include <skynet/core/expression_types.hpp>
#include <skynet/utility/iterator_facade.hpp>
#include <skynet/core/point.hpp>

namespace skynet {

	template <typename Base>
	class iterator_adaptor {
	public:
		typedef Base											base_type;
		typedef detail::index_iterator<base_type>               iterator;
		typedef detail::index_iterator<const base_type>         const_iterator;

		const base_type &operator()() const { return *static_cast<const base_type *>(this); }
		base_type &operator()() { return *static_cast<base_type *>(this); }

		iterator begin() { return iterator(&((*this)()), 0); }
		iterator end() { return iterator(&((*this)()), (*this)().size()); }
		const_iterator begin() const { return const_iterator(const_cast<base_type *>(&((*this)())), 0); }
		const_iterator end() const { return const_iterator(const_cast<base_type *>(&((*this)())), (*this)().size()); }
	};

	//template <typename Base, typename T, size_t dim, bool hasSubscript>
	//class multi_array_adaptor: public iterator_adaptor<Base>, public matrix_expression<Base>{
	//public:
	//	typedef Base									base_type;
	//	typedef T										value_type;
	//	typedef value_type &							reference;
	//	typedef const value_type &						const_reference;
	//
	//	typedef point<ptrdiff_t, dim>					index_type;
	//	typedef point<size_t, dim>						extent_type;

	//	size_t size() const {
	//		return return std::accumulate((*this)().extent().begin(), (*this)().extent().end(), 1, std::multiplies<size_t>());
	//	}

	//	typename std::enable_if<hasSubscript
	//};
}