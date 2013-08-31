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

	//double phi = (1 + Math.sqrt(5)) / 2;
	//double resphi = 2 - phi;

	//// a and c are the current bounds; the minimum is between them.
	//// b is a center point
	//// f(x) is some mathematical function elsewhere defined
	//// a corresponds to x1; b corresponds to x2; c corresponds to x3
	//// x corresponds to x4

	//public double goldenSectionSearch(double a, double b, double c, double tau) {
	//	double x;
	//	if (c - b > b - a)
	//		x = b + resphi * (c - b);
	//	else
	//		x = b - resphi * (b - a);
	//	if (Math.abs(c - a) < tau * (Math.abs(b) + Math.abs(x))) 
	//		return (c + a) / 2; 
	//	assert(f(x) != f(b));
	//	if (f(x) < f(b)) {
	//		if (c - b > b - a) return goldenSectionSearch(b, x, c, tau);
	//		else return goldenSectionSearch(a, x, b, tau);
	//	}
	//	else {
	//		if (c - b > b - a) return goldenSectionSearch(a, b, x, tau);
	//		else return goldenSectionSearch(x, b, c, tau);
	//	}
	//}
	template <typename F>
	typename unary_function_traits<F>::argument_type gold_section_search(
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
		ASSERT(is_equal(abs(ublas::inner_prod(d1,d2)), 1.0, tau), "The a b c is not in the same line.");
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
					copy(b, a);
					fa = fb;
					copy(x, b);
					fb = fx;
				}else{
					copy(x, b);
					fb = fx;
					copy(b, c);
					fc = fb;
				}
			}else{
				if (norm_2(c-b) > norm_2(b-a)){
					copy(x, c);
					fc = fx;
				}else{
					copy(x, a);
					fa = fx;
				}
			}
		}

		return (c+a)/2;
		//THROW_EXCEPTION(std::runtime_error("gold section search is not converged."));
	}


		template <typename F>
	typename unary_function_traits<F>::argument_type gold_section_search(
		F f,
		const typename unary_function_traits<F>::argument_type &a_p,
		const typename unary_function_traits<F>::argument_type &c_p,
		double tau = 1e-6, size_t max_iteration = 1000)
	{
		typedef typename unary_function_traits<F>::argument_type argument_type;
		auto b_p = (c_p-a_p) * RES_GOLD + a_p;
		return gold_section_search(f, a_p, b_p, c_p, tau, max_iteration);
	}

}}
