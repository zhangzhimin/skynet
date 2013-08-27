#pragma once

#include <skynet\utility\iterator_facade.hpp>

#include <stdexcept>
#include <array>

namespace skynet{

#pragma region neighbor

	//   *                                
	// * o *
	//   *
	//It's the diamand_neighbors, star is neighbor, o is origin.

	template <typename size_t dim_>
	class diamand_neighbor{
	public:
		typedef diamand_neighbor                type;
		static const size_t			            dim =  dim_;
		static const size_t			            neighbor_num = dim * 2;

		typedef int								value_type;
		typedef const int &						reference;
		typedef const int &						const_reference;
		typedef	point<int, dim>				    extent_type;
		typedef index_iterator<type>		    iterator;
		typedef index_iterator<const type>      const_iterator;


		diamand_neighbor(){}

		diamand_neighbor &operator=(const type &rhs){
			_extent = rhs._extent;
			_offsets = rhs._offsets;
			return *this;
		}

		const_iterator begin() const              { return const_iterator(const_cast<type *>(this), 0); }
		const_iterator end() const                { return const_iterator(const_cast<type *>(this), neighbor_num); }
		iterator begin()                          { return iterator(this, 0); }
		iterator end()                            { return iterator(this, neighbor_num); }

		diamand_neighbor(const extent_type &size): _extent(size){
			_offsets[0] = 1;
			_offsets[0+dim] = -1;
			for (int i = 1; i < dim; ++i){
				_offsets[i] = _offsets[i-1] * _extent[i-1];
				_offsets[i + dim] = -_offsets[i];
			}
		}

		void attach(const extent_type &size){
			_extent = size;
			_offsets[0] = 1;
			_offsets[0+dim] = -1;
			for (int i = 1; i < dim; ++i){
				_offsets[i] = _offsets[i-1] * _extent[i-1];
				_offsets[i + dim] = -_offsets[i];
			}
		}

		const int &operator[](const size_t &index) const{
#ifdef DEUBG
			//make sure the diamand is initialized;
			assert(_offsets[0] == 1);
#endif
			return _offsets[index];
		}

		size_t size()const                        { return neighbor_num; }

		int max_offset() const                    { return (*this)[dim-1]; }
		int min_offset() const                    { return (*this)[2 * dim - 1]; }

	private:
		extent_type	_extent;
		std::array<int, neighbor_num>	_offsets;
	};

	//    //* * *
	//    //* o *
	//    //* * *
	//    //It's the square neighbors, the stars are neighbors, 0 is origin.
	//    template <typename size_t dim_>
	//    class square_neighbor{
	//    public:
	//        typedef square_neighbor                type;
	//        static const size_t			            dim =  dim_;
	//        static const size_t			            neighbor_num = dim * 2;
	//
	//        typedef int								value_type;
	//        typedef const int &						reference;
	//        typedef const int &						const_reference;
	//        typedef	point<int, dim>				    extent_type;
	//        typedef index_iterator<type>		    iterator;
	//        typedef index_iterator<const type>      const_iterator;
	//
	//
	//        square_neighbor(){}
	//
	//        square_neighbor &operator=(const type &rhs){
	//            _extent = rhs._extent;
	//            _offsets = rhs._offsets;
	//            return *this;
	//        }
	//
	//        const_iterator begin() const              { return const_iterator(const_cast<type *>(this), 0); }
	//        const_iterator end() const                { return const_iterator(const_cast<type *>(this), neighbor_num); }
	//        iterator begin()                          { return iterator(this, 0); }
	//        iterator end()                            { return iterator(this, neighbor_num); }
	//
	//        square_neighbor(const extent_type &size): _extent(size){
	//            _offsets[0] = 1;
	//            _offsets[0+dim] = -1;
	//            for (int i = 1; i < dim; ++i){
	//                _offsets[i] = _offsets[i-1] * _extent[i-1];
	//                _offsets[i + dim] = -_offsets[i];
	//            }
	//        }
	//
	//        void attach(const extent_type &size){
	//            _extent = size;
	//            _offsets[0] = 1;
	//            _offsets[0+dim] = -1;
	//            for (int i = 1; i < dim; ++i){
	//                _offsets[i] = _offsets[i-1] * _extent[i-1];
	//                _offsets[i + dim] = -_offsets[i];
	//            }
	//        }
	//
	//        const int &operator[](const size_t &index) const{
	//#ifdef DEUBG
	//            //make sure the diamand is initialized;
	//            assert(_offsets[0] == 1);
	//#endif
	//            return _offsets[index];
	//        }
	//
	//        size_t size()const                        { return neighbor_num; }
	//
	//        int max_offset() const                    { return (*this)[dim-1]; }
	//        int min_offset() const                    { return (*this)[2 * dim - 1]; }
	//
	//    private:
	//        extent_type	_extent;
	//        std::array<int, neighbor_num>	_offsets;
	//    };

