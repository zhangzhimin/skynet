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

#include <skynet/config.hpp>

namespace skynet{namespace ml{


	///\brief	The the layer of deep learning net.
	class deep_layer{
	public:
		///\brief	Gets the encode status by the model.
		virtual vectord	status(const vectord &input) = 0;

		///\brief	
	};
	

	///\brief	The adaptor of the deep layer
	template <typename Layer>
	class deep_layer_adaptor : private deep_layer{
	public:

		///\brief	Gets the encode status by the model.
		virtual vectord status(const vectord &input){
			return Layer::status(input);
		}
	};


	///\brief	The networks of deep learning
	class deep_learning_net{
	public:

	private:

	};

}}