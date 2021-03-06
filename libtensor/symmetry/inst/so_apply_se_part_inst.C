#include <libtensor/core/scalar_transf_double.h>
#include "../so_apply_se_part.h"
#include "so_apply_se_part_impl.h"

namespace libtensor {

template
class symmetry_operation_impl< so_apply<1, double>, se_part<1, double> >;
template
class symmetry_operation_impl< so_apply<2, double>, se_part<2, double> >;
template
class symmetry_operation_impl< so_apply<3, double>, se_part<3, double> >;
template
class symmetry_operation_impl< so_apply<4, double>, se_part<4, double> >;
template
class symmetry_operation_impl< so_apply<5, double>, se_part<5, double> >;
template
class symmetry_operation_impl< so_apply<6, double>, se_part<6, double> >;

} // namespace libtensor


