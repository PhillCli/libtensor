#ifndef SPARSE_BTENSOR_H
#define SPARSE_BTENSOR_H

#include <sstream>
#include <limits>
#include "sparse_bispace.h"
#include "sparse_loop_list.h"
#include "block_load_kernel.h"
#include "block_print_kernel.h"
#include "gen_sparse_btensor.h"
#include "direct_sparse_btensor.h"
#include "batch_list_builder.h"
#include "memory_reserve.h"
#include "batch_provider.h"
#include <libtensor/expr/iface/expr_lhs.h>
#include <libtensor/expr/iface/expr_rhs.h>
#include <libtensor/expr/iface/labeled_lhs_rhs.h>
#include <libtensor/expr/operators/contract.h>
#include <libtensor/expr/operators/plus_minus.h>
#include <libtensor/core/scalar_transf_double.h>

namespace libtensor {

template<size_t N,typename T = double>
class sparse_btensor : public gen_sparse_btensor<N,T>,public expr::expr_lhs<N,T>
{
public:
    static const char *k_clazz; //!< Class name
private:
    T* m_data_ptr;
    sparse_bispace<N> m_bispace;
    memory_reserve* m_mr;
public:
    /** \brief Constructs a sparse block tensor object and populates it with the entries from mem if specified
     **/
    sparse_btensor(const sparse_bispace<N>& the_bispace,const T* mem = NULL,bool already_block_major = false);

    virtual ~sparse_btensor();

    //Copy constructor
    sparse_btensor(const sparse_btensor<N>& rhs);

    sparse_btensor<N,T>& operator=(const sparse_btensor<N>& rhs);

    /** \brief Return the sparse_bispace defining this tensor 
     **/
    const sparse_bispace<N>& get_bispace() const; 

    /** \brief Compares the tensor to another
     *         Two sparse_btensors are equal if they have the same number of elements and all of those elements match
     **/
    bool operator==(const sparse_btensor<N,T>& rhs) const;
    bool operator!=(const sparse_btensor<N,T>& rhs) const;

    const T* get_data_ptr() const { return m_data_ptr; }
    virtual batch_provider_i<T>* get_batch_provider() const { return NULL; }
    
    /** \brief Returns a string representation of the tensor in row-major order 
     **/
    std::string str() const;

    void set_memory_reserve(memory_reserve& mr);

    virtual void assign(const expr::expr_rhs<N, T>& rhs, const expr::label<N>& l);

    expr::labeled_lhs_rhs<N, T> operator()(const expr::label<N> &label) {
        return expr::labeled_lhs_rhs<N, T>(*this, label,
            any_tensor<N, T>::make_rhs(label));
    }

