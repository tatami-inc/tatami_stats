#include <gtest/gtest.h>

#include <vector>

#include "tatami_stats/counts.hpp"
#include "tatami_test/tatami_test.hpp"

TEST(ComputingDimCounts, RowNanCounts) {
    size_t NR = 99, NC = 152;
    auto dump = tatami_test::simulate_vector<double>(NR * NC, []{
        tatami_test::SimulateVectorOptions opt;
        opt.density = 0.1;
        opt.seed = 324987;
        return opt;
    }());
    for (auto& x : dump) {
        if (x == 0) {
            x = std::numeric_limits<double>::quiet_NaN();
        }
    }

    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, dump));
    auto dense_column = tatami::convert_to_dense(dense_row.get(), false);
    auto sparse_row = tatami::convert_to_compressed_sparse(dense_row.get(), true);
    auto sparse_column = tatami::convert_to_compressed_sparse(dense_row.get(), false);

    std::vector<int> ref(NR);
    for (size_t r = 0; r < NR; ++r) {
        for (size_t c = 0; c < NC; ++c) {
            ref[r] += std::isnan(dump[c + r * NC]);
        }
    }

    EXPECT_EQ(ref, tatami_stats::counts::nan::by_row(dense_row.get()));
    EXPECT_EQ(ref, tatami_stats::counts::nan::by_row(dense_column.get()));
    EXPECT_EQ(ref, tatami_stats::counts::nan::by_row(sparse_row.get()));
    EXPECT_EQ(ref, tatami_stats::counts::nan::by_row(sparse_column.get()));

    // Checking same results from parallel code.
    tatami_stats::counts::nan::Options nopt;
    nopt.num_threads = 3;
    EXPECT_EQ(ref, tatami_stats::counts::nan::by_row(dense_row.get(), nopt));
    EXPECT_EQ(ref, tatami_stats::counts::nan::by_row(dense_column.get(), nopt));
    EXPECT_EQ(ref, tatami_stats::counts::nan::by_row(sparse_row.get(), nopt));
    EXPECT_EQ(ref, tatami_stats::counts::nan::by_row(sparse_column.get(), nopt));

    // Checking same results from matrices that can yield unsorted indices.
    std::shared_ptr<tatami::NumericMatrix> unsorted_row(new tatami_test::ReversedIndicesWrapper<double, int>(sparse_row));
    EXPECT_EQ(ref, tatami_stats::counts::nan::by_row(unsorted_row.get()));
    std::shared_ptr<tatami::NumericMatrix> unsorted_column(new tatami_test::ReversedIndicesWrapper<double, int>(sparse_column));
    EXPECT_EQ(ref, tatami_stats::counts::nan::by_row(unsorted_column.get()));
}

TEST(ComputingDimCounts, ColumNanCounts) {
    size_t NR = 79, NC = 62;
    auto dump = tatami_test::simulate_vector<double>(NR * NC, []{
        tatami_test::SimulateVectorOptions opt;
        opt.density = 0.1;
        opt.seed = 28374928;
        return opt;
    }());
    for (auto& x : dump) {
        if (x == 0) {
            x = std::numeric_limits<double>::quiet_NaN();
        }
    }

    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, dump));
    auto dense_column = tatami::convert_to_dense(dense_row.get(), false);
    auto sparse_row = tatami::convert_to_compressed_sparse(dense_row.get(), true);
    auto sparse_column = tatami::convert_to_compressed_sparse(dense_row.get(), false);

    std::vector<int> ref(NC);
    for (size_t c = 0; c < NC; ++c) {
        for (size_t r = 0; r < NR; ++r) {
            ref[c] += std::isnan(dump[c + r * NC]);
        }
    }

    EXPECT_EQ(ref, tatami_stats::counts::nan::by_column(dense_row.get()));
    EXPECT_EQ(ref, tatami_stats::counts::nan::by_column(dense_column.get()));
    EXPECT_EQ(ref, tatami_stats::counts::nan::by_column(sparse_row.get()));
    EXPECT_EQ(ref, tatami_stats::counts::nan::by_column(sparse_column.get()));

    // Checking same results from parallel code.
    tatami_stats::counts::nan::Options nopt;
    nopt.num_threads = 3;
    EXPECT_EQ(ref, tatami_stats::counts::nan::by_column(dense_column.get(), nopt));
    EXPECT_EQ(ref, tatami_stats::counts::nan::by_column(dense_column.get(), nopt));
    EXPECT_EQ(ref, tatami_stats::counts::nan::by_column(sparse_column.get(), nopt));
    EXPECT_EQ(ref, tatami_stats::counts::nan::by_column(sparse_column.get(), nopt));

    // Checking same results from matrices that can yield unsorted indices.
    std::shared_ptr<tatami::NumericMatrix> unsorted_row(new tatami_test::ReversedIndicesWrapper<double, int>(sparse_row));
    EXPECT_EQ(ref, tatami_stats::counts::nan::by_column(unsorted_row.get()));
    std::shared_ptr<tatami::NumericMatrix> unsorted_column(new tatami_test::ReversedIndicesWrapper<double, int>(sparse_column));
    EXPECT_EQ(ref, tatami_stats::counts::nan::by_column(unsorted_column.get()));
}

