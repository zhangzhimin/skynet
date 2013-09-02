#pragma once

#include <fstream>
#include <stdexcept>
#include <cassert>

#include <skynet/cv/extension/image.hpp>
#include <skynet/utility/exception.hpp>

namespace skynet{ namespace cv{
	

	namespace io{
        
        struct bmp_file_header{
            uint16_t		signature;
            uint32_t		file_size;
            uint16_t		reserved1;
            uint16_t		reserved2;
            uint32_t		raw_data_offset;
        };
        
        struct bmp_info_header{};
        
        struct dib_header : public bmp_info_header{
            /// The size of this header:
            /// - 40 bytes for Windows V3 header
            /// - 12 bytes for OS/2 V1 header
            uint32_t		header_size;
            
            /// The bitmap width in pixels ( signed integer ).
            int32_t			width;
            
            /// The bitmap height in pixels ( signed integer ).
            int32_t			height;
            
            uint16_t		planes_of_color;
            
            /// The number of bits per pixel, which is the color depth of the image.
            /// Typical values are 1, 4, 8, 16, 24 and 32.
            uint16_t		bits_per_pixel;
            
            /// The compression method being used. See above for a list of possible values.
            uint32_t		compression;
            
            /// The image size. This is the size of the raw bitmap data (see below),
            /// and should not be confused with the file size.
            uint32_t		image_size;
            
            /// The horizontal resolution of the image. (pixel per meter, signed integer)
            int32_t			horizontal_resolution;
            
            /// The vertical resolution of the image. (pixel per meter, signed integer)
            int32_t			vertical_resolution;
            
            /// The number of colors in the color palette, or 0 to default to 2^n - 1.
            uint32_t        num_colors;
            
            /// The number of important colors used, or 0 when every color is important; 
            /// generally ignored.
            uint32_t		num_important_colors;
        };
        
        
		namespace detail{
			auto write_header = [](const bmp_file_header &file_header, const dib_header &info, std::ostream &fs){
				//write file header
				fs.write((char *)&(file_header.signature), 2);
				fs.write((char *)&(file_header.file_size), 4);
				fs.write((char *)&(file_header.reserved1), 2);
				fs.write((char *)&(file_header.reserved2), 2);
				fs.write((char *)&(file_header.raw_data_offset), 4);
				//write dib header
				fs.write((char *)&(info.header_size), 4);
				fs.write((char *)&(info.width), 4);
				fs.write((char *)&(info.height), 4);
				fs.write((char *)&(info.planes_of_color), 2);
				fs.write((char *)&(info.bits_per_pixel), 2);
				fs.write((char *)&(info.compression), 4);
				fs.write((char *)&(info.image_size), 4);
				fs.write((char *)&(info.horizontal_resolution), 4);
				fs.write((char *)&(info.vertical_resolution), 4);
				fs.write((char *)&(info.num_colors), 4);
				fs.write((char *)&(info.num_important_colors), 4);
			};
		}


		image_any read_bmp(const std::string &filename, bmp_info_header &info){
			//std::tr2::sys::path filepath(filename);
			//ASSERT(filepath.extension() == ".bmp", "The file is not bmp.");
			std::ifstream filestream(filename, std::ios::binary);
			if (!filestream.good()){
				THROW_EXCEPTION(std::runtime_error("Failed to open the file."));
			}

			bmp_file_header file_header;
			filestream.read((char *)&(file_header.signature), 2);
			filestream.read((char *)&(file_header.file_size), 4);
			filestream.read((char *)&(file_header.reserved1), 2);
			filestream.read((char *)&(file_header.reserved2), 2);
			filestream.read((char *)&(file_header.raw_data_offset), 4);

			/*switch (file_header.signature)
			{
			case 0x4d42:*/
			if (file_header.signature == 0x4d42){
				filestream.seekg(0x000E);
				dib_header image_info;
				filestream.read((char *)&(image_info.header_size), 4);
				ASSERT(image_info.header_size == 40, "The dib head size is 40bytes.");
				filestream.read((char *)&(image_info.width), 4);
				filestream.read((char *)&(image_info.height), 4);
				filestream.read((char *)&(image_info.planes_of_color), 2);
				filestream.read((char *)&(image_info.bits_per_pixel), 2);
				filestream.read((char *)&(image_info.compression), 4);
				filestream.read((char *)&(image_info.image_size), 4);
				filestream.read((char *)&(image_info.horizontal_resolution), 4);
				filestream.read((char *)&(image_info.vertical_resolution), 4);
				filestream.read((char *)&(image_info.num_colors), 4);
				filestream.read((char *)&(image_info.num_important_colors), 4);
				ASSERT(uint32_t(filestream.tellg()) == 0x36, "");

				//as gray image
				if (image_info.bits_per_pixel == 8){
					image_gray image(image_info.width, image_info.height);

					if ((image_info.width%4) == 0){
						filestream.seekg(file_header.raw_data_offset);
						filestream.read((char *)image.buffer().get(), image.size());
						return image_any(image);
					}else{
						int stride = (image.width()+3) & (~3);
						for (size_t row = 0; row < image.height(); ++row){
							filestream.seekg(file_header.raw_data_offset + row*stride);
							filestream.read((char *)&(image(0,row)), image.width());
						}

						return image_any(image);
					}
				}

				if (image_info.bits_per_pixel == 24){
					image_bgr image(image_info.width, image_info.height);

					if ((image_info.width%4) == 0){
						filestream.seekg(file_header.raw_data_offset);
						filestream.read((char *)image.buffer().get(), image.size()*3);
						return image_any(image);
					}else{
						int stride = (image.width()*3+3) & (~3);
						for (size_t row = 0; row < image.height(); ++row){
							filestream.seekg(file_header.raw_data_offset + row*stride);
							filestream.read((char *)&(image(0,row)), image.width()*3);
						}

						return image_any(image);
					}
				}

				THROW_EXCEPTION(std::runtime_error("The bmp decode is not supported."));

			}

			THROW_EXCEPTION(std::runtime_error("The bmp decode is not supported."));
		}

