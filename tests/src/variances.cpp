#include <gtest/gtest.h>

#include <vector>

#ifdef CUSTOM_PARALLEL_TEST
// Put this before any tatami imports.
#include "custom_parallel.h"
#endif

#include "utils.h"
#include "tatami_stats/variances.hpp"
#include "tatami_test/tatami_test.hpp"

TEST(ComputingDimVariances, RowVariances) {
    size_t NR = 109, NC = 82;
    auto dump = tatami_test::simulate_sparse_vector<double>(NR * NC, 0.1);
    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double>(NR, NC, dump));
    auto dense_column = tatami::convert_to_dense<false>(dense_row.get());
    auto sparse_row = tatami::convert_to_compressed_sparse<true>(dense_row.get());
    auto sparse_column = tatami::convert_to_compressed_sparse<false>(dense_row.get());

    // Doing the difference of squares as a quick-and-dirty reference.
    std::vector<double> ref(NR), expectedm(NR);
    for (size_t r = 0; r < NR; ++r) {
        for (size_t c = 0; c < NC; ++c) {
            double x = dump[c + r * NC];
            expectedm[r] += x;
            ref[r] += x * x;
        }
        expectedm[r] /= NC;
        ref[r] /= NC;
        ref[r] -= expectedm[r] * expectedm[r];
        ref[r] *= NC;
        ref[r] /= NC - 1;
    }

    compare_double_vectors(ref, tatami_stats::row_variances(dense_row.get()));
    compare_double_vectors(ref, tatami_stats::row_variances(dense_column.get()));
    compare_double_vectors(ref, tatami_stats::row_variances(sparse_row.get()));
    compare_double_vectors(ref, tatami_stats::row_variances(sparse_column.get()));

    // Same results from parallel code.
    compare_double_vectors(ref, tatami_stats::row_variances(dense_row.get(), 3));
    compare_double_vectors(ref, tatami_stats::row_variances(dense_column.get(), 3));
    compare_double_vectors(ref, tatami_stats::row_variances(sparse_row.get(), 3));
    compare_double_vectors(ref, tatami_stats::row_variances(sparse_column.get(), 3));
}

TEST(ComputingDimVariances, RowVariancesWithNan) {
    size_t NR = 52, NC = 83;
    auto dump = tatami_test::simulate_sparse_vector<double>(NR * NC, 0.1);
    for (size_t r = 0; r < NR; ++r) { // Injecting an NaN at the start.
        dump[r * NC] = std::numeric_limits<double>::quiet_NaN();
    }

    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double>(NR, NC, dump));
    auto dense_column = tatami::convert_to_dense<false>(dense_row.get());
    auto sparse_row = tatami::convert_to_compressed_sparse<true>(dense_row.get());
    auto sparse_column = tatami::convert_to_compressed_sparse<false>(dense_row.get());

    std::vector<double> ref(NR), expectedm(NR);
    for (size_t r = 0; r < NR; ++r) {
        for (size_t c = 1; c < NC; ++c) { // skipping the first element.
            double x = dump[c + r * NC];
            expectedm[r] += x;
            ref[r] += x * x;
        }

        double denom = NC - 1; // remember we lost an element!
        expectedm[r] /= denom;
        ref[r] /= denom;
        ref[r] -= expectedm[r] * expectedm[r];
        ref[r] *= denom / (denom - 1);
    }

    compare_double_vectors(ref, tatami_stats::row_variances<true>(dense_row.get()));
    compare_double_vectors(ref, tatami_stats::row_variances<true>(dense_column.get()));
    compare_double_vectors(ref, tatami_stats::row_variances<true>(sparse_row.get()));
    compare_double_vectors(ref, tatami_stats::row_variances<true>(sparse_column.get()));

    EXPECT_TRUE(is_all_nan(tatami_stats::row_variances<false>(dense_row.get())));
    EXPECT_TRUE(is_all_nan(tatami_stats::row_variances<false>(dense_column.get())));
    EXPECT_TRUE(is_all_nan(tatami_stats::row_variances<false>(sparse_row.get())));
    EXPECT_TRUE(is_all_nan(tatami_stats::row_variances<false>(sparse_column.get())));
}

