#include <gtest/gtest.h>

#include <vector>
#include <cstdint>

#include "tatami_stats/range.hpp"
#include "tatami_test/tatami_test.hpp"

template<typename Value_>
static void compare_result(
    const tatami_stats::RangeResult<Value_>& res, 
    const std::vector<Value_>& min,
    const std::vector<Value_>& max
) {
    EXPECT_EQ(res.minimum, min);
    EXPECT_EQ(res.maximum, max);
}

class RangeTest : public ::testing::TestWithParam<std::pair<double, double> > {};

TEST_P(RangeTest, RowSimple) {
    size_t NR = 75, NC = 62;
    auto limits = GetParam();
    auto dump = tatami_test::simulate_vector<double>(NR * NC, [&]{
        tatami_test::SimulateVectorOptions opt;
        opt.density = 0.1;
        opt.lower = limits.first;
        opt.upper = limits.second;
        opt.seed = 1239817 + limits.first * 1000 + limits.second;
        return opt;
    }());

    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, dump));
    auto dense_column = tatami::convert_to_dense<double, int>(*dense_row, false, {});
    auto sparse_row = tatami::convert_to_compressed_sparse<double, int>(*dense_row, true, {});
    auto sparse_column = tatami::convert_to_compressed_sparse<double, int>(*dense_row, false, {});

    std::vector<double> refmin(NR), refmax(NR);
    for (size_t r = 0; r < NR; ++r) {
        auto x = dump[r * NC];
        refmin[r] = x;
        refmax[r] = x;
        for (size_t c = 1; c < NC; ++c) {
            auto x = dump[c + r * NC];
            refmin[r] = std::min(refmin[r], x);
            refmax[r] = std::max(refmax[r], x);
        }
    }

    compare_result(tatami_stats::range(true, *dense_row, {}), refmin, refmax);
    compare_result(tatami_stats::range(true, *dense_column, {}), refmin, refmax);
    compare_result(tatami_stats::range(true, *sparse_row, {}), refmin, refmax);
    compare_result(tatami_stats::range(true, *sparse_column, {}), refmin, refmax);

    // Same results from parallel code.
    tatami_stats::RangeOptions ropt;
    ropt.num_threads = 3;
    compare_result(tatami_stats::range(true, *dense_row, ropt), refmin, refmax);
    compare_result(tatami_stats::range(true, *dense_column, ropt), refmin, refmax);
    compare_result(tatami_stats::range(true, *sparse_row, ropt), refmin, refmax);
    compare_result(tatami_stats::range(true, *sparse_column, ropt), refmin, refmax);

    // Same results upon skipping NaN.
    ropt.num_threads = 1;
    ropt.skip_nan = true;
    compare_result(tatami_stats::range(true, *dense_row, ropt), refmin, refmax);
    compare_result(tatami_stats::range(true, *dense_column, ropt), refmin, refmax);
    compare_result(tatami_stats::range(true, *sparse_row, ropt), refmin, refmax);
    compare_result(tatami_stats::range(true, *sparse_column, ropt), refmin, refmax);

    // Checking same results from matrices that can yield unsorted indices.
    std::shared_ptr<tatami::NumericMatrix> unsorted_row(new tatami_test::ReversedIndicesWrapper<double, int>(sparse_row));
    compare_result(tatami_stats::range(true, *unsorted_row, {}), refmin, refmax);
    std::shared_ptr<tatami::NumericMatrix> unsorted_column(new tatami_test::ReversedIndicesWrapper<double, int>(sparse_column));
    compare_result(tatami_stats::range(true, *unsorted_column, {}), refmin, refmax);
}