TEST(ComputingDimCounts, RowZeroCounts) {
    size_t NR = 99, NC = 152;
    auto dump = tatami_test::simulate_vector<double>(NR * NC, []{
        tatami_test::SimulateVectorOptions opt;
        opt.density = 0.1;
        opt.seed = 958734;
        return opt;
    }());

    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, dump));
    auto dense_column = tatami::convert_to_dense(dense_row.get(), false);
    auto sparse_row = tatami::convert_to_compressed_sparse(dense_row.get(), true);
    auto sparse_column = tatami::convert_to_compressed_sparse(dense_row.get(), false);

    std::vector<int> ref(NR);
    for (size_t r = 0; r < NR; ++r) {
        for (size_t c = 0; c < NC; ++c) {
            ref[r] += dump[c + r * NC] == 0;
        }
    }

    EXPECT_EQ(ref, tatami_stats::counts::zero::by_row(dense_row.get()));
    EXPECT_EQ(ref, tatami_stats::counts::zero::by_row(dense_column.get()));
    EXPECT_EQ(ref, tatami_stats::counts::zero::by_row(sparse_row.get()));
    EXPECT_EQ(ref, tatami_stats::counts::zero::by_row(sparse_column.get()));

    // Checking same results from parallel code.
    tatami_stats::counts::zero::Options nopt;
    nopt.num_threads = 3;
    EXPECT_EQ(ref, tatami_stats::counts::zero::by_row(dense_row.get(), nopt));
    EXPECT_EQ(ref, tatami_stats::counts::zero::by_row(dense_column.get(), nopt));
    EXPECT_EQ(ref, tatami_stats::counts::zero::by_row(sparse_row.get(), nopt));
    EXPECT_EQ(ref, tatami_stats::counts::zero::by_row(sparse_column.get(), nopt));

    // Checking same results from matrices that can yield unsorted indices.
    std::shared_ptr<tatami::NumericMatrix> unsorted_row(new tatami_test::ReversedIndicesWrapper<double, int>(sparse_row));
    EXPECT_EQ(ref, tatami_stats::counts::zero::by_row(unsorted_row.get()));
    std::shared_ptr<tatami::NumericMatrix> unsorted_column(new tatami_test::ReversedIndicesWrapper<double, int>(sparse_column));
    EXPECT_EQ(ref, tatami_stats::counts::zero::by_row(unsorted_column.get()));
}

TEST(ComputingDimCounts, RowZeroCountsWithNan) {
    size_t NR = 52, NC = 83;
    auto dump = tatami_test::simulate_vector<double>(NR * NC, []{
        tatami_test::SimulateVectorOptions opt;
        opt.density = 0.1;
        opt.seed = 3248927;
        return opt;
    }());
    for (size_t r = 0; r < NR; ++r) { // Injecting an NaN at the start.
        dump[r * NC] = std::numeric_limits<double>::quiet_NaN();
    }

    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, dump));
    auto dense_column = tatami::convert_to_dense(dense_row.get(), false);
    auto sparse_row = tatami::convert_to_compressed_sparse(dense_row.get(), true);
    auto sparse_column = tatami::convert_to_compressed_sparse(dense_row.get(), false);

    std::vector<int> ref(NR); 
    for (size_t r = 0; r < NR; ++r) {
        for (size_t c = 1; c < NC; ++c) { // skipping the first element.
            ref[r] += (dump[c + r * NC] == 0);
        }
    }

    EXPECT_EQ(ref, tatami_stats::counts::zero::by_row(dense_row.get()));
    EXPECT_EQ(ref, tatami_stats::counts::zero::by_row(dense_column.get()));
    EXPECT_EQ(ref, tatami_stats::counts::zero::by_row(sparse_row.get()));
    EXPECT_EQ(ref, tatami_stats::counts::zero::by_row(sparse_column.get()));
}

