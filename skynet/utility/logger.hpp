/******************************************************************************
Created By : Zhang Zhimin
Created On : 2013/3/18
Purpose    :  
********************************************************************************/

#pragma once

#include <skynet/config.hpp>
#include <boost/log/common.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/attributes/timer.hpp>
#include <boost/log/attributes/named_scope.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/support/date_time.hpp>

#include <sstream>


namespace skynet{ 
	namespace log = boost::log;

	enum severity_level{
		normal = 0,
		notification,
		warning,
		error,
		critcal
	};

	template <typename CharT, typename TraitsT>
	inline std::basic_ostream<CharT, TraitsT>& operator<< (std::basic_ostream<CharT, TraitsT>& strm, severity_level lvl){
		static const char* const str[] ={
			"normal",
			"notification",
			"warning",
			"error",
			"critical"
		};

		if (static_cast<std::size_t>(lvl) < (sizeof(str) / sizeof(*str)))
			strm << str[lvl];
		else
			strm << static_cast<int>(lvl);
		return strm;
	}

	inline string get_uid_filename(){
		std::time_t now;
		std::time(&now);

		std::stringstream ss;
		ss << now;
		string filename = "D:/LOG/" + ss.str() + ".log";

		return filename;
	}
}