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
	ml::database2<double, int> data;
	data.patterns = ublas::matrix<double>(4, 150);
	data.targets = ublas::zero_matrix<int>(3, 150);
	for (int i = 0; i < 50; ++i){
		column(data.patterns, i) = column(iris_data[0], i);
		column(data.targets, i)[0] = 1;
		column(data.targets, i)[1] = 0.2;
		column(data.targets, i)[2] = 0;
		column(data.patterns, i+50) = column(iris_data[1], i);
		column(data.targets, i+50)[0] = 0.2;
		column(data.targets, i+50)[1] = 1;
		column(data.targets, i+50)[2] = 0.2;
		column(data.patterns, i+100) = column(iris_data[2], i);
		column(data.targets, i+100)[0] = 0.2;
		column(data.targets, i+100)[1] = 0.2;
		column(data.targets, i+100)[2] = 1;
	}
	
	auto net = make_shared<ffnet>(4, 3);
	net->add_layer(make_shared<ffnet::layer</*sigmoid_function<>*/>>(5));
	net->add_layer(make_shared<ffnet::layer</*sigmoid_function<>*/>>(3));

	optimizer_adaptor<bfgs<ffnet>> opt(net);

	net->epoch_num(500);
	net->train(data, opt); 

	for (size_t i = 0; i < data.patterns.size2(); ++i){
		std::cout << i <<": " << (*net)(column(data.patterns, i)) << std::endl;
	}


	return 0;
}

