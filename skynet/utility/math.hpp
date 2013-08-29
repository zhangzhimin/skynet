/******************************************************************************
Created By : Zhang Zhimin
Created On : 2012/12/12
Purpose    :  
********************************************************************************/

#pragma once

#include <skynet/config.hpp>

#include <numeric>
#include <cmath>

#ifdef PI
#undef PI
#endif

namespace skynet{
	const static double PI =		3.14159265358979323846;
	const static double E =			2.71828182845904523536;
	const static double GOLD =		1.618033988749895;
	const static double RES_GOLD =	0.381966011250105;
	// sqr (square)
	inline bool          sqr(bool x)          { return x; }
	inline int           sqr(int x)           { return x*x; }
	inline unsigned int  sqr(unsigned int x)  { return x*x; }
	inline long          sqr(long x)          { return x*x; }
	inline unsigned long sqr(unsigned long x) { return x*x; }
	inline float         sqr(float x)         { return x*x; }
	inline double        sqr(double x)        { return x*x; }

	inline int newton_num(const int &m, const int &n){
		ASSERT( m <= n && m >= 0, "the parameters is invalid.");

		auto cumprod = [](const int &num)->int{
			auto temp = 1;
			for (int i = 2; i <= num; ++i){
				temp *= i;
			}

			return temp;
		};

		return cumprod(n) / (cumprod(n - m) * cumprod(m));
	}

	inline bool is_equal(const float &lhs, const float &rhs, const float &esp){
		return std::abs(lhs-rhs) < esp;
	}

	inline bool is_equal(const double &lhs, const double &rhs, const double &esp){
		return std::abs(lhs-rhs) < esp;
	}

	inline double ln(double x){
		return std::log(x) / std::log(E);
	}

	template <typename T>
	int sign(T v){
		if (v > 0)
			return 1;
		else if(v < 0)
			return -1;
		else
			return 0;
	}

	template <typename T>
	T sech(T v){
		return 1.0 / std::cosh(v);
	}

	template <typename T>
	T csch(T v){
		return 1.0 / std::sinh(v);
	}

	/*inline double sqrt(const int &x){
	return std::sqrt(double(x));
	}*/
}