TEST_P(RangeTest, RowSkipNan) {
    size_t NR = 52, NC = 83;
    auto limits = GetParam();
    auto dump = tatami_test::simulate_vector<double>(NR * NC, [&]{
        tatami_test::SimulateVectorOptions opt;
        opt.density = 0.1;
        opt.lower = limits.first;
        opt.upper = limits.second;
        opt.seed = 543343 + limits.first * 1000 + limits.second;
        return opt;
    }());

    for (int i = 0; i < 2; ++i) {
        auto copy = dump;
        if (i == 0) {
            // Injecting NaNs at the first column.
            for (size_t r = 0; r < NR; ++r) {
                copy[r * NC] = std::numeric_limits<double>::quiet_NaN();
            }
        } else {
            // Injecting NaNs at the last column.
            for (size_t r = 0; r < NR; ++r) {
                copy[r * NC + (NC - 1)] = std::numeric_limits<double>::quiet_NaN();
            }
        }

        auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, copy));
        auto dense_column = tatami::convert_to_dense<double, int>(*dense_row, false, {});
        auto sparse_row = tatami::convert_to_compressed_sparse<double, int>(*dense_row, true, {});
        auto sparse_column = tatami::convert_to_compressed_sparse<double, int>(*dense_row, false, {});

        std::pair<std::vector<double>, std::vector<double> > ref;
        auto& refmin = ref.first;
        refmin.resize(NR, std::numeric_limits<double>::infinity());
        auto& refmax = ref.second;
        refmax.resize(NR, -std::numeric_limits<double>::infinity());

        for (size_t r = 0; r < NR; ++r) {
            if (i == 0) {
                for (size_t c = 1; c < NC; ++c) { // skipping the first column.
                    double x = copy[c + r * NC];
                    refmin[r] = std::min(refmin[r], x);
                    refmax[r] = std::max(refmax[r], x);
                }
            } else {
                for (size_t c = 0; c < NC - 1; ++c) { // skipping the last column.
                    double x = copy[c + r * NC];
                    refmin[r] = std::min(refmin[r], x);
                    refmax[r] = std::max(refmax[r], x);
                }
            }
        }

        tatami_stats::RangeOptions ropt;
        ropt.skip_nan = true;
        compare_result(tatami_stats::range(true, *dense_row, ropt), refmin, refmax);
        compare_result(tatami_stats::range(true, *dense_column, ropt), refmin, refmax);
        compare_result(tatami_stats::range(true, *sparse_row, ropt), refmin, refmax);
        compare_result(tatami_stats::range(true, *sparse_column, ropt), refmin, refmax);

        ropt.num_threads = 3;
        compare_result(tatami_stats::range(true, *dense_row, ropt), refmin, refmax);
        compare_result(tatami_stats::range(true, *dense_column, ropt), refmin, refmax);
        compare_result(tatami_stats::range(true, *sparse_row, ropt), refmin, refmax);
        compare_result(tatami_stats::range(true, *sparse_column, ropt), refmin, refmax);
    }
}

TEST_P(RangeTest, ColumnSimple) {
    size_t NR = 111, NC = 52;
    auto limits = GetParam();
    auto dump = tatami_test::simulate_vector<double>(NR * NC, [&]{
        tatami_test::SimulateVectorOptions opt;
        opt.density = 0.1;
        opt.lower = limits.first;
        opt.upper = limits.second;
        opt.seed = 9919998 + limits.first * 1000 + limits.second;
        return opt;
    }());

    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, dump));
    auto dense_column = tatami::convert_to_dense<double, int>(*dense_row, false, {});
    auto sparse_row = tatami::convert_to_compressed_sparse<double, int>(*dense_row, true, {});
    auto sparse_column = tatami::convert_to_compressed_sparse<double, int>(*dense_row, false, {});

    std::vector<double> refmin(NC), refmax(NC);
    for (size_t c = 0; c < NC; ++c) {
        auto x = dump[c];
        refmin[c] = x;
        refmax[c] = x;
        for (size_t r = 1; r < NR; ++r) {
            auto x = dump[c + r * NC];
            refmin[c] = std::min(refmin[c], x);
            refmax[c] = std::max(refmax[c], x);
        }
    }

    compare_result(tatami_stats::range(false, *dense_row, {}), refmin, refmax);
    compare_result(tatami_stats::range(false, *dense_column, {}), refmin, refmax);
    compare_result(tatami_stats::range(false, *sparse_row, {}), refmin, refmax);
    compare_result(tatami_stats::range(false, *sparse_column, {}), refmin, refmax);

    // Same results from parallel code.
    tatami_stats::RangeOptions ropt;
    ropt.num_threads = 3;
    compare_result(tatami_stats::range(false, *dense_row, ropt), refmin, refmax);
    compare_result(tatami_stats::range(false, *dense_column, ropt), refmin, refmax);
    compare_result(tatami_stats::range(false, *sparse_row, ropt), refmin, refmax);
    compare_result(tatami_stats::range(false, *sparse_column, ropt), refmin, refmax);

    // Same results with skipped NaNs.
    ropt.num_threads = 1;
    ropt.skip_nan = true;
    compare_result(tatami_stats::range(false, *dense_row, ropt), refmin, refmax);
    compare_result(tatami_stats::range(false, *dense_column, ropt), refmin, refmax);
    compare_result(tatami_stats::range(false, *sparse_row, ropt), refmin, refmax);
    compare_result(tatami_stats::range(false, *sparse_column, ropt), refmin, refmax);

    // Checking same results from matrices that can yield unsorted indices.
    std::shared_ptr<tatami::NumericMatrix> unsorted_row(new tatami_test::ReversedIndicesWrapper<double, int>(sparse_row));
    compare_result(tatami_stats::range(false, *unsorted_row, {}), refmin, refmax);
    std::shared_ptr<tatami::NumericMatrix> unsorted_column(new tatami_test::ReversedIndicesWrapper<double, int>(sparse_column));
    compare_result(tatami_stats::range(false, *unsorted_column, {}), refmin, refmax);
}

