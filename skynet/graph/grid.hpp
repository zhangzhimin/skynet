#pragma once

#include <skynet/core/matrix.hpp>
#include <skynet/core/neighbor.hpp>
#include <skynet/utility/iterator_facade.hpp>

#include <algorithm>
#include <array>
#include <type_traits>


#pragma message("the graph algorithm should not be depend on the graph struct.")
#pragma message("if non_grid graph is needed, think about the boost::graph lib.")
#pragma message("The trasfer is not safe, please set the boundary for the grid when use it(for performance)")

namespace skynet { namespace graph{

	namespace detail{

		template <typename G>
		class adjacency_vertices{
		public:
			typedef  adjacency_vertices							type;
			typedef  G											graphy_type;
			typedef typename graphy_type::adjacency_type		adjacency_type;

			typedef size_t										value_type;
			typedef size_t  									reference;
			typedef size_t  									const_reference;
			typedef index_iterator<const type>					iterator;
			typedef index_iterator<const type>					const_iterator;

			adjacency_vertices(const adjacency_type &adjacency, const size_t &source)
				: m_adjacency(adjacency), m_source(source) {}

			size_t operator[](const size_t &i) const { return m_source + m_adjacency[i]; }

			const_iterator begin() const            { return const_iterator(const_cast<type *>(this), 0); }
			const_iterator end() const              { return const_iterator(const_cast<type *>(this), size()); }

			size_t size() const { return m_adjacency.size(); }

		private:
			const size_t &m_source;
			const adjacency_type &m_adjacency;
		};
	}

	/* grid as following(but not only):
	*↔*↔*
	↕  ↕  ↕
	*↔*↔*
	↕  ↕  ↕
	*↔*↔*
	*/ 

	template <typename M, typename N = diamand_neighbor<M::dim>>
	class lattice_graph{
	public:
		//  static_assert( std::is_base_of<matrix_express<M>, M>::value, "the M should be matrix_epress.");
		typedef lattice_graph									type;
		typedef lattice_graph									graphy_type;
		typedef M												matrix_type;
		typedef N												neighbor_type;
		typedef N												adjacency_type;
		typedef typename M::value_type							capability_type;
		typedef typename M::value_type							cost_type;
		typedef typename M::reference							capability_reference;
		typedef typename M::const_reference						capability_const_reference;
		typedef typename M::extent_type							extent_type;
		typedef detail::adjacency_vertices<type>				adjacency_vertices;
		static const size_t dim = M::dim;
	public:

		lattice_graph(const matrix_type &mat) : m_mat(mat),  m_neighbors(mat.extent()){ 
		}

		adjacency_vertices adjacency_vertices_at(const size_t &base) const{
			return adjacency_vertices(m_neighbors, base);
		}

		capability_const_reference edge_at(const size_t &fir, const size_t &sec) const{
			return m_mat[sec];
		}
		capability_reference edge_at(const size_t &fir, const size_t &sec){
			return m_mat[sec];
		}

		extent_type extent() const          { return m_mat.extent(); }

	private:
		const neighbor_type               m_neighbors;
		matrix_type                       m_mat;
	};

	template <typename M>
	const lattice_graph<M> make_lattice_graph(const M &mat)
	{
		return lattice_graph<M>(mat);
	}

	template <typename M, typename N>
	const lattice_graph<M, N> make_lattice_graph(const M &mat, N n){
		return lattice_graph<M, N>(mat);
	}
}}
