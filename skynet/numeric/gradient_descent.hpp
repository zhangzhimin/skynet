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

#include <skynet\numeric\common.hpp>
#include <skynet\utility\algorithm.hpp>
#include <skynet\utility\math.hpp>

namespace skynet{namespace numeric{


	template <typename M>
	class rprop{
	public:
		typedef M											model;
		typedef typename M::vector							vector;
		typedef typename vector::value_type					value_type;

		rprop(shared_ptr<model> model): _model(model), _init_delta(0.125),
			_eta_positive(1.2), _eta_negative(0.5), _max_delta(50), _min_delta(0.0){
				_dedw_old.resize(model->w().size());
				fill(_dedw_old, 0);
				_delta_w.resize(_dedw_old.size());
				_delta.resize(_dedw_old.size());
				fill(_delta, _init_delta);
		}

		virtual void step(){
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

			_dedw_old.assign_temporary(dedw);
			_model->update(_delta_w);
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

	};


	template <typename M>
	class lbfgs{

	};


}}