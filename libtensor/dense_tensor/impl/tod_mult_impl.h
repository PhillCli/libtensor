#ifndef LIBTENSOR_TOD_MULT_IMPL_H
#define LIBTENSOR_TOD_MULT_IMPL_H

#include <memory>
#include <libtensor/linalg/linalg.h>
#include <libtensor/kernels/kern_ddiv2.h>
#include <libtensor/kernels/kern_dmul2.h>
#include <libtensor/kernels/loop_list_runner.h>
#include <libtensor/core/bad_dimensions.h>
#include "../dense_tensor_ctrl.h"
#include "../tod_mult.h"

namespace libtensor {


template<size_t N>
const char *tod_mult<N>::k_clazz = "tod_mult<N>";


template<size_t N>
tod_mult<N>::tod_mult(
        dense_tensor_rd_i<N, double> &ta, const tensor_transf<N, double> &tra,
        dense_tensor_rd_i<N, double> &tb, const tensor_transf<N, double> &trb,
        bool recip, const scalar_transf<double> &trc) :

        m_ta(ta), m_tb(tb), m_perma(tra.get_perm()), m_permb(trb.get_perm()),
        m_recip(recip), m_c(trc.get_coeff()), m_dimsc(ta.get_dims()) {

    static const char *method = "tod_mult("
            "dense_tensor_rd_i<N, double>&, const tensor_transf<N, double> &, "
            "dense_tensor_rd_i<N, double>&, const tensor_transf<N, double> &, "
            "bool, const scalar_transf<double> &)";

    m_dimsc.permute(m_perma);
    dimensions<N> dimsb(tb.get_dims());
    dimsb.permute(m_permb);

    if(!m_dimsc.equals(dimsb)) {
        throw bad_dimensions(g_ns, k_clazz, method, __FILE__, __LINE__,
                "ta, tb");
    }
    if (recip && trb.get_scalar_tr().get_coeff() == 0.0) {
        throw bad_parameter(g_ns, k_clazz, "tod_mult()",
                __FILE__, __LINE__, "trb");
    }

    m_c = m_c * (recip ?
            tra.get_scalar_tr().get_coeff() / trb.get_scalar_tr().get_coeff() :
            tra.get_scalar_tr().get_coeff() * trb.get_scalar_tr().get_coeff());
}


template<size_t N>
tod_mult<N>::tod_mult(dense_tensor_rd_i<N, double> &ta,
    dense_tensor_rd_i<N, double> &tb, bool recip, double c) :

    m_ta(ta), m_tb(tb), m_recip(recip), m_c(c), m_dimsc(ta.get_dims()) {

    static const char *method = "tod_mult(dense_tensor_rd_i<N, double>&, "
        "dense_tensor_rd_i<N, double>&, bool, double)";

    if(!ta.get_dims().equals(tb.get_dims())) {
        throw bad_dimensions(g_ns, k_clazz, method, __FILE__, __LINE__,
            "ta,tb");
    }
}


template<size_t N>
tod_mult<N>::tod_mult(
        dense_tensor_rd_i<N, double> &ta, const permutation<N> &pa,
        dense_tensor_rd_i<N, double> &tb, const permutation<N> &pb,
        bool recip, double c) :

        m_ta(ta), m_tb(tb), m_perma(pa), m_permb(pb),
        m_recip(recip), m_c(c), m_dimsc(ta.get_dims()) {

    static const char *method = "tod_mult("
            "dense_tensor_rd_i<N, double>&, permutation<N>, "
            "dense_tensor_rd_i<N, double>&, permutation<N>, "
            "bool, double)";

    m_dimsc.permute(pa);
    dimensions<N> dimsb(tb.get_dims());
    dimsb.permute(pb);

    if(!m_dimsc.equals(dimsb)) {
        throw bad_dimensions(g_ns, k_clazz, method, __FILE__, __LINE__,
                "ta, tb");
    }
}


template<size_t N>
void tod_mult<N>::prefetch() {

    dense_tensor_rd_ctrl<N, double>(m_ta).req_prefetch();
    dense_tensor_rd_ctrl<N, double>(m_tb).req_prefetch();

}


template<size_t N>
void tod_mult<N>::perform(bool zero, dense_tensor_wr_i<N, double> &tc) {

    static const char *method = "perform(bool, double, "
        "dense_tensor_wr_i<N, double>&)";

    if(!m_dimsc.equals(tc.get_dims())) {
        throw bad_dimensions(g_ns, k_clazz, method, __FILE__, __LINE__, "tc");
    }

    tod_mult<N>::start_timer();

    try {

    dense_tensor_rd_ctrl<N, double> ca(m_ta), cb(m_tb);
    dense_tensor_wr_ctrl<N, double> cc(tc);
    ca.req_prefetch();
    cb.req_prefetch();
    cc.req_prefetch();

    const dimensions<N> &dimsa = m_ta.get_dims();
    const dimensions<N> &dimsb = m_tb.get_dims();
    const dimensions<N> &dimsc = tc.get_dims();

    sequence<N, size_t> mapa(0), mapb(0);
    for(size_t i = 0; i < N; i++) mapa[i] = mapb[i] = i;
    m_perma.apply(mapa);
    m_permb.apply(mapb);

    std::list< loop_list_node<2, 1> > loop_in, loop_out;
    typename std::list< loop_list_node<2, 1> >::iterator inode = loop_in.end();
    for (size_t idxc = 0; idxc < N; ) {
        size_t len = 1;
        size_t idxa = mapa[idxc], idxb = mapb[idxc];

        do {
            len *= dimsa.get_dim(idxa);
            idxa++; idxb++; idxc++;
        } while (idxc < N && mapa[idxc] == idxa && mapb[idxc] == idxb);

        inode = loop_in.insert(loop_in.end(), loop_list_node<2, 1>(len));
        inode->stepa(0) = dimsa.get_increment(idxa - 1);
        inode->stepa(1) = dimsb.get_increment(idxb - 1);
        inode->stepb(0) = dimsc.get_increment(idxc - 1);
    }

    const double *pa = ca.req_const_dataptr();
    const double *pb = cb.req_const_dataptr();
    double *pc = cc.req_dataptr();

    if(zero) {
        tod_mult<N>::start_timer("zero");
        size_t sz = dimsc.get_size();
        for(size_t i = 0; i < sz; i++) pc[i] = 0.0;
        tod_mult<N>::stop_timer("zero");
    }

    loop_registers<2, 1> r;
    r.m_ptra[0] = pa;
    r.m_ptra[1] = pb;
    r.m_ptrb[0] = pc;
    r.m_ptra_end[0] = pa + dimsa.get_size();
    r.m_ptra_end[1] = pb + dimsb.get_size();
    r.m_ptrb_end[0] = pc + dimsc.get_size();

    std::auto_ptr< kernel_base<linalg, 2, 1, double> > kern(
        m_recip ?
            kern_ddiv2::match(m_c, loop_in, loop_out) :
            kern_dmul2<linalg>::match(m_c, loop_in, loop_out));
    tod_mult<N>::start_timer(kern->get_name());
    loop_list_runner<linalg, 2, 1>(loop_in).run(0, r, *kern);
    tod_mult<N>::stop_timer(kern->get_name());

    cc.ret_dataptr(pc); pc = 0;
    cb.ret_const_dataptr(pb); pb = 0;
    ca.ret_const_dataptr(pa); pa = 0;

    } catch (...) {
        tod_mult<N>::stop_timer();
        throw;
    }

    tod_mult<N>::stop_timer();
}


} // namespace libtensor

#endif // LIBTENSOR_TOD_MULT_IMPL_H
