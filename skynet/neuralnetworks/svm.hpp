#pragma once

#include <skynet\ml\utility.hpp>
#include <skynet\utility\tag.hpp>
#include <skynet\neuralnetworks\kenel_function.hpp>
#include <skynet\utility\algorithm.hpp>
#include <skynet\utility\math.hpp>

namespace skynet{namespace nn{


	class linear_svm{
	public:
		typedef none									kenel_functor;

		linear_svm() : _tol(1e-3), _eps(1e-3), _C(1.0){}

		void C(double v)								{ _C = v; }
		double C() const								{ return _C; }

		void learn(const ml::database<double, double> &data){
			_data = data;

			init();

			int changed = 0;
			bool examine_all = true;

			size_t   counter = 0;
			while (changed || examine_all){
				changed = 0;
				if (examine_all){
					for (size_t i = 0; i < data.targets.size(); ++i){
						if (examine_example(i))
							changed++;
					}
				}else{
					for (size_t i = 0; i < data.targets.size(); ++i){
						if (_¦«[i] < _eps || (_¦«[i] > (C()-_eps)))	continue;
						if (examine_example(i))
							changed++;
					}
				}
				if (examine_all)
					examine_all = false;
				else if(!changed){
					examine_all = true;
					counter++;
					ASSERT(counter < 10, "The smo is not aconverged.");
				}
			}
		}

		template <typename E>
		double operator()(const ublas::vector_expression<E> &ve) const{
			return ublas::inner_prod(ve(), _W) + _b;
		}

	private:
		bool take_step(int i1, int i2){
			auto x1 = ublas::column(_data.patterns, i1);
			auto x2 = ublas::column(_data.patterns, i2);
			auto y1 = _data.targets[i1];
			auto y2 = _data.targets[i2];

			double lower = 0.0;
			double upper = 0.0;
			auto &¦Á1 = _¦«[i1];
			auto &¦Á2 = _¦«[i2];
			if (y1 != y2){
				lower = max(0.0, ¦Á2 - ¦Á1);
				upper = min(_C, _C+¦Á2-¦Á1);
			}else{
				lower = max(0.0, ¦Á2+¦Á1-_C);
				upper = min(_C, ¦Á2+¦Á1);
			}

			auto k11 = ublas::inner_prod(x1, x1);
			auto k22 = ublas::inner_prod(x2, x2);
			auto k12 = ublas::inner_prod(x1, x2);
			auto ¦Ç = k11 + k22 - 2*k12;
			double ¦Á2_new = 0;

			auto e1 = _E[i1];
			auto e2 = _E[i2];

			if (¦Ç > 1e-5){
				¦Á2_new = ¦Á2 + y2*(e1 - e2)/¦Ç;
				¦Á2_new = min(upper, max(lower, ¦Á2_new));
			}else{
				return false;
			}

			auto ¦Á1_new = ¦Á1 + y1*y2 * (¦Á2 - ¦Á2_new);

			if (abs(¦Á2_new-¦Á2) < _eps)
				return false;

			//updated
			auto b1 = -e1 + y1 * (¦Á1_new - ¦Á1) * k11 + y2 * (¦Á2_new - ¦Á2) * k12 + _b;
			auto b2 = -e2 + y1 * (¦Á1_new - ¦Á1) * k12 + y2 * (¦Á2_new - ¦Á2) * k22 + _b;

			_b = (b1 + b2) * 0.5;

			_W += y1 * (¦Á1_new - ¦Á1) * x1 + y2 * (¦Á2_new - ¦Á2) * x2;

			¦Á1 = ¦Á1_new;
			¦Á2 = ¦Á2_new;

			return true;
		}

		bool examine_example(size_t i2){
			auto &y2 = _data.targets[i2];
			auto &¦Á2 = _¦«[i2];
			auto &e2 = _E[i2];
			e2 = (*this)(ublas::column(_data.patterns, i2)) - y2;
			auto r2 = e2 * y2;

			if ((r2 < -_tol && ¦Á2 < C()) || (r2 > _tol && ¦Á2 > 0 )){
				size_t i1 = 0;
				if (e2 > 0){
					i1 = 1;
					double min_v = 10000000;
					for (size_t i = 0; i < _data.targets.size(); ++i){
						if (i == i2)	continue;
						if (_E[i] < min_v){
							i1 = i;
							min_v = _E[i];
						}
					}
				}else{
					i1 = 1;
					double max_v = -10000000;
					for (size_t i = 0; i < _data.targets.size(); ++i){
						if (i == i2)	continue;
						if (_E[i] > max_v){
							i1 = i;
							max_v = _E[i];
						}
					}
				}

				_E[i1] = (*this)(ublas::column(_data.patterns, i1)) - _data.targets[i1];
				if (take_step(i1, i2))
					return true;
			}

			std::uniform_int_distribution<size_t>       random(0, _data.targets.size()-1);
			std::mt19937								mt(static_cast<unsigned int>(std::time(nullptr)));
			auto start_i = random(mt);
			for (size_t i = start_i;;){
				if (!(_¦«[i] < _eps || (_¦«[i] > (C()-_eps)))){
					if (i != i2){
						_E[i] = (*this)(ublas::column(_data.patterns, i)) - _data.targets[i];
						if (take_step(i, i2))				
							return true;
					}
				}

				++i;
				i = i < _data.targets.size() ? i : i - _data.targets.size();
				if (i == start_i){	
					break;
				}
			}


			auto start_i2 = random(mt);
			for (size_t i = start_i2;;){
				if (i != i2){
					_E[i] = (*this)(ublas::column(_data.patterns, i)) - _data.targets[i];
					if (take_step(i, i2))				
						return true;
				}

				++i;
				i = i < _data.targets.size() ? i : i-_data.targets.size();
				if (i == start_i2){	
					break;
				}

			}

			return false;
		}

		void init(){
			_W.resize(_data.patterns.size1());
			std::uniform_real_distribution<double>  random1(-1.0/_W.size(), 1.0/_W.size());
			std::mt19937 mt1(static_cast<unsigned int>(std::time(nullptr)));
			std::generate(_W.begin(), _W.end(),[&](){ return random1(mt1); }); 
			_b = 0.0;
			_E.resize(_data.targets.size());

			std::uniform_real_distribution<double>  random(0.0, C());
			std::mt19937							mt(static_cast<unsigned int>(std::time(nullptr)));
			_¦«.resize(_data.targets.size());
			std::generate(_¦«.begin(), _¦«.end(), [&](){ return random(mt); });

			for (size_t i = 0; i < _data.patterns.size2(); ++i){
				_E[i] = (*this)(ublas::column(_data.patterns, i)) - _data.targets[i];
			}
		}

	private:
		ublas::vector<double>							_¦«;
		double											_C;

		ml::database<double, double>						_data;

		ublas::vector<double>							_W;
		double											_b;

		ublas::vector<double>							_E;

		double											_tol;
		double											_eps;
	};

}}