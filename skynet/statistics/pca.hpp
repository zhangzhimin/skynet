#pragma once

#include <skynet/config.hpp>

#include <Eigen/SVD>
#include <Eigen/Core>

namespace skynet{ namespace statistics{
	

	struct pca_result{
		Eigen::MatrixXd    eigen_vectors;
		Eigen::VectorXd    eigen_values;
		Eigen::VectorXd    mean;
	};

	//mxn  matrix  warning:  m is the num of samples
	pca_result pca(const Eigen::MatrixXd &mat){
		Eigen::VectorXd mean(mat.rows());
		for (int row = 0; row < mat.rows(); ++row){
			double temp = 0;
			for (int col = 0; col < mat.cols(); ++col){
				temp += mat(row, col);
			}
			temp /= mat.cols();
			mean(row) = temp;
		}

		Eigen::MatrixXd data(mat.rows(), mat.cols());
		for (int row = 0; row < mat.rows(); ++row){
			for (int col = 0; col < mat.cols(); ++col){
				data(row, col) = mat(row, col) - mean(row);
			}
		}

		//auto  data_t = data.transpose() / sqrt(mat.rows());
		Eigen::JacobiSVD<Eigen::MatrixXd>  svd(data, Eigen::ComputeFullU | Eigen::ComputeFullV);
		pca_result pca;
		pca.eigen_values = move(svd.singularValues().cwiseAbs2());
		pca.eigen_vectors = move(svd.matrixU());
		pca.mean = mean;
		
		return pca;
	//	auto  sample_size = mat.row
	}

}}