    //STUB!!!! DAT NASTY HACK!!
    virtual void assign_add(const expr::expr_rhs<N, T> &rhs, const expr::label<N> &l)
    {
        throw bad_parameter(g_ns, k_clazz,"assign_add",__FILE__, __LINE__,"assign_add not implemented!");
    }
    virtual void scale(const T &c)
    {
        throw bad_parameter(g_ns, k_clazz,"scale",__FILE__, __LINE__,"scale not implemented!");
    }

};

template<size_t N,typename T>
const char *sparse_btensor<N,T>::k_clazz = "sparse_btensor<N,T>";

template<size_t N,typename T>
sparse_btensor<N,T>::sparse_btensor(const sparse_bispace<N>& the_bispace,const T* mem,bool already_block_major) : m_bispace(the_bispace),m_mr(NULL)
{
    //Alloc storage
    m_data_ptr = new T[m_bispace.get_nnz()];

    //Create loops

    if(mem != NULL)
    {
        if(already_block_major)
        {
            memcpy(m_data_ptr,mem,the_bispace.get_nnz()*sizeof(T));
        }
        else
        {

        	std::vector<sparse_bispace_any_order> bispaces(1,m_bispace);
            std::vector<block_loop> loops;
            for(size_t i = 0; i < N; ++i)
            {
            	block_loop loop(bispaces);
            	loop.set_subspace_looped(0,i);
            	loops.push_back(loop);
            }
        	sparse_loop_list sll(loops,bispaces);

            block_load_kernel<T> blk(m_bispace,mem);
            std::vector<T*> ptrs(1,m_data_ptr);
            sll.run(blk,ptrs);
        }
    }
    else
    {
        memset(m_data_ptr,0,m_bispace.get_nnz()*sizeof(T));
    }
}

template<size_t N,typename T>
sparse_btensor<N,T>::~sparse_btensor()
{
    delete [] m_data_ptr;
    if(m_mr != NULL) m_mr->remove_tensor(this->m_bispace.get_nnz()*sizeof(T));
}

template<size_t N,typename T>
sparse_btensor<N,T>::sparse_btensor(const sparse_btensor<N>& rhs) : m_bispace(rhs.m_bispace),m_mr(NULL)
{
    if(rhs.m_mr != NULL) this->set_memory_reserve(*rhs.m_mr);
    m_data_ptr = new T[m_bispace.get_nnz()]; 
    memcpy(m_data_ptr,rhs.m_data_ptr,m_bispace.get_nnz()*sizeof(T)); 
}

template<size_t N,typename T>
sparse_btensor<N,T>& sparse_btensor<N,T>::operator=(const sparse_btensor<N>& rhs)
{
    m_bispace = rhs.m_bispace;
    if(rhs.m_mr != NULL) this->set_memory_reserve(*rhs.m_mr);
    m_data_ptr = new T[m_bispace.get_nnz()]; 
    memcpy(m_data_ptr,rhs.m_data_ptr,m_bispace.get_nnz()*sizeof(T)); 
    return *this;
}

template<size_t N,typename T>
const sparse_bispace<N>& sparse_btensor<N,T>::get_bispace() const
{
    return m_bispace;
}



template<size_t N,typename T>
bool sparse_btensor<N,T>::operator==(const sparse_btensor<N,T>& rhs) const
{
    if(this->m_bispace.get_nnz() != rhs.m_bispace.get_nnz())
    {
        throw bad_parameter(g_ns, k_clazz,"operator==(...)",
                __FILE__, __LINE__, "tensors have different numbers of nonzero elements");
    }

    for(size_t i = 0; i < m_bispace.get_nnz(); ++i)
    {
        if(m_data_ptr[i] != rhs.m_data_ptr[i])
        {
            return false;
        }
    }
    return true;
}

template<size_t N,typename T>
bool sparse_btensor<N,T>::operator!=(const sparse_btensor<N,T>& rhs) const
{
    return !(*this == rhs);
}

template<size_t N,typename T>
std::string sparse_btensor<N,T>::str() const
{

    //Generate the loops for this tensor in slow->fast index order
	std::vector<sparse_bispace_any_order> bispaces(1,m_bispace);

    std::vector<block_loop> loops;
	for(size_t i = 0; i < N; ++i)
	{
		block_loop loop(bispaces);
		loop.set_subspace_looped(0,i);
		loops.push_back(loop);
	}
	sparse_loop_list sll(loops,bispaces);

	block_print_kernel<T> bpk;
	std::vector<T*> ptrs(1,m_data_ptr);
	sll.run(bpk,ptrs);
    return bpk.str();
}

template<size_t N,typename T>
void sparse_btensor<N,T>::set_memory_reserve(memory_reserve& mr)
{ 
    if(this->m_mr != NULL) this->m_mr->remove_tensor(this->m_bispace.get_nnz()*sizeof(T));
    this->m_mr = &mr;
    try
    {
        m_mr->add_tensor(this->m_bispace.get_nnz()*sizeof(T));
    }
    catch(out_of_memory&)
    {
        m_mr = NULL;
    }
}

template<size_t N,typename T>
void sparse_btensor<N,T>::assign(const expr::expr_rhs<N, T>& rhs, const expr::label<N>& l)
{
    using namespace expr;
    node_assign root(N,false);
    expr_tree e(root);
    expr_tree::node_id_t root_id = e.get_root();
    node_ident_any_tensor<N,T> n_tensor(*this);
    e.add(root_id,n_tensor);

    permutation<N> perm = l.permutation_of(rhs.get_label());
    expr_tree::node_id_t perm_id = root_id;
    if(!perm.is_identity()) 
    {
        std::vector<size_t> perm_entries(N);
        for(size_t i = 0; i < N; i++) perm_entries[i] = perm[i];

        node_transform<T> n_tf(perm_entries, scalar_transf<T>());
        perm_id = e.add(root_id,n_tf);
    }
    e.add(perm_id, rhs.get_expr());

    //We clean up the tree a little bit to prevent spurious extra assignments
    std::vector<expr_tree::node_id_t> node_stack(1,e.get_root());
    std::vector<size_t> pos_stack(1,0);
    while(node_stack.size() > 0)
    {
        expr_tree::node_id_t cur_id = node_stack.back();
        const node& n = e.get_vertex(cur_id);
        if(n.check_type<node_assign>())
        {
            expr_tree::edge_list_t children = e.get_edges_out(cur_id);
            expr_tree::node_id_t op_id = children[1];
            const node& n_op = e.get_vertex(op_id); 
            if(n_op.check_type<node_assign>())
            {
                expr_tree::edge_list_t op_children = e.get_edges_out(op_id);
                expr_tree sub = e.get_subtree(op_children[1]);
                e.erase_subtree(op_id);
                e.add(cur_id,sub);
            }
        }
        expr_tree::edge_list_t children = e.get_edges_out(cur_id);
        if(children.size() > 0 && pos_stack.back() < children.size())
        {
            node_stack.push_back(children[pos_stack.back()]);
            pos_stack.push_back(0);
        }
        else
        {
            node_stack.pop_back();
            pos_stack.pop_back();
            if(pos_stack.size() > 0) pos_stack.back()++;
        }
    }

    expr_tree::node_id_t last_op_node_id = e.get_edges_out(root_id)[1];
    const node& last_op_node = e.get_vertex(last_op_node_id);

    direct_sparse_btensor<3>* D_unblocked_tensor_ptr = NULL;
    direct_sparse_btensor<3>* H_unblocked_tensor_ptr = NULL;
    direct_sparse_btensor<2>* M_unblocked_tensor_ptr = NULL;
    direct_sparse_btensor<2>* M_reblocked_0_tensor_ptr = NULL;
    std::vector<idx_list> batched_subspace_grps;
    bool hack = false;
#if 0
    if(N == 2)
    {
        if(last_op_node.check_type<node_contract>())
        {
            expr_tree::edge_list_t last_op_inputs = e.get_edges_out(last_op_node_id);
            if(last_op_inputs.size() == 2)
            {
                expr_tree::node_id_t op_child_0_id = last_op_inputs[0];
                const node& op_child_0 = e.get_vertex(op_child_0_id);
                expr_tree::node_id_t op_child_1_id = last_op_inputs[1];
                const node& op_child_1 = e.get_vertex(op_child_1_id);
                if(op_child_0.check_type<node_assign>() && op_child_1.check_type<node_assign>())
                {
                    const node& D_result_tensor_node_abs = e.get_vertex(e.get_edges_out(op_child_0_id)[0]);
                    const node& H_result_tensor_node_abs = e.get_vertex(e.get_edges_out(op_child_1_id)[0]);
                    if(D_result_tensor_node_abs.get_n() == 3 && H_result_tensor_node_abs.get_n() == 3)
                    {
                        const node_ident_any_tensor<3,T>& D_result_tensor_node = dynamic_cast< const node_ident_any_tensor<3,T>& >(D_result_tensor_node_abs);
                        gen_sparse_btensor<3,T>& D = dynamic_cast< gen_sparse_btensor<3,T>& >(D_result_tensor_node.get_tensor());
                        const node_ident_any_tensor<3,T>& H_result_tensor_node = dynamic_cast< const node_ident_any_tensor<3,T>& >(H_result_tensor_node_abs);
                        gen_sparse_btensor<3,T>& H = dynamic_cast< gen_sparse_btensor<3,T>& >(H_result_tensor_node.get_tensor());
                        sparse_bispace<3> D_bispace = D.get_bispace();
                        sparse_bispace<3> H_bispace = H.get_bispace();
                        if(D_bispace.get_index_group_containing_subspace(0) == 0 && D_bispace.get_index_group_order(0) == 1)
                        {
                            //Add the tree to unblock D
                            expr_tree::node_id_t D_unblocked_assign_id = e.insert(op_child_0_id,node_assign(3));
                            expr_tree::node_id_t D_ub_subtree_id = e.insert(op_child_0_id,node_unblock(3,0));
                            sparse_bispace<1> unblocked_subspace(D_bispace[0].get_dim()); 
                            sparse_bispace<3> D_unblocked_bispace = unblocked_subspace | D_bispace.contract(0);
                            D_unblocked_tensor_ptr = new direct_sparse_btensor<3>(D_unblocked_bispace);
                            expr_tree D_ub_subtree = e.get_subtree(D_ub_subtree_id);
                            e.erase(D_ub_subtree_id);
                            e.add(D_unblocked_assign_id,node_ident_any_tensor<3,T>(*D_unblocked_tensor_ptr));
                            e.add(D_unblocked_assign_id,D_ub_subtree);

                            //Add the tree to unblock H
                            expr_tree H_ub_subtree(node_assign(3));
                            sparse_bispace<3> H_unblocked_bispace = unblocked_subspace | H_bispace.contract(0);

                            expr_tree::node_id_t H_unblocked_assign_id = H_ub_subtree.get_root();
                            H_unblocked_tensor_ptr = new direct_sparse_btensor<3>(H_unblocked_bispace);
                            H_ub_subtree.add(H_unblocked_assign_id,node_ident_any_tensor<3,T>(*H_unblocked_tensor_ptr));
                            expr_tree::node_id_t n_ub_id = H_ub_subtree.add(H_unblocked_assign_id,node_unblock(3,0));
                            H_ub_subtree.add(n_ub_id,e.get_subtree(op_child_1_id));

                            //Tie that tree to the existing tree, replacing the original subtree for H
                            e.add(last_op_node_id,H_ub_subtree);
                            e.erase_subtree(op_child_1_id);

                            //Add the tree to reblock M idx 0 and 1
                            expr_tree rb_subtree(node_reblock(2,1));
                            expr_tree::node_id_t rb_root_id = rb_subtree.get_root();
                            expr_tree::node_id_t M_reblocked_0_assign_id = rb_subtree.add(rb_root_id,node_assign(2));
                            M_reblocked_0_tensor_ptr = new direct_sparse_btensor<2>(unblocked_subspace|m_bispace[1]);
                            rb_subtree.add(M_reblocked_0_assign_id,node_ident_any_tensor<2,T>(*M_reblocked_0_tensor_ptr));
                            expr_tree::node_id_t reblock_0_id = rb_subtree.add(M_reblocked_0_assign_id,node_reblock(2,0));

                            expr_tree::node_id_t M_unblocked_assign_id = rb_subtree.add(reblock_0_id,node_assign(2));
                            M_unblocked_tensor_ptr = new direct_sparse_btensor<2>(unblocked_subspace|unblocked_subspace);
                            rb_subtree.add(M_unblocked_assign_id,node_ident_any_tensor<2,T>(*M_unblocked_tensor_ptr));
                            rb_subtree.add(M_unblocked_assign_id,e.get_subtree(last_op_node_id));
                            e.erase_subtree(last_op_node_id);
                            e.add(root_id,rb_subtree);

                            batched_subspace_grps.push_back(idx_list()); // M <- M unblocked 
                            batched_subspace_grps.push_back(idx_list()); // M unblocked <- M double unblocked 
                            batched_subspace_grps.push_back(idx_list(2,1)); // M double unblocked <- D unblocked,H unblocked
                            batched_subspace_grps.push_back(idx_list(2,1)); // D unblocked <- D
                            batched_subspace_grps.push_back(idx_list(1,1)); // D <- C
                            batched_subspace_grps.push_back(idx_list(2,1)); // H  unblocked <- H
                            idx_list grp_0(1,1);
                            grp_0.push_back(2);
                            batched_subspace_grps.push_back(grp_0); // H  <- G
                            batched_subspace_grps.push_back(idx_list(3,2)); // G  <- I,E
                            batched_subspace_grps.push_back(idx_list(1,2)); // E <- C,V
                            batched_subspace_grps.push_back(idx_list(1,2)); // I
                            hack = true;
                        }
                    }
                }
            }
        }
    }
#endif
    batch_provider<T> bp(e);
    std::vector< std::vector<sparse_bispace_any_order> > direct_bispace_grps;
    bp.get_direct_bispace_grps(direct_bispace_grps);
    size_t M_unblocked_size;
    if(hack)
    {
        M_unblocked_size = direct_bispace_grps[0][0].get_nnz();
        direct_bispace_grps[0] = std::vector<sparse_bispace_any_order>();
        direct_bispace_grps[1] = std::vector<sparse_bispace_any_order>();
        direct_bispace_grps[2].erase(direct_bispace_grps[2].begin());
    }
    else
    {
        bp.get_batched_subspace_grps(batched_subspace_grps);
    }
    batch_list_builder blb(direct_bispace_grps,batched_subspace_grps);
    size_t mem_avail = (m_mr != NULL) ? m_mr->get_mem_avail() : std::numeric_limits<size_t>::max();
    idx_pair_list batch_list = blb.get_batch_list(mem_avail/sizeof(T));
    std::vector< std::vector<size_t> > batch_array_size_grps = blb.get_batch_array_size_grps(batch_list);
    bp.set_batch_info(batched_subspace_grps,batch_list);
    //Need to add back in the other bispaces so that they can be properly sized
    if(hack)
    {
        batch_array_size_grps[0].push_back(M_unblocked_size);
        batch_array_size_grps[1].push_back(M_unblocked_size);
        batch_array_size_grps[1].push_back(M_unblocked_size);
        batch_array_size_grps[2].insert(batch_array_size_grps[2].begin(),M_unblocked_size);
    }
    bp.set_batch_array_sizes(batch_array_size_grps);
    bp.get_batch(this->m_data_ptr);
#ifdef DEVELOPMENT
    bp.print_timings();
#endif

    if(D_unblocked_tensor_ptr == NULL) delete D_unblocked_tensor_ptr;
    if(H_unblocked_tensor_ptr == NULL) delete H_unblocked_tensor_ptr;
    if(M_unblocked_tensor_ptr == NULL) delete M_unblocked_tensor_ptr;
    if(M_reblocked_0_tensor_ptr == NULL) delete M_reblocked_0_tensor_ptr;
}

} // namespace libtensor

#endif /* SPARSE_BTENSOR_H */
