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

#include <skynet\ublas.hpp>
#include <skynet\core\array.hpp>

namespace boost{namespace numeric{namespace ublas{


	template <typename M>
	class array2vector : public ublas::vector_container<array2vector<M>>{
	public:
		typedef array2vector<M>											self_type;
		typedef std::ptrdiff_t											difference_type;
		typedef	typename M::value_type									value_type;
		typedef value_type *											pointer;
		typedef typename M::reference									reference;
		typedef typename M::const_reference								const_reference;
		typedef typename M::iterator									iterator;
		typedef typename M::const_iterator								const_iterator;
		typedef typename size_t											size_type;

		typedef const vector_reference<const self_type>					const_closure_type;
		typedef vector_reference<self_type>								closure_type;

		typedef M														array_type;

		//typedef inverse_iterator
		//typedef vector_temporary_type
		//typedef M array_type
		//typedef dense_type storage_category
		array2vector(){}

		array2vector(const M &data) : _data(data){}

		array2vector(const self_type &rhs): _data(rhs._data){}

		array2vector& operator=(const self_type &rhs){
			_data = rhs._data;
			return *this;
		}


		//iterator
		iterator begin()								{ return _data.begin(); }
		const_iterator begin()	const					{ return _data.begin(); }
		iterator end()									{ return _data.end(); }
		iterator end() const							{ return _data.end(); }

		//index 
		reference operator[](size_t i)					{ return _data[i]; }
		const_reference operator[](size_t i) const		{ return _data[i]; }
		reference operator()(size_t i)					{ return _data[i]; }
		const_reference operator()(size_t i) const      { return _data[i]; }

		size_type	size() const						{ return _data.size(); }
		bool empty() const								{ return size()==0; }					



		array_type	data()								{ return _data; }
		const array_type data() const					{ return _data; }


	private:
		M				_data;
	};


	template <typename M>
	auto make_feature(M &mat)->
		typename conditional<is_const<M>::value, const array2vector<M>, array2vector<M>>::type
	{
		return array2vector<M>(mat);
	}
	

	//template <typename M>
	//class matrix_vector : public vector_container<matrix_vector<M>>{
	//public:


	//private:
	//	M		

	//};

}}}