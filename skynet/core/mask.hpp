/******************************************************************************
Created By : Zhang Zhimin
Created On : 2012/11/21
Purpose    :  
********************************************************************************/

#pragma once
#include <skynet/core/array.hpp>
#include <skynet/core/neighbor.hpp>

#include <skynet/utility/math.hpp>
#include <skynet/utility/parse.hpp>

namespace skynet{


	namespace detail{
		template <typename T>
		struct mask_element{
			ptrdiff_t pos;
			T weight;
		};

		struct pos_cmp{
			template <typename T>
			bool operator()(const mask_element<T> &e1, const mask_element<T> &e2) const{
				return e1.pos < e2.pos;
			}
		};
	}


	using detail::mask_element;


	template <typename B, typename size_t dim_, typename Weight = int8_t>
	class mask_adaptor : public iterator_adaptor<B>{
	public:
		typedef B                                       mask_type;
		static const size_t                             dim = dim_;
		typedef point<ptrdiff_t, dim>                   extent_type;
		typedef mask_element<Weight>					value_type;
		typedef const value_type &                      reference;
		typedef const value_type &                      const_reference;

		ptrdiff_t max_offset() const { 
			return (*std::max_element(begin(), end(), detail::pos_cmp())).pos;
		}

		ptrdiff_t min_offset() const {
			return (*std::min_element(begin(), end(), detail::pos_cmp())).pos;
		} 

		Weight weight_sum() const {
			Weight temp = 0;
			for (auto it = begin(); it != end(); ++it){
				temp += it->weight;
			}
			return temp;
		}
	};


	template <typename size_t dim_>
	class mean_mask{
	public:
		typedef mean_mask                               type;
		static const size_t                             dim = dim_;
		typedef point<size_t, dim>						extent_type;
		typedef mask_element<ptrdiff_t>                 value_type;
		typedef const value_type &                      reference;
		typedef const value_type &                      const_reference;
		typedef detail::index_iterator<type>                    iterator;
		typedef detail::index_iterator<const type>              const_iterator;

		mean_mask() {}

		void attach(const extent_type &extent){
			_extent = extent;
			diamand_neighbor<dim> neighbors(_extent);

			for (size_t i = 0; i < neighbors.size(); ++i){
				_elements[i].weight = 1;
				_elements[i].pos = neighbors[i];
			}
		}

	private:
		extent_type                                     _extent;
		std::array<value_type, dim*2>                   _elements;
	};


	template <typename size_t dim_, typename size_t dir_>
	class center_difference_mask : public mask_adaptor<center_difference_mask<dim_, dir_>, dim_>{
	public:
		static const size_t dim = dim_;
		static const size_t direction = dir_;

		center_difference_mask() : _radius(1){}
		center_difference_mask(const ptrdiff_t &radius) : _radius(radius) {}

		void attach(const extent_type &extent){
			_extent = extent;

			diamand_neighbor<dim> neighbors;
			neighbors.attach(extent);
			_elements[0].weight = 1;
			_elements[0].pos = neighbors[direction] * _radius;
			_elements[1].weight = -1;
			_elements[1].pos = -_elements[0].pos;
		}

		const_reference operator[](const size_t &i) const    { return _elements[i]; }
		size_t size() const                                  { return 2; }

	private:
		extent_type                                    _extent;
		std::array<value_type, 2>                      _elements;
		ptrdiff_t                                            _radius;
	};


	template <typename size_t dim_>
	class gaussian_mask : public mask_adaptor<gaussian_mask<dim_>, dim_>{
	public:
		static const size_t dim = dim_;

		gaussian_mask() {}

		void attach(const extent_type &extent){
			_extent = extent;
			diamand_neighbor<dim> neighbors;neighbors.attach(extent);
			for (size_t i = 0; i < neighbors.size(); ++i){
				_elements[i].pos = neighbors[i];
				_elements[i].weight = 1;
			}

			_elements[dim*2].pos = 0;
			_elements[dim*2].weight = 2;
		}

		const_reference operator[](const size_t &i) const   { return _elements[i]; }
		size_t size() const                                 { return dim*2U + 1U; }

	private:
		extent_type                             _extent;
		std::array<value_type, dim * 2 + 1>     _elements;
	};


