/*=============================================================================
The MIT License (MIT)

Copyright @ 2013 by Zhang Zhimin 
p3.1415@qq.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
=============================================================================*/

#pragma once

#include <algorithm>
#include <random>

#include <skynet\utility\algorithm.hpp>
#include <skynet\ml\utility.hpp>


namespace skynet{namespace ml{


	class tsf_classifier{
	public:
		tsf_classifier(): _error(numeric_limits<double>::infinity()){}

		tsf_classifier(size_t feature_index)
			:_feature_index(feature_index), _error(numeric_limits<double>::infinity()){}

		tsf_classifier(const tsf_classifier &rhs) 
			: _feature_index(rhs._feature_index), _p(rhs._p), _theta(rhs._theta), _error(rhs._error){}

		tsf_classifier &operator=(const tsf_classifier &rhs){
			_p = rhs._p;
			_theta = rhs._theta;
			_error = rhs._error;
			_feature_index = rhs._feature_index;

			return *this;
		}

		double error() const{
			return _error;
		}		

		void feature_index(size_t v)							{ _feature_index = v; }
		size_t feature_index() const							{ return _feature_index; }

		void learn(const database<double, bool> &data, const ublas::vector<double> &sample_weights){
			ASSERT(data.patterns.size2() == data.targets.size(), " ");
			ASSERT(is_equal(sum(sample_weights), 1.0, 1e-2), " ");
			ASSERT(_feature_index < data.patterns.size1(), " ");

			auto patterns = data.patterns;
			auto targets = data.targets;

			std::vector<size_t> sort_index(targets.size());
			for (size_t i = 0; i < sort_index.size(); ++i){
				sort_index[i] = i;
			}

			std::sort(sort_index.begin(), sort_index.end(), [&](size_t lhs, size_t rhs)->bool{
				return patterns(_feature_index, lhs) < patterns(_feature_index, rhs);
			});

			std::vector<double> cum_positive(targets.size());
			std::vector<double> cum_negative(targets.size());
			double	sum_positive = 0; 
			double	sum_negative = 0;
			for (size_t i = 0; i < sort_index.size(); ++i){
				auto cur_index = sort_index[i];
				if (targets[cur_index]){
					sum_positive += sample_weights[cur_index];
				}else{
					sum_negative += sample_weights[cur_index];
				}

				cum_positive[i] = sum_positive;
				cum_negative[i] = sum_negative;
			}

			ASSERT(sum_positive!= 0 && sum_negative != 0, "");

			auto min_error = numeric_limits<double>::max();
			for (size_t i = 0; i < targets.size(); ++i){
				//
				auto error1 = cum_negative[i] + sum_positive - cum_positive[i];  //true  / false
				auto error2 = cum_positive[i] + sum_negative - cum_negative[i];  //false / true

				if (error1 < min_error){
					min_error = error1;
					_theta = patterns(_feature_index, sort_index[i]);  //  true > : false
					_p = true;
				}
				if (error2 < min_error){
					min_error = error2;
					_theta = patterns(_feature_index, sort_index[i]);
					_p =false;									// false > : true
				}
			}

			_error = min_error;
		}

		void learn(const database<double, bool> &data){
			auto size = data.patterns.size2();
			ublas::vector<double> sample_weights(size);
			fill(sample_weights, 1.0/size);
			learn(data, sample_weights);
		}

		template <typename T>
		bool operator()(const ublas::vector_expression<T> & input) const {
			if (_p){
				if (input()[_feature_index] > _theta)
					return false;
				else
					return true;
			}else{
				if (input()[_feature_index] > _theta)
					return true;
				else
					return false;
			}
		}

		tsf_classifier clone() const {
			tsf_classifier sub;
			sub._p = _p;
			sub._theta = _theta;
			sub._error = _error;
			sub._feature_index = _feature_index;

			return sub;
		}

	private:

		double	_theta;
		bool	_p;

		double _error;

		size_t _feature_index;
	};


	class best_tsf_classifier{
	public:
		void tsfc_prototype(const tsf_classifier &v)	{ _tsfc_prototype = v.clone(); }
		tsf_classifier tsfc_prototype() const			{ return _tsfc_prototype; }

