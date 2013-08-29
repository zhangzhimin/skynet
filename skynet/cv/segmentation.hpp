/******************************************************************************
Created By : Zhang Zhimin
Created On : 2012/11/15
Purpose    :  
********************************************************************************/

#pragma once


#include <skynet/utility/math.hpp>
#include <skynet/core/lazy_evaluation.hpp>
#include <skynet/cv/field.hpp>
#include <skynet/utility/std_expand.hpp>


//#include <queue>

namespace skynet{ namespace cv{


	template <typename M1, typename M2, typename Func>
	void gray2binary(const M1 &gray_mat, M2 &binary_mat, Func fun){
		binary_mat.resize(gray_mat.extent());
		binary_mat.set_all_zero();

		for (int i = 0; i < gray_mat.size(); ++i){
			if (fun(gray_mat[i])){
				binary_mat[i] = 1;
			}

		}
	}


	template <typename T>
	struct greater_than : std::unary_function<T, bool>{
		greater_than(const T &threshold): m_threshold(threshold) {}

		bool operator()(const T &e) const{
			return e > m_threshold;
		}

	private:
		const T m_threshold;
	};


	template <typename T>
	struct less_than : std::unary_function<T, bool>{
		less_than(const T &threshold): m_threshold(threshold) {}

		bool operator()(const T &e) const{
			return e < m_threshold;
		}

	private:
		const T m_threshold;
	};


	template <typename M, typename Func>
	auto binarize(const M &mat, Func fun)->lazy_unary_matrix<M, function<byte (const typename M::value_type &)>>{
		return apply(mat, function<byte (const typename M::value_type &)>(fun));
	}


}}


///For advanced segmentation methods, such as level set, snake edt..
namespace skynet{ namespace cv{ namespace advanced{
	using skynet::cv::field::¨Œ;
	using skynet::cv::field::div;
	using skynet::cv::field::¡÷;

#pragma message("The edge function is fixed, thinking about use functor take place of it")
#pragma message("The level set for 2D image")
	//http://www.engr.uconn.edu/~cmli/code/ 
	//if the link does not work, please search key word: Distance Regularized Level Sets, Geodesic Active Contours.
	//http://sdrv.ms/15jDtRp 
	//the related resource include paper and code(matlab)
	template <typename M1, typename M2>
	multi_array<float, M1::dim> level_set(const M1 &image,  const M2 &mat_phi0, 
		const float &mu, const float &lambda, const float &alpha, int counter = 10)
	{
		IS_SAME_DIMENTION(M1, M2);

		typedef typename M1::value_type	value_type;
		static const size_t dim = M1::dim;
		ASSERT(image.extent() == mat_phi0.extent(), "The extent of the image and mat_phi0 is not same.");
		//get image energy multi_array
		//think about other edge indicator
		auto mat_temp_gradient = clone(¨Œ(image));
		auto mat_edge = apply(mat_temp_gradient, [](const point<value_type, dim> &p){
			return cumsum(p*p);
		});
		auto mat_force = clone(apply(mat_edge, [](const value_type &v)->float{
			return 1.0f/static_cast<float>(1+0.5f*v);
		}));

		//end get image energy multi_array
		/*
		multi_array<float, 2> mat_force_temp(mat_force.extent());
		deep_copy(mat_force, mat_force_temp);
		writemtx(mat_force_temp, "E:/force.raw");*/

		auto mat_gradient_force = clone(¨Œ(mat_force));
		multi_array<float, dim> mat_phi(mat_phi0.extent());
		deep_copy(mat_phi0, mat_phi);

		while (counter--){
			//compute the curvature
			auto mat_gradient_phi = clone(¨Œ(mat_phi));
			auto mat_normalized_gradient_phi = clone(apply(mat_gradient_phi, [](const point<float, dim> &p){
				return normalize(p);
			}));

			auto mat_curvature = clone(div(mat_normalized_gradient_phi));

			//double well not implement 

			/*		multi_array<float, 2> mat_curvature_temp(mat_phi.extent());
			deep_copy(mat_curvature, mat_curvature_temp);
			writemtx(mat_curvature_temp, "E:/curvature.raw");*/
			//compute the potential term
			/*auto mat_gradient_phi_norm = apply(mat_gradient_phi, [](const point<float, dim> &p){
			return norm(p);
			});
			auto mat_a = binarize(mat_gradient_phi_norm, less_than<float>(1.0f));
			auto mat_b = not(mat_a);*/
			auto mat_dist_term = (apply(¡÷(mat_phi), mat_curvature, 
				[](const float &lhs, const float &rhs)->float{
					return lhs - rhs;
			}));
			auto dirac = [](const float &x){
				auto sigma = 1.5f;
				if (std::abs(x) > sigma)	return 0.0f;

				return (1.0f/(2*sigma))*(1.0f+std::cos(PI*x/sigma));
			};
			auto mat_dirac_phi = (apply(mat_phi, dirac));
			auto mat_area_term = apply(mat_dirac_phi, mat_force, multiplies<float>());

			auto norm_street = [](const point<float, dim> &p){ return norm(p, street_tag()); };
			auto mat_temp0 = apply(mat_gradient_force, mat_normalized_gradient_phi, multiplies<point<float, 2>>());
			auto mat_temp = apply(mat_temp0, norm_street);

			auto mat_edge_term = 
				apply(
				apply(mat_dirac_phi, mat_temp, multiplies<float>()), 
				apply(mat_area_term, mat_curvature, multiplies<float>()),
				plus<float>());

			multi_array<float, 2> mat_phi_old(mat_phi.extent());
			deep_copy(mat_phi, mat_phi_old);

			auto mat_phi_next = mat_phi_old + mu*mat_dist_term + lambda*mat_edge_term + alpha*mat_area_term;
			deep_copy(mat_phi_next, mat_phi);
		}

		return mat_phi;
	}