	template <typename size_t dim_>
	class laplace_mask : public mask_adaptor<laplace_mask<dim_>, dim_>{
	public:
		static const size_t dim = dim_;

		laplace_mask() : _radius(1) {}
		laplace_mask(const ptrdiff_t &radius) : _radius(radius) {}

		void attach(const extent_type &extent){
			_extent = extent;
		 diamand_neighbor<dim> neighbors;neighbors.attach(extent);
			for (ptrdiff_t i = 0; i < neighbors.size(); ++i){
				_elements[i].pos = neighbors[i] * _radius;
				_elements[i].weight = 1;
			}

			_elements[dim*2].pos = 0;
			_elements[dim*2].weight = -ptrdiff_t(dim)*2;
		}

		const_reference operator[](const size_t &i) const   { return _elements[i]; }
		size_t size() const                                 { return dim*2U + 1U; }

	private:
		ptrdiff_t                                     _radius;
		extent_type                             _extent;
		std::array<value_type, dim * 2 + 1>     _elements;
	};


	template <typename size_t Dim, typename size_t Dir>
	class pre_derivative_mask : public mask_adaptor<pre_derivative_mask<Dim, Dir>, Dim>{
	public:
		static_assert(Dir < Dim, "the Dir is invalid.");

		static const size_t dim = Dim;
		static const size_t direction = Dir;

		pre_derivative_mask() : _radius(1){}
		pre_derivative_mask(const ptrdiff_t &radius) : _radius(radius) {}

		void attach(const extent_type &extent){
			_extent = extent;

		 diamand_neighbor<dim> neighbors;neighbors.attach(extent);
			_elements[0].weight = 1;
			_elements[0].pos = neighbors[direction] * _radius;
			_elements[1].weight = -1;
			_elements[1].pos = 0;
		}

		const_reference operator[](const size_t &i) const    { return _elements[i]; }
		size_t size() const                                  { return 2; }

	private:
		extent_type										_extent;
		std::array<value_type, 2>						_elements;
		ptrdiff_t												_radius;
	};


	//class roberts_mask : public mask_adaptor<roberts_mask, 2, int8_t>{
	//public:
	//	static const size_t dim = 2;

	//	roberts_mask(){}

	//	void attach(const extent_type &extent){
	//		_extent = extent;
	//	 diamand_neighbor<dim> neighbors;neighbors.attach(extent);
	//		_elements[0].weight = 1;
	//		_elements[0].pos = 0;
	//		_elements[1].weight = 1;
	//		_elements[1].pos = neighbors[0]  + neighbors[1];
	//		_elements[2].weight = -1;
	//		_elements[2].pos = neighbors[0];
	//		_elements[3].weight = -1;
	//		_elements[3].pos = neighbors[1];
	//	}

	//	const_reference operator[](const size_t &i) const    { return _elements[i]; }
	//	size_t size() const                                  { return 4; }

	//private:
	//	extent_type										_extent;
	//	std::array<value_type, 4>						_elements;
	//};


	template <typename size_t Dir>
	class prewitt_mask;

	template <>
	class prewitt_mask<0U> : public mask_adaptor<prewitt_mask<0U>, 2>{
	public:
		static const size_t dim = 2;

		void attach(const extent_type &extent){
			_extent = extent;
			diamand_neighbor<dim> neighbors;
			neighbors.attach(extent);
			_elements[0].weight = 1;
			_elements[0].pos = neighbors[0] - neighbors[1];
			_elements[1].weight = 1;
			_elements[1].pos = neighbors[0];
			_elements[2].weight = 1;
			_elements[2].pos = neighbors[0] + neighbors[1];

			_elements[3].weight = -1;
			_elements[3].pos = -_elements[0].pos;
			_elements[4].weight = -1;
			_elements[4].pos = -_elements[1].pos;
			_elements[5].weight = -1;
			_elements[5].pos = -_elements[2].pos;
		}

		const_reference operator[](const size_t &i) const    { return _elements[i]; }
		size_t size() const                                  { return 6; }

	private:
		extent_type										_extent;
		std::array<value_type, 6>						_elements;
	};


	template <>
	class prewitt_mask<1U> : public mask_adaptor<prewitt_mask<1U>, 2>{
	public:
		static const size_t dim = 2;

