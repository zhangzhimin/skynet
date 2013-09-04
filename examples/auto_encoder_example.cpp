#define _CONSOLE


#include <skynet/neuralnetworks/ffnet.hpp>
//#include <skynet/neuralnetworks/auto_encoder.hpp>
#include <skynet/cv/extension/bmp.hpp>
#include <skynet/utility/file.hpp>
#include <skynet/core/ublas_extentsion.hpp>

using namespace skynet;
using namespace skynet::nn;
using namespace skynet::numeric;

int main(){
	cv::io::bmp_info_header info;

	auto files = move(load_directory("../data/mit/nonfaces/"));
	ml::database2<double, double> data;
	data.patterns.resize(400, 100);
	for (size_t i = 0;i < 100;  ){
		cv::io::bmp_info_header		info;
		auto image = cv::io::read_bmp(files[i], info);
		if (image.type() == cv::image_type::gray8){
			auto gray = cv::image_cast<cv::image_gray>(image);
			transform(gray, column(data.patterns, i), [](byte e){ return e/256.0; });
			++i;
		}else if(image.type() == cv::image_type::bgr){
			auto bgr = cv::image_cast<cv::image_bgr>(image);
			transform(bgr, column(data.patterns, i), [](cv::bgr e){ return e.b/256.0; });
			++i;
		}
	}
	data.targets = data.patterns;

	auto net = make_shared<ffnet>(400, 400);
	net->add_layer(make_shared<ffnet::sparse_layer<>>(10));
	net->add_layer(make_shared<ffnet::layer<>>(400));
	optimizer_adaptor<rprop<ffnet>> opt(net);
	net->epoch_num(500);
	net->train(data, opt); 

	//auto sp_auto_encoder = make_shared<nn::auto_encoder>(400, 200);
	//optimizer_adaptor<bfgs<auto_encoder>> opt(sp_auto_encoder);
	//sp_auto_encoder->train(data, opt);
	// auto image = cv::image_cast<cv::image_bgr>(cv::io::read_bmp("../data/mit/nonfaces/B5_00000.bmp", info));
	// cv::image_gray im_gray(image.extent());
	// transform(image, im_gray, [](cv::bgr pixel) {
	//     return byte(pixel.b);
	// });

	return 0;
}