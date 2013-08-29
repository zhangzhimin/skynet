#pragma once

#include <skynet/ublas.hpp>
#include <skynet/utility/tag.hpp>
#include <skynet/utility/algorithm.hpp>
#include <skynet/utility/math.hpp>

#include <random>

namespace skynet{namespace ml{

	template <typename T1, typename T2>
	struct database{
		typedef ublas::matrix<T1>						pattern_type;
		typedef ublas::vector<T2>						target_type;

		pattern_type									patterns;
		target_type										targets;
	};

	template <typename T1, typename T2>
	struct database2{
		typedef ublas::matrix<T1>						pattern_type;
		typedef ublas::matrix<T2>						target_type;

		pattern_type									patterns;
		target_type										targets;
	};


	template <typename Classifier, typename DataBase>
	double misclassification_rate(const Classifier &classifier, const DataBase &data){
		typedef DataBase::pattern_type					pattern_type;
		typedef DataBase::target_type					target_type;

		ASSERT(data.patterns.size2() == data.targets.size(), "The size is not match");
		target_type										test_targets(data.targets.size());
		for (size_t i = 0; i < data.targets.size(); ++i){
			test_targets[i] = classifier(ublas::column(data.patterns, i));
		}

		size_t count = 0;
		for (size_t i = 0; i < data.targets.size(); ++i){
			if (test_targets[i] != data.targets[i])	
				count++;
		}

		return double(count) / data.targets.size();
	};


	template <typename Classifier>
	double misclassification_rate(const Classifier &classifier, const database<double, double> &data){
		typedef database<double, double>::pattern_type					pattern_type;
		typedef database<double, double>::target_type					target_type;

		ASSERT(data.patterns.size2() == data.targets.size(), "The size is not match");
		target_type										test_targets(data.targets.size());
		for (size_t i = 0; i < data.targets.size(); ++i){
			test_targets[i] = classifier(ublas::column(data.patterns, i));
		}

		size_t count = 0;
		for (size_t i = 0; i < data.targets.size(); ++i){
			if (sign(test_targets[i]) != sign(data.targets[i]))	
				count++;
		}

		return double(count) / data.targets.size();
	};


	//template <typename T>
	//class classifier_test_data;

	//
	//template <>
	//class classifier_test_data<linear>{
	//public:
	//	classifier_test_data() : _W(3){
	//		_W[0] = 0;
	//		_W[1] = 1;
	//		_W[2] = -1;
	//	}

	//	void weights(const ublas::vector<double> &W)	{ _W = W; }
	//	ublas::vector<double> weights() const			{ return _W; }

	//	database<double, bool>	samples(size_t count)	const{
	//		std::uniform_real_distribution<double> uniform(0.0, 1.0);
	//		std::mt19937 mt(std::rand());
	//				
	//		database<double, bool> data;
	//		data.patterns = ublas::matrix<double>(_W.size()-1, count);
	//		data.targets = ublas::vector<bool>(count);
	//		for (size_t i = 0; i < count; ++i){
	//			auto col = ublas::column(data.patterns, i);
	//			std::generate(col.begin(), col.end(), [&](){ return uniform(mt); });
	//			ublas::vector<double> temp(_W.size());
	//			temp[0] = 1;
	//			std::copy(col.begin(), col.end(), temp.begin() + 1);
	//			data.targets[i] = ublas::inner_prod(temp, _W) > 0;
	//		}

	//		return data;
	//	}

	//private:
	//	ublas::vector<double>							_W;
	//};

	
	/*database<double, double> bipolarize(const database<double, double> &data){
		database<double, double> temp;
		temp.targets.resize(data.targets.size());
		temp.patterns.resize(data.patterns.size1(), data.patterns.size2());
		auto min_max1 = std::minmax_element(data.targets.begin(), data.targets.end());
		auto scaler1 = bipolar_function<double>(*min_max1.first, *min_max1.second);
		transform(data.targets, temp.targets, scaler1);
		for (size_t i = 0; i < data.patterns.size1(); ++i){
			auto pattern = ublas::row(data.patterns, i);
			auto min_max_it = std::minmax_element(pattern.begin(), pattern.end());
			auto scaler = bipolar_function<double>(*min_max_it.first, *min_max_it.second);
			transform(pattern, ublas::row(temp.patterns, i), scaler);
		}

		return temp;
	}*/


	template <typename M>
	ublas::vector<typename M::value_type> multarray2vector(const M &source){
		
	}
}}