TEST(ComputingDimCounts, ColumnZeroCounts) {
    size_t NR = 79, NC = 62;
    auto dump = tatami_test::simulate_vector<double>(NR * NC, []{
        tatami_test::SimulateVectorOptions opt;
        opt.density = 0.1;
        opt.seed = 93942;
        return opt;
    }());

    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, dump));
    auto dense_column = tatami::convert_to_dense(dense_row.get(), false);
    auto sparse_row = tatami::convert_to_compressed_sparse(dense_row.get(), true);
    auto sparse_column = tatami::convert_to_compressed_sparse(dense_row.get(), false);

    std::vector<int> ref(NC);
    for (size_t c = 0; c < NC; ++c) {
        for (size_t r = 0; r < NR; ++r) {
            ref[c] += dump[c + r * NC] == 0;
        }
    }

    EXPECT_EQ(ref, tatami_stats::counts::zero::by_column(dense_row.get()));
    EXPECT_EQ(ref, tatami_stats::counts::zero::by_column(dense_column.get()));
    EXPECT_EQ(ref, tatami_stats::counts::zero::by_column(sparse_row.get()));
    EXPECT_EQ(ref, tatami_stats::counts::zero::by_column(sparse_column.get()));

    // Checking same results from parallel code.
    tatami_stats::counts::zero::Options nopt;
    nopt.num_threads = 3;
    EXPECT_EQ(ref, tatami_stats::counts::zero::by_column(dense_column.get(), nopt));
    EXPECT_EQ(ref, tatami_stats::counts::zero::by_column(dense_column.get(), nopt));
    EXPECT_EQ(ref, tatami_stats::counts::zero::by_column(sparse_column.get(), nopt));
    EXPECT_EQ(ref, tatami_stats::counts::zero::by_column(sparse_column.get(), nopt));

    // Checking same results from matrices that can yield unsorted indices.
    std::shared_ptr<tatami::NumericMatrix> unsorted_row(new tatami_test::ReversedIndicesWrapper<double, int>(sparse_row));
    EXPECT_EQ(ref, tatami_stats::counts::zero::by_column(unsorted_row.get()));
    std::shared_ptr<tatami::NumericMatrix> unsorted_column(new tatami_test::ReversedIndicesWrapper<double, int>(sparse_column));
    EXPECT_EQ(ref, tatami_stats::counts::zero::by_column(unsorted_column.get()));
}

TEST(ComputingDimVariances, ColumnZeroCountsWithNan) {
    size_t NR = 82, NC = 33;
    auto dump = tatami_test::simulate_vector<double>(NR * NC, []{
        tatami_test::SimulateVectorOptions opt;
        opt.density = 0.1;
        opt.seed = 87918;
        return opt;
    }());
    for (size_t c = 0; c < NC; ++c) { // Injecting an NaN at the start.
        dump[c] = std::numeric_limits<double>::quiet_NaN();
    }

    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, dump));
    auto dense_column = tatami::convert_to_dense(dense_row.get(), false);
    auto sparse_row = tatami::convert_to_compressed_sparse(dense_row.get(), true);
    auto sparse_column = tatami::convert_to_compressed_sparse(dense_row.get(), false);

    std::vector<int> ref(NC);
    for (size_t c = 0; c < NC; ++c) {
        for (size_t r = 1; r < NR; ++r) { // skipping the first row.
            ref[c] += dump[c + r * NC] == 0;
        }
    }

    EXPECT_EQ(ref, tatami_stats::counts::zero::by_column(dense_row.get()));
    EXPECT_EQ(ref, tatami_stats::counts::zero::by_column(dense_column.get()));
    EXPECT_EQ(ref, tatami_stats::counts::zero::by_column(sparse_row.get()));
    EXPECT_EQ(ref, tatami_stats::counts::zero::by_column(sparse_column.get()));
}

TEST(ComputingDimCounts, EmptyCounts) {
    size_t NR = 0, NC = 10;
    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, std::vector<double>()));
    auto dense_column = tatami::convert_to_dense(dense_row.get(), false);
    auto sparse_row = tatami::convert_to_compressed_sparse(dense_row.get(), true);
    auto sparse_column = tatami::convert_to_compressed_sparse(dense_row.get(), false);

    tatami_stats::counts::zero::Options nopt;
    nopt.num_threads = 3;

    std::vector<int> empty_c(NC);
    EXPECT_EQ(empty_c, tatami_stats::counts::zero::by_column(dense_row.get()));
    EXPECT_EQ(empty_c, tatami_stats::counts::zero::by_column(dense_column.get()));
    EXPECT_EQ(empty_c, tatami_stats::counts::zero::by_column(sparse_row.get()));
    EXPECT_EQ(empty_c, tatami_stats::counts::zero::by_column(sparse_column.get()));

    EXPECT_EQ(empty_c, tatami_stats::counts::zero::by_column(dense_row.get(), nopt));
    EXPECT_EQ(empty_c, tatami_stats::counts::zero::by_column(dense_column.get(), nopt));
    EXPECT_EQ(empty_c, tatami_stats::counts::zero::by_column(sparse_row.get(), nopt));
    EXPECT_EQ(empty_c, tatami_stats::counts::zero::by_column(sparse_column.get(), nopt));

    std::vector<int> empty_r(NR);
    EXPECT_EQ(empty_r, tatami_stats::counts::zero::by_row(dense_row.get()));
    EXPECT_EQ(empty_r, tatami_stats::counts::zero::by_row(dense_column.get()));
    EXPECT_EQ(empty_r, tatami_stats::counts::zero::by_row(sparse_row.get()));
    EXPECT_EQ(empty_r, tatami_stats::counts::zero::by_row(sparse_column.get()));

    EXPECT_EQ(empty_r, tatami_stats::counts::zero::by_row(dense_row.get(), nopt));
    EXPECT_EQ(empty_r, tatami_stats::counts::zero::by_row(dense_column.get(), nopt));
    EXPECT_EQ(empty_r, tatami_stats::counts::zero::by_row(sparse_row.get(), nopt));
    EXPECT_EQ(empty_r, tatami_stats::counts::zero::by_row(sparse_column.get(), nopt));
}
