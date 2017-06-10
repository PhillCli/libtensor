#ifndef LIBTENSOR_KERN_DADD2_H
#define LIBTENSOR_KERN_DADD2_H

#include "kernel_base.h"

namespace libtensor {


template<typename LA> class kern_dadd2_i_i_x_x;
template<typename LA> class kern_dadd2_i_x_i_x;


/** \brief Generic addition kernel (double)
    \tparam LA Linear algebra.

    This kernel adds two multidimensional arrays with optional scaling:
    \f[
        c = c + d (k_a a + k_b b)
    \f]
    a, b, c are arrays, d, k_a, k_b are scaling factors.

    \ingroup libtensor_kernels
 **/
template<typename LA>
class kern_dadd2 : public kernel_base<LA, 2, 1, double> {
    friend class kern_dadd2_i_i_x_x<LA>;
    friend class kern_dadd2_i_x_i_x<LA>;

public:
    static const char *k_clazz; //!< Kernel name

public:
    typedef typename kernel_base<LA, 2, 1, double>::device_context_ref
        device_context_ref;
    typedef typename kernel_base<LA, 2, 1, double>::list_t list_t;
    typedef typename kernel_base<LA, 2, 1, double>::iterator_t iterator_t;

private:
    double m_ka, m_kb;
    double m_d;

public:
    virtual ~kern_dadd2() { }

    virtual const char *get_name() const {
        return k_clazz;
    }

    virtual void run(device_context_ref ctx, const loop_registers<2, 1> &r);

    static kernel_base<LA, 2, 1, double> *match(double ka, double kb, double d,
        list_t &in, list_t &out);

};


} // namespace libtensor

#endif // LIBTENSOR_KERN_DADD2_H
