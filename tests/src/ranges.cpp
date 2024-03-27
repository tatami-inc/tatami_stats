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

TEST_P(ComputingDimExtremesTest, RowMins) {
    size_t NR = 39, NC = 42;
    auto dump = simulate(NR, NC, GetParam());
    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double>(NR, NC, dump));
    auto dense_column = tatami::convert_to_dense<false>(dense_row.get());
    auto sparse_row = tatami::convert_to_compressed_sparse<true>(dense_row.get());
    auto sparse_column = tatami::convert_to_compressed_sparse<false>(dense_row.get());

    std::vector<double> ref(NR);
    for (size_t r = 0; r < NR; ++r) {
        ref[r] = dump[r * NC];
        for (size_t c = 1; c < NC; ++c) {
            ref[r] = std::min(ref[r], dump[c + r * NC]);
        }
    }

    EXPECT_EQ(ref, tatami_stats::row_mins(dense_row.get()));
    EXPECT_EQ(ref, tatami_stats::row_mins(dense_column.get()));
    EXPECT_EQ(ref, tatami_stats::row_mins(sparse_row.get()));
    EXPECT_EQ(ref, tatami_stats::row_mins(sparse_column.get()));

    // Same results from parallel code.
    EXPECT_EQ(ref, tatami_stats::row_mins(dense_row.get(), 3));
    EXPECT_EQ(ref, tatami_stats::row_mins(dense_column.get(), 3));
    EXPECT_EQ(ref, tatami_stats::row_mins(sparse_row.get(), 3));
    EXPECT_EQ(ref, tatami_stats::row_mins(sparse_column.get(), 3));

    // Checking same results from matrices that can yield unsorted indices.
    std::shared_ptr<tatami::NumericMatrix> unsorted_row(new tatami_test::UnsortedWrapper<double, int>(sparse_row));
    EXPECT_EQ(ref, tatami_stats::row_mins(unsorted_row.get()));
    std::shared_ptr<tatami::NumericMatrix> unsorted_column(new tatami_test::UnsortedWrapper<double, int>(sparse_column));
    EXPECT_EQ(ref, tatami_stats::row_mins(unsorted_column.get()));
}

TEST_P(ComputingDimExtremesTest, ColumnMins) {
    size_t NR = 55, NC = 122;
    auto dump = simulate(NR, NC, GetParam());
    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double>(NR, NC, dump));
    auto dense_column = tatami::convert_to_dense<false>(dense_row.get());
    auto sparse_row = tatami::convert_to_compressed_sparse<true>(dense_row.get());
    auto sparse_column = tatami::convert_to_compressed_sparse<false>(dense_row.get());

    std::vector<double> ref(NC);
    for (size_t c = 0; c < NC; ++c) {
        ref[c] = dump[c];
        for (size_t r = 1; r < NR; ++r) {
            ref[c] = std::min(ref[c], dump[c + r * NC]);
        }
    }

    EXPECT_EQ(ref, tatami_stats::column_mins(dense_row.get()));
    EXPECT_EQ(ref, tatami_stats::column_mins(dense_column.get()));
    EXPECT_EQ(ref, tatami_stats::column_mins(sparse_row.get()));
    EXPECT_EQ(ref, tatami_stats::column_mins(sparse_column.get()));

    // Same results from parallel code.
    EXPECT_EQ(ref, tatami_stats::column_mins(dense_row.get(), 3));
    EXPECT_EQ(ref, tatami_stats::column_mins(dense_column.get(), 3));
    EXPECT_EQ(ref, tatami_stats::column_mins(sparse_row.get(), 3));
    EXPECT_EQ(ref, tatami_stats::column_mins(sparse_column.get(), 3));

    // Checking same results from matrices that can yield unsorted indices.
    std::shared_ptr<tatami::NumericMatrix> unsorted_row(new tatami_test::UnsortedWrapper<double, int>(sparse_row));
    EXPECT_EQ(ref, tatami_stats::column_mins(unsorted_row.get()));
    std::shared_ptr<tatami::NumericMatrix> unsorted_column(new tatami_test::UnsortedWrapper<double, int>(sparse_column));
    EXPECT_EQ(ref, tatami_stats::column_mins(unsorted_column.get()));
}

