#pragma once

#include <skynet/config.hpp>
#include <skynet/core/point.hpp>

namespace skynet{ namespace geometry{

	template <typename T = point2d, typename C = std::vector<T>>
	class contour{
	public:
		typedef C								containor;
		typedef typename C::value_type			value_type;
		typedef typename C::reference			reference;
		typedef typename C::const_reference		const_reference;
		typedef typename C::iterator			iterator;
		typedef typename C::const_iterator		const_iterator;
		static const size_t						dim = T::dim;
		//typedef typename C::value_type			value_type;

		contour(): _pos(), _direction(), _normal(){}
		contour(size_t size) : _pos(size), _direction(size), _normal(size){}

		contour(const containor &pos) : _pos(pos), _diretion(pos.size()), _normal(pos.size()) {}
		contour(containor &&pos) : _pos(pos), _direction(pos.size()), _normal(pos.size()) {}

		contour&  operator=(const contour &rhs){
			_pos = rhs._pos;
			_direction = rhs._direction;
			_normal = rhs._normal;
			return *this;
		}

		const_reference operator[](ptrdiff_t i) const{
			while (i >= static_cast<ptrdiff_t>(size())){
				i -= size();
			}
			while (i < 0){
				i += size();
			}

			return _pos[i];
		}

		reference operator[](ptrdiff_t i){
			return const_cast<reference>(const_cast<const contour *>(this)->operator[](i));
		}

		iterator begin()						{ return _pos.begin(); }
		const_iterator begin() const			{ return _pos.begin(); }
		iterator end()							{ return _pos.end(); }
		const_iterator end() const				{ return _pos.end(); }

		size_t size() const{
			return _pos.size();
		}

		bool empty() const {
			return _pos.empty();
		}

		containor direction() const {
			return _direction;
		}

		std::vector<point2d> normal() const{
			return _normal;
		}

		void refresh(){
			for (size_t i = 0; i < _direction.size(); ++i){
				_direction[i] = (*this)[i+1] - (*this)[i];
			}

			for (size_t i = 0; i < _normal.size(); ++i){
				_normal[i].x = _direction[i].y;
				_normal[i].y = -_direction[i].x;
			}

			for (auto &e: _normal){
				e = normalize(e);
			}
		}

	private:
		containor   _pos;
		containor	_direction;
		containor	_normal;
	};



}}
