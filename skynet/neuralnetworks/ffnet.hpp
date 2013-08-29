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


#include <vector>

#include <skynet/ml/utility.hpp>
#include <skynet/core/function.hpp>
#include <skynet/utility/exception.hpp>
#include <skynet/utility/tag.hpp>
#include <skynet/numeric/gradient_descent.hpp>

#include <boost/math/special_functions/log1p.hpp>

namespace skynet{namespace nn{
	using namespace numeric::model;
	using namespace numeric::rprop;
	using namespace numeric::bgfs;

	template <typename Optimizer = numeric::rprop<numeric::model<>>>
	class ffnet: public model<>{
	public:
		typedef ffnet											self;

		typedef numeric::rprop<numeric::model<>>			optimizer;

		class layer_base{
		public:
			virtual void front_calculate() = 0;

			virtual vectord backpropagate(const vectord &error) = 0;

			virtual void init() = 0; 

			virtual void in(const vectord &v) = 0;

			virtual vectord out() const = 0;

			virtual size_t size() const = 0;

			virtual vectord w() const = 0;

			virtual vectord dedw() = 0;

			virtual update(const vectord &) = 0;
		};

		template <typename F = sigmoid_function<>>
		class layer : public layer_base{
		public:
			layer(size_t size) :  _out(size+1), _y(size+1), _batch_size(0){}

			void transfer_function(F f){
				_fun = f;
			}

			void init(){
				//initialize the weights, it's random and small.
				_weights.resize(_in.size(), _out.size()-1);
				auto range = 2.4 / _weights.size1();
				std::uniform_real_distribution<double> random(-range, range);
				std::mt19937_64 engine(static_cast<unsigned long>(std::rand()));
				for (size_t r = 0; r < _weights.size1(); ++r){
					for (size_t c = 0; c < _weights.size2(); ++c){
						_weights(r, c) = random(engine);
					}
				}

				_local_error.resize(_out.size()-1);
				fill(_local_error, 0.0);

				ublas::zero_matrix<double> zero_mat(_weights.size1(), _weights.size2());
				_dedws.resize(_weights.size1(), _weights.size2());
				_dedws.assign(zero_mat);
			}

			virtual void in(const vectord &v){
				_in = v;
				_in[_in.size()-1] = 1.0;
			}

			virtual vectord out() const{
				return _out;
			}

			virtual void front_calculate(){
				ASSERT(_in[_in.size()-1] == 1.0, "The bais is not 1.0.");
				auto y = ublas::prod(_in, _weights);
				std::copy(y.begin(), y.end(), _y.begin());
				transform(_y, _out, _fun);
			}

			virtual vectord backpropagate(const vectord &error){
				_batch_size++;
				//calculate local error
				derivative<F> derivative_f(_fun);
				for (size_t i = 0; i < error.size(); ++i){
					_local_error[i] = error[i] * derivative_f.value_by_self(_out[i]);
				}
				//calculate derivative errors
				for (size_t r = 0; r < _weights.size1(); ++r){
					for (size_t c = 0; c < _weights.size2(); ++c){
						_dedws(r, c) -= _in[r] * _local_error[c];
					}
				}

				auto  temp_error = ublas::prod(_weights, _local_error);
				vectord next_error(_in.size()-1);
				std::copy(temp_error.begin(), temp_error.end()-1, next_error.begin());
				return next_error;
			}

			virtual vectord dedw(){
				ASSERT(_batch_size != 0, "The backprogation is not excuted.");

				auto scale = 1.0 / _batch_size;
				vectord temp(_dedws.data().size());
				transform(_dedws.data(), temp.data(), [scale](double v){ return v * scale; });
				return temp;
			}

			virtual vectord w() const{
				vectord temp(_weights.data());
				return temp;
			}

			virtual void update(const vectord &delta_w){
				matrixd temp(_weights.size1(), _weights.size2(), delta_w.data());
				_weights += temp;
				for (size_t r = 0; r < _dedws.size1(); ++r){
					for (size_t c = 0; c < _dedws.size2(); ++c){
						_dedws(r,c) = 0.0;
					}
				}
			}

			virtual size_t  size() const						{ return _out.size()-1; }

