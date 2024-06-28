#include <gtest/gtest.h>

#include <vector>

#ifdef CUSTOM_PARALLEL_TEST
// Put this before any tatami imports.
#include "custom_parallel.h"
#endif

#include "tatami_stats/ranges.hpp"
#include "tatami_test/tatami_test.hpp"

class ComputingDimExtremesTest : public ::testing::TestWithParam<std::pair<double, double> > {
protected:
    static std::vector<double> simulate(size_t NR, size_t NC, std::pair<double, double> limits) {
        return tatami_test::simulate_sparse_vector<double>(NR * NC, 0.1, limits.first, limits.second);
    }
};

/********************************************/

TEST_P(ComputingDimExtremesTest, RowRanges) {
    size_t NR = 75, NC = 62;
    auto dump = simulate(NR, NC, GetParam());
    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, dump));
    auto dense_column = tatami::convert_to_dense(dense_row.get(), false);
    auto sparse_row = tatami::convert_to_compressed_sparse(dense_row.get(), true);
    auto sparse_column = tatami::convert_to_compressed_sparse(dense_row.get(), false);

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
    std::pair<std::vector<double>, std::vector<double> > ref(std::move(refmin), std::move(refmax));

    EXPECT_EQ(ref, tatami_stats::ranges::by_row(dense_row.get()));
    EXPECT_EQ(ref, tatami_stats::ranges::by_row(dense_column.get()));
    EXPECT_EQ(ref, tatami_stats::ranges::by_row(sparse_row.get()));
    EXPECT_EQ(ref, tatami_stats::ranges::by_row(sparse_column.get()));

    // Same results from parallel code.
    tatami_stats::ranges::Options ropt;
    ropt.num_threads = 3;
    EXPECT_EQ(ref, tatami_stats::ranges::by_row(dense_row.get(), ropt));
    EXPECT_EQ(ref, tatami_stats::ranges::by_row(dense_column.get(), ropt));
    EXPECT_EQ(ref, tatami_stats::ranges::by_row(sparse_row.get(), ropt));
    EXPECT_EQ(ref, tatami_stats::ranges::by_row(sparse_column.get(), ropt));

    // Checking same results from matrices that can yield unsorted indices.
    std::shared_ptr<tatami::NumericMatrix> unsorted_row(new tatami_test::UnsortedWrapper<double, int>(sparse_row));
    EXPECT_EQ(ref, tatami_stats::ranges::by_row(unsorted_row.get()));
    std::shared_ptr<tatami::NumericMatrix> unsorted_column(new tatami_test::UnsortedWrapper<double, int>(sparse_column));
    EXPECT_EQ(ref, tatami_stats::ranges::by_row(unsorted_column.get()));

    // Checking correct behavior with dirty output buffers, and sometimes
    // NULL pointers (so only one of the min/max is being computed).
    {
        tatami_stats::ranges::Options opt;

        std::vector<double> row_mins(NR, -1), row_maxs(NR, -1);
        tatami_stats::ranges::apply(true, dense_row.get(), row_mins.data(), static_cast<double*>(NULL), opt);
        tatami_stats::ranges::apply(true, dense_row.get(), static_cast<double*>(NULL), row_maxs.data(), opt);
        EXPECT_EQ(ref.first, row_mins);
        EXPECT_EQ(ref.second, row_maxs);

        std::fill(row_mins.begin(), row_mins.end(), -1);
        std::fill(row_maxs.begin(), row_maxs.end(), -1);
        tatami_stats::ranges::apply(true, dense_row.get(), row_mins.data(), static_cast<double*>(NULL), opt);
        tatami_stats::ranges::apply(true, dense_row.get(), static_cast<double*>(NULL), row_maxs.data(), opt);
        EXPECT_EQ(ref.first, row_mins);
        EXPECT_EQ(ref.second, row_maxs);

        std::fill(row_mins.begin(), row_mins.end(), -1);
        std::fill(row_maxs.begin(), row_maxs.end(), -1);
        tatami_stats::ranges::apply(true, sparse_row.get(), row_mins.data(), static_cast<double*>(NULL), opt);
        tatami_stats::ranges::apply(true, sparse_row.get(), static_cast<double*>(NULL), row_maxs.data(), opt);
        EXPECT_EQ(ref.first, row_mins);
        EXPECT_EQ(ref.second, row_maxs);

        std::fill(row_mins.begin(), row_mins.end(), -1);
        std::fill(row_maxs.begin(), row_maxs.end(), -1);
        tatami_stats::ranges::apply(true, sparse_row.get(), row_mins.data(), static_cast<double*>(NULL), opt);
        tatami_stats::ranges::apply(true, sparse_row.get(), static_cast<double*>(NULL), row_maxs.data(), opt);
        EXPECT_EQ(ref.first, row_mins);
        EXPECT_EQ(ref.second, row_maxs);
    }
}

