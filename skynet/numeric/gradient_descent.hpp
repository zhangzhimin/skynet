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

#include <skynet/numeric/common.hpp>
#include <skynet/utility/algorithm.hpp>
#include <skynet/utility/math.hpp>
#include <skynet/numeric/line_search.hpp>

namespace skynet{namespace numeric{

	///\brief Rprop algorithm, http://en.wikipedia.org/wiki/Rprop
	template <typename M>
	class rprop{
	public:
		typedef M											model;
		typedef typename M::vector							vector;
		typedef typename vector::value_type					value_type;

		///\brief	Constructs by the model smart point, and initialize the parameters.
		rprop(shared_ptr<model> model): _model(model), _init_delta(0.125),
			_eta_positive(1.2), _eta_negative(0.5), _max_delta(50), _min_delta(0.0), _initialized(false){}

		///\brief	Iterates once.
		virtual void step(){
			if (!_initialized){
				_initialized = true;
				_dedw_old.resize(_model->w().size());
				fill(_dedw_old, 0);
				_delta_w.resize(_dedw_old.size());
				_delta.resize(_dedw_old.size());
				fill(_delta, _init_delta);
			}

			auto dedw = _model->dedw();
			for (size_t i = 0; i < dedw.size(); ++i){
				auto re = _dedw_old[i] * dedw[i];
				if (re > 0){
					_delta[i] = min(_delta[i]*_eta_positive, _max_delta);
					_delta_w[i] = -sign(dedw[i]) * _delta[i];
				}else if(re <  0){
					_delta[i] = max(_delta[i]*_eta_negative, _min_delta);
					_delta_w[i] *= -1;
					dedw[i] = 0.0;
				}else{
					_delta_w[i] = -sign(dedw[i]) * _delta[i];
				}
			}

			_dedw_old.assign(dedw);
			_model->w(_model->w() + _delta_w);
		}

		double eta_positive() const							{ return _eta_positive; }
		void eta_positive(double v)							{ _eta_positive = v; }
		double eta_negative() const							{ return _eta_negative; }
		void eta_negative(double v)							{ _eta_negative = v; }

		double max_delta() const							{ return _max_delta; }
		void max_delta(double v)							{ _max_delta = v; }

		double min_delta() const							{ return _min_delta; }
		void min_delta(double v)							{ _min_delta = v; }

		double init_delta() const								{ return _init_delta; }
		void init_delta(double v)								{ _init_delta = v; }

	private:
		shared_ptr<M>	  _model;
		vector			  _dedw_old;
		vector			  _delta;
		vector			  _delta_w;
		value_type			  _eta_positive;
		value_type			  _eta_negative;
		value_type			  _max_delta;
		value_type			  _min_delta;
		value_type			  _init_delta;
		bool				  _initialized;
	};


	///\brief	Implements the BFGS algorithm. http://en.wikipedia.org/wiki/BFGS
	template <typename Model>
	class bfgs{
	public:
		typedef Model								model;
		typedef typename model::vector				vector;
		typedef typename vector::value_type			value_type;
		typedef ublas::matrix<value_type>			matrix;

		bfgs(shared_ptr<model> model) : _model(model), _first(true), _model_fun(_model){}

		///\brief	Iterates once
		void step(){
			//initialize the vector and matrix size, and the first derivative.
			if (_first){
				//initialize the diag element is , others are zero.
				_hession_inv.resize(_model->w().size(), false);
				_hession_inv.assign(ublas::zero_matrix<value_type>(_hession_inv.size1(), _hession_inv.size2()));
				for (size_t i = 0; i < _hession_inv.size1(); ++i){
					_hession_inv(i, i) = 1.0;
				}

				_g_k0.resize(_model->dedw().size());
				_g_k0.assign(_model->dedw());
				_start_point.resize(_model->w().size());
				_start_point.assign(_model->w());
				_end_point.resize(_model->w().size());
				_end_point = _start_point - _g_k0;

				auto best_point = gold_section_search(_model_fun, _start_point, _end_point, 1e-3, 10);
				//_s = -0.01 * _g_k0;
				_model->w(best_point);
				_s = best_point - _start_point;
				_first = false;
				return;
			}

			_g_k1 = _model->dedw();
			vector y = _g_k1 - _g_k0;
			auto s_t_y = inner_prod(_s,y);
			matrix	s_mat(_s.size(), 1, _s.data());
			matrix y_mat(y.size(), 1, y.data());
			matrix part1 = (1.0/ sqr(s_t_y)) * (s_t_y+inner_prod(y, prod(_hession_inv, y))) * prod(s_mat,trans(s_mat));
			matrix part21 = prod(y_mat, trans(s_mat));
			part21 = prod(_hession_inv, part21);
			matrix part22 = prod(trans(y_mat), _hession_inv);
			part22 = prod(s_mat, part22);
			matrix part2 = (-1.0/s_t_y) * (part21 + part22);
			_hession_inv = _hession_inv + part1 + part2;
			auto d_k = -prod(_hession_inv, _g_k1);

			_start_point.assign(_model->w());
			_end_point = _start_point + d_k;
			auto best_point = gold_section_search(_model_fun, _start_point, _end_point, 1e-3, 10);
			_model->w(best_point);
			_s = best_point - _start_point;

			_g_k0.assign(_g_k1);
		}

	private:
		shared_ptr<model>						_model;
		ublas::symmetric_matrix<value_type>		_hession_inv;
		vector									_g_k0;
		vector									_g_k1;
		vector									_s;
		bool									_first;

		vector									_start_point;
		vector									_end_point;
		model_function<model>					_model_fun;
	};

}}
