#pragma once

#include <skynet\config.hpp>
#include <skynet\core\array.hpp>
#include <skynet\cv\pixel.hpp>

#include <stdexcept>
#include <boost\any.hpp>


namespace skynet{namespace cv{

	typedef multi_array<cv::bgr, 2>				image_bgr;
	typedef multi_array<byte, 2>				image_gray;
	typedef multi_array<int, 2>					image_int;
	typedef multi_array<cv::bgra, 2>            image_bgra;
	typedef multi_array<cv::rgb, 2>				image_rgb;
	typedef multi_array<cv::rgba, 2>			iamge_rgba;

	enum class image_type{
		none = 0,
		gray8 ,   //byte
		gray16,      //unsigned short
		gray32,      //unsigned int
		rgb,         
		bgr,
		rgba,
		float32,     //float
		float64      //double
	};


	template <typename Image>
	struct image2type;


	template <>
	struct image2type<byte>{
		static const image_type type = image_type::gray8;
	};


	template <>
	struct image2type<unsigned short>{
		static const image_type type = image_type::gray16;
	};


	template <>
	struct image2type<unsigned int>{
		static const image_type type = image_type::gray32;
	};


	template <>
	struct image2type<cv::rgb>{
		static const image_type type = image_type::rgb;
	};

	template <>
	struct image2type<cv::bgr>{
		static const image_type type = image_type::bgr;
	};


	template <>
	struct image2type<cv::rgba>{
		static const image_type type = image_type::rgba;
	};


	template <>
	struct image2type<float>{
		static const image_type type = image_type::float32;
	};


	template <>
	struct image2type<double>{
		static const image_type type = image_type::float64;
	};


	class image_any{
	public:
		//not add explicited
		template <typename Image>
		image_any(const Image &img)
			:_content(img), _type(image2type<typename Image::value_type>::type),
			_pixel_size_x(0.0), _pixel_size_y(0.0){	}

		image_any(const image_any &lhs): _content(lhs._content), _type(lhs._type) {}

		image_any(): _type(image_type::none){}

		image_any& operator=(const image_any &lhs){
			_content = lhs._content;
			_type = lhs._type;
			return *this;
		}

		bool empty() const								{ return _content.empty(); }

		void pixel_size_x(double v)						{ _pixel_size_x = v; }
		void pixel_size_y(double v)						{ _pixel_size_y = v; }

		double pixel_size_x() const						{ return _pixel_size_x; }
		double pixel_size_y() const						{ return _pixel_size_y; }

		image_type type() const							{ return _type; }

		template <typename Image>
		friend Image image_cast(const image_any &any);

	private:
		boost::any _content;
		image_type _type;

		double	_pixel_size_x;
		double _pixel_size_y;
	};


	template <typename Image>
	Image image_cast(const image_any &any){
		if (image2type<typename Image::value_type>::type != any._type)
			THROW_EXCEPTION(std::runtime_error("The image type is not matched."));

		return boost::any_cast<Image>(any._content);
	}

	image_bgr gray2bgr(const image_gray &im_gray){
		image_bgr im_bgr(im_gray.width(), im_gray.height());
		transform(im_gray, im_bgr, [](const byte &e){
			bgr v = {e, e, e};
			return v;
		});

		return im_bgr;
	}
}}