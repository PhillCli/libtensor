#ifndef LIBTENSOR_CTF_DENSE_TENSOR_H
#define LIBTENSOR_CTF_DENSE_TENSOR_H

#include <vector>
#include <libtensor/timings.h>
#include <libtensor/core/immutable.h>
#include <libtensor/core/noncopyable.h>
#include "ctf_dense_tensor_i.h"

namespace libtensor {


/** \brief Distributed tensor in Cyclops Tensor Framework
    \tparam N Tensor order.
    \tparam T Tensor element type.

    \ingroup libtensor_ctf_dense_tensor
 **/
template<size_t N, typename T>
class ctf_dense_tensor :
    public ctf_dense_tensor_i<N, T>,
    public immutable,
    public noncopyable {

public:
    static const char k_clazz[]; //!< Class name

public:
    typedef T element_t; //!< Tensor element type

private:
    using typename ctf_dense_tensor_i<N, T>::ctf_tensor_adapter;

private:
    dimensions<N> m_dims; //!< Tensor dimensions
    ctf_symmetry<N, T> m_sym; //!< Tensor symmetry
    std::vector< ctf_tensor_adapter* > m_tens; //!< CTF tensors

public:
    /** \brief Creates a new tensor
        \param dims Non-zero tensor dimensions.
     **/
    ctf_dense_tensor(const dimensions<N> &dims);

    /** \brief Creates a new tensor
        \param dims Non-zero tensor dimensions.
        \param sym Tensor symmetry.
     **/
    ctf_dense_tensor(const dimensions<N> &dims, const ctf_symmetry<N, T> &sym);

    /** \brief Virtual destructor
     **/
    virtual ~ctf_dense_tensor();

    /** \brief Returns the dimensions of the tensor
     **/
    virtual const dimensions<N> &get_dims() const;

protected:
    /** \brief Handles requests for the symmetry of the CTF tensor
     **/
    virtual const ctf_symmetry<N, T> &on_req_symmetry();

    /** \brief Returns the CTF tensor object
        \param icomp Symmetry component
     **/
    virtual ctf_tensor_adapter *on_req_ctf_tensor(size_t icomp);

    /** \brief Resets the symmetry of the CTF tensor
     **/
    virtual void on_reset_symmetry(const ctf_symmetry<N, T> &sym);

    /** \brief Adjusts the symmetry of the CTF tensor
     **/
    virtual void on_adjust_symmetry(const ctf_symmetry<N, T> &sym);

    /** \brief Called when state changes to immutable
     **/
    virtual void on_set_immutable();

private:
    void cleanup();

};


} // namespace libtensor

#endif // LIBTENSOR_CTF_DENSE_TENSOR_H
