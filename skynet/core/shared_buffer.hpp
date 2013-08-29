#pragma once

#include <skynet/utility/iterator_facade.hpp>

#include <atomic>



namespace skynet{

	namespace detail{

		///引用计数器，
		class shared_buffer_count{
		public:
			shared_buffer_count() : _p_counter(new std::atomic<size_t>(0U)), _deletor([](){}) {}

			template <typename Del>
			shared_buffer_count(Del deletor) 
				: _p_counter(new std::atomic<size_t>), _deletor(deletor){
					_p_counter->store(1);
			}

			shared_buffer_count(const shared_buffer_count &sc)  
				: _p_counter(sc._p_counter), _deletor(sc._deletor) {
					++(*_p_counter);
			}

			shared_buffer_count& operator=(const shared_buffer_count &sc){
				_p_counter = sc._p_counter;
				_deletor = sc._deletor;
				++(*_p_counter);
				return *this;
			}

			~shared_buffer_count() {

				--(*_p_counter);
				if (!(*_p_counter)){
					ASSERT(_deletor, "Null deletor");
					_deletor();
				}
			}

			size_t user_count() const {
				return (*_p_counter);
			}

		private:
			std::atomic<size_t>*	_p_counter;
			function<void (void)>   _deletor;
		};
	}

	///智能buffer， 内存安全的数组
	template <typename T, typename Alloc = std::allocator<T>>
	class shared_buffer{
	public:
		typedef	shared_buffer							self_type;
		static const size_t								dim = 1;
		typedef T										value_type;
		typedef value_type &							reference;
		typedef const value_type &						const_reference;
		//typedef value_type *							iterator;
		//typedef const value_type *						const_iterator;
		typedef index_iterator<self_type>				iterator;
		typedef index_iterator<const self_type>			const_iterator;
		//define size_type  for ublas
		typedef size_t									size_type;
		typedef std::ptrdiff_t							difference_type;

		shared_buffer() : _size(1), _p(_alloc.allocate(1)){}

		shared_buffer(size_t  size) : _size(size), _p(_alloc.allocate(size)){
			if (!std::has_trivial_constructor<value_type>::value){
				auto temp = _p;
				for (size_t i = 0; i < _size; ++i){
					_alloc.construct(temp++);
				}

				auto p = _p;
				auto size = _size;

				_ref_counter = detail::shared_buffer_count([p, size](){
					Alloc alloc;
					auto temp = p;
					for (size_t i = 0; i < size; ++i){
						alloc.destroy(temp++);
					}

					alloc.deallocate(p, size);
				});
			}
			else{
				auto p = _p;
				auto size = _size;
				_ref_counter = detail::shared_buffer_count([p, size](){
					Alloc alloc;
					alloc.deallocate(p, size);
				});
			}
		}
		
		shared_buffer(size_t  size, value_type init) {
			shared_buffer temp(size);
			fill(temp, init);
			std::swap(*this, temp);
		}


		shared_buffer(const shared_buffer &buffer) 
			: _p(buffer._p), _size(buffer._size), _alloc(buffer._alloc), _ref_counter(buffer._ref_counter){}

		shared_buffer(shared_buffer &&rhs)
			:_p(rhs._p), _size(rhs._size), _alloc(rhs._alloc), _ref_counter(rhs._ref_counter){}

		shared_buffer& operator=(shared_buffer &&rhs){
			swap(rhs);
			return *this;
		}

		shared_buffer& operator=(const shared_buffer &rhs){
			shared_buffer temp(rhs);
			std::swap(*this, temp);
			return *this;
		}

		void resize(size_t size, value_type v = value_type(0)){
			shared_buffer temp(size);
			std::swap(*this, temp);
		}

		reference operator[](size_t i){ 
			ASSERT(_p != nullptr, "the buffer ptr is null");
			ASSERT(i < _size, "the i is out range.");
			return  _p[i];
		}

		const_reference operator[](size_t i) const{ 
			ASSERT(_p != nullptr, "the buffer ptr is null.");
			ASSERT(i < _size, "the i is out range.");
			return  _p[i];
		}

		iterator begin()								{ return iterator(this, 0); }
		iterator end()									{ return iterator(this, _size); }
		const_iterator begin() const					{ return const_iterator(const_cast<self_type *>(this), 0); }
		const_iterator end() const						{ return const_iterator(const_cast<self_type *>(this), _size); }


		bool empty() const{
			return size() == 0;
		}

		size_t size() const{
			return _size;
		}

		value_type* const get(){
			return _p;
		}

		const value_type * const get() const{
			return _p;
		}

		void swap(shared_buffer &rhs) {
			std::swap(this->_alloc, rhs._alloc);
			std::swap(this->_p, rhs._p);
			std::swap(this->_size, rhs._size);
			std::swap(this->_ref_counter, rhs._ref_counter);
		}

	private:
		T *	 											_p;
		size_t											_size;
		Alloc											_alloc;
		detail::shared_buffer_count						_ref_counter;
	};

}
