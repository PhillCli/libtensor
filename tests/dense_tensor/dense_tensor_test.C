#include <libutil/threads/thread.h>
#include <libtensor/core/allocator.h>
#include <libtensor/dense_tensor/dense_tensor.h>
#include <libtensor/dense_tensor/dense_tensor_ctrl.h>
#include <libtensor/dense_tensor/impl/dense_tensor_impl.h>
#include "../test_utils.h"

using namespace libtensor;


int test_ctor() {

    static const char testname[] = "dense_tensor_test::test_ctor()";

    typedef allocator<double> allocator;

    try {

    index<2> i1, i2;
    i2[0] = 2;
    i2[1] = 3;
    index_range<2> ir(i1, i2);
    dimensions<2> d1(ir);
    dense_tensor<2, double, allocator> t1(d1);

    if(t1.is_immutable()) {
        return fail_test(testname, __FILE__, __LINE__,
            "A new tensor must be mutable (t1)");
    }

    if(t1.get_dims()[0] != 3) {
        return fail_test(testname, __FILE__, __LINE__,
            "Incorrect tensor dimension 0 (t1)");
    }
    if(t1.get_dims()[1] != 4) {
        return fail_test(testname, __FILE__, __LINE__,
            "Incorrect tensor dimension 1 (t1)");
    }

    dense_tensor<2, double, allocator> t2(t1);

    if(t2.is_immutable()) {
        return fail_test(testname, __FILE__, __LINE__,
            "A new tensor must be mutable (t2)");
    }

    if(t2.get_dims()[0] != 3) {
        return fail_test(testname, __FILE__, __LINE__,
            "Incorrect tensor dimension 0 (t2)");
    }
    if(t2.get_dims()[1] != 4) {
        return fail_test(testname, __FILE__, __LINE__,
            "Incorrect tensor dimension 1 (t2)");
    }

    dense_tensor_i<2, double> &pt2 = t2;
    dense_tensor<2, double, allocator> t3(pt2);

    if(t3.is_immutable()) {
        return fail_test(testname, __FILE__, __LINE__,
            "A new tensor must be mutable (t3)");
    }

    if(t3.get_dims()[0] != 3) {
        return fail_test(testname, __FILE__, __LINE__,
            "Incorrect tensor dimension 0 (t3)");
    }
    if(t3.get_dims()[1] != 4) {
        return fail_test(testname, __FILE__, __LINE__,
            "Incorrect tensor dimension 1 (t3)");
    }

    } catch(exception &e) {
        return fail_test(testname, __FILE__, __LINE__, e.what());
    }

    return 0;
}


namespace {

/** Checks that requesting a non-const data pointer causes an exception
 **/
class op_chk_imm {
private:
    bool m_ok;

public:
    op_chk_imm() : m_ok(false) {

    }

    bool is_ok() const {
        return m_ok;
    }

    void perform(dense_tensor_i<2, int> &t) {

        m_ok = false;
        dimensions<2> d(t.get_dims());
        int *ptr = 0;
        dense_tensor_ctrl<2, int> tctrl(t);
        try {
            ptr = tctrl.req_dataptr();
        } catch(exception &e) {
            m_ok = true;
        }
        if(ptr) {
            tctrl.ret_dataptr(ptr);
            ptr = 0;
        }
    }

};

} // unnamed namespace


int test_immutable() {

    static const char testname[] = "dense_tensor_test::test_immutable()";

    typedef allocator<int> allocator;

    try {

    index<2> i1, i2;
    i2[0] = 2;
    i2[1] = 3;
    index_range<2> ir(i1, i2);
    dimensions<2> d1(ir);
    dense_tensor<2, int, allocator> t1(d1);

    if(t1.is_immutable()) {
        return fail_test(testname, __FILE__, __LINE__, "New tensor t1 is not mutable");
    }

    t1.set_immutable();

    if(!t1.is_immutable()) {
        return fail_test(testname, __FILE__, __LINE__, "Setting t1 immutable failed");
    }

    op_chk_imm op;
    op.perform(t1);
    if(!op.is_ok()) {
        return fail_test(testname, __FILE__, __LINE__,
            "Requesting non-const pointer in t1 must fail");
    }

    } catch(exception &e) {
        return fail_test(testname, __FILE__, __LINE__, e.what());
    }

    return 0;
}


