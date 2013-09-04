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

#include <skynet/neuralnetworks/ffnet.hpp>

namespace skynet{namespace nn{


	///\brief	Implement the auto encoder, a special ffnet.
	class auto_encoder : public: ffnet{
	public:
		auto_encoder(size_t input_size, size_t hidden_size): ffnet(input_size, input_size){
			ffnet::add_layer(make_shared<ffnet::sparse_layer<>>(hidden_size));
		}

		vectord status(const vectord &input){
			(*this)(input);
			return hidden_layer()->out();
		}

		shared_ptr<ffnet::sparse_layer_base> hidden_layer(){
			return this->layers().front();
		};
	};


	class stacked_auto_encode{


	};


}}
