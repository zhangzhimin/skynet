/******************************************************************************
Created By : Zhang Zhimin
Created On : 2013/2/26
Purpose    :  
********************************************************************************/

#pragma once

#include <skynet\config.hpp>

namespace skynet{
	template <typename Pred>
	priority_queue<size_t, std::vector<size_t>, Pred> make_priority_queue(Pred pre){
		return priority_queue<size_t, std::vector<size_t>, Pred>(pre);
	}
}