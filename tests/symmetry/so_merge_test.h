#ifndef LIBTENSOR_SO_MERGE_TEST_H
#define LIBTENSOR_SO_MERGE_TEST_H

#include "se_label_test_base.h"

namespace libtensor {

/** \brief Tests the libtensor::so_merge class

    \ingroup libtensor_tests_sym
**/
class so_merge_test : public se_label_test_base {
public:
    virtual void perform() throw(libtest::test_exception);

private:
    void test_1() throw(libtest::test_exception);
    void test_2() throw(libtest::test_exception);
    void test_3() throw(libtest::test_exception);
    void test_4() throw(libtest::test_exception);
    void test_5() throw(libtest::test_exception);

};

} // namespace libtensor

#endif // LIBTENSOR_SO_MERGE_TEST_H
