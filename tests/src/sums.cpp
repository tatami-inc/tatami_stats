#include <gtest/gtest.h>

#include <vector>

#include "tatami_stats/sums.hpp"
#include "tatami_test/tatami_test.hpp"
#include "utils.h"

TEST(ComputingDimSums, RowSums) {
    size_t NR = 99, NC = 152;
    auto dump = tatami_test::simulate_vector<double>(NR * NC, []{
        tatami_test::SimulateVectorOptions opt;
        opt.density = 0.1;
        opt.seed = 238947239;
        return opt;
    }());

    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, dump));
    auto dense_column = tatami::convert_to_dense(dense_row.get(), false);
    auto sparse_row = tatami::convert_to_compressed_sparse(dense_row.get(), true);
    auto sparse_column = tatami::convert_to_compressed_sparse(dense_row.get(), false);

    // Naive calculation.
    std::vector<double> ref(NR);
    for (size_t r = 0; r < NR; ++r) {
        for (size_t c = 0; c < NC; ++c) {
            ref[r] += dump[c + r * NC];
        }
    }

    compare_double_vectors(ref, tatami_stats::sums::by_row(dense_row.get()));
    compare_double_vectors(ref, tatami_stats::sums::by_row(dense_column.get()));
    compare_double_vectors(ref, tatami_stats::sums::by_row(sparse_row.get()));
    compare_double_vectors(ref, tatami_stats::sums::by_row(sparse_column.get()));

    // Checking same results from parallel code.
    tatami_stats::sums::Options sopt;
    sopt.num_threads = 3;
    compare_double_vectors(ref, tatami_stats::sums::by_row(dense_row.get(), sopt));
    compare_double_vectors(ref, tatami_stats::sums::by_row(dense_column.get(), sopt));
    compare_double_vectors(ref, tatami_stats::sums::by_row(sparse_row.get(), sopt));
    compare_double_vectors(ref, tatami_stats::sums::by_row(sparse_column.get(), sopt));
}

TEST(ComputingDimSums, RowSumsWithNan) {
    size_t NR = 52, NC = 83;
    auto dump = tatami_test::simulate_vector<double>(NR * NC, []{ 
        tatami_test::SimulateVectorOptions opt;
        opt.density = 0.1;
        opt.seed = 992929;
        return opt;
    }());
    for (size_t r = 0; r < NR; ++r) { // Injecting an NaN at the start.
        dump[r * NC] = std::numeric_limits<double>::quiet_NaN();
    }

    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, dump));
    auto dense_column = tatami::convert_to_dense(dense_row.get(), false);
    auto sparse_row = tatami::convert_to_compressed_sparse(dense_row.get(), true);
    auto sparse_column = tatami::convert_to_compressed_sparse(dense_row.get(), false);

    std::vector<double> ref(NR);
    for (size_t r = 0; r < NR; ++r) {
        for (size_t c = 1; c < NC; ++c) { // skipping the first element.
            ref[r] += dump[c + r * NC];
        }
    }

    tatami_stats::sums::Options sopt;
    sopt.skip_nan = true;
    compare_double_vectors(ref, tatami_stats::sums::by_row(dense_row.get(), sopt));
    compare_double_vectors(ref, tatami_stats::sums::by_row(dense_column.get(), sopt));
    compare_double_vectors(ref, tatami_stats::sums::by_row(sparse_row.get(), sopt));
    compare_double_vectors(ref, tatami_stats::sums::by_row(sparse_column.get(), sopt));

    EXPECT_TRUE(is_all_nan(tatami_stats::sums::by_row(dense_row.get())));
    EXPECT_TRUE(is_all_nan(tatami_stats::sums::by_row(dense_column.get())));
    EXPECT_TRUE(is_all_nan(tatami_stats::sums::by_row(sparse_row.get())));
    EXPECT_TRUE(is_all_nan(tatami_stats::sums::by_row(sparse_column.get())));
}

TEST(ComputingDimSums, ColumnSums) {
    size_t NR = 79, NC = 62;
    auto dump = tatami_test::simulate_vector<double>(NR * NC, []{
        tatami_test::SimulateVectorOptions opt;
        opt.density = 0.1;
        opt.seed = 12919010;
        return opt;
    }());

    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, dump));
    auto dense_column = tatami::convert_to_dense(dense_row.get(), false);
    auto sparse_row = tatami::convert_to_compressed_sparse(dense_row.get(), true);
    auto sparse_column = tatami::convert_to_compressed_sparse(dense_row.get(), false);

    std::vector<double> ref(NC);
    for (size_t c = 0; c < NC; ++c) {
        for (size_t r = 0; r < NR; ++r) {
            ref[c] += dump[c + r * NC];
        }
    }

    compare_double_vectors(ref, tatami_stats::sums::by_column(dense_row.get()));
    compare_double_vectors(ref, tatami_stats::sums::by_column(dense_column.get()));
    compare_double_vectors(ref, tatami_stats::sums::by_column(sparse_row.get()));
    compare_double_vectors(ref, tatami_stats::sums::by_column(sparse_column.get()));

    // Checking same results from parallel code.
    tatami_stats::sums::Options sopt;
    sopt.num_threads = 3;
    compare_double_vectors(ref, tatami_stats::sums::by_column(dense_column.get(), sopt));
    compare_double_vectors(ref, tatami_stats::sums::by_column(dense_column.get(), sopt));
    compare_double_vectors(ref, tatami_stats::sums::by_column(sparse_column.get(), sopt));
    compare_double_vectors(ref, tatami_stats::sums::by_column(sparse_column.get(), sopt));
}