	/*                                 *

	*   *   *

	*
	It's the vertex_neighbors
	*/
	template <typename size_t dim_>
	class vertex_neighbor{
	public:
		typedef vertex_neighbor                             type;
		static	const size_t			                    dim =  dim_;
		typedef point<int, dim>				                extent_type;
		static  const size_t			                    neighbor_num = extent_type::dim * 2 + 1;

		typedef typename int								value_type;
		typedef typename const int &						reference;
		typedef typename const int &						const_reference;
		typedef index_iterator<type>				        iterator;
		typedef index_iterator<const type>                  const_iterator;


		vertex_neighbor() {}

		vertex_neighbor(const int &r) : _radius(r){}

		vertex_neighbor &operator=(const vertex_neighbor &rhs){
			this->rhs._radius;
			this->_extent = rhs._extent;
			this->_offsets = rhs._offsets;

			return *this;
		}

		const int &operator  [](const size_t &index) const{
#ifdef DEUBG
			//make sure the diamand is initialized;
			assert(_offsets[0] == 1);
#endif
			return _offsets[index];
		}

		void attach(const extent_type &size) {
			_extent = size;
			_offsets[0] = _radius;
			_offsets[0+dim] = -_radius;
			_offsets[neighbor_num - 1] = 0;
			for (int i = 1; i < dim; ++i){
				_offsets[i] = _offsets[i-1] * _extent[i-1];
				_offsets[i + dim] = -_offsets[i];
			}
		};

		int get_radius(){
			return _radius;
		}

		const_iterator begin() const              { return const_iterator(const_cast<type *>(this), 0); }
		const_iterator end() const                { return const_iterator(const_cast<type *>(this), neighbor_num); }
		iterator begin()                          { return iterator(this, 0); }
		iterator end()                            { return iterator(this, neighbor_num); }

		int max_offset() const            { return (*this)[dim - 1]; }
		int min_offset() const            { return (*this)[2 * dim - 1]; }

		size_t size() const                  { return neighbor_num; }

	private:
		int		                        _radius;
		extent_type	                    _extent;
		std::array<int, neighbor_num>   _offsets;
	};

	template <typename size_t dim_>
	class squared_neighbor;

	template <>
	class squared_neighbor<3>{
	public:
		typedef squared_neighbor                type;
		static const size_t			            dim =  3;
		static const size_t			            neighbor_num = 26;

		typedef int								value_type;
		typedef const int &						reference;
		typedef const int &						const_reference;
		typedef	point<int, dim>				    extent_type;
		typedef index_iterator<type>		    iterator;
		typedef index_iterator<const type>      const_iterator;


		squared_neighbor(){}

		squared_neighbor &operator=(const type &rhs){
			_extent = rhs._extent;
			_offsets = rhs._offsets;
			return *this;
		}

		const_iterator begin() const              { return const_iterator(const_cast<type *>(this), 0); }
		const_iterator end() const                { return const_iterator(const_cast<type *>(this), neighbor_num); }
		iterator begin()                          { return iterator(this, 0); }
		iterator end()                            { return iterator(this, neighbor_num); }

		squared_neighbor(const extent_type &extent){
			attach(extent);
		}

		void attach(const extent_type &extent){
			_extent = extent;
			int shift_0 = 1;
			int shift_1 = _extent[0];
			int shift_2 = get_slice_size(_extent);

			int pos = 0;
			for (int page = -1; page <= 1; ++page){
				for (int row = -1; row <= 1; ++row){
					for (int col = -1; col <= 1; ++col){
						if (page == 0 && row == 0 && col == 0) continue;

						_offsets[pos] = col * shift_0 + row * shift_1 + page * shift_2;
						++pos;
					}
				}
			}
		}

		const int &operator[](const size_t &index) const{
			return _offsets[index];
		}

		size_t size()const                        { return neighbor_num; }

		int max_offset() const                    { return (*this)[neighbor_num-1]; }
		int min_offset() const                    { return (*this)[0]; }

	private:
		extent_type	_extent;
		std::array<int, neighbor_num>	_offsets;
	};

	template <>
	class squared_neighbor<2>{
	public:
		typedef squared_neighbor                type;
		static const size_t			            dim =  2;
		static const size_t			            neighbor_num = 8;

		typedef int								value_type;
		typedef const int &						reference;
		typedef const int &						const_reference;
		typedef	point<int, dim>				    extent_type;
		typedef index_iterator<type>		    iterator;
		typedef index_iterator<const type>      const_iterator;


		squared_neighbor(){}

		squared_neighbor &operator=(const type &rhs){
			_extent = rhs._extent;
			_offsets = rhs._offsets;
			return *this;
		}

		const_iterator begin() const              { return const_iterator(const_cast<type *>(this), 0); }
		const_iterator end() const                { return const_iterator(const_cast<type *>(this), neighbor_num); }
		iterator begin()                          { return iterator(this, 0); }
		iterator end()                            { return iterator(this, neighbor_num); }