namespace {

/** Sets all elements a given value
 **/
class op_set_int {
private:
    int m_val; //!< Value to set

public:
    op_set_int(const int val) : m_val(val) {

    }

    void perform(dense_tensor_i<2, int> &t) throw(exception) {

        dimensions<2> d(t.get_dims());
        dense_tensor_ctrl<2, int> tctrl(t);
        int *ptr = tctrl.req_dataptr();
        if(ptr) {
            for(size_t i = 0; i < d.get_size(); i++) ptr[i] = m_val;
        }
        tctrl.ret_dataptr(ptr);
    }

};


/** Check that all elements have a given value
 **/
class op_chkset_int {
private:
    int m_val; //!< Value
    bool m_ok; //!< Indicates a positive result

public:
    op_chkset_int(const int val) : m_val(val), m_ok(false) {

    }

    bool is_ok() const {
        return m_ok;
    }

    void perform(dense_tensor_i<2, int> &t) {

        m_ok = true;
        dimensions<2> d(t.get_dims());
        dense_tensor_ctrl<2, int> tctrl(t);
        const int *ptr = tctrl.req_const_dataptr();
        if(ptr) {
            for(size_t i = 0; i < d.get_size(); i++) {
                m_ok = m_ok && (ptr[i] == m_val);
            }
        }
        tctrl.ret_const_dataptr(ptr);
    }

};


/** Checks that double requests for data cause an exception
 **/
class op_chk_dblreq {
private:
    bool m_ok;

public:
    op_chk_dblreq() : m_ok(false) {

    }

    bool is_ok() const {
        return m_ok;
    }

    void perform(dense_tensor_i<2, int> &t) {

        m_ok = true;
        dense_tensor_ctrl<2, int> tctrl(t);

        // After rw-checkout, ro-checkout is not allowed
        int *ptr = tctrl.req_dataptr();
        try {
            int *ptr2 = tctrl.req_dataptr();
            m_ok = false;
        } catch(exception &e) {
        }
        try {
            const int *ptr2 = tctrl.req_const_dataptr();
            m_ok = false;
        } catch(exception &e) {
        }
        tctrl.ret_dataptr(ptr);

        // After ro-checkout, rw-checkout is not allowed
        const int *const_ptr = tctrl.req_const_dataptr();
        try {
            int *ptr2 = tctrl.req_dataptr();
            m_ok = false;
        } catch(exception &e) {
        }

        // Multiple ro-checkouts are allowed
        try {
            const int *ptr2 = tctrl.req_const_dataptr();
            tctrl.ret_const_dataptr(ptr2);
        } catch(exception &e) {
            m_ok = false;
        }
        tctrl.ret_const_dataptr(const_ptr);
    }

};

} // unnamed namespace


int test_operation() {

    static const char testname[] = "dense_tensor_test::test_operation()";

    typedef allocator<int> allocator;

    try {

    index<2> i1, i2;
    i2[0] = 2;
    i2[1] = 3;
    index_range<2> ir(i1, i2);
    dimensions<2> d1(ir);
    dense_tensor<2, int, allocator> t1(d1);

    op_set_int op1(1), op100(100);
    op_chkset_int chkop1(1), chkop100(100);

    op1.perform(t1);
    chkop1.perform(t1);
    if(!chkop1.is_ok()) {
        return fail_test(testname, __FILE__, __LINE__,
            "Operation failed to set all elements to 1 (t1)");
    }
    op100.perform(t1);
    chkop100.perform(t1);
    if(!chkop100.is_ok()) {
        return fail_test(testname, __FILE__, __LINE__,
            "Operation failed to set all elements to 100 (t1)");
    }

    op_chk_dblreq op_dblreq;
    op_dblreq.perform(t1);
    if(!op_dblreq.is_ok()) {
        return fail_test(testname, __FILE__, __LINE__,
            "Double requests for data must cause an exception");
    }

    } catch(exception &e) {
        return fail_test(testname, __FILE__, __LINE__, e.what());
    }

    return 0;
}


