#pragma once

#include <skynet/core/conv.hpp>

namespace skynet{namespace cv{

	template <typename size_t dim_, typename T = double>
	class gabor_mask;

	template <typename T>
	class gabor_mask<2, T> : public mask_adaptor<gabor_mask<2, T>, 2, T>{
	public:
		static_assert(std::is_floating_point<T>::value, "The T should be floatint point");
		static const size_t								dim = 2;

		gabor_mask(T frequency, T theta, int radius) 
			: _frequency(frequency), _theta(theta), _radius(radius) {}

		void attach(const extent_type &extent){
			_extent = extent;
			_elements.clear();

			auto gamma = T(_radius*2+1) / T(5.36);
			for (int y = -_radius; y <= _radius; ++y){
				for (int x = -_radius; x <= _radius; ++x){
					auto weight = exp(-(sqr(x)+sqr(y))/(2*sqr(gamma))) / 
						sqrt(2*PI*sqr(gamma)) *
						cos(2*PI*_frequency/(2*_radius+1)*(x*cos(_theta) + y*sin(_theta)));
					detail::mask_element<T> e;
					e.weight = weight;
					e.pos = x + y * _extent.x;
					_elements.push_back(e);
				}
			}
		}

		const_reference operator[](const size_t &i) const   { return _elements[i]; }
		size_t size() const                                 { return _elements.size(); }

	private:
		int                                     _radius;
		T										_frequency;
		T										_theta;
		extent_type                             _extent;
		std::vector<value_type>						_elements;
	};


	void gabor_filter(double frequency, double theta, double size){

	}

}}
