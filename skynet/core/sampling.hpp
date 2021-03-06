#pragma once

#include <skynet/core/adaptor_types.hpp>
#include <skynet/core/array.hpp>

namespace skynet {
	namespace cv {

		template <typename M>
		class array_down: public iterator_adaptor<array_down<M>> {
		public:
			typedef array_down							type;
			static const size_t							dim = M::dim;
			typedef M									array_container;
			typedef typename M::value_type				value_type;
			typedef value_type					        reference;
			typedef value_type							const_reference;
			typedef typename M::index_type				index_type;
			typedef typename M::extent_type				extent_type;

			array_down(const M &mat) : _mat(mat) { }

			const_reference operator[](const size_t &i) const {
				auto index = size_t2index(i, extent());
				return (*this)(index);
			}

			const_reference operator()(const index_type &index) const {
				auto index_down = index << 1;
				return _mat(index_down);
			}

			extent_type extent() const {
				return _mat.extent() >> 1;
			}

			size_t size() const {
				return std::accumulate(extent().begin(), extent().end(), 1, std::multiplies<int>());
			}

		private:
			M			        _mat;
		};

		template <typename MultiArray>
		auto down_sampling(const MultiArray &data) {
			return array_down<MultiArray>(data);
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
