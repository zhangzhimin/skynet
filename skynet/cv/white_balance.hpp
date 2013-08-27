#include <skynet\core\array.hpp>
#include <skynet\cv\pixel.hpp>
#include <skynet\core\lazy_evaluation.hpp>

namespace skynet{namespace cv{

	multi_array<cv::rgb, 2> gray_word(const multi_array<cv::rgb, 2> &img){
		auto img_double = lazy_cast(img, point3d());
		auto color_mean = mean(img_double);
		auto gray_mean = mean(color_mean);

		auto k_r = gray_mean / color_mean[0];
		auto k_g = gray_mean / color_mean[1];
		auto k_b = gray_mean / color_mean[2];


	}

}}