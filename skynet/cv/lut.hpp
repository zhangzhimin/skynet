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

#include <skynet\config.hpp>
#include <skynet\utility\exception.hpp>
#include <skynet\utility\tag.hpp>
#include <skynet\utility\serialization.hpp>

namespace skynet{ namespace cv{
	namespace detail{

		template <typename value_type_, typename value_type2_>
		void init_lut(const typename std::pair<value_type_, value_type2_> &node1, 
			const typename std::pair<value_type_, value_type2_> &node2, typename std::vector<value_type2_>::iterator lut_it)
		{
			if (node1.first == node2.first)	{ return; }

			auto len = node2.first - node1.first;
			double step = (double)(node2.second - node1.second) / len;
			//lut[0] = node1.second;
			for (int i = 0; i < len; ++i){
				*lut_it = static_cast<value_type2_>(node1.second + step * i);
				++lut_it;
			}
		}
	}

	//template <typename input_value_type_, typename output_value_type_ = input_value_type_, 
	//		typename lut_tag_ = piecewise_line_tag>
	//class lut;

	///warning : use the lut map,  the range should be not ton large;
	///it's just use to some small range map.
	template <typename input_value_type_, typename output_value_type_>
	class polygonal_lut: public std::unary_function<input_value_type_, output_value_type_>{
	public:
		typedef piecewise_line_tag			                    lut_category;
		typedef std::pair<argument_type, result_type>           point_type;

		static_assert(std::is_integral<argument_type>::value, "the argument_type is not integral.");

		polygonal_lut( ) : m_inited(false) { }

		result_type operator()(const argument_type &x) const{
			ASSERT_PLUS(m_inited, "Not Initialized!");
			ASSERT_PLUS( x >= m_start_index && x < m_end_index, "Out range");

			return m_lut[x-m_start_index];
		}

		void add_point(const point_type &point){
			if (m_points.size() != 0){
				if (point.first <= m_points.back().first){
					THROW_EXCEPTION(std::invalid_argument("The point must be not less than the latest point!"));
				}
			}

			m_points.push_back(point);
		}

		void initialize(){
			m_start_index = m_points.front().first;
			m_end_index = m_points.back().first;

			auto len = m_end_index - m_start_index;
			m_lut.resize(len);

			auto polygonal_lut = m_lut.begin();
			auto it1 = m_points.begin();
			auto it2 = it1; ++it2;
			while (it2 != m_points.end()){
				detail::init_lut(*it1, *it2, polygonal_lut);

				polygonal_lut += it2->first - it1->first;
				++it1;
				++it2;
			}

			m_lut[len - 1] = m_points.back().second;
			m_inited = true;
		}

		argument_type lower() const{
			return m_start_index;
		}

		argument_type upper() const{
			return m_end_index;
		}

	private:
		std::vector<result_type>     m_lut;		
		std::list<point_type>	m_points;

		argument_type			m_start_index;
		argument_type			m_end_index;

		bool m_inited;
	};


	template <typename input_value_type_, typename output_value_type_>
	class lut: public std::unary_function<input_value_type_, output_value_type_>{
	public:
		typedef input_value_type_			argument_type;
		typedef output_value_type_			result_type;
		typedef	palette_tag					lut_category;

		static_assert(std::is_integral<argument_type>::value, "the argument_type is not integral");

		//serialization for lut
		template <typename archive_type>
		void serialize(archive_type &ar, const unsigned int &){
			ar & boost::serialization::make_nvp("lut", m_lut);
		}
		
		lut() : m_lut(256){}

		lut(const std::array<result_type , 256> &a) : m_lut(a.begin(), a.end())	{}

		lut(const lut &lut): m_lut(lut.m_lut){}

		result_type operator()(const argument_type &i){
			return m_lut[i];
		}

	private:
		std::vector<result_type> m_lut;
	};

}}