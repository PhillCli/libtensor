#ifndef LIBTENSOR_LABELED_BTENSOR_EXPR_OPERATOR_DIRECT_PRODUCT_H
#define LIBTENSOR_LABELED_BTENSOR_EXPR_OPERATOR_DIRECT_PRODUCT_H

#include "../ident/core_ident.h"
#include "core_direct_product.h"
#include "eval_direct_product.h"

namespace libtensor {
namespace labeled_btensor_expr {


/**	\brief Direct product of two expressions
	\tparam N Order of the first %tensor.
	\tparam M Order of the second %tensor.
	\tparam T Tensor element type.
	\tparam E1 First expression.
	\tparam E2 Second expression.

	\ingroup libtensor_btensor_expr_op
 **/
template<size_t N, size_t M, typename T, typename E1, typename E2>
inline
expr<N + M, T, core_direct_product<N, M, T,
	expr<N, T, E1>,
	expr<M, T, E2>
> >
operator*(
	expr<N, T, E1> bta,
	expr<M, T, E2> btb) {

	typedef expr<N, T, E1> expr1_t;
	typedef expr<M, T, E2> expr2_t;
	typedef core_direct_product<N, M, T, expr1_t, expr2_t> core_t;
	typedef expr<N + M, T, core_t> expr_t;
	return expr_t(core_t(bta, btb));
}


/**	\brief Direct product of a %tensor and an expression
	\tparam N Order of the first %tensor.
	\tparam M Order of the second %tensor.
	\tparam T Tensor element type.
	\tparam A1 First %tensor assignable.
	\tparam E2 Second expression.

	\ingroup libtensor_btensor_expr_op
 **/
template<size_t N, size_t M, typename T, bool A1, typename E2>
inline
expr<N + M, T, core_direct_product<N, M, T,
	expr< N, T, core_ident<N, T, A1> >,
	expr< M, T, E2 >
> >
operator*(
	labeled_btensor<N, T, A1> bta,
	expr<M, T, E2> btb) {

	typedef expr< N, T, core_ident<N, T, A1> > expr1_t;
	return expr1_t(bta) * btb;
}


/**	\brief Direct product of an expression and a %tensor
	\tparam N Order of the first %tensor.
	\tparam M Order of the second %tensor.
	\tparam T Tensor element type.
	\tparam E1 First expression.
	\tparam A2 Second %tensor assignable.

	\ingroup libtensor_btensor_expr_op
 **/
template<size_t N, size_t M, typename T, typename E1, bool A2>
inline
expr<N + M, T, core_direct_product<N, M, T,
	expr< N, T, E1 >,
	expr< M, T, core_ident<M, T, A2> >
> >
operator*(
	expr<N, T, E1> bta,
	labeled_btensor<M, T, A2> btb) {

	typedef expr< M, T, core_ident<M, T, A2> > expr2_t;
	return bta * expr2_t(btb);
}


/**	\brief Direct product of two tensors
	\tparam N Order of the first tensor.
	\tparam M Order of the second tensor.
	\tparam T Tensor element type.
	\tparam A1 First %tensor assignable.
	\tparam A2 Second %tensor assignable.

	\ingroup libtensor_btensor_expr_op
 **/
template<size_t N, size_t M, typename T, bool A1, bool A2>
inline
expr<N + M, T, core_direct_product<N, M, T,
	expr< N, T, core_ident<N, T, A1> >,
	expr< M, T, core_ident<M, T, A2> >
> >
operator*(
	labeled_btensor<N, T, A1> bta,
	labeled_btensor<M, T, A2> btb) {

	typedef expr< N, T, core_ident<N, T, A1> > expr1_t;
	typedef expr< M, T, core_ident<M, T, A2> > expr2_t;
	return expr1_t(bta) * expr2_t(btb);
}


} // namespace labeled_btensor_expr

using labeled_btensor_expr::operator*;

} // namespace libtensor

#endif // LIBTENSOR_LABELED_BTENSOR_EXPR_OPERATOR_DIRECT_PRODUCT_H