TEST_P(RangeTest, ColumnSkipNan) {
    size_t NR = 52, NC = 83;
    auto limits = GetParam();
    auto dump = tatami_test::simulate_vector<double>(NR * NC, [&]{
        tatami_test::SimulateVectorOptions opt;
        opt.density = 0.1;
        opt.lower = limits.first;
        opt.upper = limits.second;
        opt.seed = 1919191 + limits.first * 1000 + limits.second;
        return opt;
    }());

    for (int i = 0; i < 2; ++i) {
        auto copy = dump;
        if (i == 0) {
            // Injecting NaNs throughout the first row.
            for (size_t c = 0; c < NC; ++c) {
                copy[c] = std::numeric_limits<double>::quiet_NaN();
            }
        } else {
            for (size_t c = 0; c < NC; ++c) {
                copy[c + (NR - 1) * NC] = std::numeric_limits<double>::quiet_NaN();
            }
        }

        auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, copy));
        auto dense_column = tatami::convert_to_dense<double, int>(*dense_row, false, {});
        auto sparse_row = tatami::convert_to_compressed_sparse<double, int>(*dense_row, true, {});
        auto sparse_column = tatami::convert_to_compressed_sparse<double, int>(*dense_row, false, {});

        std::pair<std::vector<double>, std::vector<double> > ref;
        auto& refmin = ref.first;
        refmin.resize(NC, std::numeric_limits<double>::infinity());
        auto& refmax = ref.second;
        refmax.resize(NC, -std::numeric_limits<double>::infinity());

        for (size_t c = 0; c < NC; ++c) {
            if (i == 0) {
                for (size_t r = 1; r < NR; ++r) { // skipping the first row.
                    double x = copy[c + r * NC];
                    refmin[c] = std::min(refmin[c], x);
                    refmax[c] = std::max(refmax[c], x);
                }
            } else {
                for (size_t r = 0; r < NR - 1; ++r) { // skipping the last row.
                    double x = copy[c + r * NC];
                    refmin[c] = std::min(refmin[c], x);
                    refmax[c] = std::max(refmax[c], x);
                }
            }
        }

        tatami_stats::RangeOptions ropt;
        ropt.skip_nan = true;
        compare_result(tatami_stats::range(false, *dense_row, ropt), refmin, refmax);
        compare_result(tatami_stats::range(false, *dense_column, ropt), refmin, refmax);
        compare_result(tatami_stats::range(false, *sparse_row, ropt), refmin, refmax);
        compare_result(tatami_stats::range(false, *sparse_column, ropt), refmin, refmax);

        ropt.num_threads = 3;
        compare_result(tatami_stats::range(false, *dense_row, ropt), refmin, refmax);
        compare_result(tatami_stats::range(false, *dense_column, ropt), refmin, refmax);
        compare_result(tatami_stats::range(false, *sparse_row, ropt), refmin, refmax);
        compare_result(tatami_stats::range(false, *sparse_column, ropt), refmin, refmax);
    }
}

INSTANTIATE_TEST_SUITE_P(
    Range,
    RangeTest,
    ::testing::Values(
        std::make_pair(0.1, 10.0),   // only above.
        std::make_pair(-10.0, -0.4), // only below
        std::make_pair(-5.0, 5.0)    // mix of values above and below zero.
    ) 
);

/********************************************/

TEST(Range, AllZeros) {
    // Testing for correct sparse behavior with all-zeros.
    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(10, 20, std::vector<double>(200)));
    auto dense_column = tatami::convert_to_dense<double, int>(*dense_row, false, {});
    auto sparse_row = tatami::convert_to_compressed_sparse<double, int>(*dense_row, true, {});
    auto sparse_column = tatami::convert_to_compressed_sparse<double, int>(*dense_row, false, {});

    std::vector<double> cref(20);
    std::vector<double> rref(10);
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            tatami_stats::RangeOptions ropt;
            ropt.skip_nan = (i > 0);
            ropt.num_threads = (j > 0);

            compare_result(tatami_stats::range(false, *dense_row, ropt), cref, cref);
            compare_result(tatami_stats::range(false, *dense_column, ropt), cref, cref);
            compare_result(tatami_stats::range(false, *sparse_row, ropt), cref, cref);
            compare_result(tatami_stats::range(false, *sparse_column, ropt), cref, cref);

            compare_result(tatami_stats::range(true, *dense_row, ropt), rref, rref);
            compare_result(tatami_stats::range(true, *dense_column, ropt), rref, rref);
            compare_result(tatami_stats::range(true, *sparse_row, ropt), rref, rref);
            compare_result(tatami_stats::range(true, *sparse_column, ropt), rref, rref);
        }
    }
}

