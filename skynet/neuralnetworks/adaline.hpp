#pragma once

#include <skynet\ml\utility.hpp>
#include <skynet\utility\algorithm.hpp>
#include <skynet\utility\math.hpp>

#include <random>

namespace skynet{namespace nn{
	using ml::database;

	struct perceptron{};
	struct perceptron_margin{};
	//struct pocket_margin{};

	template <typename Tag>
	class adaline;

	template <>
	class adaline<perceptron>{
	public:
		adaline() : _eta(1.5), _iter_num(100U){}

		void eta(double v)								{ _eta = v; }
		double eta() const								{ return _eta; }

		void max_iteration_num(size_t v)					{ _iter_num = v; }
		size_t max_iteration_num() const					{ return _iter_num; }

		template <typename T>
		void learn(const database<T, bool> &data,  
			const ublas::vector<double> &vec_simple_weights)
		{
			ASSERT(is_equal(sum(vec_simple_weights), 1.0, 1e-2), 
				"the sum of the simple weights should be 1");
			ASSERT(_eta > 0, "the ¦Ç should be possitive!");

			auto ¦¶ = preprocess(data);
			ublas::vector<double> vec_update_weights(vec_simple_weights.size());
			auto max_weight = *max_element(vec_simple_weights);
			transform(vec_simple_weights, vec_update_weights, [max_weight](const double &e){
				return e/max_weight;
			});

			_W.resize(¦¶.size1());
			fill(_W, 0.1);
			std::mt19937 mt;
			mt.seed(std::rand());
			std::uniform_int<size_t> rand(0, ¦¶.size2()-1);
			double weigth = 1.0;
			for (size_t i = 0; i < _iter_num; ++i){
				auto k = rand(mt);
				auto update_weight = vec_update_weights[k];
				auto X_k = ublas::column(¦¶, k);
				if (ublas::inner_prod(X_k, _W) < 0.0)
					_W = _W + _eta*update_weight*X_k;
			}
		}

		template <typename T>
		void learn(const database<T, bool> &data){
			ublas::vector<double> weights(data.targets.size());
			fill(weights, 1.0 / weights.size());
			learn(data, weights);
		}

		template <typename E>
		bool operator()(const ublas::vector_expression<E> &pattern) const{
			ublas::vector<double> X_k(pattern().size()+1);
			X_k[0] = 1;
			std::copy(pattern().begin(), pattern().end(), X_k.begin()+1);

			return ublas::inner_prod(X_k, _W) > 0;
		}

		adaline clone() const{
			adaline temp;
			temp._iter_num = _iter_num;
			temp._eta = _eta;
			temp._W.resize(_W.size());
			copy(_W, temp._W);
			return temp;
		}

	private:
		template <typename T>
		static ublas::matrix<T> preprocess(const database<T, bool> &data){
			ASSERT(data.patterns.size2() == data.targets.size(), "The size is not matched!");

			auto ¦¶ = ublas::matrix<T>(data.patterns.size1()+1, data.patterns.size2());
			for (size_t col = 0; col < ¦¶.size2(); ++ col){
				auto pattern = ublas::column(¦¶, col);
				auto pattern0 = ublas::column(data.patterns, col);
				std::copy(pattern0.begin(), pattern0.end(), pattern.begin()+1);
			}
			fill(ublas::row(¦¶, 0), 1);

			for (size_t i = 0; i < data.targets.size(); ++i){
				if (!data.targets[i]){
					ublas::column(¦¶, i) *= -1;
				} 
			}

			return ¦¶;
		}

	private:
		ublas::vector<double>	_W;
		size_t					_iter_num;
		double                  _eta;
	};

	template <>
	class adaline<perceptron_margin>{
	public:
		adaline() : _eta(1.5), _iter_num(100U), _margin(1.0){}

		void eta(double v)								{ _eta = v; }
		double eta() const								{ return _eta; }

		void margin(double v)							{ _margin = v; }
		double margin() const							{ return _margin; }

		void max_iteration_num(size_t v)					{ _iter_num = v; }
		size_t max_iteration_num() const					{ return _iter_num; }

		template <typename T>
		void learn(const database<T, bool> &data,  
			const ublas::vector<double> &vec_simple_weights)
		{
			ASSERT(is_equal(sum(vec_simple_weights), 1.0, 1e-2), 
				"the sum of the simple weights should be 1");
			ASSERT(_eta > 0, "the ¦Ç should be possitive!");
			ASSERT(data.targets.size() == vec_simple_weights.size(), "the size is not matched");

			auto ¦¶ = preprocess(data);
			ublas::vector<double> vec_update_weights(vec_simple_weights.size());
			auto max_weight = *max_element(vec_simple_weights);
			transform(vec_simple_weights, vec_update_weights, [max_weight](const double &e){
				return e/max_weight;
			});

			_W.resize(¦¶.size1());
			fill(_W, 0.1);
			std::mt19937 mt;
			mt.seed(unsigned long(std::time(nullptr)));
			std::uniform_int<size_t> rand(0, ¦¶.size2()-1);
			double weigth = 1.0;
			for (size_t i = 0; i < _iter_num; ++i){
				auto k = rand(mt);
				auto update_weight = vec_update_weights[k];
				auto X_k = ublas::column(¦¶, k);
				if (ublas::inner_prod(X_k, _W) < _margin)
					_W = _W + _eta*update_weight*X_k;
			}
		}
				
