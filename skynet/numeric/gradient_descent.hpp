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
		rprop(): _init_delta(0.125), _eta_positive(1.2), _eta_negative(0.5),
			_max_delta(50), _min_delta(0.0){}

		vector optimize(model &m, const vector &start_point){
			vector dedw_old(start_point.size());
			fill(dedw_old, 0);
			vector dedw(start_point.size());
			vector delta(dedw_old.size());
			fill(delta, _init_delta);

			vector point(start_point.size());
			point.assign(start_point);
			vector delta_w(start_point.size());
			for (size_t epoch = 0; epoch != _iteration_num; ++epoch){
				auto error = m(point);
#ifdef _CONSOLE
				std::cout << epoch << " iteration error is: " << error << std::endl;
#endif // _CONSOLE
				dedw.assign(m.derivative(point));
				for (size_t i = 0; i < dedw.size(); ++i){
					auto re = dedw_old[i] * dedw[i];
					if (re > 0){
						delta[i] = min(delta[i]*_eta_positive, _max_delta);
						delta_w[i] = -sign(dedw[i]) * delta[i];
					}else if(re <  0){
						delta[i] = max(delta[i]*_eta_negative, _min_delta);
						delta_w[i] *= -1;
						dedw[i] = 0.0;
					}else{
						delta_w[i] = -sign(dedw[i]) * delta[i];
					}
				}

				point += delta_w;
				dedw_old.assign_temporary(dedw);
			}

			return point;
		}

		double eta_positive() const							{ return _eta_positive; }
		void eta_positive(double v)							{ _eta_positive = v; }
		double eta_negative() const							{ return _eta_negative; }
		void eta_negative(double v)							{ _eta_negative = v; }

		double max_delta() const							{ return _max_delta; }
		void max_delta(double v)							{ _max_delta = v; }

		double min_delta() const							{ return _min_delta; }
		void min_delta(double v)							{ _min_delta = v; }

		double init_delta() const							{ return _init_delta; }
		void init_delta(double v)							{ _init_delta = v; }

		size_t iteration_num()	const						{ return _iteration_num; }
		void iteration_num(size_t v)						{ _iteration_num = v; }

	private:
		value_type				_eta_positive;
		value_type				_eta_negative;
		value_type				_max_delta;
		value_type				_min_delta;
		value_type				_init_delta;

		size_t					_iteration_num;
	};


	/*
	///\brief	Implements the BFGS algorithm. http://en.wikipedia.org/wiki/BFGS, it's not effective.
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

	auto best_point = golden_section_search(_model_fun, _start_point, _end_point, 1e-3, 10);
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
	auto best_point = golden_section_search(_model_fun, _start_point, _end_point, 1e-3, 10);
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


	*/

	///\brief	Implements the limited memory BFGS algorithm. http://en.wikipedia.org/wiki/LBFGS, it's not effective.
	template <typename Model>
	class lbfgs{
	public:
		typedef Model								model;
		typedef typename model::vector				vector;
		typedef typename vector::value_type			value_type;
		typedef ublas::matrix<value_type>			matrix;

		lbfgs(): _hist_num(5), _iteration_num(100){}

		vector optimize(model &m, const vector &start_point){
#ifdef _CONSOLE
			std::cout << "==========The LBFGS start==========" << std::endl;
#endif // _CONSOLE

			vector point_old(start_point.size());
			point_old.assign(start_point);

			///do none, but some model  need do this, make sure the derivative is right.
			m(start_point);
			vector g0 = m.derivative(start_point);
			vector point = backtracking_line_search(m, point_old, -1*g0);
			//vector point = golden_section_search(m, point_old, point_old - g0, 1e-10, 5);

			for (size_t i = 0; i < _iteration_num; ++i){
				value_type error = m(point);
#ifdef _CONSOLE
				std::cout << i << " iteration error is: " << error << std::endl;
#endif
				vector g1(point.size());
				g1.assign(m.derivative(point));
				vector y = g1 - g0;
				vector s = point - point_old;
				push_info(y, s);
				point_old.assign_temporary(point);
				auto dir = get_direction(g1);
				point = backtracking_line_search(m, point_old, dir);
				//point = golden_section_search(m, point_old, point_old+dir,1e-10, 5);
				g0.assign_temporary(g1);
			}
#ifdef _CONSOLE
			std::cout << "============The LBFGS end============" << std::endl;
#endif // _CONSOLE
			return point;
		}	

		size_t iteration_num()	const			{ return _iteration_num; }
		void iteration_num(size_t v)			{ _iteration_num = v; }

	private:
		void push_info(const vector &y, const vector &s){
			auto sy = inner_prod(s, y);

			if (sy < 1e-10)	return;

			_ss.push_back(s);
			_ys.push_back(y);
			auto rho = 1.0/sy;
			_rhos.push_back(rho);
			if (_ss.size() > _hist_num){
				_ss.pop_front();
				_ys.pop_front();
				_rhos.pop_front();
			}

			_hdiag = sy / inner_prod(y,y);
		}

		vector get_direction(const vector &derivative){
			vector q(derivative.size());
			q.assign(-1 * derivative);

			if (_ss.empty())
				return q;

			std::deque<value_type> alphas(_ss.size());
			for (ptrdiff_t i = _ss.size()-1; i >= 0; --i){
				alphas[i] = _rhos[i] * inner_prod(_ss[i], q);
				q -= alphas[i] * _ys[i];
			}
			q *= _hdiag;
			for (size_t i = 0; i < _ss.size(); ++i){
				value_type beta = _rhos[i] * inner_prod(_ys[i], q);
				q += (alphas[i]-beta) * _ss[i];
			}

			return q;
		}

	private:
		value_type								_hdiag;
		value_type								_threshold;
		size_t									_hist_num;
		std::deque<vector>						_ss;
		std::deque<vector>						_ys;
		std::deque<double>						_rhos;

		size_t									_iteration_num;
	};


}}
