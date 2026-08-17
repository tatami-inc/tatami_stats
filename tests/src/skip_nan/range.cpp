#include <gtest/gtest.h>

#include <vector>
#include <cstdint>

#include "tatami_stats/skip_nan/range.hpp"
#include "tatami_test/tatami_test.hpp"
#include "utils.h"

template<typename Value_, typename Count_>
static void compare_result(
    const tatami_stats::skip_nan::RangeResult<Value_, Count_>& res, 
    const std::vector<Value_>& min,
    const std::vector<Value_>& max,
    const std::vector<Count_>& count 
) {
    EXPECT_EQ(res.minimum, min);
    EXPECT_EQ(res.maximum, max);
    EXPECT_EQ(res.count, count);
}

/********************************************/

class SkipNanRangeSimpleTest : public ::testing::TestWithParam<std::tuple<std::pair<double, double>, SkipNanSimulationType, int> > {};

TEST_P(SkipNanRangeSimpleTest, Row) {
    auto params = GetParam();
    auto limits = std::get<0>(params);
    auto nan_type = std::get<1>(params);
    auto num_threads = std::get<2>(params);

    size_t NR = 75, NC = 62;
    unsigned long long seed = 1239817 + limits.first * 1000 + limits.second + num_threads * 10 + (int)nan_type * 100;
    auto dump = tatami_test::simulate_vector<double>(NR * NC, [&]{
        tatami_test::SimulateVectorOptions opt;
        opt.density = 0.1;
        opt.lower = limits.first;
        opt.upper = limits.second;
        opt.seed = seed;
        return opt;
    }());
    inject_nans_by_row(dump, NR, NC, nan_type, seed + 13);

    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, dump));
    auto dense_column = tatami::convert_to_dense<double, int>(*dense_row, false, {});
    auto sparse_row = tatami::convert_to_compressed_sparse<double, int>(*dense_row, true, {});
    auto sparse_column = tatami::convert_to_compressed_sparse<double, int>(*dense_row, false, {});

    std::vector<double> refmin(NR), refmax(NR);
    std::vector<int> refcount(NR);
    std::vector<double> buffer(NC);
    for (size_t r = 0; r < NR; ++r) {
        std::copy_n(dump.begin() + r * NC, NC, buffer.begin());
        auto remaining = tatami_stats::shift_nans(buffer.data(), NC);
        ASSERT_GT(remaining, 0);
        refmin[r] = *std::min_element(buffer.begin(), buffer.begin() + remaining);
        refmax[r] = *std::max_element(buffer.begin(), buffer.begin() + remaining);
        refcount[r] = remaining;
    }

    tatami_stats::skip_nan::RangeOptions<double> ropt;
    ropt.num_threads = num_threads;

    compare_result(tatami_stats::skip_nan::range(true, *dense_row, ropt), refmin, refmax, refcount);
    compare_result(tatami_stats::skip_nan::range(true, *dense_column, ropt), refmin, refmax, refcount);
    compare_result(tatami_stats::skip_nan::range(true, *sparse_row, ropt), refmin, refmax, refcount);
    compare_result(tatami_stats::skip_nan::range(true, *sparse_column, ropt), refmin, refmax, refcount);

    // Checking same results from matrices that can yield unsorted indices.
    std::shared_ptr<tatami::NumericMatrix> unsorted_row(new tatami_test::ReversedIndicesWrapper<double, int>(sparse_row));
    compare_result(tatami_stats::skip_nan::range(true, *unsorted_row, ropt), refmin, refmax, refcount);
    std::shared_ptr<tatami::NumericMatrix> unsorted_column(new tatami_test::ReversedIndicesWrapper<double, int>(sparse_column));
    compare_result(tatami_stats::skip_nan::range(true, *unsorted_column, ropt), refmin, refmax, refcount);
}