	///Fast Marching algorithm
	///http://sdrv.ms/15jDtRp this link is some resorce about active countor.
	template <typename M1>
	multi_array<float, M1::dim> fast_marching(const M1 &mat_energe, const size_t &seed, const float &th){
		static const size_t dim = M1::dim;

		enum node_type{
			alive = 2,
			narrow_band = 1,
			faraway = 0,
		};

		multi_array<node_type, M1::dim> mat_flag(mat_energe.extent());
		mat_flag.set_all_zero();
		//set boundary for the edge of image.
		thxy::detail::set_boundary(mat_flag, alive);
		multi_array<float, M1::dim>	mat_u(mat_energe.extent());

		static const float MAX_VALUE = numeric_limits<float>::max()/2.0f;
		fill(mat_u, MAX_VALUE);

		auto heap = make_priority_queue([&mat_u](const size_t &lhs, const size_t &rhs){
			return mat_u[lhs] > mat_u[rhs];
		});

		mat_flag[seed] = narrow_band;
		mat_u[seed] = 1;
		heap.push(seed);

		const diamand_neighbor<M1::dim> neighbors(mat_energe.extent());

		auto update_value = [&](const size_t &offset){

			std::array<size_t, dim> min_neighbors;
			for (int d = 0; d < dim; ++d){
				min_neighbors[d] = std::min(offset+neighbors[d], offset-neighbors[d], 
					[&](const size_t &lhs, const size_t &rhs){
						return mat_u[lhs] < mat_u[rhs];
				});
			}
			std::sort(min_neighbors.begin(), min_neighbors.end(), [&](const size_t &lhs, const size_t &rhs){
				return mat_u[lhs] < mat_u[rhs];
			});

			float aa = 0.0f, bb = 0.0f, cc = 0.0f;
			float solution = MAX_VALUE;

			cc = -sqr(1.0f/mat_energe[offset]);
			for (int d = 0; d < dim; ++d){
				auto neighbor_offset = min_neighbors[d];

				if (solution < mat_u[neighbor_offset])	break;

				aa += 1.0f;
				auto value = mat_u[neighbor_offset];
				bb += value;
				cc += sqr(value);
				auto discrim = sqr(bb) - aa * cc;
				if (discrim<0)	break;

				solution = (sqrt(discrim) + bb) / aa;
			}

			if (solution > th)	return;

			mat_u[offset] = solution;
			heap.push(offset);
		};

		while (!heap.empty()){
			auto foot = heap.top();
			heap.pop();
			if (mat_flag[foot] == alive)
				continue;
			else
				mat_flag[foot] = alive;

			for (int i = 0; i < neighbors.size(); ++i){
				auto offset = foot + neighbors[i];
				if (mat_flag[offset] == alive)	
					continue;
				else
					update_value(offset);
			}
		}

		return mat_u;
	}

}}}
