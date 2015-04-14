#pragma once

#include <stdexcept>
#include <array>
#include <skynet/core/adaptor_types.hpp>

namespace skynet{

	template <typename B, typename size_t dim>
	class neighbor_adaptor : public iterator_adaptor<B> {
	public:
		typedef B										neighbor_type;
		typedef point<ptrdiff_t, dim>                   extent_type;
		typedef ptrdiff_t								value_type;
		typedef const value_type &                      reference;
		typedef const value_type &                      const_reference;

		ptrdiff_t max_offset() const {
			return *std::max_element(begin(), end());
		}

		ptrdiff_t min_offset() const {
			return *std::min_element(begin(), end());
		}
	};

	//   *                                
	// * o *
	//   *
	//It's the diamand_neighbors, star is neighbor, o is origin.

	template <typename size_t dim>
	class diamand_neighbor: public neighbor_adaptor<diamand_neighbor<dim>, dim>{
	public:
		static const size_t			            neighbor_num = dim * 2;

		diamand_neighbor(){}

		diamand_neighbor &operator=(const diamand_neighbor &rhs){
			_extent = rhs._extent;
			_offsets = rhs._offsets;
			return *this;
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
			return _offsets[index];
		}

		size_t size()const                        { return neighbor_num; }

	private:
		extent_type	_extent;
		std::array<ptrdiff_t, neighbor_num>	_offsets;
	};


	/*									*

									*	*   *

										*
	It's the vertex_neighbors
	*/
	template <typename size_t dim>
	class vertex_neighbor: public neighbor_adaptor<vertex_neighbor<dim>,dim>{
	public:
		static  const size_t			                    neighbor_num = extent_type::dim * 2 + 1;

		vertex_neighbor() {}

		vertex_neighbor(const int &r) : _radius(r){}

		vertex_neighbor &operator=(const vertex_neighbor &rhs){
			this->rhs._radius;
			this->_extent = rhs._extent;
			this->_offsets = rhs._offsets;

			return *this;
		}

		const int &operator  [](const size_t &index) const{
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

		size_t size() const                  { return neighbor_num; }

	private:
		int		                        _radius;
		extent_type	                    _extent;
		std::array<int, neighbor_num>   _offsets;
	};

	template <typename size_t dim_>
	class square_neighbor;

	template <>
	class square_neighbor<3>:  public neighbor_adaptor<square_neighbor<3>, 3>{
	public:
		static const size_t			            neighbor_num = 26;

		square_neighbor(){}

		square_neighbor &operator=(const square_neighbor &rhs){
			_extent = rhs._extent;
			_offsets = rhs._offsets;
			return *this;
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
	private:
		extent_type	_extent;
		std::array<int, neighbor_num>	_offsets;
	};

	template <>
	class square_neighbor<2> : public neighbor_adaptor<square_neighbor<2>,2> {
	public:
		static const size_t			            neighbor_num = 8;

		square_neighbor(){}

		square_neighbor &operator=(const square_neighbor &rhs){
			_extent = rhs._extent;
			_offsets = rhs._offsets;
			return *this;
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

	private:
		extent_type	_extent;
		std::array<int, neighbor_num>	_offsets;
	};


	template <typename size_t dim_>
	class cube_neighbor;

	template <>
	class cube_neighbor<3>: public neighbor_adaptor<cube_neighbor<3>,3>{
	public:
		cube_neighbor &operator=(const cube_neighbor &rhs){
			_extent = rhs._extent;
			_offsets = rhs._offsets;
			return *this;
		}

		cube_neighbor(const int radius = 1): _radius(radius){}

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
			return _offsets[index];
		}

		size_t size()const                        { return _offsets.size(); }
	private:
		extent_type	_extent;
		std::vector<size_t>	_offsets;
		int _radius;
	};

	template <>
	class cube_neighbor<2>: public neighbor_adaptor<cube_neighbor<2>, 2>{
	public:
		cube_neighbor &operator=(const cube_neighbor &rhs){
			_extent = rhs._extent;
			_offsets = rhs._offsets;
			return *this;
		}

		cube_neighbor(const int radius = 1): _radius(radius){
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

		const_reference operator[](const size_t &index) const{
			return _offsets[index];
		}

		size_t size() const								{ return _offsets.size(); }

	private:
		extent_type	_extent;
		std::vector<size_t>	_offsets;
		int _radius;
	};

	template <size_t dim, typename Func>
	class custom_neighbor: public neighbor_adaptor<custom_neighbor<dim, Func>, dim>{
	public:
		custom_neighbor(Func fun): _fun(fun) {}

		void attach(extent_type extent) {
			_fun(extent, std::ref(_offsets));
		}

		const_reference operator[](size_t i)  const {
			return _offsets[i];
		}

		size_t size() const { return _offsets.size(); }

	public:
		extent_type _extent;
		std::vector<value_type> _offsets;
		Func _fun;
	};

	template <size_t dim, typename Func>
	auto customize_neighbor(Func fun) {
		return custom_neighbor<dim, Func>(fun);
	}

	template <typename N, typename Func>
	void traverse_neighbors(const size_t &base, const N &neighbors, Func fun){
		for (auto it = neighbors.begin(); it != neighbors.end(); ++it){
			fun(base+(*it));
		}
	}
	//
}
