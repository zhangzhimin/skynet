#pragma once

#include <skynet/core/exception.hpp>
#include <skynet/cv/spatial_based.hpp>
#include <boost/any.hpp>

namespace skynet{namespace cv{


	class filters {
	public:
		typedef std::function<boost::any(boost::any, std::vector<std::string>)> filter_function;

		filters() {
			register_filter("gaussian", [](boost::any input, std::vector<std::string> arguments)->boost::any{
				if (input.type() == typeid(array2s)) {
					auto tmp = boost::any_cast<array2s>(input);
					return *cv::filter(tmp, gaussian_mask<2>());
				}

				THROW_EXCEPTION(notimplement_exception());
			});
		}

		struct filter{
			std::string id;
			std::vector<std::string> arguments;
		};

		boost::any operator()(boost::any input) {
			boost::any re = input;
			for (int i = 0; i < _filters.size(); ++i) {
				re = _funs_lut[_filters[i].id](re, _filters[i].arguments);
			}

			return re;
		}

		void register_filter(std::string id, filter_function fun) {
			_funs_lut[id] = fun;
		}

		void add_filter(filter v) {
			_filters.push_back(v);
		}

		
	private:
		std::map<std::string, filter_function>  _funs_lut;
		std::vector<filter>		_filters;
	};

}}
