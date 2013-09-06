#define _CONSOLE

#include <functional>

#include <skynet/neuralnetworks/ffnet.hpp>
//#include <skynet/neuralnetworks/auto_encoder.hpp>
#include <skynet/cv/extension/bmp.hpp>
#include <skynet/core/crop.hpp>
#include <skynet/core/io.hpp>
#include <skynet/utility/algorithm.hpp>

using namespace skynet;
using namespace skynet::nn;
using namespace skynet::numeric;

int main(){
	std::srand(std::time(nullptr));

	cv::io::bmp_info_header info;
	auto image = cv::image_cast<cv::image_gray>(cv::io::read_bmp("../data/ocean_fish.bmp", info));
	auto images = grid_crop(image, extent2(5,5));

	ml::database2<double, double> data;
	data.patterns.resize(25, 7500);
	for (size_t i = 0; i < 7500; ++i){
        auto image = images[i];
        auto col_data = column(data.patterns, i);
        skynet::transform(image, col_data, [](byte e)->double{ return -1.0+2.0*e/256.0;});
	}
	data.targets = data.patterns;

	auto net = make_shared<ffnet>(25, 25);
    //net->add_layer(make_shared<ffnet::layer<sigmoid_function<>>>(128));
	net->add_layer(make_shared<ffnet::layer<>>(10));
    //net->add_layer(make_shared<ffnet::layer<sigmoid_function<>>>(128));
	net->add_layer(make_shared<ffnet::layer<>>(25));
	optimizer_adaptor<rprop<ffnet::model>> opt;
	optimizer_adaptor<lbfgs<ffnet::model>> opt1;

	opt.iteration_num(10000);
	opt1.iteration_num(2000);
	net->train(data, opt1);
   // net->train(opt1);

	auto weights = net->layers().front()->w();
	write2raw(weights, "../data/weights.raw");
	//net->train(opt1); 
	return 0;
}