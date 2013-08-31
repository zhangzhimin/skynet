/******************************************************************************
Created By : Zhang Zhimin
Created On : 2012/9/18
Purpose    : type_traits for skynet lib
********************************************************************************/

#pragma once

#include <skynet/config.hpp>
#include <type_traits>
#include <boost/mpl/if.hpp>

namespace skynet{


	/*struct is_binary_function{
	typedef 
	}*/

	/// the binary_function_concept,  it has two const & arguments  and operator();
	template <typename T>
	struct binary_function_traits : 
		public binary_function_traits<decltype(&T::operator())>{ 
	};

	template <typename R, typename O, typename P1, typename P2>
	struct binary_function_traits<R (O::*)(const P1 &, const P2&) const>{
		typedef P1										argument1_type;
		typedef P2										argument2_type;
		typedef R										result_type;
	};

	template <typename R, typename P1, typename P2>
	struct binary_function_traits<R (*)(const P1 &, const P2&)>{
		typedef P1								        argument1_type;
		typedef P2								        argument2_type;
		typedef R								        result_type;
	};

	//others  it's not an binary_function.

	template <typename T>
	struct unary_function_traits:
		public unary_function_traits<decltype(&T::operator())>{
	};

	template <typename R, typename O, typename P>
	struct unary_function_traits<R (O::*)(const P&) const>{
		typedef R				result_type;
		typedef P				argument_type;
	};

 	template <typename R, typename O, typename P>
	struct unary_function_traits<R (O::*)(const P&)>{
		typedef R				result_type;
		typedef P				argument_type;
	};

	template <typename R, typename P>
	struct unary_function_traits<R (*)(const P &)>{
		typedef R				result_type;
		typedef P				argument_type;
	};

	template <typename R, typename O, typename P>
	struct unary_function_traits<R (O::*)(P) const>{
		typedef R				result_type;
		typedef P				argument_type;
	};
}
