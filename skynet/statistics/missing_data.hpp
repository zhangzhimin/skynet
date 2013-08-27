#pragma once

#include <skynet\config.hpp>

#include <Eigen\Core>

namespace skynet{namespace statistics{
	namespace detail{

		pair<Eigen::MatrixXd, Eigen::MatrixXd>	get_missing_data_covariance_sub_matrixes(
			const Eigen::MatrixXd &mat, const std::vector<size_t> &missing_indexes)
		{
			ASSERT(mat.cols() == mat.rows(), "The rols is not equal to cols!.");

			std::vector<size_t> observed_indexes;
			for (int i = 0, j = 0; (i <mat.rows()) && (j < missing_indexes.size()); ++i){
				if (i != missing_indexes[j]){
					observed_indexes.push_back(i);
				}
				else{
					++j;
				}
			}

			Eigen::MatrixXd Sigma_oo(observed_indexes.size(), observed_indexes.size());
			for (int i = 0; i < observed_indexes.size(); ++i){
				for (int j = 0; j < observed_indexes.size(); ++j){
					Sigma_oo(i, j) = mat(observed_indexes[i], observed_indexes[j]);
				}
			}

			Eigen::MatrixXd Sigma_mo(missing_indexes.size(), observed_indexes.size());
			for (int i = 0; i < missing_indexes.size(); ++i){
				for (int j = 0; j < observed_indexes.size(); ++j){
					Sigma_mo(i,j) = mat(missing_indexes[i], observed_indexes[j]);
				}
			}

			return  make_pair(Sigma_oo, Sigma_mo);
		}

		pair<Eigen::VectorXd, Eigen::VectorXd> get_missing_data_means(
			const Eigen::MatrixXd &mat, const std::vector<size_t> &missing_indexes)
		{
			std::vector<size_t> observed_indexes;
			for (int i = 0, j = 0; (i <mat.rows()) && (j < missing_indexes.size()); ++i){
				if (i != missing_indexes[j]){
					observed_indexes.push_back(i);
				}
				else{
					++j;
				}
			}

			Eigen::VectorXd mean_o(observed_indexes.size());
			for (int i = 0; i < observed_indexes.size(); ++i){
				mean_o[i] = mat.row(observed_indexes[i]).mean();
			}

			Eigen::VectorXd mean_m(missing_indexes.size());
			for (int i = 0; i < missing_indexes.size(); ++i){
				mean_m[i] = mat.row(missing_indexes[i]).mean();
			}

			return make_pair(mean_o, mean_m);
		}

		template <typename V>
		Eigen::VectorXd get_observed_values(const V &vec, const std::vector<size_t> &missing_indexes){
			std::vector<size_t> observed_indexes;
			for (int i = 0, j = 0; (i <vec.size()) && (j < missing_indexes.size()); ++i){
				if (i != missing_indexes[j]){
					observed_indexes.push_back(i);
				}
				else{
					++j;
				}
			}

			Eigen::VectorXd observed_values(observed_indexes.size());
			for (int i = 0; i < observed_indexes.size(); ++i){
				observed_values[i] = vec[observed_indexes[i]];
			}

			return observed_values;
		}

	}

	class missing_data_EM{
	public:
		missing_data_EM(const  Eigen::MatrixXd &mat_data, const Eigen::Matrix<byte, -1, -1, 0,-1,-1> &mat_observed)
			: _mat_data(mat_data), _mat_observed(mat_observed)
		{
			ASSERT(mat_data.cols() == mat_observed.cols() && mat_data.rows() == mat_observed.rows(),
				"The size not match!");
			_missing_indexes_vec.resize(mat_data.cols());

			for (int col = 0; col < mat_observed.cols(); ++col){
				for (int row = 0; row < mat_observed.rows(); ++row){
					if (!mat_observed(row, col)){
						_missing_indexes_vec[col].push_back(row);
					}
				}
			}
		}

		Eigen::MatrixXd em(size_t  iter_num){
			for (int iter = 0; iter < iter_num; ++iter){
				refresh_covariance_matrix();
				for (size_t  sample_i = 0; sample_i < _mat_data.cols(); ++sample_i){
					auto missing_indexes = _missing_indexes_vec[sample_i];

					if (missing_indexes.empty()) continue;

					auto sub_matrixes = 
						detail::get_missing_data_covariance_sub_matrixes(_mat_covariance, missing_indexes);
					auto means = detail::get_missing_data_means(_mat_data, missing_indexes);

					auto y_o = detail::get_observed_values(_mat_data.col(sample_i), missing_indexes);
					auto y_m = means.second + 
						sub_matrixes.second * sub_matrixes.first.inverse() * (y_o - means.first);

					for (int i = 0; i < missing_indexes.size(); ++i){
						_mat_data.col(sample_i)[missing_indexes[i]] = y_m[i];
					}
				}
			}

			return _mat_data;
		}

	protected:

		void refresh_covariance_matrix(){
			_mat_covariance = _mat_data * _mat_data.transpose();
			_mat_covariance /= _mat_covariance.cols();
		}

	private:
		Eigen::MatrixXd								_mat_data;
		Eigen::Matrix<byte, -1, -1, 0, -1, -1>		_mat_observed;
		Eigen::MatrixXd								_mat_covariance;

		std::vector<std::vector<size_t>>	_missing_indexes_vec;
	};

}}