TEST_P(ComputingDimExtremesTest, RowRangesWithNan) {
    size_t NR = 52, NC = 83;
    auto dump = tatami_test::simulate_sparse_vector<double>(NR * NC, 0.1);
    for (size_t r = 0; r < NR; ++r) { // Injecting an NaN at the start.
        dump[r * NC] = std::numeric_limits<double>::quiet_NaN();
    }

    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, dump));
    auto dense_column = tatami::convert_to_dense(dense_row.get(), false);
    auto sparse_row = tatami::convert_to_compressed_sparse(dense_row.get(), true);
    auto sparse_column = tatami::convert_to_compressed_sparse(dense_row.get(), false);

    std::pair<std::vector<double>, std::vector<double> > ref;
    auto& refmin = ref.first;
    refmin.resize(NR, std::numeric_limits<double>::infinity());
    auto& refmax = ref.second;
    refmax.resize(NR, -std::numeric_limits<double>::infinity());

    for (size_t r = 0; r < NR; ++r) {
        for (size_t c = 1; c < NC; ++c) { // skipping the first element.
            double x = dump[c + r * NC];
            refmin[r] = std::min(refmin[r], x);
            refmax[r] = std::max(refmax[r], x);
        }
    }

    tatami_stats::ranges::Options ropt;
    ropt.skip_nan = true;
    EXPECT_EQ(ref, tatami_stats::ranges::by_row(dense_row.get(), ropt));
    EXPECT_EQ(ref, tatami_stats::ranges::by_row(dense_column.get(), ropt));
    EXPECT_EQ(ref, tatami_stats::ranges::by_row(sparse_row.get(), ropt));
    EXPECT_EQ(ref, tatami_stats::ranges::by_row(sparse_column.get(), ropt));
}

TEST_P(ComputingDimExtremesTest, ColumnRanges) {
    size_t NR = 111, NC = 52;
    auto dump = simulate(NR, NC, GetParam());
    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, dump));
    auto dense_column = tatami::convert_to_dense(dense_row.get(), false);
    auto sparse_row = tatami::convert_to_compressed_sparse(dense_row.get(), true);
    auto sparse_column = tatami::convert_to_compressed_sparse(dense_row.get(), false);

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
    std::pair<std::vector<double>, std::vector<double> > ref(std::move(refmin), std::move(refmax));

    EXPECT_EQ(ref, tatami_stats::ranges::by_column(dense_row.get()));
    EXPECT_EQ(ref, tatami_stats::ranges::by_column(dense_column.get()));
    EXPECT_EQ(ref, tatami_stats::ranges::by_column(sparse_row.get()));
    EXPECT_EQ(ref, tatami_stats::ranges::by_column(sparse_column.get()));

    // Same results from parallel code.
    tatami_stats::ranges::Options ropt;
    ropt.num_threads = 3;
    EXPECT_EQ(ref, tatami_stats::ranges::by_column(dense_row.get(), ropt));
    EXPECT_EQ(ref, tatami_stats::ranges::by_column(dense_column.get(), ropt));
    EXPECT_EQ(ref, tatami_stats::ranges::by_column(sparse_row.get(), ropt));
    EXPECT_EQ(ref, tatami_stats::ranges::by_column(sparse_column.get(), ropt));

    // Checking same results from matrices that can yield unsorted indices.
    std::shared_ptr<tatami::NumericMatrix> unsorted_row(new tatami_test::UnsortedWrapper<double, int>(sparse_row));
    EXPECT_EQ(ref, tatami_stats::ranges::by_column(unsorted_row.get()));
    std::shared_ptr<tatami::NumericMatrix> unsorted_column(new tatami_test::UnsortedWrapper<double, int>(sparse_column));
    EXPECT_EQ(ref, tatami_stats::ranges::by_column(unsorted_column.get()));
}