/********************************************/

TEST_P(ComputingDimExtremesTest, RowMaxs) {
    size_t NR = 125, NC = 32;
    auto dump = simulate(NR, NC, GetParam());
    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double>(NR, NC, dump));
    auto dense_column = tatami::convert_to_dense<false>(dense_row.get());
    auto sparse_row = tatami::convert_to_compressed_sparse<true>(dense_row.get());
    auto sparse_column = tatami::convert_to_compressed_sparse<false>(dense_row.get());

    std::vector<double> ref(NR);
    for (size_t r = 0; r < NR; ++r) {
        ref[r] = dump[r * NC];
        for (size_t c = 1; c < NC; ++c) {
            ref[r] = std::max(ref[r], dump[c + r * NC]);
        }
    }

    EXPECT_EQ(ref, tatami_stats::row_maxs(dense_row.get()));
    EXPECT_EQ(ref, tatami_stats::row_maxs(dense_column.get()));
    EXPECT_EQ(ref, tatami_stats::row_maxs(sparse_row.get()));
    EXPECT_EQ(ref, tatami_stats::row_maxs(sparse_column.get()));

    // Same results from parallel code.
    EXPECT_EQ(ref, tatami_stats::row_maxs(dense_row.get(), 3));
    EXPECT_EQ(ref, tatami_stats::row_maxs(dense_column.get(), 3));
    EXPECT_EQ(ref, tatami_stats::row_maxs(sparse_row.get(), 3));
    EXPECT_EQ(ref, tatami_stats::row_maxs(sparse_column.get(), 3));

    // Checking same results from matrices that can yield unsorted indices.
    std::shared_ptr<tatami::NumericMatrix> unsorted_row(new tatami_test::UnsortedWrapper<double, int>(sparse_row));
    EXPECT_EQ(ref, tatami_stats::row_maxs(unsorted_row.get()));
    std::shared_ptr<tatami::NumericMatrix> unsorted_column(new tatami_test::UnsortedWrapper<double, int>(sparse_column));
    EXPECT_EQ(ref, tatami_stats::row_maxs(unsorted_column.get()));
}

TEST_P(ComputingDimExtremesTest, ColumnMaxs) {
    size_t NR = 115, NC = 42;
    auto dump = simulate(NR, NC, GetParam());
    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double>(NR, NC, dump));
    auto dense_column = tatami::convert_to_dense<false>(dense_row.get());
    auto sparse_row = tatami::convert_to_compressed_sparse<true>(dense_row.get());
    auto sparse_column = tatami::convert_to_compressed_sparse<false>(dense_row.get());

    std::vector<double> ref(NC);
    for (size_t c = 0; c < NC; ++c) {
        ref[c] = dump[c];
        for (size_t r = 1; r < NR; ++r) {
            ref[c] = std::max(ref[c], dump[c + r * NC]);
        }
    }

    EXPECT_EQ(ref, tatami_stats::column_maxs(dense_row.get()));
    EXPECT_EQ(ref, tatami_stats::column_maxs(dense_column.get()));
    EXPECT_EQ(ref, tatami_stats::column_maxs(sparse_row.get()));
    EXPECT_EQ(ref, tatami_stats::column_maxs(sparse_column.get()));

    // Same results from parallel code.
    EXPECT_EQ(ref, tatami_stats::column_maxs(dense_row.get(), 3));
    EXPECT_EQ(ref, tatami_stats::column_maxs(dense_column.get(), 3));
    EXPECT_EQ(ref, tatami_stats::column_maxs(sparse_row.get(), 3));
    EXPECT_EQ(ref, tatami_stats::column_maxs(sparse_column.get(), 3));

    // Checking same results from matrices that can yield unsorted indices.
    std::shared_ptr<tatami::NumericMatrix> unsorted_row(new tatami_test::UnsortedWrapper<double, int>(sparse_row));
    EXPECT_EQ(ref, tatami_stats::column_maxs(unsorted_row.get()));
    std::shared_ptr<tatami::NumericMatrix> unsorted_column(new tatami_test::UnsortedWrapper<double, int>(sparse_column));
    EXPECT_EQ(ref, tatami_stats::column_maxs(unsorted_column.get()));
}

/********************************************/