TEST(ComputingDimVariances, ColumnVariances) {
    size_t NR = 99, NC = 92;
    auto dump = tatami_test::simulate_sparse_vector<double>(NR * NC, 0.1);
    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double>(NR, NC, dump));
    auto dense_column = tatami::convert_to_dense<false>(dense_row.get());
    auto sparse_row = tatami::convert_to_compressed_sparse<true>(dense_row.get());
    auto sparse_column = tatami::convert_to_compressed_sparse<false>(dense_row.get());

    // Doing the difference of squares as a quick-and-dirty reference.
    std::vector<double> ref(NC), expectedm(NC);
    for (size_t c = 0; c < NC; ++c) {
        for (size_t r = 0; r < NR; ++r) {
            double x = dump[c + r * NC];
            expectedm[c] += x;
            ref[c] += x * x;
        }
        expectedm[c] /= NR;
        ref[c] /= NR;
        ref[c] -= expectedm[c] * expectedm[c];
        ref[c] *= NR;
        ref[c] /= NR - 1;
    }

    compare_double_vectors(ref, tatami_stats::column_variances(dense_row.get()));
    compare_double_vectors(ref, tatami_stats::column_variances(dense_column.get()));
    compare_double_vectors(ref, tatami_stats::column_variances(sparse_row.get()));
    compare_double_vectors(ref, tatami_stats::column_variances(sparse_column.get()));

    // Same results from parallel code.
    compare_double_vectors(ref, tatami_stats::column_variances(dense_row.get(), 3));
    compare_double_vectors(ref, tatami_stats::column_variances(dense_column.get(), 3));
    compare_double_vectors(ref, tatami_stats::column_variances(sparse_row.get(), 3));
    compare_double_vectors(ref, tatami_stats::column_variances(sparse_column.get(), 3));
}

TEST(ComputingDimVariances, ColumnVariancesWithNan) {
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
            double x = dump[c + r * NC];
            expectedm[c] += x;
            ref[c] += x * x;
        }

        double denom = NR - 1; // remember we lost an element.
        expectedm[c] /= denom;
        ref[c] /= denom;
        ref[c] -= expectedm[c] * expectedm[c];
        ref[c] *= denom / (denom - 1);
    }

    compare_double_vectors(ref, tatami_stats::column_variances<true>(dense_row.get()));
    compare_double_vectors(ref, tatami_stats::column_variances<true>(dense_column.get()));
    compare_double_vectors(ref, tatami_stats::column_variances<true>(sparse_row.get()));
    compare_double_vectors(ref, tatami_stats::column_variances<true>(sparse_column.get()));

    EXPECT_TRUE(is_all_nan(tatami_stats::column_variances<false>(dense_row.get())));
    EXPECT_TRUE(is_all_nan(tatami_stats::column_variances<false>(dense_column.get())));
    EXPECT_TRUE(is_all_nan(tatami_stats::column_variances<false>(sparse_row.get())));
    EXPECT_TRUE(is_all_nan(tatami_stats::column_variances<false>(sparse_column.get())));
}

TEST(ComputingDimVariances, DirtyOutput) {
    size_t NR = 99, NC = 152;
    auto dump = tatami_test::simulate_sparse_vector<double>(NR * NC, 0.1);
    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double>(NR, NC, dump));
    auto dense_column = tatami::convert_to_dense<false>(dense_row.get());
    auto sparse_row = tatami::convert_to_compressed_sparse<true>(dense_row.get());
    auto sparse_column = tatami::convert_to_compressed_sparse<false>(dense_row.get());

    auto ref = tatami_stats::row_variances(dense_row.get());

    // Works when the input vector is a bit dirty.
    std::vector<double> dirty(NR, -1);
    tatami_stats::row_variances(dense_row.get(), dirty.data());
    compare_double_vectors(ref, dirty);

    std::fill(dirty.begin(), dirty.end(), -1);
    tatami_stats::row_variances(dense_column.get(), dirty.data());
    compare_double_vectors(ref, dirty);

    std::fill(dirty.begin(), dirty.end(), -1);
    tatami_stats::row_variances(sparse_row.get(), dirty.data());
    compare_double_vectors(ref, dirty);

    std::fill(dirty.begin(), dirty.end(), -1);
    tatami_stats::row_variances(sparse_column.get(), dirty.data());
    compare_double_vectors(ref, dirty);
}

