/*
 * sparse_loop_list.cpp
 *
 *  Created on: Nov 13, 2013
 *      Author: smanzer
 */

#include "sparse_loop_list.h"

using namespace std;

namespace libtensor
{

const char* sparse_loop_list::k_clazz = "sparse_loop_list";

sparse_loop_list::sparse_loop_list(const vector<block_loop>& loops,const vector<sparse_bispace_any_order>& bispaces,const idx_list& direct_tensors) : m_loops(loops),m_bispaces(bispaces),m_direct_tensors(direct_tensors)
{
#ifdef LIBTENSOR_DEBUG
    if(m_loops.size() == 0)
    {
        throw bad_parameter(g_ns, k_clazz,"sparse_loop_list(...)",__FILE__, __LINE__,
            "Cannot have an empty loop list");
    }

    for(size_t loop_idx = 0; loop_idx < m_loops.size(); ++loop_idx)
    {
        const block_loop& loop = m_loops[loop_idx];

        //Check that no loop is an empty loop - one that touches no bispaces/subspaces
        bool all_ignored = true;
        for(size_t bispace_idx = 0 ; bispace_idx < m_bispaces.size(); ++bispace_idx)
        {
            if(!loop.is_bispace_ignored(bispace_idx))
            {
                all_ignored = false;
                break;
            }
        }
        if(all_ignored)
        {
            throw bad_parameter(g_ns, k_clazz,"sparse_loop_list(...)",__FILE__, __LINE__,
                "a loop may not ignore all bispaces");
        }

		//Check that no two loops access the same subspace
		for(size_t other_loop_idx = loop_idx + 1; other_loop_idx < m_loops.size(); ++other_loop_idx)
		{
			const block_loop& other_loop = m_loops[other_loop_idx];
            for(size_t bispace_idx = 0 ; bispace_idx < m_bispaces.size(); ++bispace_idx)
			{
				if(!other_loop.is_bispace_ignored(bispace_idx) && !loop.is_bispace_ignored(bispace_idx))
				{
					if(other_loop.get_subspace_looped(bispace_idx) == loop.get_subspace_looped(bispace_idx))
					{
						throw bad_parameter(g_ns, k_clazz,"sparse_loop_list(...)",__FILE__, __LINE__,
                            "Two loops cannot access the same subspace of the same bispace");
					}
				}
			}
		}
    }
#endif
}

std::vector<size_t> sparse_loop_list::get_loops_that_access_bispace(
		size_t bispace_idx) const
{
	std::vector<size_t> loops_that_access_bispace;
#ifdef LIBTENSOR_DEBUG
	if(bispace_idx >= m_bispaces.size())
	{
		throw out_of_bounds(g_ns, k_clazz,"get_loops_that_access_bispace(...)",
				__FILE__, __LINE__, "bispace index out of bounds");
	}
#endif
	for(size_t loop_idx = 0; loop_idx < m_loops.size(); ++loop_idx)
	{
		if(!m_loops[loop_idx].is_bispace_ignored(bispace_idx))
		{
			loops_that_access_bispace.push_back(loop_idx);
		}
	}
	return loops_that_access_bispace;
}

std::vector<size_t> sparse_loop_list::get_loops_that_access_group(
		size_t bispace_idx, size_t group_idx) const
{
	std::vector<size_t> loops_that_access_bispace = get_loops_that_access_bispace(bispace_idx);
	const sparse_bispace_any_order& cur_bispace = m_bispaces[bispace_idx];
	if(group_idx >= cur_bispace.get_n_sparse_groups())
	{
		throw out_of_bounds(g_ns, k_clazz,"get_loops_that_access_group(...)",
				__FILE__, __LINE__, "group index out of bounds");
	}

	//Filter loops that only access this group
	std::vector<size_t> loops_that_access_group;
	for(size_t bispace_loop_idx = 0; bispace_loop_idx < loops_that_access_bispace.size(); ++bispace_loop_idx)
	{
		size_t loop_idx = loops_that_access_bispace[bispace_loop_idx];
		size_t cur_subspace_idx = m_loops[loop_idx].get_subspace_looped(bispace_idx);
		size_t group_offset = cur_bispace.get_sparse_group_offset(group_idx);
		size_t group_order = cur_bispace.get_sparse_group_tree(group_idx).get_order();
		if((group_offset <= cur_subspace_idx) && (cur_subspace_idx < group_offset+group_order))
		{
			loops_that_access_group.push_back(loop_idx);
		}
	}
	return loops_that_access_group;
}

} /* namespace libtensor */