TEST_P(SkipNanRangeSimpleTest, Column) {
    auto params = GetParam();
    auto limits = std::get<0>(params);
    auto nan_type = std::get<1>(params);
    auto num_threads = std::get<2>(params);

    size_t NR = 111, NC = 52;
    unsigned long long seed = 9919998 + limits.first * 1000 + limits.second + num_threads * 5 + (int)nan_type * 20;
    auto dump = tatami_test::simulate_vector<double>(NR * NC, [&]{
        tatami_test::SimulateVectorOptions opt;
        opt.density = 0.1;
        opt.lower = limits.first;
        opt.upper = limits.second;
        opt.seed = seed;
        return opt;
    }());
    inject_nans_by_column(dump, NR, NC, nan_type, seed + 17);

    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, dump));
    auto dense_column = tatami::convert_to_dense<double, int>(*dense_row, false, {});
    auto sparse_row = tatami::convert_to_compressed_sparse<double, int>(*dense_row, true, {});
    auto sparse_column = tatami::convert_to_compressed_sparse<double, int>(*dense_row, false, {});

    std::vector<double> refmin(NC), refmax(NC);
    std::vector<int> refcount(NC);
    std::vector<double> buffer(NR);
    for (size_t c = 0; c < NC; ++c) {
        for (size_t r = 0; r < NR; ++r) {
            buffer[r] = dump[r * NC + c];
        }
        auto remaining = tatami_stats::shift_nans(buffer.data(), NR);
        ASSERT_GT(remaining, 0);
        refmin[c] = *std::min_element(buffer.begin(), buffer.begin() + remaining);
        refmax[c] = *std::max_element(buffer.begin(), buffer.begin() + remaining);
        refcount[c] = remaining;
    }

    tatami_stats::skip_nan::RangeOptions<double> ropt;
    ropt.num_threads = num_threads;

    compare_result(tatami_stats::skip_nan::range(false, *dense_row, ropt), refmin, refmax, refcount);
    compare_result(tatami_stats::skip_nan::range(false, *dense_column, ropt), refmin, refmax, refcount);
    compare_result(tatami_stats::skip_nan::range(false, *sparse_row, ropt), refmin, refmax, refcount);
    compare_result(tatami_stats::skip_nan::range(false, *sparse_column, ropt), refmin, refmax, refcount);

    // Checking same results from matrices that can yield unsorted indices.
    std::shared_ptr<tatami::NumericMatrix> unsorted_row(new tatami_test::ReversedIndicesWrapper<double, int>(sparse_row));
    compare_result(tatami_stats::skip_nan::range(false, *unsorted_row, ropt), refmin, refmax, refcount);
    std::shared_ptr<tatami::NumericMatrix> unsorted_column(new tatami_test::ReversedIndicesWrapper<double, int>(sparse_column));
    compare_result(tatami_stats::skip_nan::range(false, *unsorted_column, ropt), refmin, refmax, refcount);
}

INSTANTIATE_TEST_SUITE_P(
    SkipNanRange,
    SkipNanRangeSimpleTest,
    ::testing::Combine(
        ::testing::Values(
            std::make_pair(0.1, 10.0),   // only above.
            std::make_pair(-10.0, -0.4), // only below
            std::make_pair(-5.0, 5.0)    // mix of values above and below zero.
        ),
        ::testing::Values(NONE, RANDOM, BLOCK), 
        ::testing::Values(1, 3) // number of threads
    )
);

/********************************************/

class SkipNanRangeZeroTest : public ::testing::TestWithParam<std::tuple<SkipNanSimulationType, int> > {};

