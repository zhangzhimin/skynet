/******************************************************************************
Created By : Zhang Zhimin
Created On : 2012/12/28
Purpose    :  
********************************************************************************/

#pragma once

#include <skynet/core/conv.hpp>
#include <skynet/core/lazy_evaluation.hpp>

#include <array>

#pragma message("The the gradient operator just support the volume.")
#pragma message("Because of the std::function, the lazy_array has the peformance problem, \
Please refactor in The forture.")

namespace skynet{ namespace cv{ namespace field{
	namespace detail{
		template <typename size_t dim_>
		struct gradient_operator;

		template <>
		struct gradient_operator<3>{
			template <typename M>
			static auto do_it(const M &mat, const int &radius)
				->lazy_array<M::dim, function<point<typename M::value_type, M::dim>(const size_t &)>>
			{
				typedef typename M::value_type      value_type;

				center_difference_mask<3, 0> mask_diff0(radius);
				mask_diff0.attach(mat.extent());
				center_difference_mask<3, 1> mask_diff1(radius);
				mask_diff1.attach(mat.extent());
				center_difference_mask<3, 2> mask_diff2(radius);
				mask_diff2.attach(mat.extent());

				auto mat_diff0 = conv(mat, mask_diff0);
				auto mat_diff1 = conv(mat, mask_diff1);
				auto mat_diff2 = conv(mat, mask_diff2);

				function<point<value_type, M::dim>(const size_t &)> fun = 
					[=](const size_t &i)->point<value_type, 3>{
						typedef decltype(mat_diff0[0]) value_type;
						return point<value_type, 3>(mat_diff0[i], mat_diff1[i], mat_diff2[i]);
				};

				return make_lazy_array(mat.extent(), fun);
			}
		};

		template <>
		struct gradient_operator<2>{
			template <typename M>
			static auto do_it(const M &mat, const int &radius)
			{
				typedef typename M::value_type      value_type;

				center_difference_mask<2, 0> mask_diff0(radius);
				mask_diff0.attach(mat.extent());
				center_difference_mask<2, 1> mask_diff1(radius);
				mask_diff1.attach(mat.extent());

				auto mat_diff0 = conv(mat, mask_diff0);
				auto mat_diff1 = conv(mat, mask_diff1);

				return make_lazy_array(mat.extent(), [=](const size_t &i)->point<value_type, 2> {
					typedef decltype(mat_diff0[0]) value_type;
					return point<value_type, 2>(mat_diff0[i], mat_diff1[i]);
				});
			}
		};
	}

	template <typename M>
	auto gradient(const M &mat, const int &radius = 1)
	{
		return detail::gradient_operator<M::dim>::do_it(mat, radius);
	}

	template <typename M>
	auto laplace(const M &mat, const int &radius = 1)
	{
		return conv(mat, laplace_mask<M::dim>(radius));
	}

	template <typename M>
	auto gradient_vector_flow(const M &mat, double mu, size_t  iterations) {
		typedef decltype(gradient(mat))			GradientType;
		typedef GradientType::value_type		vector_type;
		typedef vector_type::value_type			value_type;
		static const size_t dim = GradientType::dim;

		multi_array<vector_type, dim> gradient_field(mat.extent());
		copy(gradient(mat), gradient_field);
		multi_array<value_type, dim> gradient_magnitude_square(mat.extent());
		transform(gradient_field, gradient_magnitude_square, [](auto v) { return square_sum(v); });
		
		multi_array<vector_type, dim> gradient_flow(mat.extent());
		multi_array<vector_type, dim> new_gradient_flow(mat.extent());
		copy(gradient_field, gradient_flow);

		auto delta_t = 0.2 / mu;

		for (int i = 0; i < iterations; ++i) {
			auto term1 = gradient_magnitude_square * (gradient_field - gradient_flow);
			auto term2 = mu * laplace(gradient_flow);
			copy(gradient_flow +  term1 + term2, new_gradient_flow);
			copy(new_gradient_flow, gradient_flow);
		}
		
		return gradient_flow;
	}


	//template <typename M>
	//auto divergence(const M &mat, const int &radius = 1)
	//	->lazy_array<M::dim, function<
	//	typename std::remove_const<typename std::remove_reference<decltype(mat[0][0])>::type>::type	(const size_t &)>>
	//{
	//	static const size_t dim = M::dim;
	//	typedef typename M::value_type				vector_type;
	//	typedef typename vector_type::value_type	value_type;
	//	static_assert(std::is_same<vector_type, point<value_type, dim>>::value, "The M should be std::vector field multi_array");
	//	

	//	auto mat_gradient = ▽(mat, radius);
	//	function<value_type (const size_t &)> fun =
	//		[mat_gradient](const size_t &i){
	//			//walk around for the type
	//			std::remove_const<typename std::remove_reference<decltype(mat_gradient[0][0][0])>::type>::type	temp(0);
	//			
	//			for (int k = 0; k < dim; ++k){
	//			auto a = mat_gradient[i][k][k];
	//				temp += a;
	//			}
	//			return temp;
	//	};

	//	return make_lazy_array(mat.extent(), fun);
	//}
}}}