TEST(ComputingDimVariances, InvalidVariances) {
    // No observations.
    {
        auto dense = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double>(111, 0, std::vector<double>()));

        auto cref = tatami_stats::column_variances<false>(dense.get());
        EXPECT_EQ(cref.size(), 0);
        auto cref2 = tatami_stats::column_variances<true>(dense.get());
        EXPECT_EQ(cref.size(), 0);

        auto rref = tatami_stats::row_variances<false>(dense.get());
        EXPECT_TRUE(rref.size() > 0);
        EXPECT_TRUE(is_all_nan(rref));

        auto rref2 = tatami_stats::row_variances<true>(dense.get());
        EXPECT_TRUE(rref2.size() > 0);
        EXPECT_TRUE(is_all_nan(rref2));
    }

    // One observation.
    {
        auto dense = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double>(10, 1, std::vector<double>(10)));

        auto rref = tatami_stats::row_variances<false>(dense.get());
        EXPECT_TRUE(rref.size() > 0);
        EXPECT_TRUE(is_all_nan(rref));

        auto rref2 = tatami_stats::row_variances<true>(dense.get());
        EXPECT_TRUE(rref2.size() > 0);
        EXPECT_TRUE(is_all_nan(rref2));
    }
}

TEST(RunningVariances, SensibleZeros) {
    size_t NR = 55, NC = 52;
    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double>(NR, NC, tatami_test::simulate_sparse_vector<double>(NR * NC, 0.1)));
    auto sparse_column = tatami::convert_to_compressed_sparse<false>(dense_row.get());

    // We force the first (non-zero) value to be zero, and we check that 
    // the number of non-zeros is still correctly reported.
    std::vector<double> running_vars(NR);
    std::vector<double> running_means(NR);
    std::vector<int> running_nzeros(NR);
    {
        std::vector<int> ibuffer(NR);
        std::vector<double> vbuffer(NR);
        std::vector<int> ref_nzeros(NR);
        auto wrk = sparse_column->sparse_column();
        tatami_stats::variance::RunningSparse<false, true> runner(NR, running_means.data(), running_vars.data(), running_nzeros.data());

        for (int c = 0; c < static_cast<int>(NC); ++c) {
            auto range = wrk->fetch(c, vbuffer.data(), ibuffer.data());
            tatami::copy_n(range.value, range.number, vbuffer.data());
            vbuffer[0] = 0; 
            runner.add(vbuffer.data(), range.index, range.number);
            for (int r = 1; r < range.number; ++r) {
                ref_nzeros[range.index[r]] += (range.value[r] != 0);
            }
        }

        runner.finish();
        EXPECT_EQ(ref_nzeros, running_nzeros);
    }

    // Unless we set skip_zeros = false.
    std::vector<double> running_vars2(NR);
    std::vector<double> running_means2(NR);
    std::vector<int> running_nzeros2(NR);
    {
        std::vector<int> ibuffer(NR);
        std::vector<double> vbuffer(NR);
        std::vector<int> ref_nzeros(NR);
        auto wrk = sparse_column->sparse_column();
        tatami_stats::variance::RunningSparse<false, false> runner(NR, running_means2.data(), running_vars2.data(), running_nzeros2.data());

        for (int c = 0; c < static_cast<int>(NC); ++c) {
            auto range = wrk->fetch(c, vbuffer.data(), ibuffer.data());
            tatami::copy_n(range.value, range.number, vbuffer.data());
            vbuffer[0] = 0; 
            runner.add(vbuffer.data(), range.index, range.number);
            for (int r = 0; r < range.number; ++r) {
                ++ref_nzeros[range.index[r]];
            }
        }

        runner.finish();
        EXPECT_EQ(ref_nzeros, running_nzeros2);
    }

    for (size_t i = 0; i < NR; ++i) {
        EXPECT_FLOAT_EQ(running_means2[i], running_means[i]);
        EXPECT_FLOAT_EQ(running_vars2[i], running_vars[i]);
    }
}
