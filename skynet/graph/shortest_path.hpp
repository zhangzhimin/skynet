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

#include <list>
#include <forward_list>
#include <numeric>

#include <skynet\core\array.hpp>
#include <skynet\utility\algorithm.hpp>
#include <skynet\core\common.hpp>
#include <skynet\utility\std_expand.hpp>


#pragma message("using the forward_list take place of the list")
#pragma message("for the performance, the dijkstra algorithm could be break off")

namespace skynet{ namespace graph{

    template <typename G>
    std::vector<size_t> dijkstra(const G &graph, const size_t &source, const size_t &target){
        static_assert(std::is_integral<typename G::cost_type>::value, "the cost type must be integral");

		auto extent = graph.extent();
        matrix<byte, G::dim>  mat_accessed_flag(extent);
        mat_accessed_flag.set_all_zero();
        skynet::set_boundary(mat_accessed_flag, 2, 1);
        matrix<size_t, G::dim> mat_energy_cumsum(extent);
        mat_energy_cumsum.set_all_zero();

        if (mat_accessed_flag[source] == 2 || mat_accessed_flag[target] == 2){
            LOG_ERROR("The source or target is outside.");
            return std::vector<size_t>();
        }

		auto heap = make_priority_queue([&mat_energy_cumsum](const size_t &lhs, const size_t &rhs){
			return mat_energy_cumsum[lhs] > mat_energy_cumsum[rhs];
		});
		heap.push(source);
        mat_energy_cumsum[source] = 0;
		matrix<size_t, G::dim> mat_prepos(extent);

        while (!heap.empty()){
            auto root = heap.top();
            heap.pop();
            if (mat_accessed_flag[root] == 2)
                continue;
            else
                mat_accessed_flag[root] = 2;
            if (root == target) break;

            auto adj_vertices = graph.adjacency_vertices_at(root);
            for (auto it = adj_vertices.begin(); it != adj_vertices.end(); ++it){
                if (mat_accessed_flag[*it] == 2)     continue;

                if (mat_accessed_flag[*it] == 0){
                    mat_accessed_flag[*it] = 1;
                    auto cur_energy = mat_energy_cumsum[root] + graph.edge_at(root, *it);
                    mat_energy_cumsum[*it] = cur_energy;
					
                    heap.push(*it);
					mat_prepos[*it] = root;
                }
                else{
                    auto cur_energy = mat_energy_cumsum[root] + graph.edge_at(root, *it);
                    if (cur_energy >= mat_energy_cumsum[*it]) continue;

                    mat_energy_cumsum[*it] = cur_energy;
					heap.push(*it);
					mat_prepos[*it] = root;
                }
            }
        }

		std::vector<size_t> path;
        if (mat_accessed_flag[target] != 2) return path;

		path.reserve(std::accumulate(extent.begin(), extent.end(), 0));
        path.push_back(target);
        while (path.back() != source){
           path.push_back(mat_prepos[path.back()]);
        }

        return path;
    }

}}