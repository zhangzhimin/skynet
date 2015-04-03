#pragma once

#include <skynet/core/conv.hpp>

namespace skynet {
	namespace cv {

		template <typename M>
		class down_sampling_array {
		public:
			typedef down_sampling_array					type;
			static const size_t							dim = M::dim;
			typedef M									array_container;
			typedef typename M::value_type				value_type;
			typedef typename M::reference               reference;
			typedef typename M::const_reference 		const_reference;
			typedef typename M::index_type				index_type;
			typedef typename M::extent_type				extent_type;

			typedef index_iterator<const type>			const_iterator;
			typedef index_iterator<type>				iterator;

			down_sampling_array(const M &mat, const size_t &rate) : _mat(mat) { }

			const_reference operator[](const size_t &i) const {
				auto index = size_t2index(i, extent());
				return (*this)(index);
			}

			const_reference operator()(const index_type &index) const {
				auto index_down = index << 1;
				return _mat(index_down);
			}

			const_iterator begin() 	const { return const_iterator(const_cast<type *>(this), 0); }
			const_iterator end() const { return const_iterator(const_cast<type *>(this), size()); }

			extent_type extent() const {
				return _mat.extent() >> _sampling_rate;
			}

			size_t size() const {
				return std::accumulate(extent().begin(), extent().end(), 1, std::multiplies<int>());
			}

		private:
			M			        _mat;
		};

		template <typename M>
		down_sampling_array<M> down_sampling(const M mat) {
			return down_sampling_array<M>(mat);
		}

		//template <typename M>
		//class array_up{
		//public:
		//    typedef array_up							type;
		//    static const size_t							dim = M::dim;
		//    typedef M									array_container;
		//    typedef typename M::value_type				value_type;
		//    typedef typename M::reference				reference;
		//    typedef typename M::const_reference			const_reference;
		//    typedef typename M::index_type				index_type;
		//    typedef typename M::extent_type				extent_type;

		//    typedef index_iterator<const type>			const_iterator;
		//    typedef index_iterator<type>				iterator;

		//    array_up(const M &mat, const size_t &rate) : _mat(mat), _sampling_rate(rate) {}

		//    const_reference operator[](const size_t &i) const {
		//        auto index = size_t2index(i, extent()) >> _sampling_rate;
		//        auto offset = index2size_t(index, _mat.extent());
		//        return _mat[offset];
		//    }

		//    const_iterator begin() 	const			    { return const_iterator(const_cast<type *>(this), 0); }
		//    const_iterator end() const					{ return const_iterator(const_cast<type *>(this), size()); }

		//    extent_type extent() const{
		//        return _mat.extent()<<_sampling_rate;
		//    }

		//    size_t size() const{
		//        return std::accumulate(extent().begin(), extent().end(), 1, std::multiplies<int>());
		//    }

		//private:
		//    M			        _mat;
		//    const size_t		_sampling_rate;
		//};

		//template <typename M>
		//array_up<M> up_sample(const M mat, const size_t &rate = 1U){
		//    return array_up<M>(mat, rate);
		//}
	}
}
