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

#include <skynet/ublas.hpp>
#include <skynet/utility/algorithm.hpp>
#include <skynet/numeric/linear_ algebra.hpp>
#include <skynet/utility/tag.hpp>

namespace skynet{namespace numeric{


	template <typename T>
	struct indicator;


	template <typename T>
	struct proximal{};


	template <>
	struct indicator<linear_equations>{
		linear_equations equations;
	};


	template <>
	struct proximal<indicator<linear_equations>>{
		indicator<linear_equations>					indicator;

		ublas::vector<double> operator()(const ublas::vector<double> &x, double tau) const{
			auto equations = indicator.equations;
			auto A = equations.A;
			auto y = equations.y;

			ublas::matrix<double> A_t = trans(A);
			ublas::matrix<double> A1 = prod(A, A_t);
			ublas::vector<double> b = y - prod(A, x);
			//sovle A1*x1 = b;
			//ublas::permutation_matrix<dobule> per(A1.size1());
			///ublas::matrix<double> A1_temp(A1.size1(), A1.size2());
			//A1_temp.assign(A1):
			//ublas::lu_factorize(A1_temp, pm);
			auto x1 = ublas::solve(A1, b, ublas::upper_tag());

			/*auto part2 = prod(prod(trans(A), 
									inverse(prod(A, tran(A)))),
								y-prod(A, x));*/
			return x + prod(A_t, x1);
		}

		size_t input_size() const{
			return indicator.equations.A.size2();
		}
	};


	template <>
	struct proximal<norm_1>{

		ublas::vector<double> operator()(const ublas::vector<double> &x, double tau) const{
			ublas::vector<double> temp(x.size());
			transform(x, temp, [&](double e){
				auto abs_value = abs(e) + numeric_limits<double>::epsilon();
				return max(0.0, 1 - tau / abs_value) * e;
			});

			return temp;
		}

	};


	template <typename ProxG, typename ProxF>
	ublas::vector<double> douglas_rachford(ProxG g, ProxF f, double mu, double gamma, size_t max_iteration){
		auto rProxG = [&g](const ublas::vector<double> &x, double tau)->ublas::vector<double>{
			return 2 * g(x, tau) - x;
		};
		auto rProxF = [&f](const ublas::vector<double> &x, double tau)->ublas::vector<double>{
			return 2 * f(x, tau) - x;
		};

		ublas::vector<double> x(f.input_size());
		fill(x, 0.0);
		ublas::vector<double> y(x.size());
		fill(y, 0.0);

		for (size_t i = 0; i < max_iteration; ++i){
			y = (1-mu/2) * y + mu/2*rProxF(rProxG(y, gamma), gamma);
			x = g(y, gamma);
		}

		return x;
	}
	

	void min_norm1(linear_equations &equations, double mu, double gamma, size_t max_iteration){
		ASSERT(is_underdetermined(equations), "The equations should be underdetermined.");
		auto proxG = proximal<norm_1>();
		auto proxF = proximal<indicator<linear_equations>>();
		proxF.indicator.equations = equations;

		equations.x = douglas_rachford(proxG, proxF, mu, gamma, max_iteration);
	}

}}
