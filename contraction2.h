#ifndef LIBTENSOR_CONTRACTION2_H
#define	LIBTENSOR_CONTRACTION2_H

#include "defs.h"
#include "exception.h"
#include "dimensions.h"
#include "permutation.h"
#include "contraction2_list.h"

namespace libtensor {

/**	\brief Specifies how two tensors should be contracted
	\tparam N Order of the first %tensor (a) less the contraction degree
	\tparam M Order of the second %tensor (b) less the contraction degree
	\tparam K Contraction degree (the number of indexes over which the
		tensors are contracted)

	The contraction class provides mediation between the user of a
	contraction %tensor operation and the operation itself by providing
	convenient interfaces for both.

	The user specifies which indexes in \em a and \em b are to be contracted
	and how the uncontracted indexes should be permuted in \em c.

	Contraction operations would prefer a different interface, which
	returns an iterator over list nodes that correspond to contraction
	loops. There are two flavors of the list: fused and regular (non-fused).
	The regular list contains all contraction loops starting from those
	that correspond indexes in the resulting %tensor and ending with
	contracted indexes (those that present in \em a and \em b, but don't
	make it	to \em c). The number of nodes in the regular list equals N+M+K.
	The fused list is optimized: its nodes represent loops over combined
	indexes if those exist. The number of nodex is less or equal to N+M+K.

	For example, the contraction
	\f$ c_{ijkl} = \sum_{pq} a_{ijpq} b_{klpq} \f$ can be rewritten as
	\f$ c_{mn} = \sum_{r} a_{mr} b_{nr} \f$, where \em m represents \em i
	and \em j combined (same for \em n and \em r). The new contraction
	will only have three fused loop nodes instead of six in the original
	one.

	\ingroup libtensor
 **/
template<size_t N, size_t M, size_t K>
class contraction2 {
private:
	static const size_t k_invalid = (size_t) (-1);
	static const size_t k_ordera = N + K;
	static const size_t k_orderb = M + K;
	static const size_t k_orderc = N + M;
	static const size_t k_totidx = N + M + K;
	static const size_t k_maxconn = 2 * k_totidx;

private:
	permutation<N + K> m_permc; //!< Permutation of result indexes
	size_t m_k; //!< Number of contracted indexes specified
	size_t m_conn[k_maxconn]; //!< Index connections
	size_t m_num_nodes; //!< Number of fused nodes
	size_t m_nodes[k_totidx]; //!< Fused nodes
	size_t m_nodesz[k_totidx]; //!< Fused node sizes (weights)

public:
	//!	\name Construction and destruction
	//@{

	/**	\brief Creates the contraction object
		\param perm Specifies how argument indexes should be permuted
			in the output.
	 **/
	contraction2(const permutation<N + M> &perm);

	/**	\brief Copy constructor
	 **/
	contraction2(const contraction2<N, M, K> &contr);

	//@}

	//!	\name Contraction setup
	//@{

	/**	\brief Returns whether this contraction is complete
	 **/
	bool is_complete() const;

	/**	\brief Designates a contracted index
		\param ia Index number in the first %tensor argument (a).
		\param ib Index number in the second %tensor argument (b).
		\throw exception if index numbers are invalid or this
			contraction is complete.
	 **/
	void contract(size_t ia, size_t ib) throw (exception);

	/**	\brief Adjusts %index numbering when the arguments come in a
			permuted form

		The contraction must be specified for unpermuted arguments
		first. Calling this method on an incomplete contraction will
	 `	cause an exception.

		\param perma Permutation of the first %tensor argument (a).
		\param permb Permutation of the second %tensor argument (b).
		\throw exception if the contraction is incomplete.
	 **/
	void permute(const permutation<k_ordera> &perma,
		const permutation<k_orderb> &permb) throw (exception);

	//@}

	//!	\name Interface for the computational party
	//@{

	/**	\brief Checks the dimensions of the arguments and populates
			the loop node list
	 **/
	void populate(contraction2_list<k_totidx> &list,
		const dimensions<k_ordera> &dima,
		const dimensions<k_orderb> &dimb) const throw(exception);

	/**	\brief Returns the number of contraction loop nodes
	 **/
	size_t get_num_nodes() const throw (exception);

	/**	\brief Returns the weight of a contraction loop node
		\param node Node number
		\param dima Dimensions of %tensor a
		\param dimb Dimensions of %tensor b
	 **/
	size_t get_weight(size_t node, const dimensions<N + K> &dima,
		const dimensions<M + K> &dimb) const throw (exception);

	/**	\brief Returns the linear %index increment in a
			(first argument)
		\param dima Dimensions of %tensor a
		\param dimb Dimensions of %tensor b
	 **/
	size_t get_increment_a(const dimensions<N + K> &dima,
		const dimensions<M + K> &dimb) const throw (exception);

	/**	\brief Returns the linear %index increment in b
			(second argument)
		\param dima Dimensions of %tensor a
		\param dimb Dimensions of %tensor b
	 **/
	size_t get_increment_b(const dimensions<N + K> &dima,
		const dimensions<M + K> &dimb) const throw (exception);

	/**	\brief Returns the linear %index increment in c (result)
		\param dima Dimensions of %tensor a
		\param dimb Dimensions of %tensor b
	 **/
	size_t get_increment_c(const dimensions<N + K> &dima,
		const dimensions<M + K> &dimb) const throw (exception);

