#pragma once

#include <skynet/core/conv.hpp>
#include <skynet/cv/extension/image.hpp>

namespace skynet{namespace cv{


	image_int integrate_image(const image_gray &image){
		image_int row_cum_image(image.extent());
		for (int y = 0; y < row_cum_image.height(); ++y){
			row_cum_image(0, y) = image(0, y);
		}

		for (int y = 0; y < row_cum_image.height(); ++y){
			for (int x = 1; x < row_cum_image.width(); ++x){
				row_cum_image(x, y) = row_cum_image(x-1, y) + image(x, y);
			}
		}

		image_int integral_image(image.extent());
		for (int x = 0; x < integral_image.width(); ++x){
			integral_image(x, 0) = row_cum_image(x, 0);
		}

		for (int y = 1; y < integral_image.height(); ++y){
			for (int x = 0; x < integral_image.width(); ++x){
				integral_image(x, y) = integral_image(x, y-1) + row_cum_image(x, y);
			}
		}

		return integral_image;
	}


	image_int rect_sum(const image_int &integratal_image, const extent2 &rect)
	{		
		ASSERT((integratal_image.extent()%rect) == extent2::zero, "The size is not matched.");

		auto extent = integratal_image.extent() / rect;
		image_int im_rect_sum(extent);
		for (size_t y = 0; y < im_rect_sum.height(); ++y){
			for (size_t x = 0; x < im_rect_sum.width(); ++x){
				auto top_left = index2(x, y) * rect;
				auto bottome_right = top_left + rect - extent2::unit;
				auto top_right = index2(bottome_right.x, top_left.y);
				auto bottome_left = index2(top_left.x, bottome_right.y);
				im_rect_sum(x, y) = integratal_image(top_left) + integratal_image(bottome_right)
					- integratal_image(top_right) - integratal_image(bottome_left);
			}
		}

		return im_rect_sum;
	}

	//1 2 4 8...down sample
	std::vector<image_int> image_pyramid(const image_gray &image){
		auto integral_image = integrate_image(image);
		std::vector<image_int>  images;
		images.push_back(clone(lazy_cast(image,int(0))));

		for (auto rect = 2*extent2::unit; rect.x < image.extent().x && rect.y < image.extent().y; rect *= 2){
			images.push_back(rect_sum(integral_image, rect));
		}
		
		return images;
	}





}}
