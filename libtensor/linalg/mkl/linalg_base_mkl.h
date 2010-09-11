#ifndef LIBTENSOR_LINALG_BASE_MKL_H
#define LIBTENSOR_LINALG_BASE_MKL_H

#include <mkl.h>
#include "../generic/linalg_base_lowlevel.h"
#include "../adaptive/linalg_base_highlevel.h"
#include "../generic/linalg_base_memory_generic.h"
#include "../cblas/linalg_base_level1_cblas.h"
#include "../cblas/linalg_base_level2_cblas.h"
#include "../cblas/linalg_base_level3_cblas.h"

namespace libtensor {


/**	\brief Linear algebra implementation based on
		Intel Math Kernel Library (MKL)

	\ingroup libtensor_linalg
 **/
struct linalg_base_mkl :
	public linalg_base_lowlevel<
		linalg_base_memory_generic,
		linalg_base_level1_cblas,
		linalg_base_level2_cblas,
		linalg_base_level3_cblas>,
	public linalg_base_highlevel<
		linalg_base_memory_generic,
		linalg_base_level1_cblas,
		linalg_base_level2_cblas,
		linalg_base_level3_cblas>
{ };


} // namespace libtensor

#endif // LIBTENSOR_LINALG_BASE_MKL_H
