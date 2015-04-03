#pragma once

#include <skynet/utility/iterator_facade.hpp>

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



}