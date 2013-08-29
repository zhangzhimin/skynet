

#include <skynet/neuralnetworks/ffnet.hpp>
#include <skynet/utility/load_data.hpp>

using namespace skynet;
using namespace skynet::nn;

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
		column(data.targets, i)[1] = 0;
		column(data.targets, i)[2] = 0;
		column(data.patterns, i+50) = column(iris_data[1], i);
		column(data.targets, i+50)[0] = 0;
		column(data.targets, i+50)[1] = 1;
		column(data.targets, i+50)[2] = 0;
		column(data.patterns, i+100) = column(iris_data[2], i);
		column(data.targets, i+100)[0] = 0;
		column(data.targets, i+100)[1] = 0;
		column(data.targets, i+100)[2] = 1;
	}
	/*ml::database2<double, int> data;
	data.patterns = ublas::matrix<double>(2, 4);
	data.targets = ublas::zero_matrix<int>(2, 4);
	data.patterns(0, 0) = -1;
	data.patterns(1, 0) = -1;
	data.patterns(0, 1) = -1;
	data.patterns(1, 1) = 1;
	data.patterns(0, 2) = 1;
	data.patterns(1, 2) = 1;
	data.patterns(0, 3) = 1;
	data.patterns(1, 3) = -1;
	data.targets(0, 0) = 1;
	data.targets(1, 0) = 0;
	data.targets(0, 1) = 0;
	data.targets(1, 1) = 1;
	data.targets(0, 2) = 1;
	data.targets(1, 2) = 0;
	data.targets(0, 3) = 0;
	data.targets(1, 3) = 1;*/


	ffnet<> net(4, 3);
	//shared_ptr<ffnet::layer_base> hidden_layer(new ffnet::layer<>(10));
	//shared_ptr<ffnet::layer_base> hidden_layer2(new ffnet::layer<>(20));
	//auto layer2 = new ffnet::layer<>(2);
	//shared_ptr<ffnet::layer_base> out_layer(layer2);
	net.add_layer(make_shared<ffnet<>::layer<>>(5));
	net.add_layer(make_shared<ffnet<>::layer<>>(3));
	net.epoch_num(500);
	net.train(data); 

	for (size_t i = 0; i < data.patterns.size2(); ++i){
		std::cout << i <<": " << net(column(data.patterns, i)) << std::endl;
	}


	return 0;
}

