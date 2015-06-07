/******************************************************************************
Created By : Zhang Zhimin
Created On : 20129/6
Purpose    : Implements the std::array
********************************************************************************/
#pragma once

#include <skynet/core/point.hpp>
#include <skynet/core/expression_types.hpp>
#include <skynet/core/shared_buffer.hpp>
#include <skynet/core/common.hpp>
#include <skynet/core/algorithm.hpp>
#include <skynet/core/type_traits.hpp>
#include <skynet/core/iterator.hpp>

#include <unordered_map>

namespace skynet{

	template <typename T, size_t D, typename B = shared_buffer<T>>
	class multi_array: public array_expression<multi_array<T, D, B>>{
	public:
		typedef T												value_type; 
		static	const size_t									dim = D;
		typedef B												buffer_type;
		typedef typename reference_of_buffer<B>::type			reference;
		typedef typename const_reference_of_buffer<B>::type     const_reference;
		typedef typename iterator_of_buffer<B>::type			iterator;
		typedef typename const_iterator_of_buffer<B>::type		const_iterator;
		typedef point<ptrdiff_t, dim>				index_type;
		typedef point<size_t, dim>					extent_type;

		multi_array() : _buffer(), _extent() {}

		multi_array(int cols, int rows) {
			multi_array temp(extent2(cols, rows));
			swap(temp);
		}

		multi_array(extent_type extent)	: _extent(extent), _buffer(whole_size(extent)){
			_slide[0] = _extent[0];
			for (int i = 1; i < dim; ++i){				
				_slide[i] = _extent[i] * _slide[i - 1];
			} 
		}

		multi_array(extent_type extent, const buffer_type &buffer)
			: _extent(extent), _buffer(buffer){
			///TODO: pointer buffer has not size method.
				//ASSERT(size(extent) == buffer.size(), "the size is not match");
		}

		multi_array &operator=(const multi_array &rhs){
			_extent = rhs._extent;
			_buffer = rhs._buffer;
			_slide = rhs._slide;

			return *this;
		}

		const_reference operator()(const index_type &index) const{
			size_t pos = index[0];

			for (int i = 1; i < dim; ++i){	
				pos += index[i] * _slide[i - 1];
			} 
			return _buffer[pos];
		}

		reference operator()(const index_type &index){
			size_t pos = index[0];

			for (int i = 1; i < dim; ++i){	
				pos += index[i] * _slide[i - 1];
			} 
			return _buffer[pos];
		}

		typename std::enable_if<dim == 2, reference>::type operator()(int x, int y){
			return _buffer[x+_slide[0]*y];
		}

		typename std::enable_if<dim == 2, const_reference>::type operator()(int x, int y)const{
			return _buffer[x+_slide[0]*y];
		}

		typename std::enable_if<dim == 2, size_t>::type width() const{
			return _extent.x;
		}

		typename std::enable_if<dim == 2, size_t>::type height() const{
			return _extent.y;
		}

		typename std::enable_if<dim == 2, size_t>::type col_size() const{
			return _extent.x;
		}

		typename std::enable_if<dim == 2, size_t>::type row_size() const{
			return _extent.y;
		}

		const_reference operator[](const size_t &i) const              { return _buffer[i]; }
		reference operator[](const size_t &i)                          { return _buffer[i]; }

		iterator begin()								{ return skynet::begin(_buffer); }
		const_iterator begin() const					{ return skynet::begin(_buffer); }

		iterator end()									{ return skynet::end(_buffer, size()); }
		const_iterator end() const						{ return skynet::end(_buffer, size()); }

		extent_type extent() const						{ return _extent; }
		size_t		size() const						{ return whole_size(_extent); }

		void swap(multi_array &rhs){
			std::swap(_buffer, rhs._buffer);
			std::swap(_extent, rhs._extent);
			std::swap(_slide, rhs._slide);
		}

		const buffer_type	buffer() const							{ return _buffer; }
		buffer_type buffer()										{ return _buffer; };

	protected:
		buffer_type			_buffer;
		extent_type			_extent;
		extent_type			_slide;
	};


	//template <typename T, size_t D>
	//class raw_array: public array_expression<raw_array<T, D>>{
	//public:
	//	typedef raw_array						self_type;
	//	typedef T								value_type;
	//	static	const size_t					dim = D;
	//	typedef typename value_type &			reference;
	//	typedef typename const value_type &     const_reference;

	//	typedef typename detail::index_iterator<self_type>			iterator;
	//	typedef typename detail::index_iterator<const self_type>	const_iterator;

	//	typedef typename B::const_iterator		const_iterator;
	//	typedef point<ptrdiff_t, dim>				index_type;
	//	typedef point<size_t, dim>					extent_type;

	//private:
	//	T*					_p_raw;
	//	extent_type			_extent;
	//	extent_type			_slide;
	//};


	template <typename M, byte bit_mask_>
	class bit_array: public array_expression<bit_array<M,bit_mask_>>{
	public:
		template <bool is_const>
		class bit;
		static_assert(std::is_arithmetic<typename M::value_type>::value, "the value_type should be arithmetic.");
		static_assert(bit_mask_ != 0, "the bit mask should not be zero.");