TEST_P(ComputingDimExtremesTest, ColumnRangesWithNan) {
    size_t NR = 52, NC = 83;
    auto dump = tatami_test::simulate_sparse_vector<double>(NR * NC, 0.1);
    for (size_t c = 0; c < NC; ++c) { // Injecting an NaN at the start.
        dump[c] = std::numeric_limits<double>::quiet_NaN();
    }

    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, dump));
    auto dense_column = tatami::convert_to_dense(dense_row.get(), false);
    auto sparse_row = tatami::convert_to_compressed_sparse(dense_row.get(), true);
    auto sparse_column = tatami::convert_to_compressed_sparse(dense_row.get(), false);

    std::pair<std::vector<double>, std::vector<double> > ref;
    auto& refmin = ref.first;
    refmin.resize(NC, std::numeric_limits<double>::infinity());
    auto& refmax = ref.second;
    refmax.resize(NC, -std::numeric_limits<double>::infinity());

    for (size_t c = 0; c < NC; ++c) {
        for (size_t r = 1; r < NR; ++r) { // skipping the first element.
            double x = dump[c + r * NC];
            refmin[c] = std::min(refmin[c], x);
            refmax[c] = std::max(refmax[c], x);
        }
    }

    tatami_stats::ranges::Options ropt;
    ropt.skip_nan = true;
    EXPECT_EQ(ref, tatami_stats::ranges::by_column(dense_row.get(), ropt));
    EXPECT_EQ(ref, tatami_stats::ranges::by_column(dense_column.get(), ropt));
    EXPECT_EQ(ref, tatami_stats::ranges::by_column(sparse_row.get(), ropt));
    EXPECT_EQ(ref, tatami_stats::ranges::by_column(sparse_column.get(), ropt));
}

/********************************************/

INSTANTIATE_TEST_SUITE_P(
    ComputingDimExtremes,
    ComputingDimExtremesTest,
    ::testing::Values(
        std::make_pair(0.1, 10.0),   // only above.
        std::make_pair(-10.0, -0.4), // only below
        std::make_pair(-5.0, 5.0)    // mix of values above and below zero.
    ) 
);

/********************************************/

TEST(ComputingDimExtremes, AllZeros) {
    // Testing for correct sparse behavior with all-zeros.
    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(10, 20, std::vector<double>(200)));
    auto dense_column = tatami::convert_to_dense(dense_row.get(), false);
    auto sparse_row = tatami::convert_to_compressed_sparse(dense_row.get(), true);
    auto sparse_column = tatami::convert_to_compressed_sparse(dense_row.get(), false);

    auto cref = std::make_pair(std::vector<double>(20), std::vector<double>(20));
    EXPECT_EQ(cref, tatami_stats::ranges::by_column(dense_row.get()));
    EXPECT_EQ(cref, tatami_stats::ranges::by_column(dense_column.get()));
    EXPECT_EQ(cref, tatami_stats::ranges::by_column(sparse_row.get()));
    EXPECT_EQ(cref, tatami_stats::ranges::by_column(sparse_column.get()));

    auto rref = std::make_pair(std::vector<double>(10), std::vector<double>(10));
    EXPECT_EQ(rref, tatami_stats::ranges::by_row(dense_row.get()));
    EXPECT_EQ(rref, tatami_stats::ranges::by_row(dense_column.get()));
    EXPECT_EQ(rref, tatami_stats::ranges::by_row(sparse_row.get()));
    EXPECT_EQ(rref, tatami_stats::ranges::by_row(sparse_column.get()));

    // Checking correct behavior with dirty output buffers.
    {
        tatami_stats::ranges::Options opt;

        std::vector<double> column_mins(20, -1), column_maxs(20, -1);
        tatami_stats::ranges::apply(false, dense_column.get(), column_mins.data(), column_maxs.data(), opt);
        EXPECT_EQ(cref.first, column_mins);
        EXPECT_EQ(cref.second, column_maxs);

        std::fill(column_mins.begin(), column_mins.end(), -1);
        std::fill(column_maxs.begin(), column_maxs.end(), -1);
        tatami_stats::ranges::apply(false, dense_column.get(), column_mins.data(), column_maxs.data(), opt);
        EXPECT_EQ(cref.first, column_mins);
        EXPECT_EQ(cref.second, column_maxs);

        std::fill(column_mins.begin(), column_mins.end(), -1);
        std::fill(column_maxs.begin(), column_maxs.end(), -1);
        tatami_stats::ranges::apply(false, sparse_column.get(), column_mins.data(), column_maxs.data(), opt);
        EXPECT_EQ(cref.first, column_mins);
        EXPECT_EQ(cref.second, column_maxs);

        std::fill(column_mins.begin(), column_mins.end(), -1);
        std::fill(column_maxs.begin(), column_maxs.end(), -1);
        tatami_stats::ranges::apply(false, sparse_column.get(), column_mins.data(), column_maxs.data(), opt);
        EXPECT_EQ(cref.first, column_mins);
        EXPECT_EQ(cref.second, column_maxs);
    }
}

