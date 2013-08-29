/******************************************************************************
Created By : Zhang Zhimin
Created On : 2012/9/27
Purpose    :  
********************************************************************************/

#ifndef PALETTES_HPP
#define	PALETTES_HPP	

#pragma once

#include <skynet/cv/pixel.hpp>
#include <skynet/utility/exception.hpp>
#include <skynet/utility/track.hpp>
#include <skynet/cv/lut.hpp>
#include <skynet/utility/serialization.hpp>

#include <fstream>
//const char *PALETTE_DIR_PATH;

#pragma error //"Please fixes the bugs"

namespace skynet{namespace cv{

	
	typedef skynet::lut<byte, rgb> palette;

	palette make_palette(const std::string &filename){
		std::ifstream ifs(filename);

		if (!ifs.good()){
			std::stringstream ss;
			ss << "failed open " << filename << std::endl;
			THROW_EXCEPTION(std::exception(ss.str().c_str()));
		}
		
		try{
			palette palette_lut;

			boost::archive::xml_iarchive ia(ifs);
			ia >> boost::serialization::make_nvp("palette", palette_lut);

			return palette_lut;
		}
		catch(std::exception &e){
			LOG_ERROR("Failed to Unserialization the palette.");
			throw e;
		}
	}


	class palettes{
	public:
		static void init(const char *dir){
			imp = new palettes(dir);
		}

		static palettes *instance(){
			ASSERT(imp != nullptr, "Please initialize the palettes by init.");

			return imp;
		}

		const palette& gray(){
			if (m_gray == nullptr){

				std::array<rgb, 256> a;

				for (int i = 0; i < 256; ++i){
					a[i].r = (byte)i;
					a[i].g = (byte)i;
					a[i].b = (byte)i;
				}

				m_gray = new palette(a);
			}

			return *m_gray;
		}

		const palette &HUE(){
			if (m_HUE == nullptr){
				std::string temp = "\\HUE.lut";
				m_HUE = new palette(make_palette(m_dir + temp));
			}
			return *m_HUE;
		}

		const palette &perfusion(){
			if (m_perfusion == nullptr){
				std::string temp = "\\perfusion.lut";
				m_perfusion = new palette(make_palette(m_dir + temp));
			}

			return *m_perfusion;
		}		

		const palette &rainbow(){
			if (m_rainbow == nullptr){
				std::string temp = "\\rainbow.lut";
				m_rainbow = new palette(make_palette(m_dir + temp));
			}

			return *m_rainbow;
		}

		const palette &jet(){
			if (m_jet == nullptr){
				std::string temp = "\\jet.lut";
				m_jet = new palette(make_palette(m_dir + temp));
			}

			return *m_jet;
		}

		const palette& GE(){
			if (m_GE == nullptr){
				std::string temp = "\\GE.lut";
				m_GE = new palette(make_palette(m_dir + temp));
			}

			return *m_GE;
		}

		const palette& hot_green(){
			if (m_hot_green == nullptr){
				std::string temp = "\\hot_green.lut";
				m_hot_green = new palette(make_palette(m_dir + temp));
			}

			return *m_hot_green;
		}

	private:
		palettes(const std::string &dir)
			: m_dir(dir), m_HUE(nullptr), m_perfusion(nullptr), m_GE(nullptr),
			m_jet(nullptr), m_hot_green(nullptr), m_gray(nullptr), m_rainbow(nullptr){}

		~palettes(){
			//in fact, it's not neccessary.
			if (m_HUE != nullptr){
				delete m_HUE;
			}

			if (m_perfusion != nullptr){
				delete m_perfusion;
			}

			if (m_gray != nullptr){
				delete m_gray;
			}

			if (m_rainbow != nullptr)	delete m_rainbow;

			if (m_GE != nullptr)		delete m_GE;
			if (m_jet != nullptr)		delete m_jet;
			if (m_hot_green != nullptr)	delete m_hot_green;
		}

	private:
		static palettes *imp;
		const std::string m_dir;

		palette *m_gray;
		
		palette *m_HUE;
		palette *m_perfusion;
		palette *m_rainbow;
		palette *m_jet;
		palette *m_GE;
		palette *m_hot_green;
	};

	//warning:  it will cause redefine error.
//	palettes *palettes::imp = nullptr;
}}

#endif