int test_1() {

    static const char testname[] = "dense_tensor_test::test_1()";

    typedef allocator<double> allocator;

    try {

    index<2> i1, i2;
    i2[0] = 5;
    i2[1] = 5;
    dimensions<2> dims(index_range<2> (i1, i2));

    dense_tensor<2, double, allocator> t(dims);

    dense_tensor_ctrl<2, double> c1(t);
    const double *p1 = c1.req_const_dataptr();

    {
        dense_tensor_ctrl<2, double> c2(t);
        const double *p2 = c2.req_const_dataptr();
        c2.ret_const_dataptr(p2);
        p2 = 0;
    }

    c1.ret_const_dataptr(p1);
    p1 = 0;

    } catch(exception &e) {
        return fail_test(testname, __FILE__, __LINE__, e.what());
    }

    return 0;
}


/** \test Opens and closes 33 sessions with a tensor
 **/
int test_2() {

    static const char testname[] = "dense_tensor_test::test_2()";

    typedef allocator<double> allocator;

    try {

    index<2> i1, i2;
    i2[0] = 5;
    i2[1] = 5;
    dimensions<2> dims(index_range<2> (i1, i2));

    dense_tensor<2, double, allocator> t(dims);

    dense_tensor_ctrl<2, double> c00(t), c01(t), c02(t), c03(t), c04(t), c05(t),
        c06(t), c07(t), c08(t), c09(t), c10(t), c11(t), c12(t), c13(t),
        c14(t), c15(t), c16(t), c17(t), c18(t), c19(t), c20(t), c21(t),
        c22(t), c23(t), c24(t), c25(t), c26(t), c27(t), c28(t), c29(t),
        c30(t), c31(t), c32(t);

    const double *p00, *p01, *p02, *p03, *p04, *p05, *p06, *p07, *p08,
        *p09, *p10, *p11, *p12, *p13, *p14, *p15, *p16, *p17, *p18, *p19,
        *p20, *p21, *p22, *p23, *p24, *p25, *p26, *p27, *p28, *p29, *p30,
        *p31, *p32;

    p00 = c00.req_const_dataptr();
    p01 = c01.req_const_dataptr();
    p02 = c02.req_const_dataptr();
    p03 = c03.req_const_dataptr();
    p04 = c04.req_const_dataptr();
    p05 = c05.req_const_dataptr();
    p06 = c06.req_const_dataptr();
    p07 = c07.req_const_dataptr();
    p08 = c08.req_const_dataptr();
    p09 = c09.req_const_dataptr();
    p10 = c10.req_const_dataptr();
    p11 = c11.req_const_dataptr();
    p12 = c12.req_const_dataptr();
    p13 = c13.req_const_dataptr();
    p14 = c14.req_const_dataptr();
    p15 = c15.req_const_dataptr();
    p16 = c16.req_const_dataptr();
    p17 = c17.req_const_dataptr();
    p18 = c18.req_const_dataptr();
    p19 = c19.req_const_dataptr();
    p20 = c20.req_const_dataptr();
    p21 = c21.req_const_dataptr();
    p22 = c22.req_const_dataptr();
    p23 = c23.req_const_dataptr();
    p24 = c24.req_const_dataptr();
    p25 = c25.req_const_dataptr();
    p26 = c26.req_const_dataptr();
    p27 = c27.req_const_dataptr();
    p28 = c28.req_const_dataptr();
    p29 = c29.req_const_dataptr();
    p30 = c30.req_const_dataptr();
    p31 = c31.req_const_dataptr();
    p32 = c32.req_const_dataptr();

    c00.ret_const_dataptr(p00);
    c01.ret_const_dataptr(p01);
    c02.ret_const_dataptr(p02);
    c03.ret_const_dataptr(p03);
    c04.ret_const_dataptr(p04);
    c05.ret_const_dataptr(p05);
    c06.ret_const_dataptr(p06);
    c07.ret_const_dataptr(p07);
    c08.ret_const_dataptr(p08);
    c09.ret_const_dataptr(p09);
    c10.ret_const_dataptr(p10);
    c11.ret_const_dataptr(p11);
    c12.ret_const_dataptr(p12);
    c13.ret_const_dataptr(p13);
    c14.ret_const_dataptr(p14);
    c15.ret_const_dataptr(p15);
    c16.ret_const_dataptr(p16);
    c17.ret_const_dataptr(p17);
    c18.ret_const_dataptr(p18);
    c19.ret_const_dataptr(p19);
    c20.ret_const_dataptr(p20);
    c21.ret_const_dataptr(p21);
    c22.ret_const_dataptr(p22);
    c23.ret_const_dataptr(p23);
    c24.ret_const_dataptr(p24);
    c25.ret_const_dataptr(p25);
    c26.ret_const_dataptr(p26);
    c27.ret_const_dataptr(p27);
    c28.ret_const_dataptr(p28);
    c29.ret_const_dataptr(p29);
    c30.ret_const_dataptr(p30);
    c31.ret_const_dataptr(p31);
    c32.ret_const_dataptr(p32);

    } catch(exception &e) {
        return fail_test(testname, __FILE__, __LINE__, e.what());
    }

    return 0;
}