TEST_P(ComputingDimExtremesTest, RowRanges) {
    size_t NR = 75, NC = 62;
    auto dump = simulate(NR, NC, GetParam());
    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double>(NR, NC, dump));
    auto dense_column = tatami::convert_to_dense<false>(dense_row.get());
    auto sparse_row = tatami::convert_to_compressed_sparse<true>(dense_row.get());
    auto sparse_column = tatami::convert_to_compressed_sparse<false>(dense_row.get());

    auto ref = tatami_stats::row_ranges(dense_row.get());
    EXPECT_EQ(ref.first, tatami_stats::row_mins(dense_row.get()));
    EXPECT_EQ(ref.second, tatami_stats::row_maxs(dense_row.get()));

    EXPECT_EQ(ref, tatami_stats::row_ranges(dense_column.get()));
    EXPECT_EQ(ref, tatami_stats::row_ranges(sparse_row.get()));
    EXPECT_EQ(ref, tatami_stats::row_ranges(sparse_column.get()));

    // Same results from parallel code.
    EXPECT_EQ(ref, tatami_stats::row_ranges(dense_row.get(), 3));
    EXPECT_EQ(ref, tatami_stats::row_ranges(dense_column.get(), 3));
    EXPECT_EQ(ref, tatami_stats::row_ranges(sparse_row.get(), 3));
    EXPECT_EQ(ref, tatami_stats::row_ranges(sparse_column.get(), 3));

    // Checking same results from matrices that can yield unsorted indices.
    std::shared_ptr<tatami::NumericMatrix> unsorted_row(new tatami_test::UnsortedWrapper<double, int>(sparse_row));
    EXPECT_EQ(ref, tatami_stats::row_ranges(unsorted_row.get()));
    std::shared_ptr<tatami::NumericMatrix> unsorted_column(new tatami_test::UnsortedWrapper<double, int>(sparse_column));
    EXPECT_EQ(ref, tatami_stats::row_ranges(unsorted_column.get()));

    // Checking correct behavior with dirty output buffers.
    {
        std::vector<double> row_mins(NR, -1), row_maxs(NR, -1);
        tatami_stats::row_ranges(dense_row.get(), row_mins.data(), row_maxs.data());
        EXPECT_EQ(ref.first, row_mins);
        EXPECT_EQ(ref.second, row_maxs);

        std::fill(row_mins.begin(), row_mins.end(), -1);
        std::fill(row_maxs.begin(), row_maxs.end(), -1);
        tatami_stats::row_ranges(dense_row.get(), row_mins.data(), row_maxs.data());
        EXPECT_EQ(ref.first, row_mins);
        EXPECT_EQ(ref.second, row_maxs);

        std::fill(row_mins.begin(), row_mins.end(), -1);
        std::fill(row_maxs.begin(), row_maxs.end(), -1);
        tatami_stats::row_ranges(sparse_row.get(), row_mins.data(), row_maxs.data());
        EXPECT_EQ(ref.first, row_mins);
        EXPECT_EQ(ref.second, row_maxs);

        std::fill(row_mins.begin(), row_mins.end(), -1);
        std::fill(row_maxs.begin(), row_maxs.end(), -1);
        tatami_stats::row_ranges(sparse_row.get(), row_mins.data(), row_maxs.data());
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

    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double>(NR, NC, dump));
    auto dense_column = tatami::convert_to_dense<false>(dense_row.get());
    auto sparse_row = tatami::convert_to_compressed_sparse<true>(dense_row.get());
    auto sparse_column = tatami::convert_to_compressed_sparse<false>(dense_row.get());

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

    EXPECT_EQ(ref, tatami_stats::row_ranges<true>(dense_row.get()));
    EXPECT_EQ(ref, tatami_stats::row_ranges<true>(dense_column.get()));
    EXPECT_EQ(ref, tatami_stats::row_ranges<true>(sparse_row.get()));
    EXPECT_EQ(ref, tatami_stats::row_ranges<true>(sparse_column.get()));
}