TEST(ComputingDimExtremes, NoZeros) {
    // Testing for correct behavior with no zeros.
    std::vector<double> stuff(200);
    for (size_t s = 0; s < stuff.size(); ++s) {
        stuff[s] = s + 1;
    }

    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(10, 20, stuff));
    auto dense_column = tatami::convert_to_dense(dense_row.get(), false);
    auto sparse_row = tatami::convert_to_compressed_sparse(dense_row.get(), true);
    auto sparse_column = tatami::convert_to_compressed_sparse(dense_row.get(), false);

    auto cref = tatami_stats::ranges::by_column(dense_row.get());
    EXPECT_EQ(cref, tatami_stats::ranges::by_column(dense_column.get()));
    EXPECT_EQ(cref, tatami_stats::ranges::by_column(sparse_row.get()));
    EXPECT_EQ(cref, tatami_stats::ranges::by_column(sparse_column.get()));

    auto rref = tatami_stats::ranges::by_row(dense_row.get());
    EXPECT_EQ(rref, tatami_stats::ranges::by_row(dense_column.get()));
    EXPECT_EQ(rref, tatami_stats::ranges::by_row(sparse_row.get()));
    EXPECT_EQ(rref, tatami_stats::ranges::by_row(sparse_column.get()));
}

TEST(ComputingDimExtremes, NewType) {
    size_t NR = 198, NC = 52;
    auto dump = tatami_test::simulate_sparse_vector<double>(NR * NC, 0.1, /* lower = */ 1, /* upper = */ 100);
    for (auto& d : dump) { 
        d = std::round(d);
    }
    auto ref = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, dump));
    auto rexpected = tatami_stats::ranges::by_row(ref.get());
    auto cexpected = tatami_stats::ranges::by_column(ref.get());

    std::vector<int8_t> ivec(dump.begin(), dump.end());
    auto dense_row = std::make_shared<tatami::DenseRowMatrix<int8_t, uint8_t> >(NR, NC, std::move(ivec));
    auto dense_column = tatami::convert_to_dense(dense_row.get(), false);
    auto sparse_row = tatami::convert_to_compressed_sparse(dense_row.get(), true);
    auto sparse_column = tatami::convert_to_compressed_sparse(dense_row.get(), false);

    EXPECT_EQ(tatami_stats::ranges::by_row(dense_row.get()), rexpected);
    EXPECT_EQ(tatami_stats::ranges::by_row(dense_column.get()), rexpected);
    EXPECT_EQ(tatami_stats::ranges::by_row(sparse_row.get()), rexpected);
    EXPECT_EQ(tatami_stats::ranges::by_row(sparse_column.get()), rexpected);

    EXPECT_EQ(tatami_stats::ranges::by_column(dense_row.get()), cexpected);
    EXPECT_EQ(tatami_stats::ranges::by_column(dense_column.get()), cexpected);
    EXPECT_EQ(tatami_stats::ranges::by_column(sparse_row.get()), cexpected);
    EXPECT_EQ(tatami_stats::ranges::by_column(sparse_column.get()), cexpected);
}

TEST(ComputingDimExtremes, Empty) {
    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(10, 0, std::vector<double>()));
    auto cres = tatami_stats::ranges::by_column(dense_row.get());
    EXPECT_EQ(cres.first.size(), 0);

    auto rres = tatami_stats::ranges::by_row(dense_row.get());
    EXPECT_EQ(rres.first, std::vector<double>(10, std::numeric_limits<double>::infinity()));
    EXPECT_EQ(rres.second, std::vector<double>(10, -std::numeric_limits<double>::infinity()));

    // Early return will still sanitize dirty output buffers.
    {
        tatami_stats::ranges::Options ropt;
        std::vector<double> row_mins(10, -1), row_maxs(10, -1);
        tatami_stats::ranges::apply(true, dense_row.get(), row_mins.data(), row_maxs.data(), ropt);
        EXPECT_EQ(row_mins, std::vector<double>(10, std::numeric_limits<double>::infinity()));
        EXPECT_EQ(row_maxs, std::vector<double>(10, -std::numeric_limits<double>::infinity()));
    }

    // Trying with the integer types.
    {
        tatami::DenseRowMatrix<uint8_t, int, std::vector<uint8_t> > dense_row(10, 0, std::vector<uint8_t>());
        auto out = tatami_stats::ranges::by_row<uint8_t, uint8_t, int>(&dense_row);
        EXPECT_EQ(out.first, std::vector<uint8_t>(10, 255));
        EXPECT_EQ(out.second, std::vector<uint8_t>(10, 0));
    }

    {
        tatami::DenseRowMatrix<int8_t, int, std::vector<int8_t> > dense_row(0, 10, std::vector<int8_t>());
        auto out = tatami_stats::ranges::by_column<int8_t, int8_t, int>(&dense_row);
        EXPECT_EQ(out.first, std::vector<int8_t>(10, 127));
        EXPECT_EQ(out.second, std::vector<int8_t>(10, -128));
    }
}