		void attach(const extent_type &extent){
			_extent = extent;
			diamand_neighbor<dim> neighbors;
			neighbors.attach(extent);
			_elements[0].weight = 1;
			_elements[0].pos = neighbors[1] - neighbors[0];
			_elements[1].weight = 1;
			_elements[1].pos = neighbors[1];
			_elements[2].weight = 1;
			_elements[2].pos = neighbors[1] + neighbors[0];

			_elements[3].weight = -1;
			_elements[3].pos = -_elements[0].pos;
			_elements[4].weight = -1;
			_elements[4].pos = -_elements[1].pos;
			_elements[5].weight = -1;
			_elements[5].pos = -_elements[2].pos;
		}

		const_reference operator[](const size_t &i) const    { return _elements[i]; }
		size_t size() const                                  { return 6; }

	private:
		extent_type										_extent;
		std::array<value_type, 6>						_elements;
	};


	//template <typename size_t Dir>
	//class sobel_mask;

	//template <>
	//class sobel_mask<0U> : public mask_adaptor<sobel_mask<0U>, 2>{
	//public:
	//	static const size_t dim = 2;

	//	void attach(const extent_type &extent) {
	//		_extent = extent;
	//	 diamand_neighbor<dim> neighbors;neighbors.attach(extent);
	//		_elements[0].weight = 1;
	//		_elements[0].pos = neighbors[0] - neighbors[1];
	//		_elements[1].weight = 1;
	//		_elements[1].pos = neighbors[0];
	//		_elements[2].weight = 1;
	//		_elements[2].pos = neighbors[0] + neighbors[1];

	//		_elements[3].weight = -1;
	//		_elements[3].pos = -_elements[0].pos;
	//		_elements[4].weight = -1;
	//		_elements[4].pos = -_elements[1].pos;
	//		_elements[5].weight = -1;
	//		_elements[5].pos = -_elements[2].pos;
	//	}

	//	const_reference operator[](const size_t &i) const { return _elements[i]; }
	//	size_t size() const { return 6; }

	//private:
	//	extent_type										_extent;
	//	std::array<value_type, 6>						_elements;
	//};


	//template <>
	//class sobel_mask<1U> : public mask_adaptor<sobel_mask<1U>, 2>{
	//public:
	//	static const size_t dim = 2;

	//	void attach(const extent_type &extent) {
	//		_extent = extent;
	//	 diamand_neighbor<dim> neighbors;neighbors.attach(extent);
	//		_elements[0].weight = 1;
	//		_elements[0].pos = neighbors[1] - neighbors[0];
	//		_elements[1].weight = 1;
	//		_elements[1].pos = neighbors[1];
	//		_elements[2].weight = 1;
	//		_elements[2].pos = neighbors[1] + neighbors[0];

	//		_elements[3].weight = -1;
	//		_elements[3].pos = -_elements[0].pos;
	//		_elements[4].weight = -1;
	//		_elements[4].pos = -_elements[1].pos;
	//		_elements[5].weight = -1;
	//		_elements[5].pos = -_elements[2].pos;
	//	}

	//	const_reference operator[](const size_t &i) const { return _elements[i]; }
	//	size_t size() const { return 6; }

	//private:
	//	extent_type										_extent;
	//	std::array<value_type, 6>						_elements;
	//};


	template <typename T, typename size_t dim = 2>
	class mask;

	template <typename T>
	class mask<T, 2> : public mask_adaptor<mask<T>, 2, T>{
	public:
		const static size_t	dim = 2;

		mask(const string &s){
			_array2 = parse_array2<T>(s);
		}
			
		void attach(const extent_type &extent){
			_extent = extent;
			diamand_neighbor<dim> neighbors(_extent);

			//the center of mask
			auto center = (_array2.extent()-index2::unit)/2;

			for (size_t row = 0; row < _array2.height(); ++row){
				for (size_t col = 0; col < _array2.width(); ++col){
					index2 index(col, row);
					auto shift = index - center;
					value_type e;
					e.pos  = shift.x * neighbors[0];
					e.pos += shift.y * neighbors[1];
					e.weight = _array2(col, row);

					_elements.push_back(e);
				}
			}
		}

		const_reference operator[](const size_t &i) const    { return _elements[i]; }
		size_t size() const                                  { return _elements.size(); }

	private:
		extent_type										_extent;
		std::vector<value_type>							_elements;
		multi_array<T,2>								_array2;
	};


}
