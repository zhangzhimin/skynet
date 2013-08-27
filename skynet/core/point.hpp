/******************************************************************************
Created By : Zhang Zhimin
Created On : 2012/6/9 
Purpose    : implements point
********************************************************************************/
#pragma once

#include <array>
#include <numeric>
#include <type_traits>

#include <skynet\config.hpp>
#include <skynet\core\common.hpp>
#include <skynet\utility\iterator_facade.hpp>
#include <skynet\utility\type_traits.hpp>

namespace skynet{

	///mutil-dim point. the clas is not used now, but it support the above 4D point.
	template <typename T,typename size_t dim_>
	class point{
	public:
		typedef point												type;
		static const size_t											dim = dim_;
		typedef T													value_type;
		typedef value_type &										reference;
		typedef const value_type &									const_reference;
		typedef typename std::array<value_type, dim>::iterator      iterator;
		typedef const iterator										const_iterator;					

		point()	{}

		point(const value_type &value){
			for (int i = 0; i < dim; ++i){
				(*this)[i] = value;
			}
		}

		point(const point &p) : _array(p._array) { }

		point &operator=(const point &p){
			this->_array = p._array;
			return *this;
		}

		~point() { }

		const_iterator  begin() const                       { return _array.begin(); }
		iterator begin()                                    { return _array.begin(); }
		const_iterator end() const                          { return _array.end(); }
		iterator end()                                      { return _array.end(); }

		inline  const value_type &operator[](size_t index)	const	{ return _array[index]; }
		inline	value_type &operator[](size_t index)				{ return _array[index]; }

		size_t size() const        { return _array.size(); }

	public:
		const static type zero;
		const static type unit;
	private:
		std::array<T, dim> _array;
	};

	template <typename T, typename size_t dim>
	const point<T, dim> point<T,dim>::zero = point<T, dim>(0);

	template <typename T, typename size_t dim>
	const point<T, dim> point<T,dim>::unit = point<T, dim>(1);


	template <typename T>
	class point<T, 2U>{
	public:
		typedef	point			                        type;
		typedef T				                        value_type;
		typedef value_type &                            reference;
		typedef const value_type &                      const_reference;
		const static size_t								dim = 2;

		typedef index_iterator<type>					iterator;
		typedef index_iterator<const type>				const_iterator;


		point( ) : x(0), y(0) { }

		explicit point(const value_type &value): x(value), y(value) {}

		point(const value_type &v1, const value_type &v2):x(v1), y(v2) { }

		point(const point &p) : x(p.x), y(p.y)	{}

		//for type lazy_cast
		template <typename TT>
		explicit point(const point<TT, 2> &p) : x(p.x), y(p.y) {}

		point(point &&p) : x(p.x), y(p.y){
		}

		point &operator=(const point &p){
			this->x = p.x;
			this->y = p.y;

			return *this;
		}

		reference operator[](const size_t i)					{ return this->*me_array[i]; }			
		const_reference operator[](const size_t i)	const		{ return this->*me_array[i]; }		

		const_iterator begin() const					{ return const_iterator(const_cast<type *>(this), 0); }
		iterator begin()								{ return iterator(this, 0); }
		const_iterator end() const						{ return const_iterator(const_cast<type *>(this), dim); }
		iterator end() 									{ return iterator(this, dim); }

		size_t size() const								{ return 2; }

	public:
		T x,y;

	public:
		const static type zero;
		const static type unit;

	private:
		static T point<T, 2U>::* const me_array[dim];
	};

	template <typename T>
	T  point<T, 2U>::*const point<T, 2U>::me_array[point<T, 2U>::dim] = {&point<T, 2U>::x, &point<T, 2U>::y};

	template <typename T>
	const point<T, 2U> point<T, 2U>::zero = point<T, 2U>(0, 0);

	template <typename T>
	const point<T, 2U> point<T, 2U>::unit = point<T, 2U>(1, 1);


