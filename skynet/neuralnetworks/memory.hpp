#pragma once

#include <skynet/ublas.hpp>
#include <skynet/utility/algorithm.hpp>

#include <boost/numeric/ublas/symmetric.hpp>
#include <boost/numeric/ublas/banded.hpp>

#include <random>
#include <type_traits>

namespace skynet{namespace nn{

	template <typename T>
	class cam{
	public:
		static_assert(std::is_integral<T>::value, "The T should be integral.");

		typedef ublas::matrix<T>					matrix;
		typedef ublas::symmetric_matrix<T>			symmetric_matrix;
		typedef ublas::diagonal_matrix<T>			diagonal_matrix;
		typedef ublas::vector<T>					vector;		

		cam(matrix input){
			auto size = input.size1();
			_W.resize(size);
			_W = ublas::prod(input, ublas::trans(input));
			diagonal_matrix I(size);
			for (size_t i = 0; i < size; ++i){
				I(i,i) = input.size2();
			}

			_W -= I;		

		}

		symmetric_matrix weights() const { return _W; }

		template <typename VE>
		vector decode(const ublas::vector_expression<VE> &code){
			vector state(code().size());
			copy(code(), state);
			std::uniform_int_distribution<size_t> rand_index(0, _W.size2()-1);
			std::mt19937						  mt_index;
			mt_index.seed(unsigned long(std::time(nullptr)));
			while(true){
				size_t variation_count = 0;
				for (size_t j = 0; j < (_W.size2()*5); ++j){
					auto index = rand_index(mt_index);
					auto weights = ublas::row(_W, index);
					auto re = ublas::inner_prod(weights, state);
					if (re == 0)
						continue;
					else if (re > 0 && state[index] == -1){
						state[index] = 1;
						variation_count++;
					}
					else if(re < 0 && state[index] == 1){
						state[index] = -1;
						variation_count++;
					}
				}

				if (variation_count == 0)
					return state;					
			}
		}


	private:
		symmetric_matrix		_W;
	};


	template <typename T>
	class bam{
	public:
		static_assert(std::is_integral<T>::value, "The T should be integral.");
		
		typedef ublas::matrix<T>					matrix;
		typedef ublas::vector<T>					vector;

		bam(matrix input, matrix output) 
			: _W(input.size1(), output.size1()), _X(input.size1()), _Y(output.size1())
		{
			_W = ublas::prod(input, ublas::trans(output));
		}

		template <typename E>
		vector operator()(const ublas::vector_expression<E> &ve) const{
#ifdef	ENABLE_ASSERT
			for (auto e : ve()){
				ASSERT(e == -1 || e == 1, "The input should be bipolar.");
			}
#endif			
			vector pre_X = ve();
			vector pre_Y = ublas::inner_prod(pre_X, _W);
			bipolarize(pre_Y);

			while(true){
				vector X = ublas::inner_prod(_W, pre_Y);
				bipolarize(X);
				vector Y = ublas::inner_prod(X, _W);
				bipolarize(Y);

				//TODO: the check is expensive.
				if (ublas::norm_1(X-pre_X) == 0 && ublas::norm1(Y-pre_Y) == 0)
					return X;
			}
		}

	private:
		matrix			_W;
	};

}}
