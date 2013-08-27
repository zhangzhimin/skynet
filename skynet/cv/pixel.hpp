/******************************************************************************
Created By : Zhang Zhimin
Created On : 2012/9/27
Purpose    :  
********************************************************************************/

#pragma once

#include <skynet\config.hpp>
#include <skynet\core\point.hpp>

#include <boost\serialization\nvp.hpp>

namespace skynet{ namespace cv{
	struct rgba{
		byte	r;
		byte	g;
		byte	b;
		byte	a;
	};

	struct bgra{
		byte	b;
		byte	g;
		byte	r;
		byte	a;
	};

	struct rgb{
		byte	r;
		byte	g;
		byte	b;
	};

	struct bgr{
		byte	b;
		byte	g;
		byte	r;

		static bgr blue;
		static bgr green;
		static bgr red;
	};

	bgr bgr::blue = {255, 0, 0};
	bgr bgr::green = {0,255, 0};
	bgr bgr::red = {0,0,255};


	template <typename archive_type>
	void serialize(archive_type &ar, rgba &pixel, const unsigned int &){
		ar & boost::serialization::make_nvp("r", pixel.r);
		ar & boost::serialization::make_nvp("g", pixel.g);
		ar & boost::serialization::make_nvp("b", pixel.b);
		ar & boost::serialization::make_nvp("a", pixel.a);
	}

	template <typename archive_type>
	void serialize(archive_type &ar, rgb &pixel, const unsigned int&){
		ar & boost::serialization::make_nvp("r", pixel.r);
		ar & boost::serialization::make_nvp("g", pixel.g);
		ar & boost::serialization::make_nvp("b", pixel.b);
	}
}}