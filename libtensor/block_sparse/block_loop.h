/*
 * block_loop_new.h
 *
 *  Created on: Nov 13, 2013
 *      Author: smanzer
 */

#ifndef BLOCK_LOOP_H_
#define BLOCK_LOOP_H_

#include <map>
#include <vector>
#include "sparse_bispace.h"

namespace libtensor
{

class block_loop
{
private:
    static const char* k_clazz; //!< Class name
    std::vector<size_t> m_subspaces_looped; //!< What subspaces of what bispaces does this loop span?
    std::vector<bool> m_bispaces_ignored; //!< What bispaces are completely ignored by this loop?
#ifdef LIBTENSOR_DEBUG
    std::vector< sparse_bispace_any_order > m_bispaces; //!< What bispaces does this loop access
#endif
public:
	block_loop(const std::vector< sparse_bispace_any_order >& bispaces);

	void set_subspace_looped(size_t bispace_idx,size_t subspace_idx);
	size_t get_subspace_looped(size_t bispace_idx) const;

	bool is_bispace_ignored(size_t bispace_idx) const;

#ifdef LIBTENSOR_DEBUG
    const std::vector< sparse_bispace_any_order >& get_bispaces() const { return m_bispaces; }
#endif
};

} /* namespace libtensor */

#endif /* BLOCK_LOOP_H_ */