		typedef bit_array                              type;
		typedef M                                       array_container;

		static const size_t                             dim = M::dim;
		static const size_t                             bit_mask = bit_mask_;
		typedef typename M::index_type                  index_type;
		typedef typename M::extent_type                 extent_type;

		typedef bool                                    value_type;
		typedef bit<false>                              reference;
		typedef bit<true>                               const_reference;
		typedef detail::index_iterator<type>                    iterator;
		typedef detail::index_iterator<const type>              const_iterator;

		template <bool is_const>
		class bit{
		public:
			typedef typename std::conditional<is_const, const byte &, byte &>::type   Ref;

			operator bool() const {
				return (_ref_value&bit_mask) == bit_mask;
			}
            
			bit &operator=(const bool &b){
				static_assert(!std::is_const<Ref>::value, "the operator= is just for non const.");
				if(b){
					_ref_value |= bit_mask;
				}
				else{
					_ref_value &= ~bit_mask;
				}

				return *this;
			}

			explicit bit(Ref v): _ref_value(v) {}

		private:
			Ref                _ref_value;
		};

		bit_array(){}

		const_reference operator[](const size_t &i) const      { return const_reference(_mat[i]); }
		reference operator[](const size_t &i)                  { return reference(_mat[i]); }

		const_reference get_value(const index_type &index) const {
			return (*this)[index2size_t(index, extent())];
		}
		reference get_value(const index_type &index){
			return (*this)[index2size_t(index, extent())];
		}

		void attach(const M &mat){
			_mat = mat;
		}

		void set_value(const index_type &index, const value_type &value){
			(*this)[index2size_t(index, extent())] = value;
		}

		void resize(const extent_type &extent){
			ASSERT(_mat.extent() == extent, "the extent is not match.");
		}

		const_iterator begin() const                    { return const_iterator(const_cast<type *>(this), 0); }
		const_iterator end() const                      { return const_iterator(const_cast<type *>(this), size()); }
		iterator begin()                                { return iterator(this, 0); }
		iterator end()                                  { return iterator(this, size()); }

		extent_type extent() const                  { return _mat.extent(); }
		size_t  size() const                            { return _mat.size(); }
	private:
		M                   _mat;
	};

	template <typename T, size_t D, typename Hash = boost::hash<size_t>>
	class hash_array: array_expression<hash_array<T, D, Hash>>{
	public:
		typedef hash_array                             type;
		static const size_t                             dim = D;

		typedef T                                       value_type;
		typedef T &                                     reference;
		typedef const T &                               const_reference;

		typedef point<ptrdiff_t, 3>                 	 index_type;
		typedef point<size_t, 3>                  		extent_type;

		hash_array(const T &value) : _default_value(value) {}

		value_type operator[](const size_t &i) const{
			auto it = _hash_table.find(i);
			if (it == _hash_table.end())  
				return _default_value;
			else
				return (*it).second;
		}

		void set_value(const size_t &i, const value_type &v){
			auto it = _hash_table.find(i);
			if (it == _hash_table.end())
				_hash_table.insert(std::make_pair(i, v));
			else
				(*it).second = v;
		}

	private:
		T _default_value;
		std::unordered_map<size_t, value_type, Hash>  _hash_table;
	};

	template <typename M>
	class clamp_array : array_expression<clamp_array<M>>{
	public:
		typedef clamp_array							type;
		typedef typename M::value_type					value_type;
		typedef typename M::reference					reference;
		typedef typename M::const_reference				const_reference;
		typedef typename M::index_type					index_type;
		typedef typename M::extent_type					extent_type;
		const static size_t								dim = M::dim;

		clamp_array(const M &mat) : _mat(mat) {} 

		extent_type extent() const {
			return _mat.extent();
		}

		const_reference get_value(const index_type &index) const{
			return _mat.get_value(bound_index(index, extent()));
		}

	private:
		M	_mat;
	};

	template <typename M>
	const clamp_array<M> make_clamp_array(const M &mat){
		return clamp_array<M>(mat);
	}

	template <typename M>
	auto clone(const M &mat)->multi_array<typename M::value_type, M::dim>{
		multi_array<typename M::value_type, M::dim> temp_mat(mat.extent());
		copy(mat, temp_mat);
		return temp_mat;
	}

	template <typename M>
	void set_value(M &mat, const std::vector<size_t> &seeds, const typename M::value_type &v){
		for (auto it = seeds.begin(); it != seeds.end(); ++it){
			mat[*it] = v;
		}
	}


	typedef multi_array<byte, 2>						mask2d;
	typedef multi_array<byte, 3>						mask3d;

	typedef multi_array<short, 3>						ct_volume;
	typedef multi_array<short, 2>						ct_array;

	typedef multi_array<byte, 3>						array3b;
	typedef multi_array<float, 3>						array3f;
	typedef multi_array<int, 3>							array3i;
	typedef multi_array<short, 3>						array3s;
	typedef multi_array<double, 3>						array3d;

	typedef multi_array<byte, 2>						array2b;
	typedef multi_array<float, 2>						array2f;
	typedef multi_array<int, 2>							array2i;
	typedef multi_array<short, 2>						array2s;
	typedef multi_array<double, 2>						array2d;
}