namespace {

class thread_1 : public libutil::thread {
private:
    dense_tensor_i<1, double> &m_t1;
    dense_tensor_i<1, double> &m_t2;
    dense_tensor_i<1, double> &m_t3;
    size_t m_n;
    bool m_ok;
    std::string m_error;

public:
    thread_1(dense_tensor_i<1, double> &t1, dense_tensor_i<1, double> &t2,
        dense_tensor_i<1, double> &t3, size_t n) :
        m_t1(t1), m_t2(t2), m_t3(t3), m_n(n), m_ok(true) { }
    virtual ~thread_1() { }
    virtual void run() {
        m_ok = true;
        try {
        for(size_t i = 0; i < m_n; i++) {
            size_t sz = m_t1.get_dims().get_size();
            dense_tensor_ctrl<1, double> c1(m_t1), c2(m_t2), c3(m_t3);
            const double *p1 = c1.req_const_dataptr();
            const double *p2 = c2.req_const_dataptr();
            double *p3 = c3.req_dataptr();

            for(size_t j = 0; j < sz; j++) p3[j] = p1[j] + p2[j];

            c3.ret_dataptr(p3);
            c2.ret_const_dataptr(p2);
            c1.ret_const_dataptr(p1);
        }
        } catch(std::exception &e) {
            m_error = e.what();
            m_ok = false;
        } catch(...) {
            m_ok = false;
        }
    }
    bool is_ok() const {
        return m_ok;
    }
    const std::string &get_error() const {
        return m_error;
    }
};

} // unnamed namespace


int test_mp_1() {

    static const char testname[] = "dense_tensor_test::test_mp_1()";

    typedef allocator<double> allocator_t;

    try {

    index<2> i1, i2;
    i2[0] = 10; i2[1] = 10;
    dimensions<2> dims(index_range<2>(i1, i2));
    size_t sz = dims.get_size();

    dense_tensor<2, double, allocator_t> t1(dims);
    dense_tensor_ctrl<2, double> c1(t1);

    c1.req_prefetch();

    double *p1 = c1.req_dataptr();
    for(size_t i = 0; i < sz; i++) p1[i] = (double)i;
    c1.ret_dataptr(p1); p1 = 0;

    const double *p2 = c1.req_const_dataptr();
    for(size_t i = 0; i < sz; i++) {
        if(p2[i] != (double)i) {
            return fail_test(testname, __FILE__, __LINE__,
                "Data corruption detected.");
        }
    }
    c1.ret_const_dataptr(p2);

    } catch(exception &e) {
        return fail_test(testname, __FILE__, __LINE__, e.what());
    }

    return 0;
}