TEST(ComputingDimSums, ColumnSumsWithNan) {
    size_t NR = 82, NC = 33;
    auto dump = tatami_test::simulate_vector<double>(NR * NC, []{
        tatami_test::SimulateVectorOptions opt;
        opt.density = 0.1;
        opt.seed = 8768372;
        return opt;
    }());
    for (size_t c = 0; c < NC; ++c) { // Injecting an NaN at the start.
        dump[c] = std::numeric_limits<double>::quiet_NaN();
    }

    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, dump));
    auto dense_column = tatami::convert_to_dense(dense_row.get(), false);
    auto sparse_row = tatami::convert_to_compressed_sparse(dense_row.get(), true);
    auto sparse_column = tatami::convert_to_compressed_sparse(dense_row.get(), false);

    std::vector<double> ref(NC), expectedm(NC);
    for (size_t c = 0; c < NC; ++c) {
        for (size_t r = 1; r < NR; ++r) { // skipping the first row.
            ref[c] += dump[c + r * NC];
        }
    }

    tatami_stats::sums::Options sopt;
    sopt.skip_nan = true;
    compare_double_vectors(ref, tatami_stats::sums::by_column(dense_row.get(), sopt));
    compare_double_vectors(ref, tatami_stats::sums::by_column(dense_column.get(), sopt));
    compare_double_vectors(ref, tatami_stats::sums::by_column(sparse_row.get(), sopt));
    compare_double_vectors(ref, tatami_stats::sums::by_column(sparse_column.get(), sopt));

    EXPECT_TRUE(is_all_nan(tatami_stats::sums::by_column(dense_row.get())));
    EXPECT_TRUE(is_all_nan(tatami_stats::sums::by_column(dense_column.get())));
    EXPECT_TRUE(is_all_nan(tatami_stats::sums::by_column(sparse_row.get())));
    EXPECT_TRUE(is_all_nan(tatami_stats::sums::by_column(sparse_column.get())));
}

TEST(ComputingDimSums, NewType) {
    size_t NR = 198, NC = 52;
    auto dump = tatami_test::simulate_vector<double>(NR * NC, []{
        tatami_test::SimulateVectorOptions opt;
        opt.density = 0.1;
        opt.lower = 1;
        opt.upper = 100;
        opt.seed = 8768372;
        return opt;
    }());
    for (auto& d : dump) { 
        d = std::round(d);
    }

    auto ref = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, dump));
    auto rexpected = tatami_stats::sums::by_row(ref.get());
    auto cexpected = tatami_stats::sums::by_column(ref.get());

    std::vector<int8_t> ivec(dump.begin(), dump.end());
    auto dense_row = std::make_shared<tatami::DenseRowMatrix<int8_t, uint8_t> >(NR, NC, std::move(ivec));
    auto dense_column = tatami::convert_to_dense(dense_row.get(), false);
    auto sparse_row = tatami::convert_to_compressed_sparse(dense_row.get(), true);
    auto sparse_column = tatami::convert_to_compressed_sparse(dense_row.get(), false);

    EXPECT_EQ(tatami_stats::sums::by_row(dense_row.get()), rexpected);
    EXPECT_EQ(tatami_stats::sums::by_row(dense_column.get()), rexpected);
    EXPECT_EQ(tatami_stats::sums::by_row(sparse_row.get()), rexpected);
    EXPECT_EQ(tatami_stats::sums::by_row(sparse_column.get()), rexpected);

    EXPECT_EQ(tatami_stats::sums::by_column(dense_row.get()), cexpected);
    EXPECT_EQ(tatami_stats::sums::by_column(dense_column.get()), cexpected);
    EXPECT_EQ(tatami_stats::sums::by_column(sparse_row.get()), cexpected);
    EXPECT_EQ(tatami_stats::sums::by_column(sparse_column.get()), cexpected);
}

TEST(ComputingDimSums, DirtyOutput) {
    size_t NR = 99, NC = 152;
    auto dump = tatami_test::simulate_vector<double>(NR * NC, []{
        tatami_test::SimulateVectorOptions opt;
        opt.density = 0.1;
        opt.seed = 5936893;
        return opt;
    }());

    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, dump));
    auto dense_column = tatami::convert_to_dense(dense_row.get(), false);
    auto sparse_row = tatami::convert_to_compressed_sparse(dense_row.get(), true);
    auto sparse_column = tatami::convert_to_compressed_sparse(dense_row.get(), false);

    auto ref = tatami_stats::sums::by_row(dense_row.get());

    tatami_stats::sums::Options sopt;

    // Works when the input vector is a bit dirty.
    std::vector<double> dirty(NR, -1);
    tatami_stats::sums::apply(true, dense_row.get(), dirty.data(), sopt);
    compare_double_vectors(ref, dirty);

    std::fill(dirty.begin(), dirty.end(), -1);
    tatami_stats::sums::apply(true, dense_column.get(), dirty.data(), sopt);
    compare_double_vectors(ref, dirty);

    std::fill(dirty.begin(), dirty.end(), -1);
    tatami_stats::sums::apply(true, sparse_row.get(), dirty.data(), sopt);
    compare_double_vectors(ref, dirty);

    std::fill(dirty.begin(), dirty.end(), -1);
    tatami_stats::sums::apply(true, sparse_column.get(), dirty.data(), sopt);
    compare_double_vectors(ref, dirty);
}
