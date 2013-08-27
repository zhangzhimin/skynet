#pragma once

#include <skynet\config.hpp>

#include <exception>
#include <stdexcept>



namespace skynet{


    class not_implement : public std::exception{
    public:
        typedef std::exception				base_type;
        typedef not_implement		type;

        not_implement(const char *info) : base_type(info)	{}
    };


}