		template <typename T>
		void learn(const database<T, bool> &data){
			ublas::vector<double> &weights(data.targets.size());
			fill(weights, 1.0 / weights.size());
			learn(data, weights);
		}

		bool operator()(const ublas::vector<double> &pattern) const{
			ublas::vector<double> X_k(pattern.size()+1);
			X_k[0] = 1;
			std::copy(pattern.begin(), pattern.end(), X_k.begin()+1);

			return ublas::inner_prod(X_k, _W) > 0;
		}

		adaline clone() const{
			adaline temp;
			temp._iter_num = _iter_num;
			temp._eta = _eta;
			temp._W.resize(_W.size());
			temp._margin = _margin;
			copy(_W, temp._W);
			return temp;
		}


	private:
		template <typename T>
		static ublas::matrix<T> preprocess(const database<T, bool> &data){
			ASSERT(data.patterns.size2() == data.targets.size(), "The size is not matched!");

			auto ¦¶ = ublas::matrix<T>(data.patterns.size1()+1, data.patterns.size2());
			for (size_t col = 0; col < ¦¶.size2(); ++ col){
				auto pattern = ublas::column(¦¶, col);
				auto pattern0 = ublas::column(data.patterns, col);
				std::copy(pattern0.begin(), pattern0.end(), pattern.begin()+1);
			}
			fill(ublas::row(¦¶, 0), 1);

			for (size_t i = 0; i < data.targets.size(); ++i){
				if (!data.targets[i]){
					ublas::column(¦¶, i) *= -1;
				} 
			}

			return ¦¶;
		}

	private:
		ublas::vector<double>	_W;
		size_t					_iter_num;
		double                  _eta;
		double					_margin;
	};


	/*template <>
	class adaline<pocket_margin>{
	public:
	///the patterns as {1.0 x1 x2 ....xn}
	adaline() : _eta(1.5), _margin(0.0){}

	void eta(double v)								{ _eta = v; }
	double eta() const								{ return _eta; }

	void margin(double v)							{ _margin = v; }
	double margin() const							{ return _margin; }

	template <typename T>
	void learn(int iter_num, const database<T, bool> &data, 
	const ublas::vector<double> &vec_simple_weights)
	{
	ASSERT(is_equal(sum(vec_simple_weights), 1.0, 1e-2), 
	"the sum of the simple weights should be 1");
	ASSERT(_eta > 0, "the ¦Ç should be possitive!");

	auto ¦¶ = preprocess(data);
	ublas::vector<double> vec_update_weights(vec_simple_weights.size());
	auto max_weight = max(vec_simple_weights);
	transform(vec_simple_weights, vec_update_weights, [max_weight](const double &e){
	return e/max_weight;
	});

	ublas::vector<double> W_k(¦¶.size1());
	fill(W_k, 0);
	_W = W_k;
	std::mt19937 mt(std::time(nullptr));
	std::uniform_int<size_t> rand(0, _vec_targets.size()-1);
	double weigth = 1.0;
	for (int i = 0; i < iter_num; ++i){
	auto k = rand(mt);
	auto update_weight = vec_update_weights[k];
	auto X_k = ublas::column(_mat_patterns, k);
	size_t runlength = 0;
	size_t maxrunlength = 0;
	if (ublas::inner_prod(X_k, W_k) > _margin){
	runlength++;
	if (runlength > maxrunlength){
	_W = W_k;
	maxrunlength = runlength;
	runlength = 0;
	}
	}
	else{
	W_k += _eta*update_weight*X;
	}
	}
	}

	bool operator()(const ublas::vector<double> &pattern){
	ublas::vector<double> X_k(pattern.size()+1);
	X_k[0] = 1;
	std::copy(pattern.begin(), pattern.end(), X_k.begin()+1);

	return ublas::inner_prod(X_k, _W);
	}

	private:
	template <typename T>
	static ublas::matrix<T> preprocess(const database<T, bool> &data){
	ASSERT(data.patters.size() == data.targets.size(), "The size is not matched!");

	ublas::matrix<T> ¦¶(data.patterns.size1()+1, data.patterns.size2());
	ublas::row(¦¶, 0).assign(1);

	for (int col = 0; col < ¦¶.size2(); ++ col){
	auto pattern = ublas::column(¦¶, col);
	auto pattern0 = ublas::column(data.patterns, col);
	std::copy(pattern0.begin(), pattern0.end(), pattern.begin()+1);
	}

	for (int i = 0; i < _vec_targets.size(); ++i){
	if (!_vec_targets[i]){
	ublas::column(¦¶, i) *= -1;
	} 
	}
	}

	private:
	ublas::vector<double>	_W;

	double					_margin;
	double                  _eta;
	};
	*/
}}