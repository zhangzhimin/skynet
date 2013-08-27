/*=============================================================================
The MIT License (MIT)

Copyright @ 2013 Zhang Zhimin

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

#include <skynet\ublas.hpp>
#include <skynet\utility\algorithm.hpp>

namespace skynet{namespace nn{

	class art1{
	public:
		typedef double							value_type;
		typedef ublas::matrix<double>			matrix;
		typedef ublas::vector<double>			vector;

		double rho() const						{ return _rho; }
		void rho(double v)						{ _rho = v; }

		art1(const matrix &patterns):_patterns(patterns), _rho(0.5){

			//init weights
			int n = _patterns.size1();
			int m = 1;
			_Out.resize(m, n);
			for (int row = 0; row < _Out.size1(); ++row){
				for (int col = 0; col < _Out.size2(); ++col){
					_Out(row, col) = 1;
				}
			}
			_In = (2 / (1+n)) * ublas::trans(_Out);

			//training
			bool new_node_flag = true;
			while(new_node_flag){
				new_node_flag = false;

				for (int k = 0; k < _patterns.size2(); ++k){
					bool outstar_flag = false;
					vector index(m);
					fill(index, 1);					//all nodes can compute

					auto pattern = ublas::column(_patterns, k);
					vector Y = ublas::prod(pattern, _In);

					while(sum(index) != 0){
						auto max_it = std::max_element(Y.begin(), Y.end());
						auto max_index = max_it - Y.begin();
						auto s = ublas::element_prod(ublas::row(_Out, max_index), pattern);
						auto sum_s = sum(s);
						double M = sum_s / sum(pattern);

						if (M > _rho){
							ublas::row(_Out, max_index) = s;
							ublas::column(_In, max_index) = (2/(1+sum_s)) * s;
							outstar_flag = true;
							break;
						}else{
							index[max_index] = 0;
							Y[max_index] = -1;
						}
					}

					if ((!outstar_flag) && (sum(index) == 0)){
						m += 1;
						matrix new_out(m, n);
						for (int row = 0; row < _Out.size1(); ++row){
							for (int col = 0; col < _Out.size2(); ++col){
								new_out(row, col) = _Out(row, col);
							}
						}
						ublas::row(new_out, m) = pattern;
						_Out = new_out;

						matrix new_in(n, m);
						for (int row = 0; row < _In.size1(); ++row){
							for (int col = 0; col < _In.size2(); ++col){
								new_in(row, col) = _In(row, col);
							}
						}
						_In = new_in;

						new_node_flag = true;
					}
				}
			}
		}

	private:
		double			_rho;
		matrix			_patterns;
		matrix			_In;
		matrix			_Out;
	};

}}