#pragma once

#include <skynet\utility\tag.hpp>
#include <skynet\ublas.hpp>

#include <random>
#include <boost\numeric\ublas\symmetric.hpp>

namespace skynet{namespace nn{

	template <typename T>
	class boltzmann_machine;


	template <>
	class boltzmann_machine<full_connected> {
	public:
		boltzmann_machine(size_t visible_num, size_t hidden_num)    
			: _visible_num(visible_num), _hidden_num(visible_num), _T(10), _c(0.99), _cycles_num(10),
			_eta(1.0), _k(10){

		}

		template <typename T>
		void learn(const ublas::matrix_expression<T> &patterns){
			init(patterns);

			std::uniform_real_distribution<double> rand_energe(0, 1.0);
			std::mt19937						   mt1(std::time(nullptr));
			std::uniform_int_distribution<size_t> rand_row(0, _clamped_signals.size1()-1);
			std::mt19937						   mt2(std::time(nullptr));

			for (size_t k = 0; k < k(); ++k){
				while (_T > 1.0){
					auto Xf = ublas::prod(_W, _free_signals);
					for (size_t col = 0; col < pf.size2(); ++col){
						for (size_t i = 0; i < _cycles_num; ++i){
							auto row = rand_row(mt2);
							auto probability = 1.0 / (1.0 + exp(-Xf(row, col)/_T));
							if (rand_energe(mt1) < probability)
								_free_signals(row, col) = 1;
							else
								_free_signals(row, col) = -1;
						}
					}

					auto Xc = ublas::prod(_W, _clamped_signals);
					for (size_t col = 0; col < pf.size2(); ++col){
						for (size_t i = 0; i < _cycles_num; ++i){
							auto row = rand_row(mt2);
							//if the state is clamped, do none
							if (row < _visible_num)	continue;

							auto probability = 1.0 / (1.0 + exp(-Xc(row, col)/_T));
							if (rand_energe(mt1) < probability)
								_clamped_signals(row, col) = 1;
							else
								_clamped_signals(row, col) = -1;
						}
					}

					_T *= _c;
				}

				auto rep_num = _clamped_signals.size2();
				//update the weights
				ublas::matrix<double> Expc =
					(1.0/rep_num) * ublas::prod(ublas::trans(_clamped_signals), _clamped_signals);
				ublas::matrix<double> Expf =
					(1.0/rep_num) * ublas::prod(ublas::trans(_free_signals), _free_signals);
				auto deltaW = Expc - Expf;
				_W = _W + (_eta/_T) * deltaW;
			}
		}

		size_t cycles_num() const						{ return _cycles_num; }
		void cycles_num(size_t v)						{ _cycles_num = v; }

		double T() const								{ return _T; }
		void T(double v)								{ _T = v; }

		double c() const								{ return _c; }
		void c(double v)								{ _c = v; }

		double eta() const								{ return _eta; }
		void eta(double v)								{ _eta = v; }

		double k() const								{ return _k; }
		void k(size_t v)								{ _k = v; }

	private:
		template <typename T>
		void init(const ublas::matrix_expression<T> &patterns){
			ASSERT(patterns.size1() == _visible_num; "The size is not matched!");
			//initialize weights
			_W.resize(_visible_num+_hidden_num+1);
			for (size_t i = 0; i < _W.size1(); ++i){
				for (size_t j = 0; j < _W.size2(); ++j){
					_W(i, j) = 0;
				}
			}

			//random initialize the state
			_free_signals.resize(_visible_num + _hidden_num + 1, patterns().size2());
			_clamped_signals.resize(_visible_num + _hidden_num + 1, patterns().size2());
			std::uniform_int_distribution<int> rand(0, 1);
			std::mt19937 mt(std::time(nullptr));
			for (int col = 0; col < _free_signals.size2(); ++col){
				for (int row = 0; row < _free_signals.size1(); ++row){
					_free_signals(row, col) = rand(mt) == 0 ? -1 : 1;
					_clamped_signals(row, col) = rand(mt) == 0 ? -1 : 1;
				}
			}
			for (int col = 0; col < _clamped_signals.size2(); ++col){
				_clamped_signals(_clamped_signals.size1()-1, col) = -1;
			}

			//set the  visible state 
			for (int col = 0; col _clamped_signals.size2(); ++col){
				for (int row = 0; row < _visible_num; ++row){
					_clamped_signals(row, col) = patterns()(row, col);
				}
			}
		}

	private:
		ublas::matrix<double> _free_signals;
		ublas::matrix<double> _clamped_signals;

		ublas::symmetric_matrix<double> _W;

		size_t _visible_num;
		size_t _hidden_num;

		double _T;
		double _c;
		double _eta;
		size_t _cycles_num;
		size_t _k;
	};

}}