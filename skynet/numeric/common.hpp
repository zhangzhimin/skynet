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

	template <typename Vector = vectord>
	class model{
	public:
		typedef	Vector						vector;

		virtual vector dedw() = 0;
		
		virtual vector w() = 0;

		virtual void update(const vector &delta_w) = 0;
	};

	
	///\brief The interface of the optimizer.
	class optimizer{
	public:
		virtual void step() = 0;
	};

	///\brief	The adaptor of the optimizer
	template <typename Opt>
	class optimizer_adaptor: public optimizer, public Opt{
	public:
		typedef Opt							optimizer;
		typedef typename optimizer::model	model;
		
		///\brief Constructs by the model
		optimizer_adaptor(shared_ptr<model> sp_model) : Opt(sp_model){}


		virtual void step(){
			Opt::step();
		}
	};

}}
