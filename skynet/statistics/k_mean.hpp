/******************************************************************************
Created By : Zhang Zhimin
Created On : 2013/4/9
Purpose    :  
********************************************************************************/

#pragma once

#include <skynet/config.hpp>
#include <skynet/core/point.hpp>
#include <random>

namespace skynet{ namespace statistics{

	using skynet::distance;

	template <typename C1, typename C2>
	std::vector<typename C1::value_type> calculate_centeroids(const C1 &dates, const C2 &labels, size_t clusters_num){
		typedef typename C1::value_type point_type;
		//avoid div 0;
		std::vector<size_t>  count(clusters_num, 1);
		std::vector<point_type> clusters(clusters_num);
		std::fill(clusters.begin(), clusters.end(), point_type(0));

		for (size_t i = 0; i < dates.size(); ++i){
			auto label = labels[i];
			clusters[label] += dates[i];
			++count[label];
		}

		for (size_t c = 0; c < clusters_num; ++c){
			clusters[c] /= count[c];
		}

		return clusters;
	}


	template <typename C1, typename C2, typename Dis>
	bool reassign_clusters(const C1 &dates, C2 &labels, const std::vector<typename C1::value_type> &clusters, Dis dis){
		typedef typename C1::value_type  point_type;

		ASSERT(dates.size() == labels.size(), "The sizes are not same");
		bool changed = false;
		for (size_t i = 0; i < labels.size(); ++i){
			auto min_it = std::min_element(clusters.begin(), clusters.end(), 
				[&](const point_type &lhs, const point_type &rhs){
					return dis(lhs, dates[i]) < dis(rhs, dates[i]);
			});

			if (labels[i] != (min_it - clusters.begin())){
				changed = true;
				labels[i] = min_it - clusters.begin();
			}
		}

		return changed;
	}
	
	template <typename C, typename C2, typename Dis>
	bool k_mean(const C &dates, C2 &labels, std::vector<typename C::value_type> &clusters,  Dis dis, size_t max_iter){
		typedef typename C::value_type		point_type;
		ASSERT(labels.size() == dates.size(), "The size of the input and output are not same.");

		auto clusters_num = clusters.size();
		std::uniform_int_distribution<byte> label_random(0, clusters_num-1);
		std::mt19937_64 engine;
		engine.seed(time(nullptr));

		std::generate(labels.begin(), labels.end(), [&](){ return label_random(engine); });

		for (size_t i = 0; i < max_iter; ++i){
			clusters = std::move(calculate_centeroids(dates, labels, clusters_num));
			auto re = reassign_clusters(dates, labels, clusters, dis);

			if (!re)	return true;;			
		}

		return false;
	}

	template <typename C1, typename C2>
	bool k_mean(const C1 &samples, C2 &labels, std::vector<typename C1::value_type> &clusters, size_t max_iter = 100){
		return k_mean(samples, labels, clusters, &distance<typename C1::value_type>, max_iter);
	}
		
}}
