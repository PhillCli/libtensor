#ifndef LIBTENSOR_BLOCK_TENSOR_I_TRAITS_H
#define LIBTENSOR_BLOCK_TENSOR_I_TRAITS_H

namespace libtensor {


template<size_t N, typename T> class dense_tensor_i;


/** \brief Block tensor interface traits
    \tparam N Tensor order.
    \tparam T Tensor element type.

    This structure specifies the types that define the identity of simple
    block tensors.

    \sa block_tensor_i

    \ingroup libtensor_block_tensor
 **/
template<size_t N, typename T>
struct block_tensor_i_traits {

    //! Type of tensor elements
    typedef T element_type;

    //! Type of read-only blocks as returned by the block tensor
    typedef dense_tensor_i<N, T> rd_block_type;

    //! Type of read-write blocks as returned by the block tensor
    typedef dense_tensor_i<N, T> wr_block_type;

};


} // namespace libtensor

#endif // LIBTENSOR_BLOCK_TENSOR_I_TRAITS_H
