
#include <skynet/neuralnetworks/ffnet.hpp>
#include <skynet/cv/extension/bmp.hpp>
#include <skynet/utility/file.hpp>

#include <skynet/core/ublas_extentsion.hpp>

using namespace skynet;

int main(){
    cv::io::bmp_info_header info;
    
    auto files = load_directory("../data/mit/nonfaces/");
    
    auto image = cv::image_cast<cv::image_bgr>(cv::io::read_bmp("../data/mit/nonfaces/B5_00000.bmp", info));
    cv::image_gray im_gray(image.extent());
    transform(image, im_gray, [](cv::bgr pixel) {
        return byte(pixel.b);
    });
    
    int a= 3;
    
}