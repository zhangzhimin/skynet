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

#include <skynet/core/conv.hpp>

#include <algorithm>
#include <cmath>
#include <functional>

namespace skynet{ namespace cv{
    using std::function;

    template <typename M>
    auto gaussian_filter(const M &mat_src, const int &radius=1)
        ->lazy_array<M::dim, function<typename M::value_type(const size_t &)>>
    {
        gaussian_mask<M::dim> mask(radius);
		mask.attach(mat_src.extent());
        auto temp = conv(mat_src, mask);
        
        typedef typename M::value_type value_type;
         function<value_type (const size_t &)> fun = [=](const size_t &i){
			 auto scale = 1.0 / mask.weight_sum();
             return scale * temp[i];
         };

         return make_lazy_array(mat_src.extent(), fun);
    }
}}
