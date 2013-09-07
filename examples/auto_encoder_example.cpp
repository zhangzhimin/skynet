#define _CONSOLE

#include <functional>

#include <boost/lexical_cast.hpp>

#include <skynet/neuralnetworks/ffnet.hpp>
//#include <skynet/neuralnetworks/auto_encoder.hpp>
#include <skynet/cv/extension/bmp.hpp>
#include <skynet/core/crop.hpp>
#include <skynet/core/io.hpp>
#include <skynet/utility/algorithm.hpp>
#include <skynet/utility/load_data.hpp>
#include <skynet/core/interpolate.hpp>

using namespace skynet;
using namespace skynet::nn;
using namespace skynet::numeric;

int main(){
	std::srand(std::time(nullptr));

	cv::io::bmp_info_header info;
	auto image = cv::image_cast<cv::image_gray>(cv::io::read_bmp("../data/ocean_fish.bmp", info));
	//auto images = grid_crop(image, extent2(8,8));
    //auto images = utility::get_digit_images("../data/mnist/t10k-images-idx3-ubyte");
    auto train_images = utility::get_digit_images("../data/mnist/train-images-idx3-ubyte");
    auto train_labels = utility::get_digit_labels("../data/mnist/train-labels-idx1-ubyte");
    
    std::vector<array2b> digit6_images;    
    for (size_t i = 0; i < train_images.size(); ++i){
        if (train_labels[i] == 6 || train_labels[i] == 9){
            digit6_images.push_back(train_images[i]);
           // cv::io::write_bmp(digit6_images.back(), "../data/mnist/train/digit6/"+boost::lexical_cast<string>(digit6_images.size())+".bmp");
        }
    }
   
	ml::database2<double, double> data;
   // size_t samples_size = digit6_images.size();
    size_t samples_size = 100;
	data.patterns.resize(digit6_images.front().size(), samples_size);
	for (size_t i = 0; i < samples_size; ++i){
        auto image = digit6_images[i];
        auto col_data = column(data.patterns, i);
        skynet::transform(image, col_data, [](byte e)->double{ return 0.25+0.5*e/256.0;});
	}
	data.targets = data.patterns;
	auto net = make_shared<ffnet>(data.patterns.size1(), data.patterns.size1());
    //net->add_layer(make_shared<ffnet::layer<sigmoid_function<>>>(128));
	net->add_layer(make_shared<ffnet::sparse_layer<sigmoid_function<>>>(10));
    //net->add_layer(make_shared<ffnet::layer<sigmoid_function<>>>(128));
	net->add_layer(make_shared<ffnet::layer<sigmoid_function<>>>(data.patterns.size1()));
	optimizer_adaptor<rprop<ffnet::model>> opt;
	optimizer_adaptor<lbfgs<ffnet::model>> opt1;

	opt.iteration_num(100);
	opt1.iteration_num(200);
	net->train(data, opt1);
    
    for (size_t i = 0; i < samples_size; ++i){
        auto re = (*net)(column(data.patterns, i));
        array2b re_image(28,28);
        transform(re, re_image,[](double e){ return byte((e-0.25)/0.5 * 256);});
        cv::io::write_bmp(re_image, "../data/mnist/train/digit6_encode/"+boost::lexical_cast<string>(i)+".bmp");
    }


	auto weights = net->layers().front()->w();
	write2raw(weights, "../data/weights.raw");
	return 0;
}