TEST(Range, NoZeros) {
    // Testing for correct behavior with no zeros.
    std::vector<double> stuff(200);
    for (size_t s = 0; s < stuff.size(); ++s) {
        stuff[s] = s + 1;
    }

    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(10, 20, stuff));
    auto dense_column = tatami::convert_to_dense<double, int>(*dense_row, false, {});
    auto sparse_row = tatami::convert_to_compressed_sparse<double, int>(*dense_row, true, {});
    auto sparse_column = tatami::convert_to_compressed_sparse<double, int>(*dense_row, false, {});

    auto cref = tatami_stats::range(false, *dense_row, {});
    auto rref = tatami_stats::range(true, *dense_row, {});

    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            tatami_stats::RangeOptions ropt;
            ropt.skip_nan = (i > 0);
            ropt.num_threads = (j > 0);

            compare_result(tatami_stats::range(false, *dense_column, ropt), cref.minimum, cref.maximum);
            compare_result(tatami_stats::range(false, *dense_column, ropt), cref.minimum, cref.maximum);
            compare_result(tatami_stats::range(false, *sparse_row, ropt), cref.minimum, cref.maximum);
            compare_result(tatami_stats::range(false, *sparse_column, ropt), cref.minimum, cref.maximum);

            compare_result(tatami_stats::range(true, *dense_row, ropt), rref.minimum, rref.maximum);
            compare_result(tatami_stats::range(true, *dense_column, ropt), rref.minimum, rref.maximum);
            compare_result(tatami_stats::range(true, *sparse_row, ropt), rref.minimum, rref.maximum);
            compare_result(tatami_stats::range(true, *sparse_column, ropt), rref.minimum, rref.maximum);
        }
    }
}

TEST(Range, NewType) {
    size_t NR = 198, NC = 52;
    auto dump = tatami_test::simulate_vector<double>(NR * NC, []{
        tatami_test::SimulateVectorOptions opt;
        opt.density = 0.1;
        opt.lower = 1;
        opt.upper = 100;
        opt.seed = 29842;
        return opt;
    }());
    for (auto& d : dump) { 
        d = std::round(d);
    }

    auto ref = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, dump));
    auto rexpected = tatami_stats::range(true, *ref, {});
    std::vector<std::int8_t> rexpected_min(rexpected.minimum.begin(), rexpected.minimum.end());
    std::vector<std::int8_t> rexpected_max(rexpected.maximum.begin(), rexpected.maximum.end());
    auto cexpected = tatami_stats::range(false, *ref, {});
    std::vector<std::int8_t> cexpected_min(cexpected.minimum.begin(), cexpected.minimum.end());
    std::vector<std::int8_t> cexpected_max(cexpected.maximum.begin(), cexpected.maximum.end());

    std::vector<std::int8_t> ivec(dump.begin(), dump.end());
    auto dense_row = std::make_shared<tatami::DenseRowMatrix<std::int8_t, std::uint8_t> >(NR, NC, std::move(ivec));
    auto dense_column = tatami::convert_to_dense<std::int8_t, std::uint8_t>(*dense_row, false, {});
    auto sparse_row = tatami::convert_to_compressed_sparse<std::int8_t, std::uint8_t>(*dense_row, true, {});
    auto sparse_column = tatami::convert_to_compressed_sparse<std::int8_t, std::uint8_t>(*dense_row, false, {});

    compare_result(tatami_stats::range(true, *dense_row, {}), rexpected_min, rexpected_max);
    compare_result(tatami_stats::range(true, *dense_column, {}), rexpected_min, rexpected_max);
    compare_result(tatami_stats::range(true, *sparse_row, {}), rexpected_min, rexpected_max);
    compare_result(tatami_stats::range(true, *sparse_column, {}), rexpected_min, rexpected_max);

    compare_result(tatami_stats::range(false, *dense_row, {}), cexpected_min, cexpected_max);
    compare_result(tatami_stats::range(false, *dense_column, {}), cexpected_min, cexpected_max);
    compare_result(tatami_stats::range(false, *sparse_row, {}), cexpected_min, cexpected_max);
    compare_result(tatami_stats::range(false, *sparse_column, {}), cexpected_min, cexpected_max);

    // Trying with skipping.
    tatami_stats::RangeOptions ropt;
    ropt.skip_nan = true;

    compare_result(tatami_stats::range(true, *dense_row, ropt), rexpected_min, rexpected_max);
    compare_result(tatami_stats::range(true, *dense_column, ropt), rexpected_min, rexpected_max);
    compare_result(tatami_stats::range(true, *sparse_row, ropt), rexpected_min, rexpected_max);
    compare_result(tatami_stats::range(true, *sparse_column, ropt), rexpected_min, rexpected_max);

    compare_result(tatami_stats::range(false, *dense_row, ropt), cexpected_min, cexpected_max);
    compare_result(tatami_stats::range(false, *dense_column, ropt), cexpected_min, cexpected_max);
    compare_result(tatami_stats::range(false, *sparse_row, ropt), cexpected_min, cexpected_max);
    compare_result(tatami_stats::range(false, *sparse_column, ropt), cexpected_min, cexpected_max);
}

