/*=============================================================================

The MIT License (MIT)
Copyright @ 2013 Zhang Zhimin5 

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

============================================================================*/

#pragma once

#include <skynet/config.hpp>

namespace skynet{

	template <typename array_type_>
	class array_express{
	public:
		typedef array_express							type;
		typedef array_type_								array_type;
			
		const array_type &operator()() const{
			return *static_cast<const array_type *>(this);
		}

		array_type &operator()(){
			return *static_cast<array_type *>(this);
		}

	protected:
		array_express(){}
		array_express(const array_express &){}
	};
}
