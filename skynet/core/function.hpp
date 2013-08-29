*=============================================================================
The MIT License (MIT)

Copyright @ 2013 by Zhang Zhimin 
p3.1415@qq.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
=============================================================================*/

#pragma once

#include <skynet/config.hpp>
#include <skynet/utility/math.hpp>

namespace skynet{

	template <typename T>
	struct derivative;


	template <typename T = double>
	struct null_function{
		T operator()(T v) const  { return v; }
	};


	template <typename T>
	struct derivative<null_function<T>>{
		derivative(null_function<T> f){}

		T operator()(T v) const	{ return T(1); }

		T value_by_self(T v) const{
			return T(1);
		}
	};
	

	template <typename T = double>
	class linear_function{
	public:
		static_assert(is_floating_point<T>::value, "The T should be float.");

		linear_function(): _scale(1){}

		linear_function(T a) : _scale(a){}

		linear_function(const linear_function &rhs): _scale(rhs._scale){}

		linear_function& operator=(const linear_function &rhs){
			_scale = rhs._scale;
			return *this;
		}

		T operator()(T v) const { 
			return _scale * v;
		}

		T scale() const							{ return _scale; }
		void scale(T v)							{ _scale = v; }

	private:
		T	_scale;
	};


	template <typename T>
	struct derivative<linear_function<T>>{
		derivative(linear_function<T> f): _f(f){}

		T operator()(T v) const{
			return _f.scale();
		}

		T value_by_self(T v) const{
			return _f.scale();
		}

	private:
		linear_function<T>		_f;
	};


	template <typename T = double>
	class sigmoid_function{
	public:
		static_assert(is_floating_point<T>::value, "The T should be float.");

		sigmoid_function() : _scale(1.716), _gamma(0.66){}

		sigmoid_function(T scale, T gamma): _scale(scale), _gamma(gamma){}

		sigmoid_function(const sigmoid_function &rhs) : _gamma(rhs._gamma), _scale(rhs._scale){}

		sigmoid_function& operator=(const sigmoid_function &rhs){
			_gamma = rhs._gamma;
			_scale = rhs._scale;
			return *this;
		}

		T operator()(T v) const{
			return  _scale * std::tanh(_gamma*v);
		}

		T gamma() const							{ return _gamma; }
		void gamma(T v)							{ _gamma = v; }

		T scale() const							{ return _scale; }
		void scale(T v)							{ _scale = v; }

	private:
		T	_gamma;
		T	_scale;
	};


	template <typename T>
	class derivative<sigmoid_function<T>>{
	public:
		derivative(sigmoid_function<T> f): _f(f){}

		T operator()(T v) const{
			auto gamma = _f.gamma();
			return _f.scale() * gamma * sqr(sech(gamma*v));
		}

		T value_by_self(T v) const{
			return max(_f.gamma() *(_f.scale() - sqr(v) / _f.scale()), numeric_limits<T>::epsilon());
		}

	private:
		sigmoid_function<T>			_f;
	};


	template <typename T = double>
	class fast_sigmoid_function{
	public:
		static_assert(is_floating_point<T>::value, "The T should be float.");

		fast_sigmoid_function() : _scale(1.716), _gamma(0.66){}

		fast_sigmoid_function(T scale, T gamma): _scale(scale), _gamma(gamma){}

		fast_sigmoid_function(const fast_sigmoid_function &rhs) : _gamma(rhs._gamma), _scale(rhs._scale){}

		fast_sigmoid_function& operator=(const fast_sigmoid_function &rhs){
			_gamma = rhs._gamma;
			_scale = rhs._scale;
			return *this;
		}

		T operator()(T v) const{
			return  _scale*_gamma*v/(1+abs(_gamma*v));
		}

		T gamma() const							{ return _gamma; }
		void gamma(T v)							{ _gamma = v; }

		T scale() const							{ return _scale; }
		void scale(T v)							{ _scale = v; }

	private:
		T	_gamma;
		T	_scale;
	};


	template <typename T>
	class derivative<fast_sigmoid_function<T>>{
	public:
		derivative(fast_sigmoid_function<T> f): _f(f){}

		//T operator()(T v) const{
		//	auto gamma = _f.gamma();
		//	return _f.scale() * gamma * sqr(sech(gamma*v));
		//}

		T value_by_self(T v) const{
			return max(_f.gamma()*_f.scale()*sqr(1-v/_f.scale()) , numeric_limits<T>::epsilon());
		}

	private:
		fast_sigmoid_function<T>			_f;
	};
}                   