	///special 3d point
	template <typename T>
	class point<T, 3U>{
	public:
		typedef	point			                        type;
		typedef T				                        value_type;
		typedef value_type &                            reference;
		typedef const value_type &                      const_reference;
		const static size_t								dim = 3;

		typedef index_iterator<type>					iterator;
		typedef index_iterator<const type>				const_iterator;

		point() : x(0), y(0), z(0) { }

		explicit point(const value_type &value): x(value), y(value), z(value) {}

		explicit point(const value_type &newX, const value_type &newY, const value_type &newZ) 
			:x(newX), y(newY), z(newZ){ }

		point(const point &p) : x(p.x), y(p.y), z(p.z) { }

		template <typename TT>
		explicit point(const point<TT, 3> &p): x(p.x), y(p.y), z(p.z) {}

		point &operator=(const point &p){
			x = p.x;
			y = p.y;
			z = p.z;

			return *this;
		}

		~point() { }

		const value_type &operator[](const size_t &i) const		{ return this->*me_array[i]; }
		value_type &operator[](const size_t &i)					{ return this->*me_array[i]; }

		const_iterator begin() const					{ return const_iterator(const_cast<type *>(this), 0); }
		iterator begin()								{ return iterator(this, 0); }
		const_iterator end() const						{ return const_iterator(const_cast<type *>(this), dim); }
		iterator end() 									{ return iterator(this, dim); }

		size_t size() const                        { return 3; }

	public:
		value_type x,y,z;

	public:
		const static type zero;
		const static type unit;

	private:
		static T point<T, 3U>::*const me_array[dim];
	};

	template <typename T>
	T point<T, 3U>::*const  point<T, 3U>::me_array[point<T, 3U>::dim] = 
	{&point<T, 3U>::x, &point<T, 3U>::y, &point<T, 3U>::z};

	template <typename T>
	const point<T, 3U> point<T, 3U>::zero = point<T, 3U>(0, 0, 0);

	template <typename T>
	const point<T, 3U> point<T, 3U>::unit = point<T, 3U>(1, 1, 1);


#pragma region operation(+,-...)
	template <typename T, typename size_t D>
	bool operator==(const typename point<T, D> &lhs, const typename point<T, D> &rhs){
		for (int i = 0; i < D; ++i){
			if (lhs[i] != rhs[i])	return false;
		}

		return true;
	}

	template <typename T, typename size_t D>
	bool operator!=(const point<T, D> &lhs, const point<T, D> &rhs){		
		for (int i = 0; i < D; ++i){
			if (lhs[i] == rhs[i]) return true;
		}

		return false;
	}	

	template <typename T, typename size_t D>
	point<T, D> operator-(const point<T, D> &lhs, const point<T, D> &rhs){
		point<T, D> temp;
		for (int i = 0; i < D; ++i){
			temp[i] = lhs[i] - rhs[i];
		}

		return temp;
	}

	template <typename T, typename size_t D>
	void operator-=(point<T, D> &lhs, const point<T, D> &rhs){
		for (int i = 0; i < D; ++i){
			lhs[i] -= rhs[i];
		}
	}

	template <typename T, typename size_t D>
	point<T, D> operator+(const point<T, D> &lhs, const point<T, D> &rhs){
		point<T, D> temp;
		for (int i = 0; i < D; ++i){
			temp[i] = lhs[i] + rhs[i];
		}

		return temp;
	}

	template <typename T, typename size_t D>
	void operator+=(point<T, D> &lhs, const point<T, D> &rhs){		
		for (int i = 0; i < D; ++i){
			lhs[i] += rhs[i];
		}
	}

	template <typename T, typename size_t D>
	point<T, D> operator-(const point<T, D> &p){
		point<T, D> temp;
		for (int i = 0; i < D; ++i){
			temp[i] = -p[i];
		}

		return temp;
	}

	template <typename T, typename size_t D>
	point<T, D> operator*(const point<T, D> &lhs, const point<T, D> &rhs){
		point<T, D> temp;
		for (int i = 0; i < D; ++i){
			temp[i] = lhs[i] * rhs[i];
		}

		return temp;
	}

