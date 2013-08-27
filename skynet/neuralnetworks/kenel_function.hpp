#pragma once

#include <skynet\config.hpp>
#include <skynet\ublas.hpp>

namespace skynet{namespace nn{


	class poly_kenel_function{
	public:
		poly_kenel_function() : _degree(2){}

		int degree() const								{ return _degree; }
		void degree(int v)								{ _degree = v; }

		double operator()(const ublas::vector<double> &X, const ublas::vector<double> &Y) const{
			auto re = 1 + ublas::inner_prod(X, Y);	
			return pow(re, _degree);
		}
		
	private:
		int				_degree;
	};

}}