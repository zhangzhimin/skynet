#define _CONSOLE

#include <skynet/neuralnetworks/ffnet.hpp>
#include <skynet/utility/load_data.hpp>

using namespace skynet;
using namespace skynet::nn;
using namespace skynet::numeric;

int main()
{
	std::srand(uint(std::time(nullptr)));

	auto iris_data = get_iris("../data/Iris.data");
	ml::database2<double, double> data;
	data.patterns = ublas::matrix<double>(4, 150);
	data.targets = ublas::zero_matrix<double>(3, 150);
	for (int i = 0; i < 50; ++i){
		column(data.patterns, i) = column(iris_data[0], i);
		column(data.targets, i)[0] = 0.8;
		column(data.targets, i)[1] = 0.2;
		column(data.targets, i)[2] = 0.2;
		column(data.patterns, i+50) = column(iris_data[1], i);
		column(data.targets, i+50)[0] = 0.2;
		column(data.targets, i+50)[1] = 0.8;
		column(data.targets, i+50)[2] = 0.2;
		column(data.patterns, i+100) = column(iris_data[2], i);
		column(data.targets, i+100)[0] = 0.2;
		column(data.targets, i+100)[1] = 0.2;
		column(data.targets, i+100)[2] = 0.8;
	}
	
	auto net = make_shared<ffnet>(4, 3);
	net->add_layer(make_shared<ffnet::layer<sigmoid_function<>>>(6));
	net->add_layer(make_shared<ffnet::layer<sigmoid_function<>>>(3));
	/Users/zhangzhimin/git/skynet/examples/ffnet_example.cpp
	//optimizer_adaptor<lbfgs<ffnet::model>> opt;
	optimizer_adaptor<rprop<ffnet::model>> opt;

	opt.iteration_num(500);

	net->train(data, opt); 

	for (size_t i = 0; i < data.patterns.size2(); ++i){
		std::cout << i <<": " << (*net)(column(data.patterns, i)) << std::endl;
	}


	return 0;
}

