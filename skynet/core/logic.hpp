******************************************************************************
Created By : Zhang Zhimin
Created On : 2012/12/24
Purpose    :  
********************************************************************************/

#pragma once

#include <skynet/core/lazy_evaluation.hpp>

namespace skynet{ 
    template <typename M>
    auto not(const M &mat_src)->lazy_array<M::dim, function<bool (const size_t &)>>{
        function<bool (const size_t &)> fun = [=](const size_t &i)->bool{
            return !mat_src[i];
        };

        return make_lazy_array(mat_src.extent(), fun);
    }

    template <typename M1, typename M2>
    auto and(const M1 &mat1, const M2 &mat2)->lazy_array<M1::dim, function<bool (const size_t &)>>{
        ASSERT(mat1.extent() == mat2.extent(), "The mat1 and mat2 should have the same extent.");
        function<bool (const size_t &)> fun = [=](const size_t &i)->bool{
            return mat1[i]&&mat2[i];
        };

        return make_lazy_array(mat1.extent(), fun);
    }

    template <typename M1, typename M2>
    auto or(const M1 &mat1, const M2 &mat2)->lazy_array<M1::dim, function<bool (const size_t &)>>{
        ASSERT(mat1.extent() == mat2.extent(), "The mat1 and mat2 should have the same extent.");
        function<bool (const size_t &)> fun = [=](const size_t &i)->bool{
            return mat1[i]||mat2[i];
        };

        return make_lazy_array(mat1.extent(), fun);
    }
}