		squared_neighbor(const extent_type &extent){
			attach(extent);
		}

		void attach(const extent_type &extent){
			_extent = extent;
			int shift_0 = 1;
			int shift_1 = _extent[0];

			int pos = 0;
			for (int row = -1; row <= 1; ++row){
				for (int col = -1; col <= 1; ++col){
					if (row == 0 && col == 0) continue;

					_offsets[pos] = col * shift_0 + row * shift_1;
					++pos;
				}
			}
		}

		const int &operator[](const size_t &index) const{
			return _offsets[index];
		}

		size_t size()const                        { return neighbor_num; }

		int max_offset() const                    { return (*this)[neighbor_num-1]; }
		int min_offset() const                    { return (*this)[0]; }

	private:
		extent_type	_extent;
		std::array<int, neighbor_num>	_offsets;
	};


	template <typename size_t dim_>
	class cubic_neighbor;

	template <>
	class cubic_neighbor<3>{
	public:
		typedef cubic_neighbor                  type;
		static const size_t			            dim =  3;
		//   static const size_t			            neighbor_num = 27;

		typedef int								value_type;
		typedef const int   					reference;
		typedef const int  						const_reference;
		typedef	point<int, dim>				    extent_type;
		typedef index_iterator<type>		    iterator;
		typedef index_iterator<const type>      const_iterator;


		cubic_neighbor(){}

		cubic_neighbor &operator=(const type &rhs){
			_extent = rhs._extent;
			_offsets = rhs._offsets;
			return *this;
		}

		const_iterator begin() const              { return const_iterator(const_cast<type *>(this), 0); }
		const_iterator end() const                { return const_iterator(const_cast<type *>(this), size()); }
		iterator begin()                          { return iterator(this, 0); }
		iterator end()                            { return iterator(this, size()); }

		cubic_neighbor(const int radius): _radius(radius){
			// attach(extent);
		}

		void attach(const extent_type &extent){
			_extent = extent;
			int shift_0 = 1;
			int shift_1 = _extent[0];
			int shift_2 = get_slice_size(_extent);

			for (int page = -_radius; page <= _radius; ++page){
				for (int row = -_radius; row <= _radius; ++row){
					for (int col = -_radius; col <= _radius; ++col){
						_offsets.push_back(col * shift_0 + row * shift_1 + page * shift_2);
					}
				}
			}
		}

		const int operator[](const size_t &index) const{
#ifdef DEUBG
			//make sure the diamand is initialized;
			assert(_offsets[0] == 1);
#endif
			return _offsets[index];
		}

		size_t size()const                        { return _offsets.size(); }

		int max_offset() const                    { return (*this)[dim-1]; }
		int min_offset() const                    { return (*this)[2 * dim - 1]; }

	private:
		extent_type	_extent;
		std::vector<size_t>	_offsets;
		int _radius;
	};

	template <>
	class cubic_neighbor<2>{
	public:
		typedef cubic_neighbor                  type;
		static const size_t			            dim =  2;
		typedef int								value_type;
		typedef const int   					reference;
		typedef const int  						const_reference;
		typedef	point<int, dim>				    extent_type;
		typedef index_iterator<type>		    iterator;
		typedef index_iterator<const type>      const_iterator;


		cubic_neighbor(){}

		cubic_neighbor &operator=(const type &rhs){
			_extent = rhs._extent;
			_offsets = rhs._offsets;
			return *this;
		}

		const_iterator begin() const              { return const_iterator(const_cast<type *>(this), 0); }
		const_iterator end() const                { return const_iterator(const_cast<type *>(this), size()); }
		iterator begin()                          { return iterator(this, 0); }
		iterator end()                            { return iterator(this, size()); }

		cubic_neighbor(const int radius): _radius(radius){
			// attach(extent);
		}

		void attach(const extent_type &extent){
			_extent = extent;
			int shift_0 = 1;
			int shift_1 = _extent[0];

			for (int row = -_radius; row <= _radius; ++row){
				for (int col = -_radius; col <= _radius; ++col){
					_offsets.push_back(col * shift_0 + row * shift_1);
				}
			}
		}

		const int operator[](const size_t &index) const{
#ifdef DEUBG
			//make sure the diamand is initialized;
			assert(_offsets[0] == 1);
#endif
			return _offsets[index];
		}

		size_t size()const                        { return _offsets.size(); }

		int max_offset() const                    { return (*this)[dim-1]; }
		int min_offset() const                    { return (*this)[2 * dim - 1]; }

	private:
		extent_type	_extent;
		std::vector<size_t>	_offsets;
		int _radius;
	};


	template <typename N, typename Func>
	void traverse_neighbors(const size_t &base, const N &neighbors, Func fun){
		for (auto it = neighbors.begin(); it != neighbors.end(); ++it){
			fun(base+(*it));
		}
	}
	//
#pragma endregion
}