	//@}

private:
	/**	\brief Fuses the indexes
	 **/
	void fuse();
};

template<size_t N, size_t M, size_t K>
contraction2<N, M, K>::contraction2(const permutation<N + M> &perm) :
m_permc(perm), m_k(0), m_num_nodes(0) {
	for(size_t i = 0; i < k_maxconn; i++) {
		m_conn[i] = k_invalid;
	}
	for(size_t i = 0; i < k_totidx; i++) {
		m_nodes[i] = 0;
		m_nodesz[i] = 0;
	}
}

template<size_t N, size_t M, size_t K>
contraction2<N, M, K>::contraction2(const contraction2<N, M, K> &contr) :
m_permc(contr.m_permc), m_k(contr.m_k), m_num_nodes(contr.m_num_nodes) {
	for(size_t i = 0; i < k_maxconn; i++) {
		m_conn[i] = contr.m_conn[i];
	}
	for(size_t i = 0; i < k_totidx; i++) {
		m_nodes[i] = contr.m_nodes[i];
		m_nodesz[i] = contr.m_nodesz[i];
	}
}

template<size_t N, size_t M, size_t K>
inline bool contraction2<N, M, K>::is_complete() const {
	return m_k == K;
}

template<size_t N, size_t M, size_t K>
void contraction2<N, M, K>::contract(size_t ia, size_t ib) throw (exception) {
	if(is_complete()) {
		throw_exc("contraction2<N, M, K>", "contract()",
			"Contraction is complete");
	}
	if(ia >= k_ordera) {
		throw_exc("contraction2<N, M, K>", "contract()",
			"Contraction index ia is invalid");
	}
	if(ib >= k_orderb) {
		throw_exc("contraction2<N, M, K>", "contract()",
			"Contract"
			"ion index ib is invalid");
	}

	size_t ja = k_orderc + ia;
	size_t jb = k_orderc + k_ordera + ib;

	if(m_conn[ja] != k_invalid) {
		throw_exc("contraction2<N, M, K>", "contract()",
			"Index ia is already contracted");
	}
	if(m_conn[jb] != k_invalid) {
		throw_exc("contraction2<N, M, K>", "contract()",
			"Index ib is already contracted");
	}

	m_conn[ja] = jb;
	m_conn[jb] = ja;

	if(++m_k == K) {
		// Once contracted indexes are specified, collect all the
		// remaining ones, permute them properly, and put them in place
		size_t connc[k_orderc];
		size_t iconnc = 0;
		for(size_t i = k_orderc; i < k_maxconn; i++)
			if(m_conn[i] == k_invalid) connc[iconnc++] = i;
		m_permc.apply(k_orderc, connc);
		for(size_t i = 0; i < k_orderc; i++) {
			m_conn[i] = connc[i];
			m_conn[connc[i]] = i;
		}
		fuse();
	}
}

template<size_t N, size_t M, size_t K>
inline size_t contraction2<N, M, K>::get_num_nodes() const throw (exception) {
	if(!is_complete()) {
		throw_exc("contraction2<N, M, K>", "get_num_nodes()",
			"Contraction is incomplete");
	}
	return m_num_nodes;
}

template<size_t N, size_t M, size_t K>
size_t contraction2<N, M, K>::get_weight(size_t node,
	const dimensions<N + K> &dima, const dimensions<M + K> &dimb) const
throw (exception) {

	if(!is_complete()) {
		throw_exc("contraction2<N, M, K>", "get_weight()",
			"Contraction is incomplete");
	}
	if(node >= m_num_nodes) {
		throw_exc("contraction2<N, M, K>", "get_weight()",
			"Node number is out of bounds");
	}

	// Here the index is from c or a
	size_t i = m_nodes[node];
	if(i < N + M) i = m_conn[i]; // For indexes in c get the connected idx

	size_t w = 1;

	// Now the index is from a or b
	if(i < 2 * N + M + K) {
		// The index is from a
		for(size_t j = 0; j < m_nodesz[node]; j++)
			w *= dima[i + j - (N + M)];
	} else {
		// The index is from b
		for(size_t j = 0; j < m_nodesz[node]; j++)
			w *= dimb[i + j - (2 * N + M + K)];
	}

	return w;
}

template<size_t N, size_t M, size_t K>
void contraction2<N, M, K>::fuse() {
	size_t i = 0;
	// Take care of indexes in result
	while(i < N + M) {
		size_t ngrp = 1;
		while(m_conn[i + ngrp] == m_conn[i] + ngrp &&
			i + ngrp < N + M) ngrp++;
		m_nodes[m_num_nodes] = i;
		m_nodesz[m_num_nodes] = ngrp;
		m_num_nodes++;
		i += ngrp;
	}
	// Take care of contracted indexes
	while(i < 2 * N + M + K) {
		size_t ngrp = 1;
		if(m_conn[i] > i) {
			while(m_conn[i + ngrp] == m_conn[i] + ngrp &&
				i + ngrp < 2 * N + M + K) ngrp++;
			m_nodes[m_num_nodes] = i;
			m_nodesz[m_num_nodes] = ngrp;
			m_num_nodes++;
		}
		i += ngrp;
	}
}

} // namespace libtensor

#endif // LIBTENSOR_CONTRACTION2_H
