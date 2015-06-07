#pragma once

namespace skynet {

	template <typename T>
	struct reference_of_buffer {
		typedef typename T::reference type;
	};

	template <typename T>
	struct const_reference_of_buffer {
		typedef typename T::const_reference	type;
	};

	template <typename T>
	struct reference_of_buffer<T *> {
		typedef T &		type;
	};

	template <typename T>
	struct const_reference_of_buffer<T *> {
		typedef const T &  type;
	};

	template <typename T>
	struct iterator_of_buffer {
		typedef typename T::iterator type;
	};

	template <typename T>
	struct const_iterator_of_buffer {
		typedef typename T::const_iterator type;
	};

	template <typename T>
	struct iterator_of_buffer<T *> {
		typedef T *	type;
	};

	template <typename T>
	struct const_iterator_of_buffer<T *> {
		typedef const T * type;
	};
}