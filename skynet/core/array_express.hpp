/******************************************************************************
Created By : Zhang Zhimin
Created On : 2012/9/11
Purpose    : 
********************************************************************************/
#pragma once

#include <skynet\config.hpp>

namespace skynet{

	//this class help std::array implement std::array operation.
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