TEST_P(ComputingDimExtremesTest, ColumnRanges) {
    size_t NR = 111, NC = 52;
    auto dump = simulate(NR, NC, GetParam());
    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double>(NR, NC, dump));
    auto dense_column = tatami::convert_to_dense<false>(dense_row.get());
    auto sparse_row = tatami::convert_to_compressed_sparse<true>(dense_row.get());
    auto sparse_column = tatami::convert_to_compressed_sparse<false>(dense_row.get());

    auto ref = tatami_stats::column_ranges(dense_row.get());
    EXPECT_EQ(ref.first, tatami_stats::column_mins(dense_row.get()));
    EXPECT_EQ(ref.second, tatami_stats::column_maxs(dense_row.get()));

    EXPECT_EQ(ref, tatami_stats::column_ranges(dense_column.get()));
    EXPECT_EQ(ref, tatami_stats::column_ranges(sparse_row.get()));
    EXPECT_EQ(ref, tatami_stats::column_ranges(sparse_column.get()));

    // Same results from parallel code.
    EXPECT_EQ(ref, tatami_stats::column_ranges(dense_row.get(), 3));
    EXPECT_EQ(ref, tatami_stats::column_ranges(dense_column.get(), 3));
    EXPECT_EQ(ref, tatami_stats::column_ranges(sparse_row.get(), 3));
    EXPECT_EQ(ref, tatami_stats::column_ranges(sparse_column.get(), 3));

    // Checking same results from matrices that can yield unsorted indices.
    std::shared_ptr<tatami::NumericMatrix> unsorted_row(new tatami_test::UnsortedWrapper<double, int>(sparse_row));
    EXPECT_EQ(ref, tatami_stats::column_ranges(unsorted_row.get()));
    std::shared_ptr<tatami::NumericMatrix> unsorted_column(new tatami_test::UnsortedWrapper<double, int>(sparse_column));
    EXPECT_EQ(ref, tatami_stats::column_ranges(unsorted_column.get()));
}

TEST_P(ComputingDimExtremesTest, ColumnRangesWithNan) {
    size_t NR = 52, NC = 83;
    auto dump = tatami_test::simulate_sparse_vector<double>(NR * NC, 0.1);
    for (size_t c = 0; c < NC; ++c) { // Injecting an NaN at the start.
        dump[c] = std::numeric_limits<double>::quiet_NaN();
    }

    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double>(NR, NC, dump));
    auto dense_column = tatami::convert_to_dense<false>(dense_row.get());
    auto sparse_row = tatami::convert_to_compressed_sparse<true>(dense_row.get());
    auto sparse_column = tatami::convert_to_compressed_sparse<false>(dense_row.get());

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

    EXPECT_EQ(ref, tatami_stats::column_ranges<true>(dense_row.get()));
    EXPECT_EQ(ref, tatami_stats::column_ranges<true>(dense_column.get()));
    EXPECT_EQ(ref, tatami_stats::column_ranges<true>(sparse_row.get()));
    EXPECT_EQ(ref, tatami_stats::column_ranges<true>(sparse_column.get()));
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
    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double>(10, 20, std::vector<double>(200)));
    auto dense_column = tatami::convert_to_dense<false>(dense_row.get());
    auto sparse_row = tatami::convert_to_compressed_sparse<true>(dense_row.get());
    auto sparse_column = tatami::convert_to_compressed_sparse<false>(dense_row.get());

    auto cref = std::make_pair(std::vector<double>(20), std::vector<double>(20));
    EXPECT_EQ(cref, tatami_stats::column_ranges(dense_row.get()));
    EXPECT_EQ(cref, tatami_stats::column_ranges(dense_column.get()));
    EXPECT_EQ(cref, tatami_stats::column_ranges(sparse_row.get()));
    EXPECT_EQ(cref, tatami_stats::column_ranges(sparse_column.get()));

    auto rref = std::make_pair(std::vector<double>(10), std::vector<double>(10));
    EXPECT_EQ(rref, tatami_stats::row_ranges(dense_row.get()));
    EXPECT_EQ(rref, tatami_stats::row_ranges(dense_column.get()));
    EXPECT_EQ(rref, tatami_stats::row_ranges(sparse_row.get()));
    EXPECT_EQ(rref, tatami_stats::row_ranges(sparse_column.get()));

    // Checking correct behavior with dirty output buffers.
    {
        std::vector<double> column_mins(20, -1), column_maxs(20, -1);
        tatami_stats::column_ranges(dense_column.get(), column_mins.data(), column_maxs.data());
        EXPECT_EQ(cref.first, column_mins);
        EXPECT_EQ(cref.second, column_maxs);

        std::fill(column_mins.begin(), column_mins.end(), -1);
        std::fill(column_maxs.begin(), column_maxs.end(), -1);
        tatami_stats::column_ranges(dense_column.get(), column_mins.data(), column_maxs.data());
        EXPECT_EQ(cref.first, column_mins);
        EXPECT_EQ(cref.second, column_maxs);

        std::fill(column_mins.begin(), column_mins.end(), -1);
        std::fill(column_maxs.begin(), column_maxs.end(), -1);
        tatami_stats::column_ranges(sparse_column.get(), column_mins.data(), column_maxs.data());
        EXPECT_EQ(cref.first, column_mins);
        EXPECT_EQ(cref.second, column_maxs);

        std::fill(column_mins.begin(), column_mins.end(), -1);
        std::fill(column_maxs.begin(), column_maxs.end(), -1);
        tatami_stats::column_ranges(sparse_column.get(), column_mins.data(), column_maxs.data());
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

    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double>(10, 20, stuff));
    auto dense_column = tatami::convert_to_dense<false>(dense_row.get());
    auto sparse_row = tatami::convert_to_compressed_sparse<true>(dense_row.get());
    auto sparse_column = tatami::convert_to_compressed_sparse<false>(dense_row.get());

    auto cref = tatami_stats::column_ranges(dense_row.get());
    EXPECT_EQ(cref, tatami_stats::column_ranges(dense_column.get()));
    EXPECT_EQ(cref, tatami_stats::column_ranges(sparse_row.get()));
    EXPECT_EQ(cref, tatami_stats::column_ranges(sparse_column.get()));

    auto rref = tatami_stats::row_ranges(dense_row.get());
    EXPECT_EQ(rref, tatami_stats::row_ranges(dense_column.get()));
    EXPECT_EQ(rref, tatami_stats::row_ranges(sparse_row.get()));
    EXPECT_EQ(rref, tatami_stats::row_ranges(sparse_column.get()));
}

