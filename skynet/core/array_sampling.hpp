#pragma once

#include <skynet/config.hpp>
#include <skynet/utility/iterator_facade.hpp>

#include <type_traits>

#pragma message("the sampling of the std::array has bug")

namespace skynet{

    template <typename M>
    class array_down{
    public:
        typedef array_down							type;
        static const size_t							dim = M::dim;
        typedef M									array_container;
        typedef typename M::value_type				value_type;
        typedef typename M::reference               reference;
        typedef typename M::const_reference 		const_reference;
        typedef typename M::index_type				index_type;
        typedef typename M::extent_type				extent_type;

        typedef index_iterator<const type>			const_iterator;
        typedef index_iterator<type>				iterator;

        array_down(const M &mat, const size_t &rate) : _mat(mat), _sampling_rate(rate){
#ifdef DEBUG
            for (int i = 0; i < dim - 1; ++i){
                ASSERT(_mat.extent()[i]%_sampling_rate == 0, "the extent of std::array shouble be exp2.");
            }
#endif
        }

        const_reference operator[](const size_t &i) const {
            auto index = size_t2index(i, extent())<<_sampling_rate;
            auto offset = index2size_t(index, _mat.extent());
            return _mat[offset];
        }

        const_iterator begin() 	const			    { return const_iterator(const_cast<type *>(this), 0); }
        const_iterator end() const					{ return const_iterator(const_cast<type *>(this), size()); }

        extent_type extent() const{
            return _mat.extent()>>_sampling_rate;
        }

        size_t size() const{
            return std::accumulate(extent().begin(), extent().end(), 1, std::multiplies<int>());

        }

    private:
        M			        _mat;
        const size_t		_sampling_rate;
    };

    template <typename M>
    array_down<M> down_sample(const M mat, const size_t &rate = 1U){
        return array_down<M>(mat, rate);
    }

    template <typename M>
    class array_up{
    public:
        typedef array_up							type;
        static const size_t							dim = M::dim;
        typedef M									array_container;
        typedef typename M::value_type				value_type;
        typedef typename M::reference				reference;
        typedef typename M::const_reference			const_reference;
        typedef typename M::index_type				index_type;
        typedef typename M::extent_type				extent_type;

        typedef index_iterator<const type>			const_iterator;
        typedef index_iterator<type>				iterator;

        array_up(const M &mat, const size_t &rate) : _mat(mat), _sampling_rate(rate) {}

        const_reference operator[](const size_t &i) const {
            auto index = size_t2index(i, extent()) >> _sampling_rate;
            auto offset = index2size_t(index, _mat.extent());
            return _mat[offset];
        }

        const_iterator begin() 	const			    { return const_iterator(const_cast<type *>(this), 0); }
        const_iterator end() const					{ return const_iterator(const_cast<type *>(this), size()); }

        extent_type extent() const{
            return _mat.extent()<<_sampling_rate;
        }

        size_t size() const{
            return std::accumulate(extent().begin(), extent().end(), 1, std::multiplies<int>());
        }

    private:
        M			        _mat;
        const size_t		_sampling_rate;
    };

    template <typename M>
    array_up<M> up_sample(const M mat, const size_t &rate = 1U){
        return array_up<M>(mat, rate);
    }
}