	template <typename T, typename size_t D>
	void operator*=(point<T, D> &lhs, const point<T, D> &rhs){
		for (int i = 0; i < D; ++i){
			lhs[i] *= rhs[i];
		}
	}
		
	template <typename T, typename size_t D>
	void operator*=(point<T, D> &lhs, const T &scale){
		for (int i = 0; i <D; ++i){
			lhs[i] *= scale;
		}
	}

	template <typename T, typename size_t D>
	point<T, D> operator*(typename point<T,D>::value_type scale, const point<T, D> &rhs){		
		point<T, D> temp;
		for (int i = 0; i < D; ++i){
			temp[i] = rhs[i] * scale;
		}

		return temp;
	}

	template <typename T, typename size_t D>
	point<T, D> operator/(const point<T, D> &lhs, const point<T, D> &rhs){
		point<T, D> temp;
		for (int i = 0; i < D; ++i){
			temp[i] = lhs[i] / rhs[i];
		}

		return temp;
	}

	template <typename T, typename size_t D>
	void operator/=(point<T, D> &lhs, const point<T, D> &rhs){
		for (int i = 0; i < D; ++i){
			lhs[i] /= rhs[i];
		}
	}

	template <typename T, typename size_t D>
	point<T, D> operator/(const point<T, D> &p, const double &scale){
		point<T, D> temp;
		for (int i = 0; i < D; ++i){
			temp[i] = p[i] / scale;
		}
		return temp;
	}

	template <typename T, typename size_t D>
	void operator/=(point<T, D> &point, const double &scale){
		for (int i = 0; i < D; ++i){
			point[i] /= scale;
		}
	}

	template <typename T, typename size_t D>
	point<T, D> operator%(const point<T, D> &p1, const point<T, D> &p2){
		point<T, D> re;
		for (size_t i = 0; i < D; ++i){
			re[i] = p1[i] % p2[i];
		}

		return re;
	}

	template <typename T, typename size_t D>
	void operator%=(point<T, D> &p1, const point<T, D> &p2){
		for (size_t i = 0; i < D; ++i){
			p1[i] %= p2[i];
		}
	}

	template <typename T, typename size_t D>
	point<T, D> operator>>(const point<T, D> &p, const size_t &shift){
		point<T, D> temp;
		for (int i = 0; i < D; ++i){
			temp[i] = p[i]>>shift;
		}

		return temp;
	}

	template <typename T, typename size_t D>
	point<T, D> operator<<(const point<T, D> &p, const size_t &shift){
		point<T, D> temp;
		for (int i = 0; i < D; ++i){
			temp[i] = p[i]<<shift;
		}

		return temp;
	}

	template <typename T, typename size_t D>
	std::ostream &operator<<(std::ostream &stream, const point<T, D> &point){
		stream <<"(";
		for (int i = 0; i < D; ++i){
			stream << point[i] << ",";
		}
		return stream <<")";
	}

	template <typename T, typename size_t D>
	point<T, D> abs(const point<T, D> &p){
		point <T, D> temp;
		for (i = 0; i < D; ++i){
			temp[i] = std::abs(p[i]);
		}

		return temp;
	}

	template <typename T, typename size_t D>
	point<int, D> nearest_neighbor(const point<T, D> &p){
		point<int, D> temp;
		for (int i = 0; i < D; ++i){
			temp[i] = int(p[i]+0.5);
		}
		return temp;
	}

	template <typename size_t D>
	point<int, D> nearest_neighbor(const point<int, D> &p){
		return p;
	}


	///point is used by std::vector sometimes, so we define the operation for it
	///-----------------define the std::vector operation----------------------------------
	class Euclidean_tag{};
	class street_tag{};
	class max_nor_tag{};
	class min_nor_tag{};
		
