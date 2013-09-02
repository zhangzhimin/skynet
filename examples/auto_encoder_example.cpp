
#include <skynet/neuralnetworks/ffnet.hpp>
#include <skynet/neuralnetworks/auto_encoder.hpp>
#include <skynet/cv/extension/bmp.hpp>
#include <skynet/utility/file.hpp>

#include <skynet/core/ublas_extentsion.hpp>

using namespace skynet;
using namespace skynet::nn;
using namespace skynet::numeric;

int main(){
    cv::io::bmp_info_header info;
    
    auto files = load_directory("../data/mit/nonfaces/");
	ml::database2<double, double> data;
	data.patterns.resize(400, files.size());
	for (size_t i = 0;i < files.size(); ++i){
		auto image = cv::image_cast<cv::image_bgr>(cv::io::read_bmp(files[i]));
		transform(image, column(data.patterns, i), [](cv::bgr e){ return double(e.b); });
	}
    data.targets = data.patterns;

	auto sp_auto_encoder = make_shared<nn::auto_encoder>(400, 200);
	optimizer_adaptor<bfgs<auto_encoder>> opt(sp_auto_encoder);
	sp_auto_encoder->train(data, opt);
   // auto image = cv::image_cast<cv::image_bgr>(cv::io::read_bmp("../data/mit/nonfaces/B5_00000.bmp", info));
   // cv::image_gray im_gray(image.extent());
   // transform(image, im_gray, [](cv::bgr pixel) {
   //     return byte(pixel.b);
   // });
    
    int a= 3;
    
}