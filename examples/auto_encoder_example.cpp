#define _CONSOLE


#include <skynet/neuralnetworks/ffnet.hpp>
//#include <skynet/neuralnetworks/auto_encoder.hpp>
#include <skynet/cv/extension/bmp.hpp>
#include <skynet/utility/file.hpp>
#include <skynet/core/crop.hpp>
#include <skynet/core/io.hpp>

using namespace skynet;
using namespace skynet::nn;
using namespace skynet::numeric;

int main(){
	std::srand(std::time(nullptr));

	cv::io::bmp_info_header info;
	auto image = cv::image_cast<cv::image_gray>(cv::io::read_bmp("../data/ocean.bmp", info));
	auto images = grid_crop(image, extent2(8,8));

	ml::database2<double, double> data;
	data.patterns.resize(64, 10000);
	for (size_t i = 0; i < 10000; ++i){
		transform(images[i], column(data.patterns, i), [](byte e) { return 0.1+0.8*e/256.0;});
	}
	data.targets = data.patterns;

	auto net = make_shared<ffnet>(64, 64);
	net->add_layer(make_shared<ffnet::sparse_layer<>>(25));
	net->add_layer(make_shared<ffnet::layer<sigmoid_function<>>>(64));
	optimizer_adaptor<rprop<ffnet::model>> opt;
	optimizer_adaptor<lbfgs<ffnet::model>> opt1;

	opt.iteration_num(10000);
	opt1.iteration_num(2000);
	net->train(data, opt1); 

	auto weights = net->layers().front()->w();
	write2raw(weights, "../data/weights.raw");
	//net->train(opt1); 
	return 0;
}