	template <typename T, typename size_t D>
	T norm(const point<T, D> &p, const Euclidean_tag &){
		T temp = 0;
		for (int i = 0; i < D; ++i){
			temp += p[i] * p[i];
		}

		return sqrt(temp);
	}

	
	template <typename T, typename size_t D>
	T norm(const point<T, D> &p, const street_tag &){
		T temp = 0;
		for (int i = 0; i < D; ++i){
			temp += std::abs(p[i]); 
		}

		return temp;
	}

	template <typename T, typename size_t D>
	T norm(const point<T, D> &p){
		return norm(p, Euclidean_tag());
	}

	template <typename T, typename size_t D, typename distance_tag>
	T distance(const point<T, D> &lhs, const point<T, D> &rhs, const distance_tag &){
		return norm(lhs - rhs, distance_tag());
	}

	template <typename T>
	T distance(const typename std::enable_if<std::is_arithmetic<T>::value, T>::type &lhs, 
		const typename std::enable_if<std::is_arithmetic<T>::value, T>::type &rhs){
			return std::abs(lhs-rhs);
	}

	template <typename T, typename size_t D>
	T distance(const point<T, D> &lhs, const point<T, D> &rhs){
		return distance(lhs, rhs, Euclidean_tag());
	}

	template <typename T, typename size_t D>
	point<T, D> center_point(const point<T, D> &fir, const point<T, D> &sec){
		point<T, D> center;
		for (int i = 0; i < D; ++i){
			center[i] = (fir[i] + sec[i]) >> 1; 
		}
		return center;
	}

	template <typename size_t D>
	point<float, D> normalize(const point<float, D>  &p){
		auto n = norm(p) + std::numeric_limits<float>::epsilon();

		point<float, D> temp;
		for (int i = 0; i < D; ++i){
			temp[i] = p[i] / n; 
		}

		return temp;
	}

	template <typename size_t D>
	point<double, D> normalize(const point<double, D>  &p){
		auto n = norm(p) + std::numeric_limits<double>::epsilon();

		point<double, D> temp;
		for (int i = 0; i < D; ++i){
			temp[i] = p[i] / n; 
		}

		return temp;
	}
	
	template <typename T, typename size_t D>
	T product(const point<T, D> &lhs, const point<T, D> &rhs){
		T temp = 0;
		for (size_t i = 0; i < D; ++i){
			temp += lhs[i] * rhs[i];
		}
		return temp;
	}

	template <typename T>
	point<T, 3> cross_product(const point<T, 3> &p1, const point<T, 3> &p2){
		point<T, 3> temp;
		temp[0] = p1[1] * p2[2] - p1[2] * p2[1];
		temp[1] = p1[2] * p2[0] - p1[0] * p2[2];
		temp[2] = p1[0] * p2[1] - p1[1] * p2[0];

		return temp;
	}
	
#pragma endregion

	///-----------------typdef the size type and index type
	typedef point<int, 2> extent2;
	typedef point<int, 3> extent3;

	typedef point<int, 2> index2;
	typedef point<int, 3> index3;	

	typedef point<float, 3> point3f;
	typedef point<float, 2> point2f;

	typedef point<double, 3> point3d;
	typedef point<double, 2> point2d;

	typedef point<int, 3> point3i;
	typedef point<int, 2> point2i;

	inline size_t get_volume_size(extent3 extent){
		return static_cast<size_t>(std::accumulate(extent.begin(), extent.end(), int(1), std::multiplies<int>()));
	}

	template <typename T, typename size_t D>
	inline size_t whole_size(const point<T, D> &p){
		return std::accumulate(p.begin(), p.end(), size_t(1), multiplies<T>());
	}

	inline size_t get_slice_size(extent3 extent){
		return extent.x * extent.y;
	}

	inline size_t row_size(const point<int, 2> &p){
		return p.y;
	}

	inline size_t col_size(const point<int, 2> &p){
		return p.x;
	}

	template <typename T>
	struct get_dim{
	private:
		struct di_1D{
			static const size_t dim = 1;
		};
		typedef typename std::conditional<std::is_arithmetic<T>::value, di_1D, T>::type 		      di_imp;
	public:
		static const size_t value = di_imp::dim;
	};
}