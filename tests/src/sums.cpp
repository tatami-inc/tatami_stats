#include <gtest/gtest.h>

#include <vector>

#ifdef CUSTOM_PARALLEL_TEST
// Put this before any tatami imports.
#include "custom_parallel.h"
#endif

#include "tatami_stats/sums.hpp"
#include "tatami_test/tatami_test.hpp"
#include "utils.h"

TEST(ComputingDimSums, RowSums) {
    size_t NR = 99, NC = 152;
    auto dump = tatami_test::simulate_sparse_vector<double>(NR * NC, 0.1);
    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double>(NR, NC, dump));
    auto dense_column = tatami::convert_to_dense<false>(dense_row.get());
    auto sparse_row = tatami::convert_to_compressed_sparse<true>(dense_row.get());
    auto sparse_column = tatami::convert_to_compressed_sparse<false>(dense_row.get());

    // Naive calculation.
    std::vector<double> ref(NR);
    for (size_t r = 0; r < NR; ++r) {
        for (size_t c = 0; c < NC; ++c) {
            ref[r] += dump[c + r * NC];
        }
    }

    compare_double_vectors(ref, tatami_stats::row_sums(dense_row.get()));
    compare_double_vectors(ref, tatami_stats::row_sums(dense_column.get()));
    compare_double_vectors(ref, tatami_stats::row_sums(sparse_row.get()));
    compare_double_vectors(ref, tatami_stats::row_sums(sparse_column.get()));

    // Checking same results from parallel code.
    compare_double_vectors(ref, tatami_stats::row_sums(dense_row.get(), 3));
    compare_double_vectors(ref, tatami_stats::row_sums(dense_column.get(), 3));
    compare_double_vectors(ref, tatami_stats::row_sums(sparse_row.get(), 3));
    compare_double_vectors(ref, tatami_stats::row_sums(sparse_column.get(), 3));
}

TEST(ComputingDimSums, RowSumsWithNan) {
    size_t NR = 52, NC = 83;
    auto dump = tatami_test::simulate_sparse_vector<double>(NR * NC, 0.1);
    for (size_t r = 0; r < NR; ++r) { // Injecting an NaN at the start.
        dump[r * NC] = std::numeric_limits<double>::quiet_NaN();
    }

    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double>(NR, NC, dump));
    auto dense_column = tatami::convert_to_dense<false>(dense_row.get());
    auto sparse_row = tatami::convert_to_compressed_sparse<true>(dense_row.get());
    auto sparse_column = tatami::convert_to_compressed_sparse<false>(dense_row.get());

    std::vector<double> ref(NR);
    for (size_t r = 0; r < NR; ++r) {
        for (size_t c = 1; c < NC; ++c) { // skipping the first element.
            ref[r] += dump[c + r * NC];
        }
    }

    compare_double_vectors(ref, tatami_stats::row_sums<true>(dense_row.get()));
    compare_double_vectors(ref, tatami_stats::row_sums<true>(dense_column.get()));
    compare_double_vectors(ref, tatami_stats::row_sums<true>(sparse_row.get()));
    compare_double_vectors(ref, tatami_stats::row_sums<true>(sparse_column.get()));

    EXPECT_TRUE(is_all_nan(tatami_stats::row_sums<false>(dense_row.get())));
    EXPECT_TRUE(is_all_nan(tatami_stats::row_sums<false>(dense_column.get())));
    EXPECT_TRUE(is_all_nan(tatami_stats::row_sums<false>(sparse_row.get())));
    EXPECT_TRUE(is_all_nan(tatami_stats::row_sums<false>(sparse_column.get())));
}