int test_mp_2() {

    static const char testname[] = "dense_tensor_test::test_mp_2()";

    typedef allocator<double> allocator_t;

    try {

    index<2> i1, i2;
    i2[0] = 10; i2[1] = 10;
    dimensions<2> dims(index_range<2>(i1, i2));
    size_t sz = dims.get_size();

    dense_tensor<2, double, allocator_t> t1(dims);
    dense_tensor_ctrl<2, double> c1(t1), c2(t1);

    c1.req_prefetch();

    double *p1 = c1.req_dataptr();
    for(size_t i = 0; i < sz; i++) p1[i] = (double)i;
    c1.ret_dataptr(p1); p1 = 0;

    const double *p2 = c1.req_const_dataptr();
    const double *p3 = c2.req_const_dataptr();
    for(size_t i = 0; i < sz; i++) {
        if(p2[i] != (double)i || p3[i] != (double)i) {
            return fail_test(testname, __FILE__, __LINE__,
                "Data corruption detected (1).");
        }
    }
    c1.ret_const_dataptr(p2); p2 = 0;
    c2.ret_const_dataptr(p3); p3 = 0;

    p1 = c2.req_dataptr();
    for(size_t i = 0; i < sz; i++) p1[i] = (double)(i * 2);
    c1.ret_dataptr(p1); p1 = 0;

    p2 = c1.req_const_dataptr();
    p3 = c2.req_const_dataptr();
    for(size_t i = 0; i < sz; i++) {
        if(p2[i] != (double)(i * 2) || p3[i] != (double)(i * 2)) {
            return fail_test(testname, __FILE__, __LINE__,
                "Data corruption detected (2).");
        }
    }
    c1.ret_const_dataptr(p2); p2 = 0;
    c2.ret_const_dataptr(p3); p3 = 0;

    } catch(exception &e) {
        return fail_test(testname, __FILE__, __LINE__, e.what());
    }

    return 0;
}


int test_mp_3() {

    static const char testname[] = "dense_tensor_test::test_mp_3()";

    typedef allocator<double> allocator_t;

    try {

    index<1> i1, i2;
    i2[0] = 999;
    dimensions<1> dims(index_range<1>(i1, i2));

    dense_tensor<1, double, allocator_t> t1(dims), t2(dims), t3a(dims),
        t3b(dims), t3c(dims), t3d(dims);
    thread_1 thra(t1, t2, t3a, 50);
    thread_1 thrb(t1, t2, t3b, 20);
    thread_1 thrc(t1, t2, t3c, 100);
    thread_1 thrd(t1, t2, t3d, 60);

    thra.start();
    thrb.start();
    thrc.start();
    thrd.start();

    thra.join();
    thrb.join();
    thrc.join();
    thrd.join();

    if(!thra.is_ok()) {
        std::ostringstream ss;
        ss << "Thread A failed (" << thra.get_error() << ").";
        return fail_test(testname, __FILE__, __LINE__, ss.str().c_str());
    }
    if(!thrb.is_ok()) {
        std::ostringstream ss;
        ss << "Thread B failed (" << thrb.get_error() << ").";
        return fail_test(testname, __FILE__, __LINE__, ss.str().c_str());
    }
    if(!thrc.is_ok()) {
        std::ostringstream ss;
        ss << "Thread C failed (" << thrc.get_error() << ").";
        return fail_test(testname, __FILE__, __LINE__, ss.str().c_str());
    }
    if(!thrd.is_ok()) {
        std::ostringstream ss;
        ss << "Thread D failed (" << thrd.get_error() << ").";
        return fail_test(testname, __FILE__, __LINE__, ss.str().c_str());
    }

    } catch(exception &e) {
        return fail_test(testname, __FILE__, __LINE__, e.what());
    }

    return 0;
}


int main() {

    return

    test_ctor() |
    test_immutable() |
    test_operation() |
    test_1() |
    test_2() |

    0;
}