		void learn(const database<double, bool> &data, const ublas::vector<double> &sample_weights){
			ASSERT(data.patterns.size2() == data.targets.size(), " ");
			ASSERT(is_equal(sum(sample_weights), 1.0, 1e-2), " ");

			std::vector<tsf_classifier> classifiers(data.patterns.size1());
			for (size_t i = 0; i < classifiers.size(); ++i){
				classifiers[i] = _tsfc_prototype.clone();
				classifiers[i].feature_index(i);
				classifiers[i].learn(data, sample_weights);
			}

			auto min_it = std::min_element(classifiers.begin(), classifiers.end(), 
				[&](const tsf_classifier &lhs, const tsf_classifier &rhs){
					return lhs.error() < rhs.error();
			});

			_best_classifier = classifiers[min_it - classifiers.begin()];
		}

		void learn(const database<double, bool> &data){
			auto size = data.patterns.size2();
			ublas::vector<double> sample_weights(size);
			fill(sample_weights, 1.0/size);
			learn(data, sample_weights);
		}

		template <typename T>
		bool operator()(const ublas::vector_expression<T> & input) const {
			return _best_classifier(input);
		}

		best_tsf_classifier clone() const{
			return *this;
		}

	private:
		tsf_classifier	_tsfc_prototype;
		tsf_classifier	_best_classifier;
	};


	template <typename WeakClassifer>
	class adaboost{
	public:
		typedef WeakClassifer							weak_classifier;

		adaboost() : _classifier_size(10){}

		void classifier_size(size_t v)					{ _classifier_size = v; }
		size_t classifier_size() const					{ return _classifier_size; } 

		weak_classifier classifier_prototype() const					{ return _master_classifier; }
		void classifier_prototype(const weak_classifier &classifier) { _master_classifier = classifier; }

		template <typename T>
		void learn(const database<T, bool> &data,
			ublas::vector<double> vec_sample_weights)
		{
			ASSERT(data.patterns.size2() == data.targets.size(), "");

			_weak_classifiers.clear();
			_¦Á.clear();
			for (size_t k = 0; k < classifier_size(); ++k){
				auto classifier = _master_classifier.clone();
				auto sum = ublas::sum(vec_sample_weights);
				for (auto &e:vec_sample_weights){
					e /= sum;
				}

				classifier.learn(data, vec_sample_weights);

				double eps = 0.0;
				ublas::vector<bool> re(data.targets.size());
				for (size_t i = 0; i < data.patterns.size2(); ++i){
					re[i] = classifier(ublas::column(data.patterns, i));
					if (re[i] != (data.targets)[i]){
						eps += vec_sample_weights[i];
					}
				}
#ifdef _CONSOLE
				std::cout << k << "Adaboost Classifier error : " << eps << std::endl;
#endif
				if (eps >= 0.5)							continue;

				auto _¦Ák = 0.5*ln((1.0-eps) / (eps+1e-4));
				for (size_t i = 0; i < vec_sample_weights.size(); ++i){
					if ((data.targets)[i] != re[i])
						vec_sample_weights[i] *= exp(_¦Ák);
					else
						vec_sample_weights[i] *= exp(-_¦Ák);
				}
				_weak_classifiers.push_back(classifier);
				_¦Á.push_back(_¦Ák);
			}
		}

		template <typename T>
		void learn(const database<T, bool> &data){
			ublas::vector<double> weights(data.targets.size());
			fill(weights, 1.0 / weights.size());
			learn(data, weights);
		}

		template <typename E>
		bool operator()(const ublas::vector_expression<E> &ve) const{
			double sign = 0;
			for (size_t i = 0; i < _weak_classifiers.size(); ++i){
				if (_weak_classifiers[i](ve)){
					sign += _¦Á[i];
				}else{
					sign -= _¦Á[i];
				}
			}

			return sign > 0;
		}

	private:
		std::vector<weak_classifier>					_weak_classifiers;
		size_t											_classifier_size;
		weak_classifier									_master_classifier;
		std::vector<double>								_¦Á;
	};


}}