		private:
			F _fun;
			vectord				_in;
			matrixd				_weights;
			vectord				_y;
			vectord				_out;
			vectord				_local_error;
			matrixd				_dedws;
			size_t				_batch_size;
		};
	
	///\brief	Implements the interface of numeric::model
	public:

		///\brief	Returns the weights of ffnet.
		virtual vectord w(){
			auto it_begin = _w.begin();
			for (size_t i = 0; i < _layers.size(); ++i){
				auto layer = *(_layers[i]);
				std::copy(layer.begin(), layer.end(), it_begin);
				it_begin += layer.size();
			}

			return _w;
		}

		///\brief	Returns the derivative of the error on the weights
		virtual vectord dedw(){
			auto it_begin = _dedw.begin();
			for (size_t i = 0; i < _layers.size(); ++i){
				auto layer = *(_layers[i]);
				std::copy(layer.begin(), layer.end(), it_begin);
				it_begin += layer.size();
			}

			return _dedw;
		}

		///\brief	Update the weights by the delta.
		virtual vectord update(const vectord &delta){
			_w += delta;
		}
		

	public:
		ffnet(size_t in_size, size_t out_size): _input(in_size+1), _output(out_size), _epoch_num(100){	}

		vectord operator()(const vectord &input){
			ASSERT(input.size() == (_input.size()-1), "The input size is not matched.");

			std::copy(input.begin(), input.end(), _input.begin());
			_input[_input.size()-1] = 1.0;
			_layers.front()->in(_input);
			_layers.front()->front_calculate();
			for (auto it = _layers.begin()+1; it != _layers.end(); ++it){
				(*it)->in((*std::prev(it))->out());
				(*it)->front_calculate();
			}

			auto out = _layers.back()->out();
			std::copy(out.begin(), out.end()-1, _output.begin());
			return _output;
		}

		void add_layer(shared_ptr<layer_base> sp_layer){
			_layers.push_back(sp_layer);
		}

		size_t epoch_num()	const				{ return _epoch_num; }
		void epoch_num(size_t v)				{ _epoch_num = v; }
		//
		void train(const ml::database2<double, int> &data){
			init();

			ASSERT(_output.size() == _layers.back()->size(), "");

#ifdef _CONSOLE
			std::cout << "====The bp net begin training.====" << std::endl;
			std::cout << "The samples size is: " << data.targets.size2() << std::endl;
#endif // _CONSOLE

			optimizer opt(make_shared(this));
			for (size_t epoch = 0; epoch < _epoch_num; ++epoch){
				double error = 0;
				for (size_t i = 0; i < data.targets.size2(); ++i){
					auto out = (*this)(ublas::column(data.patterns, i));
					vectord e = column(data.targets, i) - out;
					error += ublas::norm_2(e);

					for (auto it = _layers.rbegin(); it != _layers.rend(); ++it){
						e = (*it)->backpropagate(e);
					}
				}
				error /= data.targets.size2();
#ifdef _CONSOLE
				std::cout << epoch << " epoch error: " << error << std::endl;
#endif // _CONSOLE
				opt.step();
//				for (auto &e : _optimizers){
//					e.step();
//				}
			}
		};

	private:
		void init(){
			ASSERT(!_layers.empty(), "Please add the layers to ffnet.");

			//this make the weights matrix size is matched.
			_layers.front()->in(_input);
			_layers.front()->init();

			size_t size_pre = _layers.front().size(); //pre-layer neuron size
			size_t w_size = (_input.size()+1) * size_pre;    //the size of the all weights
			for (auto it = _layers.begin()+1; it != _layers.end(); ++it){
				(*it)->in((*std::prev(it))->out());
				(*it)->init();

				size_t size_cur = (*it)->size();
				w_size += (size_pre+1)*size_cur;
			}

			_w.resize(w_size);
			_dedw.resize(w_size);

			for (size_t i = 0; i < _layers.size(); ++i){
				_optimizers.push_back(optimizer(_layers[i]));
			}
		}

	private:
		std::vector<shared_ptr<layer_base>>					_layers;
		std::vector<optimizer>								_optimizers;
		vectord												_input;
		vectord												_output;

		size_t												_epoch_num;
		
		vectord												_w;
		vectord												_dedw;	
	};


}}