TEST(Range, Empty) {
    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(10, 0, std::vector<double>()));
    auto cres = tatami_stats::range(false, *dense_row, {});
    EXPECT_EQ(cres.minimum.size(), 0);
    EXPECT_EQ(cres.maximum.size(), 0);

    auto rres = tatami_stats::range(true, *dense_row, {});
    EXPECT_EQ(rres.minimum, std::vector<double>(10, std::numeric_limits<double>::infinity()));
    EXPECT_EQ(rres.maximum, std::vector<double>(10, -std::numeric_limits<double>::infinity()));

    // Trying with sparsity. 
    {
        auto sparse_row = tatami::convert_to_compressed_sparse<double, int>(*dense_row, false, {});
        auto csres = tatami_stats::range(false, *sparse_row, {});
        EXPECT_EQ(csres.minimum.size(), 0);
        EXPECT_EQ(csres.maximum.size(), 0);

        auto rsres = tatami_stats::range(true, *sparse_row, {});
        EXPECT_EQ(rsres.minimum, std::vector<double>(10, std::numeric_limits<double>::infinity()));
        EXPECT_EQ(rsres.maximum, std::vector<double>(10, -std::numeric_limits<double>::infinity()));
    }

    // Trying with column-major matrices. 
    {
        auto dense_row2 = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(0, 10, std::vector<double>()));
        auto rres = tatami_stats::range(false, *dense_row2, {});
        EXPECT_EQ(rres.minimum, std::vector<double>(10, std::numeric_limits<double>::infinity()));
        EXPECT_EQ(rres.maximum, std::vector<double>(10, -std::numeric_limits<double>::infinity()));

        rres = tatami_stats::range(true, *dense_row2, {});
        EXPECT_EQ(rres.minimum.size(), 0);
        EXPECT_EQ(rres.maximum.size(), 0);

        auto sparse_row2 = tatami::convert_to_compressed_sparse<double, int>(*dense_row2, false, {});
        auto sres = tatami_stats::range(false, *sparse_row2, {});
        EXPECT_EQ(sres.minimum, std::vector<double>(10, std::numeric_limits<double>::infinity()));
        EXPECT_EQ(sres.maximum, std::vector<double>(10, -std::numeric_limits<double>::infinity()));

        sres = tatami_stats::range(true, *sparse_row2, {});
        EXPECT_EQ(sres.minimum.size(), 0);
        EXPECT_EQ(sres.maximum.size(), 0);
    }

    // Trying with the integer types.
    {
        tatami::DenseRowMatrix<std::uint8_t, int, std::vector<std::uint8_t> > dense_row_i(10, 0, std::vector<std::uint8_t>());
        auto out = tatami_stats::range<std::uint8_t>(true, dense_row_i, {});
        EXPECT_EQ(out.minimum, std::vector<std::uint8_t>(10, 255));
        EXPECT_EQ(out.maximum, std::vector<std::uint8_t>(10, 0));

        tatami::DenseRowMatrix<std::int8_t, int, std::vector<std::int8_t> > dense_row_i2(0, 10, std::vector<std::int8_t>());
        auto out2 = tatami_stats::range(false, dense_row_i2, {});
        EXPECT_EQ(out2.minimum, std::vector<std::int8_t>(10, 127));
        EXPECT_EQ(out2.maximum, std::vector<std::int8_t>(10, -128));
    }
}