		void write_bmp(const multi_array<byte, 2> &image, const std::string &filename){
			//std::tr2::sys::path filepath(filename);
			//ASSERT(filepath.extension() == ".bmp", "The file is not bmp.");

			bmp_file_header file_header;
			file_header.signature = 0x4d42;
			file_header.reserved1 = 0;
			file_header.reserved2 = 0;
			file_header.raw_data_offset = 54 + 256 * 4;

			dib_header image_info;
			image_info.header_size = 40;
			image_info.width = image.width();
			image_info.height = image.height();
			image_info.planes_of_color = 1;
			image_info.bits_per_pixel = 8;
			image_info.compression = 0;
			image_info.horizontal_resolution = 0;
			image_info.vertical_resolution = 0;
			image_info.num_colors = 256;
			image_info.num_important_colors = 0;

			//the stride must be 4x num.
			if ((image_info.width%4) != 0){
				int stride = (image.width()+3) & (~3);
				image_info.image_size = image.height() * stride;
				file_header.file_size = image_info.image_size + file_header.raw_data_offset;

				std::ofstream filestream(filename, std::ios::binary);
				ASSERT(filestream.good(), "");

				detail::write_header(file_header, image_info, filestream);
				//write color table
				for (int i = 0; i < 256; ++i){
					cv::bgra v = {byte(i), byte(i), byte(i), 0};
					filestream.write((char *)&v, 4);
				}
				ASSERT(uint32_t(filestream.tellp()) == file_header.raw_data_offset, "");

				uint32_t pos = filestream.tellp();
				for (int i = 0; i < image.height(); ++i, pos += stride){
					filestream.seekp(pos);
					filestream.write((char *)&(image(0,i)), image.width());
					uint32_t zero = 0;
					filestream.write((char *)&zero, 4);
				}

				filestream.close();
				return;
			}else{
				image_info.image_size = image.size();
				file_header.file_size = image_info.image_size + file_header.raw_data_offset;

				std::ofstream filestream(filename, std::ios::binary);
				ASSERT(filestream.good(), "");
				//write header
				detail::write_header(file_header, image_info, filestream);
				ASSERT(uint32_t(filestream.tellp()) == 54, "");
				//write color table
				for (int i = 0; i < 256; ++i){
					cv::bgra v = {byte(i), byte(i), byte(i), 0};
					filestream.write((char *)&v, 4);
				}
				ASSERT(uint32_t(filestream.tellp()) == file_header.raw_data_offset, "");
				//write raw data
				filestream.write((char *)image.buffer().get(), image_info.image_size);

				filestream.close();
				return;
			}
		}

		void write_bmp(const multi_array<bgr, 2> &image, const std::string &filename){
			//std::tr2::sys::path filepath(filename);
			//ASSERT(filepath.extension() == ".bmp", "The file is not bmp.");

			bmp_file_header file_header;
			file_header.signature = 0x4d42;
			file_header.reserved1 = 0;
			file_header.reserved2 = 0;
			file_header.raw_data_offset = 54;

			dib_header image_info;
			image_info.header_size = 40;
			image_info.width = image.width();
			image_info.height = image.height();
			image_info.planes_of_color = 1;
			image_info.bits_per_pixel = 24;
			image_info.compression = 0;
			image_info.horizontal_resolution = 0;
			image_info.vertical_resolution = 0;
			image_info.num_colors = 0;
			image_info.num_important_colors = 0;

			//the stride must be 4x num.
			if ((image_info.width%4) != 0){
				int stride = (image.width()*24+31)/32 * 4;
				image_info.image_size = image.height() * stride;
				file_header.file_size = image_info.image_size + image_info.header_size + 14;

				std::ofstream filestream(filename, std::ios::binary);
				ASSERT(filestream.good(), "");

				detail::write_header(file_header, image_info, filestream);
				ASSERT(uint32_t(filestream.tellp()) == 0x36, "");

				uint32_t pos = filestream.tellp();
				for (int i = 0; i < image.height(); ++i, pos += stride){
					filestream.seekp(pos);
					filestream.write((char *)&(image(0,i)), image.width()*3);
					uint32_t zero = 0;
					filestream.write((char *)&zero, 4);
				}

				filestream.close();
				return;
			}else{
				image_info.image_size = image.size() * sizeof(bgr);
				file_header.file_size = image_info.image_size + image_info.header_size + 14;

				std::ofstream filestream(filename, std::ios::binary);
				ASSERT(filestream.good(), "");

				//write header
				detail::write_header(file_header, image_info, filestream);

				ASSERT(uint32_t(filestream.tellp()) == 0x36, "");
				//write raw data
				filestream.write((char *)image.buffer().get(), image_info.image_size);

				filestream.close();
				return;
			}
		}


		void write_bmp(const image_any &any, const std::string &filename){
			//std::tr2::sys::path filepath(filename);
			//ASSERT(filepath.extension() == ".bmp", "The file is not bmp.");

			if (any.type() == image_type::gray8){
				auto image = image_cast<array2b>(any);
				write_bmp(image, filename);
				return;
			}

			if (any.type() == image_type::bgr){
				auto image = image_cast<multi_array<bgr, 2>>(any);
				write_bmp(image, filename);
				return;
			}

			THROW_EXCEPTION(std::runtime_error("The image type is not supported."));
		}


	}
}}