TEST_P(SkipNanRangeZeroTest, AllZeros) {
    // Testing for correct sparse behavior with all-zeros.
    auto params = GetParam();
    auto nan_type = std::get<0>(params);
    auto num_threads = std::get<1>(params);

    tatami_stats::skip_nan::RangeOptions ropt;
    ropt.num_threads = num_threads;
    size_t NR = 10, NC = 20;

    {
        std::vector<double> empty(NR * NC);
        inject_nans_by_row(empty, NR, NC, nan_type, /* seed = */ num_threads + 17);

        auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, empty));
        auto dense_column = tatami::convert_to_dense<double, int>(*dense_row, false, {});
        auto sparse_row = tatami::convert_to_compressed_sparse<double, int>(*dense_row, true, {});
        auto sparse_column = tatami::convert_to_compressed_sparse<double, int>(*dense_row, false, {});

        std::vector<double> rref(NR);
        std::vector<int> rcount(NR);
        for (size_t r = 0; r < NR; ++r) {
            for (size_t c = 0; c < NC; ++c) {
                rcount[r] += !std::isnan(empty[r * NC + c]);
            }
        }

        compare_result(tatami_stats::skip_nan::range(true, *dense_row, ropt), rref, rref, rcount);
        compare_result(tatami_stats::skip_nan::range(true, *dense_column, ropt), rref, rref, rcount);
        compare_result(tatami_stats::skip_nan::range(true, *sparse_row, ropt), rref, rref, rcount);
        compare_result(tatami_stats::skip_nan::range(true, *sparse_column, ropt), rref, rref, rcount);
    }

    {
        std::vector<double> empty(NR * NC);
        inject_nans_by_column(empty, NR, NC, nan_type, /* seed = */ num_threads + 23);

        auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, empty));
        auto dense_column = tatami::convert_to_dense<double, int>(*dense_row, false, {});
        auto sparse_row = tatami::convert_to_compressed_sparse<double, int>(*dense_row, true, {});
        auto sparse_column = tatami::convert_to_compressed_sparse<double, int>(*dense_row, false, {});

        std::vector<double> cref(NC);
        std::vector<int> ccount(NC);
        for (size_t c = 0; c < NC; ++c) {
            for (size_t r = 0; r < NR; ++r) {
                ccount[c] += !std::isnan(empty[r * NC + c]);
            }
        }

        compare_result(tatami_stats::skip_nan::range(false, *dense_row, ropt), cref, cref, ccount);
        compare_result(tatami_stats::skip_nan::range(false, *dense_column, ropt), cref, cref, ccount);
        compare_result(tatami_stats::skip_nan::range(false, *sparse_row, ropt), cref, cref, ccount);
        compare_result(tatami_stats::skip_nan::range(false, *sparse_column, ropt), cref, cref, ccount);
    }
}

TEST_P(SkipNanRangeZeroTest, NoZeros) {
    // Testing for correct behavior of the sparse algorithms when there are no structural non-zeros.
    auto params = GetParam();
    auto nan_type = std::get<0>(params);
    auto num_threads = std::get<1>(params);

    tatami_stats::skip_nan::RangeOptions ropt;
    ropt.num_threads = num_threads;
    size_t NR = 10, NC = 20;

    {
        std::vector<double> stuff(NR * NC);
        std::iota(stuff.begin(), stuff.end(), 1);
        inject_nans_by_row(stuff, NR, NC, nan_type, /* seed = */ num_threads + 17);

        auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, stuff));
        auto dense_column = tatami::convert_to_dense<double, int>(*dense_row, false, {});
        auto sparse_row = tatami::convert_to_compressed_sparse<double, int>(*dense_row, true, {});
        auto sparse_column = tatami::convert_to_compressed_sparse<double, int>(*dense_row, false, {});

        auto rref = tatami_stats::skip_nan::range(true, *dense_row, ropt);
        compare_result(tatami_stats::skip_nan::range(true, *dense_column, ropt), rref.minimum, rref.maximum, rref.count);
        compare_result(tatami_stats::skip_nan::range(true, *sparse_row, ropt), rref.minimum, rref.maximum, rref.count);
        compare_result(tatami_stats::skip_nan::range(true, *sparse_column, ropt), rref.minimum, rref.maximum, rref.count);
    }

    {
        std::vector<double> stuff(NR * NC);
        std::iota(stuff.begin(), stuff.end(), 1);
        inject_nans_by_column(stuff, NR, NC, nan_type, /* seed = */ num_threads + 19);

        auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, stuff));
        auto dense_column = tatami::convert_to_dense<double, int>(*dense_row, false, {});
        auto sparse_row = tatami::convert_to_compressed_sparse<double, int>(*dense_row, true, {});
        auto sparse_column = tatami::convert_to_compressed_sparse<double, int>(*dense_row, false, {});

        auto cref = tatami_stats::skip_nan::range(false, *dense_row, ropt);
        compare_result(tatami_stats::skip_nan::range(false, *dense_column, ropt), cref.minimum, cref.maximum, cref.count);
        compare_result(tatami_stats::skip_nan::range(false, *sparse_row, ropt), cref.minimum, cref.maximum, cref.count);
        compare_result(tatami_stats::skip_nan::range(false, *sparse_column, ropt), cref.minimum, cref.maximum, cref.count);
    }
}

INSTANTIATE_TEST_SUITE_P(
    SkipNanRange,
    SkipNanRangeZeroTest,
    ::testing::Combine(
        ::testing::Values(NONE, RANDOM, BLOCK), 
        ::testing::Values(1, 3) // number of threads
    )
);

/********************************************/

class SkipNanRangeEdgeTest : public ::testing::TestWithParam<int> {};