TEST(ComputingDimExtremes, Empty) {
    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double>(10, 0, std::vector<double>()));
    EXPECT_EQ(tatami_stats::column_mins(dense_row.get()).size(), 0);
    EXPECT_EQ(tatami_stats::row_mins(dense_row.get()), std::vector<double>(10, std::numeric_limits<double>::infinity()));
    EXPECT_EQ(tatami_stats::row_maxs(dense_row.get()), std::vector<double>(10, -std::numeric_limits<double>::infinity()));

    // Early return will still sanitize dirty output buffers.
    {
        std::vector<double> row_mins(10, -1), row_maxs(10, -1);
        tatami_stats::row_ranges(dense_row.get(), row_mins.data(), row_maxs.data());
        EXPECT_EQ(row_mins, std::vector<double>(10, std::numeric_limits<double>::infinity()));
        EXPECT_EQ(row_maxs, std::vector<double>(10, -std::numeric_limits<double>::infinity()));
    }

    // Trying with the integer types.
    {
        auto dense_row = std::make_unique<tatami::DenseRowMatrix<uint8_t, int, std::vector<uint8_t> > >(10, 0, std::vector<uint8_t>());
        auto minout = tatami_stats::row_mins<false, uint8_t>(dense_row.get());
        EXPECT_EQ(minout, std::vector<uint8_t>(10, 255));
        auto maxout = tatami_stats::row_maxs<false, uint8_t>(dense_row.get());
        EXPECT_EQ(maxout, std::vector<uint8_t>(10, 0));
    }

    {
        auto dense_row = std::make_unique<tatami::DenseRowMatrix<int8_t, int, std::vector<int8_t> > >(10, 0, std::vector<int8_t>());
        auto minout = tatami_stats::row_mins<false, int8_t>(dense_row.get());
        EXPECT_EQ(minout, std::vector<int8_t>(10, 127));
        auto maxout = tatami_stats::row_maxs<false, int8_t>(dense_row.get());
        EXPECT_EQ(maxout, std::vector<int8_t>(10, -128));
    }


}
