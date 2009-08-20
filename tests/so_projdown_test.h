#ifndef LIBTENSOR_SO_PROJDOWN_TEST_H
#define LIBTENSOR_SO_PROJDOWN_TEST_H

#include <libtest.h>

namespace libtensor {

/**	\brief Tests the libtensor::so_projdown class

	\ingroup libtensor_tests
**/
class so_projdown_test : public libtest::unit_test {
public:
	virtual void perform() throw(libtest::test_exception);

private:
	void test_1() throw(libtest::test_exception);
	void test_2() throw(libtest::test_exception);

};

} // namespace libtensor

#endif // LIBTENSOR_SO_PROJDOWN_TEST_H