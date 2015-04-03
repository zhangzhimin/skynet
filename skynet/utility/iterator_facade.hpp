/******************************************************************************
Created By : Zhang Zhimin
Created On : 2012/11/9
Purpose    :  
********************************************************************************/

#pragma once

#include <boost/iterator/iterator_adaptor.hpp>
#include <skynet/utility/type_traits.hpp>


namespace skynet {
	namespace detail {
		namespace mpl = boost::mpl;

		template <typename index_type_>
		class index_iterator :public boost::iterator_facade<
			index_iterator<index_type_>,
			typename index_type_::value_type,
			std::random_access_iterator_tag,
			typename conditional<is_const<index_type_>::value,
			typename index_type_::const_reference,
			typename index_type_::reference>::type>
		{
		public:
			typedef index_type_	   							 index_type;
			typedef typename conditional<is_const<index_type_>::value,
				typename index_type_::const_reference,
				typename index_type_::reference>::type		reference;
			typedef ptrdiff_t								difference_type;

			index_iterator() : _index(nullptr), _pos(0) {}
			index_iterator(index_type *p_n, size_t pos) : _index(p_n), _pos(pos) {	}
			index_iterator(const index_iterator &it) : _index(it._index), _pos(it._pos) { }

			index_iterator &operator=(const index_iterator &it) {
				this->_index = it._index;
				this->_pos = it._pos;

				return *this;
			}

		private:
			friend class boost::iterator_core_access;
			reference dereference() const { return (*_index)[_pos]; }
			reference dereference() { return (*_index)[_pos]; }
			void increment() { ++_pos; }
			void decrement() { --_pos; }

			inline void advance(difference_type d) {
				_pos += d;
			}

			difference_type distance_to(const index_iterator& it) const { return it._pos - _pos; }

			bool equal(const index_iterator& it) const {
				return _pos == it._pos;
			}
		private:
			index_type			        *_index;
			size_t			             _pos;
		};

	/*	template <typename Array>
		class slice_iterator :public boost::iterator_facade<
			slice_iterator<Array>,
			typename Array::value_type,
			std::random_access_iterator_tag,
			typename Array::referecne>
		{
		public:
			typedef	Array									data_iterator;
			typedef typename Array::reference				reference;
			typedef ptrdiff_t								difference_type;

			slice_iterator() : _index(nullptr), _pos(0) {}
			slice_iterator(index_type *p_n, size_t pos) : _index(p_n), _pos(pos) {	}
			slice_iterator(const slice_iterator &it) : _index(it._index), _pos(it._pos) { }

			slice_iterator &operator=(const slice_iterator &it) {
				this->_index = it._index;
				this->_pos = it._pos;

				return *this;
			}

		private:
			friend class boost::iterator_core_access;
			reference dereference() const { return (*_index)[_pos]; }
			reference dereference() { return (*_index)[_pos]; }
			void increment() { ++_pos; }
			void decrement() { --_pos; }

			inline void advance(difference_type d) {
				_pos += d;
			}

			difference_type distance_to(const slice_iterator& it) const { return it._pos - _pos; }

			bool equal(const slice_iterator& it) const {
				return _pos == it._pos;
			}
		private:
			Array							_array;
			size_t			             _pos;
		};*/
	}
}
