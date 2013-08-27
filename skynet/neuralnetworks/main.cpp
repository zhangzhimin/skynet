#include <skynet\neuralnetworks\memory.hpp>

using namespace skynet;

int main(){
	ublas::matrix<int> samples(3,2);
	samples(0,0) = 1, samples(1,0) = 1, samples(2,0) = -1;
	samples(0,1) = -1, samples(1,1) = -1, samples(2,1) = 1;

	nn::cam<int> cam(samples);

	ublas::vector<int> init_code(3);
	init_code[0] = -1; init_code[1] = 1; init_code[2] = -1;
	auto code = cam.decode(init_code);

	for (size_t i = 0 ; i < samples.size1(); ++i){
		std::cout << code[i] << std::endl;
	}
	
}