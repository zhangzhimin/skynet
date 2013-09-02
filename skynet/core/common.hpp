

#pragma once

#include <skynet/utility/tag.hpp>

#pragma warning(disable : 4018)
#pragma warning(disable : 4244)
#pragma warning(disable : 4267)


namespace skynet {
	template <typename T1, typename T2>
	bool bound_check(const T1 &index, const T2 &size){
		IS_SAME_DIMENTION(T1,T2);

		for (int i = 0; i < T1::dim; ++i){
			if (index[i] < 0 || index[i] >= size[i])	return false;
		}

		return true;
	}

	template <typename index_type, typename extent_type>
	index_type bound_index(const index_type &index, const extent_type &size){
		IS_SAME_DIMENTION(index_type, extent_type);
		auto temp = index;
		for (int i = 0; i < index_type::dim; ++i){
			temp[i] = index[i] < 0 ? 0 : index[i] >= size[i] ? size[i] - 1 : index[i];
		}

		return temp;
	}

	template <typename extent_type>
	extent_type size_t2index(const size_t &pos, const extent_type &size){
		extent_type index;

		auto temp = pos;
		for (int i = 0; i < extent_type::dim; ++i){
			size_t k = temp / size[i];
			index[i] = temp - k * size[i];
			temp = k;
		}

		return index;
	}

	template <typename index_type, typename extent_type>
	size_t index2size_t(const index_type &index, const extent_type &extent){
		IS_SAME_DIMENTION(index_type, extent_type);

		size_t pos = 0;
		size_t slide = 1;
		for (int i = 0; i < extent_type::dim; ++i){
			pos += slide * index[i];
			slide *= extent[1];
		}

		return pos;
	}


	template <size_t dim_>
	struct set_boundary_imp;

	template <>
	struct set_boundary_imp<3U>{
		template <typename M>
		static void do_it(M &mat, const typename M::value_type &value, const size_t thickness = 1){
			auto size = mat.extent();
			ASSERT(size[0] >= thickness && size[1] >= thickness && size[2] >= thickness, "the thickness is more than size");

			for (int t = 0; t < thickness; ++t){
				for (int i = 0; i < size[1]; ++i){
					for (int j = 0; j < size[2]; ++j){
						mat.set_value(index3(t, i, j), value);
						mat.set_value(index3(size[0] - 1-t, i, j), value);
					}
				}

				for (int i = 0; i < size[0]; ++i){
					for (int j = 0; j < size[2]; ++j){
						mat.set_value(index3(i, t, j), value);
						mat.set_value(index3(i, size[1] - 1-t, j), value);
					}
				}

				for (int i = 0; i < size[0]; ++i){
					for (int j = 0; j < size[1]; ++j){
						mat.set_value(index3(i, j, t), value);
						mat.set_value(index3(i, j, size[2] - 1-t), value);
					}
				}
			}
		}
	};

	template <>
	struct set_boundary_imp<2U>{
		template <typename M>
		static void do_it(M &mat, const typename M::value_type &value, const size_t thickness = 1){
			auto size = mat.extent();

			#ifdef DEBUG
						for (int i = 0; i < typename M::extent_type::dim; ++i){
							ASSERT(size[i] >= thickness, "the thickness is more than size");
						}
			#endif

			for (int col = 0; col < size.x; ++ col){
				mat.set_value(index2(col, 0), value);
				mat.set_value(index2(col, size.y - 1), value);
			}

			for (int row = 0; row < size.y; ++row){
				mat.set_value(index2(0,row), value);
				mat.set_value(index2(size.x-1, row), value);
			}
		}
	};

	template <typename M>
	void set_boundary(M &mat, const typename M::value_type &value, const size_t &thickness = 1){
		set_boundary_imp<M::dim>::do_it(mat, value, thickness);				
	}
}
