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

#include <skynet/ublas.hpp>

namespace skynet{namespace numeric{


	///\brief	The interface of the model which has derivative function.
	//template <typename Vector = vectord>
	//class model{
	//public:
	//	typedef	Vector							vector;
	//	typedef typename vector::value_type		value_type;

	//	///\brief	Gets the derivative of the weights on the Error.
	//	virtual vector derivative(const vector &) = 0;

	//	virtual value_type operator()(const vector &) = 0;
	//};


	///\brief	Adapte the model to function with derivative.
	//template <typename Model>
	//class model_function{
	//public:
	//	typedef Model		model;
	//	typedef typename model::vector			vector;
	//	typedef typename model::value_type		value_type;

	//	model_function(shared_ptr<model> sp_model): _sp_model(sp_model){}

	//	///\brief	Gets the function result.
	//	value_type operator()(const vector &input){
	//		return _sp_model->error(input);
	//	}

	//	///\brief	Gets the function derivative, the function should be invoked after operator().
	//	///			Because the input are unused.
	//	vector	derivative(const vector &input){
	//		return _sp_model->dedw();
	//	}

	//private:
	//	shared_ptr<model>		_sp_model;
	//};

	///\brief The interface of the optimizer_base.
	template <typename Model>
	class optimizer_base{
	public:
		typedef Model								model;
		typedef typename model::vector				vector;
		///\brief	Iterates once.
		virtual vector optimize(model &m, const vector &) = 0;
	};

	///\brief	The adaptor of the optimizer_base
	template <typename Opt>
	class optimizer_adaptor: public optimizer_base<typename Opt::model>, public Opt{
	public:
		typedef typename Opt::model					model;
		typedef typename model::vector				vector;
		
	//	optimizer_adaptor(): Opt(){}
		///\brief	Initializes
		virtual vector optimize(model &m, const vector &start_point){
			return Opt::optimize(m, start_point);
		}

	};

}}
