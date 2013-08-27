#include <skynet\numeric\line_search.hpp>

using namespace skynet;
using namespace skynet::numeric;

int main(){
	//ublas::matrix<double> A(2,4);
	//A(0,0) = 1; A(0,1) = 0; A(0,2) = 1; A(0,3) = -1;
	//A(1,0) = 0; A(1,1) = 1; A(1,2) = 1; A(1,3) = 1;
	//ublas::vector<double> y(2);
	//y(0) = 2;
	//y(1) = 3;
	//
	//linear_equations equations;
	//equations.A = A;
	//equations.y = y;

	//min_norm1(equations, 0.5, 0.5, 1000);
	auto f = [](vectord x)->double{
		return norm_2(x); 
	};
	vectord a(2);
	vectord b(2);
	vectord c(2);
	a[0] = -2;		a[1] = -2;
	b[0] = -1;	b[1] =-1;
	c[0] = 1;		c[1] = 1;


	auto best = gold_section_search(f, a, b, c, 1e-6);
	auto re = f(best);
	
	return 0;
}