TEST_P(SkipNanRangeEdgeTest, Empty) {
    const auto num_threads = GetParam();

    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(10, 0, std::vector<double>()));

    tatami_stats::skip_nan::RangeOptions ropt;
    ropt.num_threads = num_threads;

    {
        auto cres = tatami_stats::skip_nan::range(false, *dense_row, ropt);
        EXPECT_EQ(cres.minimum.size(), 0);
        EXPECT_EQ(cres.maximum.size(), 0);

        auto rres = tatami_stats::skip_nan::range(true, *dense_row, ropt);
        EXPECT_EQ(rres.minimum, std::vector<double>(10, std::numeric_limits<double>::infinity()));
        EXPECT_EQ(rres.maximum, std::vector<double>(10, -std::numeric_limits<double>::infinity()));

        auto sparse_row = tatami::convert_to_compressed_sparse<double, int>(*dense_row, false, {});
        auto csres = tatami_stats::skip_nan::range(false, *sparse_row, ropt);
        EXPECT_EQ(csres.minimum.size(), 0);
        EXPECT_EQ(csres.maximum.size(), 0);

        auto rsres = tatami_stats::skip_nan::range(true, *sparse_row, ropt);
        EXPECT_EQ(rsres.minimum, std::vector<double>(10, std::numeric_limits<double>::infinity()));
        EXPECT_EQ(rsres.maximum, std::vector<double>(10, -std::numeric_limits<double>::infinity()));
    }

    // Trying with column-major matrices. 
    {
        auto dense_row2 = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(0, 10, std::vector<double>()));
        auto rres = tatami_stats::skip_nan::range(false, *dense_row2, ropt);
        EXPECT_EQ(rres.minimum, std::vector<double>(10, std::numeric_limits<double>::infinity()));
        EXPECT_EQ(rres.maximum, std::vector<double>(10, -std::numeric_limits<double>::infinity()));

        rres = tatami_stats::skip_nan::range(true, *dense_row2, ropt);
        EXPECT_EQ(rres.minimum.size(), 0);
        EXPECT_EQ(rres.maximum.size(), 0);

        auto sparse_row2 = tatami::convert_to_compressed_sparse<double, int>(*dense_row2, false, {});
        auto sres = tatami_stats::skip_nan::range(false, *sparse_row2, ropt);
        EXPECT_EQ(sres.minimum, std::vector<double>(10, std::numeric_limits<double>::infinity()));
        EXPECT_EQ(sres.maximum, std::vector<double>(10, -std::numeric_limits<double>::infinity()));

        sres = tatami_stats::skip_nan::range(true, *sparse_row2, ropt);
        EXPECT_EQ(sres.minimum.size(), 0);
        EXPECT_EQ(sres.maximum.size(), 0);
    }
}

TEST_P(SkipNanRangeEdgeTest, AllInvalid) {
    const auto num_threads = GetParam();
    std::size_t NR = 15, NC = 10;

    constexpr double nan = std::numeric_limits<double>::quiet_NaN();
    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, std::vector<double>(NR * NC, nan)));

    tatami_stats::skip_nan::RangeOptions ropt;
    ropt.num_threads = num_threads;

    {
        auto cres = tatami_stats::skip_nan::range(false, *dense_row, ropt);
        EXPECT_EQ(cres.minimum, std::vector<double>(NC, std::numeric_limits<double>::infinity()));
        EXPECT_EQ(cres.maximum, std::vector<double>(NC, -std::numeric_limits<double>::infinity()));
        EXPECT_EQ(cres.count, std::vector<int>(NC));

        auto rres = tatami_stats::skip_nan::range(true, *dense_row, ropt);
        EXPECT_EQ(rres.minimum, std::vector<double>(NR, std::numeric_limits<double>::infinity()));
        EXPECT_EQ(rres.maximum, std::vector<double>(NR, -std::numeric_limits<double>::infinity()));
        EXPECT_EQ(rres.count, std::vector<int>(NR));
    }

    // Trying with sparsity. 
    {
        auto sparse_row = tatami::convert_to_compressed_sparse<double, int>(*dense_row, false, {});
        auto csres = tatami_stats::skip_nan::range(false, *sparse_row, ropt);
        EXPECT_EQ(csres.minimum, std::vector<double>(NC, std::numeric_limits<double>::infinity()));
        EXPECT_EQ(csres.maximum, std::vector<double>(NC, -std::numeric_limits<double>::infinity()));
        EXPECT_EQ(csres.count, std::vector<int>(NC));

        auto rsres = tatami_stats::skip_nan::range(true, *sparse_row, ropt);
        EXPECT_EQ(rsres.minimum, std::vector<double>(NR, std::numeric_limits<double>::infinity()));
        EXPECT_EQ(rsres.maximum, std::vector<double>(NR, -std::numeric_limits<double>::infinity()));
        EXPECT_EQ(rsres.count, std::vector<int>(NR));
    }
}

