#ifndef LIBTENSOR_KERN_DMUL2_IJ_J_I_H
#define LIBTENSOR_KERN_DMUL2_IJ_J_I_H

#include "kern_dmul2_i_i_x.h"

namespace libtensor {


/** \brief Specialized kernel for \f$ c_{ij} = c_{ij} + a_j b_i d \f$
    \tparam LA Linear algebra.

    \ingroup libtensor_kernels
 **/
template<typename LA>
class kern_dmul2_ij_j_i : public kernel_base<LA, 2, 1> {
public:
    static const char *k_clazz; //!< Kernel name

public:
    typedef typename kernel_base<LA, 2, 1>::device_context_ref
        device_context_ref;
    typedef typename kernel_base<LA, 2, 1>::list_t list_t;
    typedef typename kernel_base<LA, 2, 1>::iterator_t iterator_t;

private:
    double m_d;
    size_t m_ni, m_nj;
    size_t m_sja, m_sib, m_sic;

public:
    virtual ~kern_dmul2_ij_j_i() { }

    virtual const char *get_name() const {
        return k_clazz;
    }

    virtual void run(device_context_ref ctx, const loop_registers<2, 1> &r);

    static kernel_base<LA, 2, 1> *match(const kern_dmul2_i_i_x<LA> &z,
        list_t &in, list_t &out);

};


} // namespace libtensor

#endif // LIBTENSOR_KERN_DMUL2_IJ_J_I_H