TEST(ComputingDimSums, ColumnSums) {
    size_t NR = 79, NC = 62;
    auto dump = tatami_test::simulate_sparse_vector<double>(NR * NC, 0.1);
    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double>(NR, NC, dump));
    auto dense_column = tatami::convert_to_dense<false>(dense_row.get());
    auto sparse_row = tatami::convert_to_compressed_sparse<true>(dense_row.get());
    auto sparse_column = tatami::convert_to_compressed_sparse<false>(dense_row.get());

    std::vector<double> ref(NC);
    for (size_t c = 0; c < NC; ++c) {
        for (size_t r = 0; r < NR; ++r) {
            ref[c] += dump[c + r * NC];
        }
    }

    compare_double_vectors(ref, tatami_stats::column_sums(dense_row.get()));
    compare_double_vectors(ref, tatami_stats::column_sums(dense_column.get()));
    compare_double_vectors(ref, tatami_stats::column_sums(sparse_row.get()));
    compare_double_vectors(ref, tatami_stats::column_sums(sparse_column.get()));

    // Checking same results from parallel code.
    compare_double_vectors(ref, tatami_stats::column_sums(dense_column.get(), 3));
    compare_double_vectors(ref, tatami_stats::column_sums(dense_column.get(), 3));
    compare_double_vectors(ref, tatami_stats::column_sums(sparse_column.get(), 3));
    compare_double_vectors(ref, tatami_stats::column_sums(sparse_column.get(), 3));
}

TEST(ComputingDimSums, ColumnSumsWithNan) {
    size_t NR = 82, NC = 33;
    auto dump = tatami_test::simulate_sparse_vector<double>(NR * NC, 0.1);
    for (size_t c = 0; c < NC; ++c) { // Injecting an NaN at the start.
        dump[c] = std::numeric_limits<double>::quiet_NaN();
    }

    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double>(NR, NC, dump));
    auto dense_column = tatami::convert_to_dense<false>(dense_row.get());
    auto sparse_row = tatami::convert_to_compressed_sparse<true>(dense_row.get());
    auto sparse_column = tatami::convert_to_compressed_sparse<false>(dense_row.get());

    std::vector<double> ref(NC), expectedm(NC);
    for (size_t c = 0; c < NC; ++c) {
        for (size_t r = 1; r < NR; ++r) { // skipping the first row.
            ref[c] += dump[c + r * NC];
        }
    }

    compare_double_vectors(ref, tatami_stats::column_sums<true>(dense_row.get()));
    compare_double_vectors(ref, tatami_stats::column_sums<true>(dense_column.get()));
    compare_double_vectors(ref, tatami_stats::column_sums<true>(sparse_row.get()));
    compare_double_vectors(ref, tatami_stats::column_sums<true>(sparse_column.get()));

    EXPECT_TRUE(is_all_nan(tatami_stats::column_sums<false>(dense_row.get())));
    EXPECT_TRUE(is_all_nan(tatami_stats::column_sums<false>(dense_column.get())));
    EXPECT_TRUE(is_all_nan(tatami_stats::column_sums<false>(sparse_row.get())));
    EXPECT_TRUE(is_all_nan(tatami_stats::column_sums<false>(sparse_column.get())));
}

TEST(ComputingDimSums, DirtyOutput) {
    size_t NR = 99, NC = 152;
    auto dump = tatami_test::simulate_sparse_vector<double>(NR * NC, 0.1);
    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double>(NR, NC, dump));
    auto dense_column = tatami::convert_to_dense<false>(dense_row.get());
    auto sparse_row = tatami::convert_to_compressed_sparse<true>(dense_row.get());
    auto sparse_column = tatami::convert_to_compressed_sparse<false>(dense_row.get());

    auto ref = tatami_stats::row_sums(dense_row.get());

    // Works when the input vector is a bit dirty.
    std::vector<double> dirty(NR, -1);
    tatami_stats::row_sums(dense_row.get(), dirty.data());
    compare_double_vectors(ref, dirty);

    std::fill(dirty.begin(), dirty.end(), -1);
    tatami_stats::row_sums(dense_column.get(), dirty.data());
    compare_double_vectors(ref, dirty);

    std::fill(dirty.begin(), dirty.end(), -1);
    tatami_stats::row_sums(sparse_row.get(), dirty.data());
    compare_double_vectors(ref, dirty);

    std::fill(dirty.begin(), dirty.end(), -1);
    tatami_stats::row_sums(sparse_column.get(), dirty.data());
    compare_double_vectors(ref, dirty);
}