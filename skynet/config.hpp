#pragma once

///include the stl
#include <stdlib.h>

#include <list>
#include <vector>
#include <map>
#include <deque>
#include <exception>
#include <fstream>
#include <iostream>
#include <functional>
#include <memory>
#include <numeric>
#include <array>
#include <tuple>
#include <type_traits>
#include <cmath>
#include <queue>
#include <tuple>


//The skynet lib depended on the boost lib.just the serialization static_lib is needed.
#include <boost/config.hpp>


#include <boost/throw_exception.hpp>
#include <boost/exception/diagnostic_information.hpp>

#include <boost/shared_array.hpp>
//#include <boost\assign.hpp>

//using namespace boost::assign;


#define skynet_ALGORITH_LIB

#ifdef _DEBUG

#define ENABLE_ASSERT

#ifndef DEBUG
#define DEBUG
#endif

#ifndef DEBUG_ON
#define DEBUG_ON
#endif

#endif


//the out couldn't be define. it's an error.
//#ifdef out
//#undef out
//#define out
//#else
//#define out
//#endif

//#ifdef max
//#undef max  //don't define max,please use inline function;
//#endif
//
//#ifdef min
//#undef min
//#endif

typedef unsigned char  byte;
typedef unsigned int   uint;

using std::size_t;
using std::ptrdiff_t;
using std::string;

//using std::vector;
using std::list;
using std::map;
//using std::array;
using std::pair;
using std::priority_queue;

using std::tuple;

using std::shared_ptr;
using std::weak_ptr;
using std::make_shared;

using std::exp;
using std::abs;
using std::make_pair;
using std::get;
using std::sqrt;
using std::move;
using std::swap;
using std::max;
using std::min;
using std::pow;
using std::log;

using std::plus;
using std::multiplies;
using std::minus;
using std::numeric_limits;

using std::binary_function;
using std::unary_function;
using std::function;

using std::conditional;
using std::enable_if;
using std::remove_const;
using std::remove_reference;
using std::is_const;
using std::is_same;
using std::is_base_of;
using std::is_integral;
using std::is_floating_point;

#ifndef no_throw
#define no_throw throw()
#endif


#ifndef    BOOST_THROW_EXCEPTION
#error
#endif

#ifdef THROW_EXCEPTION
#undef THROW_EXCEPTION
#define THROW_EXCEPTION BOOST_THROW_EXCEPTION
#else 
#define THROW_EXCEPTION BOOST_THROW_EXCEPTION
#endif

namespace skynet{
	class assert_failed : public std::exception{
	public:
		assert_failed(string info) : std::exception(), _info(info) {
#ifdef _CONSOLE
			std::cout << "Assert failed: " << info << std::endl;
#endif // _CONSOLE
		}

	private:
		string _info;
	};
}

#define IS_SAME_DIMENTION(P1, P2)		static_assert(P1::dim == P2::dim, "the dimension is not the same")

#ifdef ASSERT
#pragma error	//the ASSERT define error
#endif

#ifndef ASSERT
#ifndef	DISABLE_ASSERT
#define ASSERT(exp, mes)	if (!(exp))	{ THROW_EXCEPTION(skynet::assert_failed(mes)); }
#else
#define ASSERT(exp, mes)    //none
#endif
#else
#pragma error("the ASSERT is defined")
#endif

#ifdef CHECK_INDEX
#pragma error("The CHECK_INDEX is defined")
#endif

#define ASSERT_SAME_EXTENT(lhs, rhs)	ASSERT(lhs.extent() == rhs.extent(), "the extents is not match")



#ifdef ENABLE_ASSERT_PLUS
#define ASSERT_PLUS(exp, mes)		ASSERT(exp, mes);
#else
#define ASSERT_PLUS(exp, mes)		//none
#endif


#ifndef VERIFY
//#error  The define VERIFY has defined by other lib, fix the error.
#ifdef DEBUG
#define VERIFY(exp, mes)    ASSERT(exp, mes)
#else
#define VERIFY(exp, mes)    exp
#endif
#endif

