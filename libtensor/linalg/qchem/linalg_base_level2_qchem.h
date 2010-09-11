#ifndef LIBTENSOR_LINALG_BASE_LEVEL2_QCHEM_H
#define LIBTENSOR_LINALG_BASE_LEVEL2_QCHEM_H

#include "../generic/linalg_base_level2_generic.h"

namespace libtensor {


/**	\brief Level-2 linear algebra operations (Q-Chem)

	\ingroup libtensor_linalg
 **/
struct linalg_base_level2_qchem : public linalg_base_level2_generic {


	static void i_ip_p_x(
		size_t ni, size_t np,
		const double *a, size_t sia,
		const double *b, size_t spb,
		double *c, size_t sic,
		double d) {

		CL_DGEMV('T', np, ni, d, (double*)a, sia, (double*)b, spb, 1.0,
			c, sic);
	}


	static void i_pi_p_x(
		size_t ni, size_t np,
		const double *a, size_t spa,
		const double *b, size_t spb,
		double *c, size_t sic,
		double d) {

		CL_DGEMV('N', ni, np, d, (double*)a, spa, (double*)b, spb, 1.0,
			c, sic);
	}


	static void ij_i_j_x(
		size_t ni, size_t nj,
		const double *a, size_t sia,
		const double *b, size_t sjb,
		double *c, size_t sic,
		double d) {

		CL_DGER(nj, ni, d, (double*)b, sjb, (double*)a, sia, c, sic);
	}


};


} // namespace libtensor

#endif // LIBTENSOR_LINALG_BASE_LEVEL2_QCHEM_H