INSTANTIATE_TEST_SUITE_P(
    SkipNanRange,
    SkipNanRangeEdgeTest,
    ::testing::Values(1, 3) // number of threads
);

/********************************************/

TEST(SkipNanRange, NewType) {
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
    auto rexpected = tatami_stats::skip_nan::range(true, *ref, {});
    std::vector<std::int8_t> rexpected_min(rexpected.minimum.begin(), rexpected.minimum.end());
    std::vector<std::int8_t> rexpected_max(rexpected.maximum.begin(), rexpected.maximum.end());
    std::vector<std::uint8_t> rexpected_count(rexpected.count.begin(), rexpected.count.end());
    auto cexpected = tatami_stats::skip_nan::range(false, *ref, {});
    std::vector<std::int8_t> cexpected_min(cexpected.minimum.begin(), cexpected.minimum.end());
    std::vector<std::int8_t> cexpected_max(cexpected.maximum.begin(), cexpected.maximum.end());
    std::vector<std::uint8_t> cexpected_count(cexpected.count.begin(), cexpected.count.end());

    std::vector<std::int8_t> ivec(dump.begin(), dump.end());
    auto dense_row = std::make_shared<tatami::DenseRowMatrix<std::int8_t, std::uint8_t> >(NR, NC, std::move(ivec));
    auto dense_column = tatami::convert_to_dense<std::int8_t, std::uint8_t>(*dense_row, false, {});
    auto sparse_row = tatami::convert_to_compressed_sparse<std::int8_t, std::uint8_t>(*dense_row, true, {});
    auto sparse_column = tatami::convert_to_compressed_sparse<std::int8_t, std::uint8_t>(*dense_row, false, {});

    compare_result(tatami_stats::skip_nan::range(true, *dense_row, {}), rexpected_min, rexpected_max, rexpected_count);
    compare_result(tatami_stats::skip_nan::range(true, *dense_column, {}), rexpected_min, rexpected_max, rexpected_count);
    compare_result(tatami_stats::skip_nan::range(true, *sparse_row, {}), rexpected_min, rexpected_max, rexpected_count);
    compare_result(tatami_stats::skip_nan::range(true, *sparse_column, {}), rexpected_min, rexpected_max, rexpected_count);

    compare_result(tatami_stats::skip_nan::range(false, *dense_row, {}), cexpected_min, cexpected_max, cexpected_count);
    compare_result(tatami_stats::skip_nan::range(false, *dense_column, {}), cexpected_min, cexpected_max, cexpected_count);
    compare_result(tatami_stats::skip_nan::range(false, *sparse_row, {}), cexpected_min, cexpected_max, cexpected_count);
    compare_result(tatami_stats::skip_nan::range(false, *sparse_column, {}), cexpected_min, cexpected_max, cexpected_count);
}

TEST(SkipNanRange, NewTypeEmpty) {
    tatami::DenseRowMatrix<std::uint8_t, int, std::vector<std::uint8_t> > dense_row_i(10, 0, std::vector<std::uint8_t>());
    auto out = tatami_stats::skip_nan::range<std::uint8_t>(true, dense_row_i, {});
    EXPECT_EQ(out.minimum, std::vector<std::uint8_t>(10, 255));
    EXPECT_EQ(out.maximum, std::vector<std::uint8_t>(10, 0));

    tatami::DenseRowMatrix<std::int8_t, int, std::vector<std::int8_t> > dense_row_i2(0, 10, std::vector<std::int8_t>());
    auto out2 = tatami_stats::skip_nan::range(false, dense_row_i2, {});
    EXPECT_EQ(out2.minimum, std::vector<std::int8_t>(10, 127));
    EXPECT_EQ(out2.maximum, std::vector<std::int8_t>(10, -128));
}
