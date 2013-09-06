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

#include <skynet/ml/common.hpp>
#include <skynet/core/function.hpp>
#include <skynet/utility/exception.hpp>
#include <skynet/numeric/gradient_descent.hpp>

#pragma warning(disable: 4996)

namespace skynet{namespace nn{

	using namespace skynet::numeric;


	///\brief	Implements the forward feedback neural networks
	class ffnet{
	public:
		typedef ffnet											self;

		///\brief	The interface of hidden layer or output layer
		class layer_base{
		public:
			virtual void front_calculate() = 0;

			virtual vectord back_propagate(const vectord &error) = 0;

			virtual void init() = 0; 

			virtual void in(const vectord &v) = 0;

			virtual vectord out() const = 0;

			virtual size_t size() const = 0;

			virtual vectord w() = 0;

			virtual void w(const vectord &)  = 0;

			virtual vectord dedw() = 0;

			virtual void update() = 0;
		};

		///\brief	The hidden or output layer with transfer function template parameter.
		template <typename F = tanh_function<>>
		class layer : public layer_base{
		public:
			layer(size_t size) :  _out(size+1), _y(size+1), _batch_size(0){}

			void transfer_function(F f){
				_fun = f;
			}

			virtual void init(){
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

			virtual vectord back_propagate(const vectord &error){
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
				ASSERT(_batch_size > 0, "The backprogation is not excuted.");

				auto scale = 1.0 / _batch_size;
				vectord dedws(_dedws.data().size());
				//transform(_dedws.data(), dedws, [scale](double e){ return scale * e;});
                transform(_dedws.data(), dedws, [scale](double e){ return e;});
				return dedws;
			}

			virtual vectord w() {
				return vectord(_weights.data());
			}

			virtual void w(const vectord &v){
				_weights = matrixd(_weights.size1(), _weights.size2(), v.data());
			}

			///\brief	Zero the derivative and the counter.
			virtual void update(){
				for (size_t r = 0; r < _dedws.size1(); ++r){
					for (size_t c = 0; c < _dedws.size2(); ++c){
						_dedws(r,c) = 0.0;
					}
				}
				_batch_size = 0;
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

		class sparse_layer_base: public layer_base{
		public:
			virtual double kl_divergence() = 0;
			virtual double beta() const = 0;
		};

		template <typename F = sigmoid_function<>>
		class sparse_layer: public sparse_layer_base{
		public:
			sparse_layer(size_t size) :  _out(size+1), _y(size+1), _batch_size(0), _local_error(size, 0.0),
				_activaties(size), _sparseness(0.05), _beta(1.0), _refresh(true){}

			void transfer_function(F f){
				_fun = f;
			}

			virtual void init(){
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

			virtual vectord back_propagate(const vectord &error){
				_batch_size++;
				//calculate local error
				derivative<F> derivative_f(_fun);
				for (size_t i = 0; i < error.size(); ++i){
					_local_error[i] = error[i]*derivative_f.value_by_self(_out[i]); 
					_local_error[i]	-= derivative_f.value_by_self(_out[i]) * _beta * (-_sparseness/max(_out[i],1e-10) + (1-_sparseness)/max((1-_out[i]), 1e-10));
				}
				//calculate derivative errors
				for (size_t r = 0; r < _weights.size1(); ++r){
					for (size_t c = 0; c < _weights.size2(); ++c){
						_dedws(r, c) -= _in[r] * _local_error[c];
					}
				}

				for (size_t i = 0; i < _activaties.size(); ++i){
					_activaties[i] += _out[i];
				}

				auto  temp_error = ublas::prod(_weights, _local_error);
				vectord next_error(_in.size()-1);
				std::copy(temp_error.begin(), temp_error.end()-1, next_error.begin());
				return next_error;
			}

			virtual vectord dedw(){
				ASSERT(_batch_size != 0, "");
				auto scale = 1.0 / _batch_size;
				_dedws *= scale;

				return vectord(_dedws.data());
			}

			virtual vectord w() {
				return vectord(_weights.data());
			}

			virtual void w(const vectord &v){
				_weights = matrixd(_weights.size1(), _weights.size2(), v.data());
			}

			///\brief	Zero the derivative and the counter.
			virtual void update(){
				for (size_t r = 0; r < _dedws.size1(); ++r){
					for (size_t c = 0; c < _dedws.size2(); ++c){
						_dedws(r,c) = 0.0;					
					}
				}
				_batch_size = 0;
				fill(_activaties, 0.0);
				_refresh = true;
			}

			virtual double kl_divergence(){
				/*_activaties /= _batch_size;*/
				double scale = 1.0/_batch_size;
				vectord mean_activaties(_activaties.size());
				transform(_activaties, mean_activaties, [scale](double e){ return scale*e;});
				vectord kl(mean_activaties.size());
				for (size_t i = 0; i < kl.size(); ++i){
					kl[i] = _sparseness*log(_sparseness/max(1e-10,mean_activaties[i])) + (1-_sparseness)*log((1-_sparseness)/max(1e-10,(1-mean_activaties[i])));
				}

#ifdef _CONSOLE
				std::cout << "the kl error is: " << sum(kl) << std::endl;
#endif // _CONSOLE


				return sum(kl);
			}

			virtual size_t  size() const						{ return _out.size()-1; }

			double sparseness() const									{ return _sparseness; }
			void sparseness(double v)									{ _sparseness = v; }

			virtual double beta() const											{ return _beta; }
			void beta(double v)											{ _beta = v; }

		private:
			F _fun;
			vectord				_in;
			matrixd				_weights;
			vectord				_y;
			vectord				_out;
			vectord				_local_error;
			matrixd				_dedws;
			size_t				_batch_size;

			vectord				_activaties;
			double				_sparseness;
			double				_beta;

			bool				_refresh;
		};

		///\brief	Implements the interface of numeric::model
	private:

		///\brief	Gets the weights of ffnet(shallow copy).
		vectord w(){
			auto it_begin = _w.begin();
			for (size_t i = 0; i < _layers.size(); ++i){
				auto w = _layers[i]->w();
				std::copy(w.begin(), w.end(), it_begin);
				it_begin += w.size();
			}

			return _w;
		}

		///\brief	Gets the derivative of the error on the weights(shallow copy)
		vectord dedw(){
			auto it_begin = _dedw.begin();
			for (size_t i = 0; i < _layers.size(); ++i){
				auto layer_dedw = _layers[i]->dedw();
				std::copy(layer_dedw.begin(), layer_dedw.end(), it_begin);
				it_begin += layer_dedw.size();
			}

			return _dedw;
		}

		///\brief	Sets the weights of ffnet(shallow copy)
		void w(const vectord &v){
			_w = v;

			//copy the ffnet weights to the layers weights
			auto it_begin = _w.begin();
			for (size_t i = 0; i < _layers.size(); ++i){
				vectord  w_temp(_layers[i]->w().size());
				std::copy(it_begin, it_begin+w_temp.size(), w_temp.begin());
				_layers[i]->w(w_temp);

				it_begin += w_temp.size();
				_layers[i]->update();
			}
		}

		double error(const vectord &v){
			this->w(v);

			double mse = 0;
			for (size_t i = 0; i < _data.targets.size2(); ++i){
				auto out = (*this)(ublas::column(_data.patterns, i));
				vectord e = column(_data.targets, i) - out;
				mse += ublas::norm_2(e);

				for (auto it = _layers.rbegin(); it != _layers.rend(); ++it){
					e = (*it)->back_propagate(e);
				}
			}
			mse /= _data.targets.size2();

			for (size_t i = 0; i < _layers.size(); ++i){
				auto sp = std::dynamic_pointer_cast<sparse_layer_base>(_layers[i]);
				if (sp){
					mse += sp->beta() * sp->kl_divergence();
				}
			}

			return mse;
		}
	public:
		///\brief	Adapte the model to function with derivative.		 
		class model{
		public:
			typedef vectord		vector;
			typedef double		value_type;

			model(ffnet &sp_model): _model(sp_model){}

			///\brief	Gets the function result.
			value_type operator()(const vector &input){
				return _model.error(input);
			}

			///\brief	Gets the function derivative, the function should be invoked after operator().
			///			Because the input are unused.
			vector	derivative(const vector &input){
				return _model.dedw();
			}

		private:
			ffnet		&_model;
		};

	public:
		ffnet(const self &rhs): _layers(rhs._layers), _input(rhs._input), _output(rhs._output), 
			_w(rhs._w), _dedw(rhs._dedw){}
		///\brief	Constructs the ffnet by the input size and output size.
		ffnet(size_t in_size, size_t out_size): _input(in_size+1), _output(out_size){	}

		///\brief	Returs the prediction value.
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

		///\brief	Adds the hidden or output layer to ffnet.
		void add_layer(shared_ptr<layer_base> sp_layer){
			_layers.push_back(sp_layer);
		}

		std::vector<shared_ptr<layer_base>>	layers() const{
			return _layers;
		}

		///\brief		Trains the ffnet
		///\param [in] data	The traing data
		///\param [in] opt	The optimizer based on gradient descent.
		void train(const ml::database2<double, double> &data, optimizer_base<model> &opt){
			init();

			ASSERT(_output.size() == _layers.back()->size(), "");
			_data = data;
			model f(*this);

			opt.optimize(f, w());
		};

		void train(const ml::database2<double, double> &data){
			optimizer_adaptor<lbfgs<ffnet::model>> opt;
			train(data, opt);			
		}

		void train(optimizer_base<model> &opt){
			model f(*this);
			opt.optimize(f, w());
		}

	private:
		void init(){
			ASSERT(!_layers.empty(), "Please add the layers to ffnet.");

			//this make the weights matrix size is matched.
			_layers.front()->in(_input);
			_layers.front()->init();

			size_t size_pre = _layers.front()->size(); //pre-layer neuron size
			size_t w_size = (_input.size()) * size_pre;    //the size of the all weights
			for (auto it = _layers.begin()+1; it != _layers.end(); ++it){
				(*it)->in((*std::prev(it))->out());
				(*it)->init();

				size_t size_cur = (*it)->size();
				w_size += (size_pre+1)*size_cur;
			}

			_w.resize(w_size);
			_dedw.resize(w_size);
		}

	private:
		std::vector<shared_ptr<layer_base>>					_layers;
		vectord												_input;
		vectord												_output;

		vectord												_w;
		vectord												_dedw;	

		ml::database2<double, double>						_data;
	};

}}
