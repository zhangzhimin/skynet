/*=============================================================================
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

#include <skynet/ublas.hpp>
#include <skynet/utility/math.hpp>
#include <skynet/utility/algorithm.hpp>
#include <skynet/utility/type_traits.hpp>

namespace skynet{namespace numeric{

	using namespace boost::numeric::ublas;


	///\brief				Implements the gold section search algorithm,
	///						http://en.wikipedia.org/wiki/Golden_section_search
	///\param[in] f			The criterion function which whil be opimized.
	///\param[in] a_p		The initial a point.
	///\param[in] b_p		The initial b point.
	///\param[in] c_p		The initial c point.
	///\param[in] tau		The precision
	template <typename F>
	typename unary_function_traits<F>::argument_type golden_section_search(
		F f,
		const typename unary_function_traits<F>::argument_type &a_p,
		const typename unary_function_traits<F>::argument_type &b_p,
		const typename unary_function_traits<F>::argument_type &c_p,
		double tau = 1e-6, size_t max_iteration = 1000)
	{
		typedef typename unary_function_traits<F>::argument_type	argument_type;
		static_assert(std::is_arithmetic<typename unary_function_traits<F>::result_type>::value, "");

		//function will make sure the size of a_p, b_p, c_p is the same.
#ifndef DISABLE_ASSERT
		auto d1 = normalize(c_p-b_p);
		auto d2 = normalize(c_p-a_p);
		auto dir = ublas::inner_prod(d1,d2);
		ASSERT(is_equal(abs(dir), 1.0, 0.1), "The a b c is not in the same line.");
#endif // !DISABLE_ASSERT

		argument_type a(a_p.size());
		argument_type b(b_p.size());
		argument_type c(c_p.size());
		copy(a_p, a);
		copy(b_p, b);
		copy(c_p, c);

		auto fa = f(a);
		auto fb = f(b);
		auto fc = f(c);
		for (size_t i = 0; i < max_iteration; ++i){
			argument_type x;
			if (norm_2(c-b) > norm_2(b-a))
				x = b + RES_GOLD * (c - b);
			else
				x = b - RES_GOLD * (b - a);
			if (norm_1(c-a) < tau * (norm_1(b) + norm_1(x)))
				return (c+a)/2;
			auto fx = f(x);
			if (fx < fb){
				if (norm_2(c-b) > norm_2(b-a)){
					a = b;
					fa = fb;
					b = x;
					fb = fx;
				}else{
					c = b;
					fc = fb;
					b = x;
					fb = fx;
				}
			}else{
				if (norm_2(c-b) > norm_2(b-a)){
					c = x;
					fc = fx;
				}else{
					a = x;
					fa = fx;
				}
			}
		}

		return (a+c)/2;
	}


	template <typename F>
	typename unary_function_traits<F>::argument_type golden_section_search(
		F f,
		const typename unary_function_traits<F>::argument_type &a_p,
		const typename unary_function_traits<F>::argument_type &c_p,
		double tau = 1e-6, size_t max_iteration = 1000)
	{
		typedef typename unary_function_traits<F>::argument_type argument_type;
		auto b_p = (c_p-a_p) * RES_GOLD + a_p;
		return golden_section_search(f, a_p, b_p, c_p, tau, max_iteration);
	}


	template <typename F>
	typename unary_function_traits<F>::argument_type backtracking_line_search(
		F f,
		const typename unary_function_traits<F>::argument_type &start_point,
		const typename unary_function_traits<F>::argument_type &direction,
		double tau = 0.8, double c1 = 1e-4, double c2 = 0.9, size_t max_iteration = 10)
	{
		typedef typename unary_function_traits<F>::argument_type	argument_type;
		ASSERT(c2 > c1, "c2 must be greater than c1.");

		argument_type	point(start_point.size());
		point.assign(start_point);
		auto value_old = f(point);
		argument_type derivative_old(point.size());
		derivative_old.assign(f.derivative(point));

		double step_scale = 1.0;
		for (size_t i = 0; i < max_iteration; ++i, step_scale *= tau){
			point = start_point + step_scale * direction;
			auto value = f(point);
			auto derivative = f.derivative(point);

			auto pf = inner_prod(direction, derivative_old);
			//wolfe condition
			if (value <= value_old + c1 * step_scale * pf){
				if (abs(inner_prod(direction, derivative)) <= abs(c2 * pf)){
					return point;
				}
			}
		}

		return point;
	}
}}
