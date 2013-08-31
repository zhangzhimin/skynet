#pragma once

#include <boost/numeric/ublas/fwd.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/lu.hpp>
#include <boost/numeric/ublas/symmetric.hpp>

#include <skynet/utility/algorithm.hpp>

namespace skynet{

	namespace ublas = boost::numeric::ublas;


	using ublas::prod;
	using ublas::inner_prod;
	using ublas::trans;
	using ublas::row;
	using ublas::column;


	typedef ublas::vector<double>   vectord;
	typedef ublas::matrix<double>   matrixd;

	//namespace boost{namespace numeric{namespace ublas{

	//	

	////	template <typename T>
	////	matrix<T> inverse(const matrix<T>& A) {
	////		typedef permutation_matrix<size_t> pmatrix;

	////		pmatrix pm(A.size1());
	////		// perform LU-factorization

	////		matrix<T> A_LU(A.size1(), A.size2());
	////		A_LU.assign(A);
	////		int res = lu_factorize(A_LU,pm);
	////		if( res != 0 ){
	////			THROW_EXCEPTION(std::runtime_error("The matrix is singular."));
	////		}

	////		// create identity matrix of "inverse"
	////		matrix<T> mat_inv(A.size1(), A.size2());
	////		mat_inv.assign(ublas::identity_matrix<T>(A.size1()));

	////		// backsubstitute to get the inverse
	////		lu_substitute(A_LU, pm, mat_inv);

	////		return mat_inv;
	////	}

	//}}}

	template <typename E>
	std::ostream& operator<<(std::ostream &os, const ublas::vector_expression<E> &ve){
		os << "[ ";
		for (size_t i = 0; i < ve().size(); ++i){
			os << ve()[i] << " ";
		}
		os <<"]";
		return os;
	}

}

namespace boost{namespace numeric{namespace ublas{


	template <typename V>
	typename enable_if<is_floating_point<typename V::value_type>, vector<typename V::value_type>>::type
		normalize(const vector_expression<V> &v_e)
	{
		typedef typename V::value_type			value_type;
		auto norm = norm_2(v_e());
		if(norm == 0){
			norm += numeric_limits<value_type>::epsilon();
		}

		vector<value_type> temp(v_e().size());
		skynet::transform(v_e(), temp, [norm](value_type e){
			return e/norm;
		});

		return temp;
	}


}}}
