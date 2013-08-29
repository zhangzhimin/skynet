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


#include <skynet/config.hpp>

#include <skynet/core/function.hpp>
#include <skynet/ml/utility.hpp>

namespace skynet{namespace ml{


	class negative_log_likehood{
	public:
		static_assert(is_floating_point<T>::value, "the T should be float or double...");
		
		template <typename E1, typename E2>
		typename E1::value_type operator()(const ublas::vector_expression<E> &prediction_e, const ublas::vector_expression<E2> &target_e) const{
			static_assert(is_integral<typename E2::value_type>::value, "");
			ASSERT(prediction_e().size() == target_e().size(), "The size is not matched.");

			typedef typename E1::value_type	value_type;
			value_type error = 0;
			for (size_t i  = 0; i < prediction_e().size(); ++i){
				auto prediction = prediction_e()[i];
				if (target_e()[i]){
					if (prediction > 1){
						//do none
					}else if (prediction < 0.01){
						error += 2;
					}else{
						//error += -std::log(prediction)        
						error -= log(prediction);
					}
				}else{
					if (prediction < -1){
						//don none
					}else if(prediction > -0.01){
						error += 2;
					}else{
						error -= log(-prediction);
					}
				}
			}

			return error;
		}


		template <typename T1, typename T2>
		T1 operator()(const ublas::matrix<T1> &prediction, const ublas::matrix<T2> &target) const{
			static_assert(is_integral<T2>::value, "T2 should be integral type.");
			ASSERT(prediction.size2() == target.size2() && prediction.size1() == target.size1(), "The size is not matched.");

			T1 error = 0;
			for (size_t i = 0; i < prediction.size2(2); ++i){
				error += (*this)(column(prediction, i), column(target, i));
			}

			return error;
		}

	
	}


}}
