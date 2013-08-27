/******************************************************************************
Created By : Zhang Zhimin
Created On : 2012/11/21
Purpose    :  
********************************************************************************/

#pragma once

#include <skynet\core\array.hpp>
#include <skynet\core\neighbor.hpp>
#include <skynet\utility\iterator_facade.hpp>
#include <skynet\utility\math.hpp>
#include <skynet\utility\parse.hpp>

namespace skynet{


	namespace detail{
		template <typename T>
		struct mask_element{
			int pos;
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


	template <typename B, typename size_t dim_, typename WeightType = int>
	class mask_adaptor{
	public:
		typedef B                                       mask_type;
		static const size_t                             dim = dim_;
		typedef point<int, dim>                         extent_type;
		typedef mask_element<WeightType>                value_type;
		typedef const value_type &                      reference;
		typedef const value_type &                      const_reference;
		typedef index_iterator<mask_type>               iterator;
		typedef index_iterator<const mask_type>         const_iterator;

		const mask_type &operator()() const             { return *static_cast<const mask_type *>(this); }
		mask_type &operator()()                         { return *static_cast<mask_type *>(this); }

		iterator begin()                                { return iterator(&((*this)()), 0); }
		iterator end()                                  { return iterator(&((*this)()), (*this)().size()); }
		const_iterator begin() const	{ return const_iterator(const_cast<mask_type *>(&((*this)())), 0); }
		const_iterator end() const		{ return const_iterator(const_cast<mask_type *>(&((*this)())), (*this)().size()); }

		int max_offset() const { 
			return (*std::max_element(begin(), end(), detail::pos_cmp())).pos;
		}

		int min_offset() const {
			return (*std::min_element(begin(), end(), detail::pos_cmp())).pos;
		} 

		WeightType weight_sum() const {
			WeightType temp = 0;
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
		typedef point<int, dim>                         extent_type;
		typedef mask_element<int>                       value_type;
		typedef const value_type &                      reference;
		typedef const value_type &                      const_reference;
		typedef index_iterator<type>                    iterator;
		typedef index_iterator<const type>              const_iterator;

		mean_mask() {}

		void attach(const extent_type &extent){
			_extent = extent;
			diamand_neighbor<dim> neighbors(_extent);

			for (int i = 0; i < neighbors.size(); ++i){
				_elements[i].weight = 1;
				_elements[i].pos = neighbors[i];
			}

		}

	private:
		extent_type                                     _extent;
		std::array<value_type, dim*2>                   _elements;
	};


	template <typename size_t dim_, typename size_t dir_>
	class center_derivative_mask : public mask_adaptor<center_derivative_mask<dim_, dir_>, dim_>{
	public:
		static const size_t dim = dim_;
		static const size_t direction = dir_;

		center_derivative_mask() : _radius(1){}
		center_derivative_mask(const int &radius) : _radius(radius) {}

		void attach(const extent_type &extent){
			_extent = extent;

			diamand_neighbor<dim> neighbors(extent);
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
		int                                            _radius;
	};


	template <typename size_t dim_>
	class gaussian_mask : public mask_adaptor<gaussian_mask<dim_>, dim_>{
	public:
		static const size_t dim = dim_;

		gaussian_mask() {}
		gaussian_mask(const int &radius) : _radius(radius) {}

		void attach(const extent_type &extent){
			_extent = extent;
			diamand_neighbor<dim> neighbors(extent);
			for (int i = 0; i < neighbors.size(); ++i){
				_elements[i].pos = neighbors[i] * _radius;
				_elements[i].weight = 1;
			}

			_elements[dim*2].pos = 0;
			_elements[dim*2].weight = 2;
		}

		const_reference operator[](const size_t &i) const   { return _elements[i]; }
		size_t size() const                                 { return dim*2U + 1U; }

	private:
		int                                     _radius;
		extent_type                             _extent;
		std::array<value_type, dim * 2 + 1>     _elements;
	};


	template <typename size_t dim_>
	class laplace_mask : public mask_adaptor<laplace_mask<dim_>, dim_>{
	public:
		static const size_t dim = dim_;

		laplace_mask() : _radius(1) {}
		laplace_mask(const int &radius) : _radius(radius) {}

		void attach(const extent_type &extent){
			_extent = extent;
			diamand_neighbor<dim> neighbors(extent);
			for (int i = 0; i < neighbors.size(); ++i){
				_elements[i].pos = neighbors[i] * _radius;
				_elements[i].weight = 1;
			}

			_elements[dim*2].pos = 0;
			_elements[dim*2].weight = -int(dim)*2;
		}

		const_reference operator[](const size_t &i) const   { return _elements[i]; }
		size_t size() const                                 { return dim*2U + 1U; }

	private:
		int                                     _radius;
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
		pre_derivative_mask(const int &radius) : _radius(radius) {}

		void attach(const extent_type &extent){
			_extent = extent;

			diamand_neighbor<dim> neighbors(extent);
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
		int												_radius;
	};


	class corner_derivative_mask : public mask_adaptor<corner_derivative_mask, 2>{
	public:
		static const size_t dim = 2;

		corner_derivative_mask(){}

		void attach(const extent_type &extent){
			_extent = extent;
			diamand_neighbor<dim> neighbors(extent);
			_elements[0].weight = 1;
			_elements[0].pos = 0;
			_elements[1].weight = 1;
			_elements[1].pos = neighbors[0]  + neighbors[1];
			_elements[2].weight = -1;
			_elements[2].pos = neighbors[0];
			_elements[3].weight = -1;
			_elements[3].pos = neighbors[1];
		}

		const_reference operator[](const size_t &i) const    { return _elements[i]; }
		size_t size() const                                  { return 4; }

	private:
		extent_type										_extent;
		std::array<value_type, 4>						_elements;
	};


	template <typename size_t Dir>
	class prewitt_mask;


	template <>
	class prewitt_mask<0U> : public mask_adaptor<prewitt_mask<0U>, 2>{
	public:
		static const size_t dim = 2;

		void attach(const extent_type &extent){
			_extent = extent;
			diamand_neighbor<dim> neighbors(extent);
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
			diamand_neighbor<dim> neighbors(extent);
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