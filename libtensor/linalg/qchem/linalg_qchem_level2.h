#ifndef LIBTENSOR_LINALG_BASE_LEVEL2_QCHEM_H
#define LIBTENSOR_LINALG_BASE_LEVEL2_QCHEM_H

#include "../linalg_timings.h"
#include "../generic/linalg_generic_level2.h"

namespace libtensor {


/** \brief Level-2 linear algebra operations (Q-Chem)

    \ingroup libtensor_linalg
 **/
class linalg_qchem_level2 :
    public linalg_generic_level2<double>,
    public linalg_timings<linalg_qchem_level2> {

public:
    static const char k_clazz[]; //!< Class name

private:
    typedef linalg_timings<linalg_qchem_level2> timings_base;

public:
    static void copy_ij_ji(
        void*,
        size_t ni, size_t nj,
        const double *a, size_t sja,
        double *c, size_t sic);

    static void mul2_i_ip_p_x(
        void*,
        size_t ni, size_t np,
        const double *a, size_t sia,
        const double *b, size_t spb,
        double *c, size_t sic,
        double d);

    static void mul2_i_pi_p_x(
        void*,
        size_t ni, size_t np,
        const double *a, size_t spa,
        const double *b, size_t spb,
        double *c, size_t sic,
        double d);

    static void mul2_ij_i_j_x(
        void*,
        size_t ni, size_t nj,
        const double *a, size_t sia,
        const double *b, size_t sjb,
        double *c, size_t sic,
        double d);

};


} // namespace libtensor

#endif // LIBTENSOR_LINALG_BASE_LEVEL2_QCHEM_H
