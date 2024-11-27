#include <gtest/gtest.h>

#include <vector>

#include "utils.h"
#include "tatami_stats/variances.hpp"
#include "tatami_test/tatami_test.hpp"

TEST(ComputingDimVariances, RowVariances) {
    size_t NR = 109, NC = 82;
    auto dump = tatami_test::simulate_vector<double>(NR * NC, []{
        tatami_test::SimulateVectorOptions opt;
        opt.density = 0.1;
        opt.seed = 1987191;
        return opt;
    }());

    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, dump));
    auto dense_column = tatami::convert_to_dense(dense_row.get(), false);
    auto sparse_row = tatami::convert_to_compressed_sparse(dense_row.get(), true);
    auto sparse_column = tatami::convert_to_compressed_sparse(dense_row.get(), false);

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

    compare_double_vectors(ref, tatami_stats::variances::by_row(dense_row.get()));
    compare_double_vectors(ref, tatami_stats::variances::by_row(dense_column.get()));
    compare_double_vectors(ref, tatami_stats::variances::by_row(sparse_row.get()));
    compare_double_vectors(ref, tatami_stats::variances::by_row(sparse_column.get()));

    // Same results from parallel code.
    tatami_stats::variances::Options vopt;
    vopt.num_threads = 3;
    compare_double_vectors(ref, tatami_stats::variances::by_row(dense_row.get(), vopt));
    compare_double_vectors(ref, tatami_stats::variances::by_row(dense_column.get(), vopt));
    compare_double_vectors(ref, tatami_stats::variances::by_row(sparse_row.get(), vopt));
    compare_double_vectors(ref, tatami_stats::variances::by_row(sparse_column.get(), vopt));
}

TEST(ComputingDimVariances, RowVariancesWithNan) {
    size_t NR = 52, NC = 83;
    auto dump = tatami_test::simulate_vector<double>(NR * NC, []{
        tatami_test::SimulateVectorOptions opt;
        opt.density = 0.1;
        opt.seed = 44982197;
        return opt;
    }());
    for (size_t r = 0; r < NR; ++r) { // Injecting an NaN at the start.
        dump[r * NC] = std::numeric_limits<double>::quiet_NaN();
    }

    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, dump));
    auto dense_column = tatami::convert_to_dense(dense_row.get(), false);
    auto sparse_row = tatami::convert_to_compressed_sparse(dense_row.get(), true);
    auto sparse_column = tatami::convert_to_compressed_sparse(dense_row.get(), false);

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

    tatami_stats::variances::Options vopt;
    vopt.skip_nan = true;
    compare_double_vectors(ref, tatami_stats::variances::by_row(dense_row.get(), vopt));
    compare_double_vectors(ref, tatami_stats::variances::by_row(dense_column.get(), vopt));
    compare_double_vectors(ref, tatami_stats::variances::by_row(sparse_row.get(), vopt));
    compare_double_vectors(ref, tatami_stats::variances::by_row(sparse_column.get(), vopt));

    EXPECT_TRUE(is_all_nan(tatami_stats::variances::by_row(dense_row.get())));
    EXPECT_TRUE(is_all_nan(tatami_stats::variances::by_row(dense_column.get())));
    EXPECT_TRUE(is_all_nan(tatami_stats::variances::by_row(sparse_row.get())));
    EXPECT_TRUE(is_all_nan(tatami_stats::variances::by_row(sparse_column.get())));
}

TEST(ComputingDimVariances, ColumnVariances) {
    size_t NR = 99, NC = 92;
    auto dump = tatami_test::simulate_vector<double>(NR * NC, []{
        tatami_test::SimulateVectorOptions opt;
        opt.density = 0.1;
        opt.seed = 21823818;
        return opt;
    }());

    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, dump));
    auto dense_column = tatami::convert_to_dense(dense_row.get(), false);
    auto sparse_row = tatami::convert_to_compressed_sparse(dense_row.get(), true);
    auto sparse_column = tatami::convert_to_compressed_sparse(dense_row.get(), false);

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

    compare_double_vectors(ref, tatami_stats::variances::by_column(dense_row.get()));
    compare_double_vectors(ref, tatami_stats::variances::by_column(dense_column.get()));
    compare_double_vectors(ref, tatami_stats::variances::by_column(sparse_row.get()));
    compare_double_vectors(ref, tatami_stats::variances::by_column(sparse_column.get()));

    // Same results from parallel code.
    tatami_stats::variances::Options vopt;
    vopt.num_threads = 3;
    compare_double_vectors(ref, tatami_stats::variances::by_column(dense_row.get(), vopt));
    compare_double_vectors(ref, tatami_stats::variances::by_column(dense_column.get(), vopt));
    compare_double_vectors(ref, tatami_stats::variances::by_column(sparse_row.get(), vopt));
    compare_double_vectors(ref, tatami_stats::variances::by_column(sparse_column.get(), vopt));
}

TEST(ComputingDimVariances, ColumnVariancesWithNan) {
    size_t NR = 82, NC = 33;
    auto dump = tatami_test::simulate_vector<double>(NR * NC, []{
        tatami_test::SimulateVectorOptions opt;
        opt.density = 0.1;
        opt.seed = 191353;
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

    tatami_stats::variances::Options vopt;
    vopt.skip_nan = true;
    compare_double_vectors(ref, tatami_stats::variances::by_column(dense_row.get(), vopt));
    compare_double_vectors(ref, tatami_stats::variances::by_column(dense_column.get(), vopt));
    compare_double_vectors(ref, tatami_stats::variances::by_column(sparse_row.get(), vopt));
    compare_double_vectors(ref, tatami_stats::variances::by_column(sparse_column.get(), vopt));

    EXPECT_TRUE(is_all_nan(tatami_stats::variances::by_column(dense_row.get())));
    EXPECT_TRUE(is_all_nan(tatami_stats::variances::by_column(dense_column.get())));
    EXPECT_TRUE(is_all_nan(tatami_stats::variances::by_column(sparse_row.get())));
    EXPECT_TRUE(is_all_nan(tatami_stats::variances::by_column(sparse_column.get())));
}

TEST(ComputingDimVariances, NewType) {
    size_t NR = 198, NC = 52;
    auto dump = tatami_test::simulate_vector<double>(NR * NC, []{
        tatami_test::SimulateVectorOptions opt;
        opt.density = 0.1;
        opt.lower = 1;
        opt.upper = 100;
        opt.seed = 1982719;
        return opt;
    }());
    for (auto& d : dump) { 
        d = std::round(d);
    }

    auto ref = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, dump));
    auto rexpected = tatami_stats::variances::by_row(ref.get());
    auto cexpected = tatami_stats::variances::by_column(ref.get());

    std::vector<int8_t> ivec(dump.begin(), dump.end());
    auto dense_row = std::make_shared<tatami::DenseRowMatrix<int8_t, uint8_t> >(NR, NC, std::move(ivec));
    auto dense_column = tatami::convert_to_dense(dense_row.get(), false);
    auto sparse_row = tatami::convert_to_compressed_sparse(dense_row.get(), true);
    auto sparse_column = tatami::convert_to_compressed_sparse(dense_row.get(), false);

    EXPECT_EQ(tatami_stats::variances::by_row(dense_row.get()), rexpected);
    compare_double_vectors(tatami_stats::variances::by_row(dense_column.get()), rexpected);
    compare_double_vectors(tatami_stats::variances::by_row(sparse_row.get()), rexpected);
    compare_double_vectors(tatami_stats::variances::by_row(sparse_column.get()), rexpected);

    EXPECT_EQ(tatami_stats::variances::by_column(dense_row.get()), cexpected);
    compare_double_vectors(tatami_stats::variances::by_column(dense_column.get()), cexpected);
    compare_double_vectors(tatami_stats::variances::by_column(sparse_row.get()), cexpected);
    compare_double_vectors(tatami_stats::variances::by_column(sparse_column.get()), cexpected);
}

TEST(ComputingDimVariances, DirtyOutput) {
    size_t NR = 99, NC = 152;
    auto dump = tatami_test::simulate_vector<double>(NR * NC, []{ 
        tatami_test::SimulateVectorOptions opt;
        opt.density = 0.1;
        opt.seed = 457633478;
        return opt;
    }());

    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, dump));
    auto dense_column = tatami::convert_to_dense(dense_row.get(), false);
    auto sparse_row = tatami::convert_to_compressed_sparse(dense_row.get(), true);
    auto sparse_column = tatami::convert_to_compressed_sparse(dense_row.get(), false);

    auto ref = tatami_stats::variances::by_row(dense_row.get());

    tatami_stats::variances::Options vopt;

    // Works when the input vector is a bit dirty.
    std::vector<double> dirty(NR, -1);
    tatami_stats::variances::apply(true, dense_row.get(), dirty.data(), vopt);
    compare_double_vectors(ref, dirty);

    std::fill(dirty.begin(), dirty.end(), -1);
    tatami_stats::variances::apply(true, dense_column.get(), dirty.data(), vopt);
    compare_double_vectors(ref, dirty);

    std::fill(dirty.begin(), dirty.end(), -1);
    tatami_stats::variances::apply(true, sparse_row.get(), dirty.data(), vopt);
    compare_double_vectors(ref, dirty);

    std::fill(dirty.begin(), dirty.end(), -1);
    tatami_stats::variances::apply(true, sparse_column.get(), dirty.data(), vopt);
    compare_double_vectors(ref, dirty);
}

TEST(ComputingDimVariances, InvalidVariances) {
    tatami_stats::variances::Options vopt;
    vopt.skip_nan = true;

    // No observations.
    {
        auto dense = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(111, 0, std::vector<double>()));

        auto cref = tatami_stats::variances::by_column(dense.get());
        EXPECT_EQ(cref.size(), 0);
        auto cref2 = tatami_stats::variances::by_column(dense.get(), vopt);
        EXPECT_EQ(cref.size(), 0);

        auto rref = tatami_stats::variances::by_row(dense.get());
        EXPECT_TRUE(rref.size() > 0);
        EXPECT_TRUE(is_all_nan(rref));

        auto rref2 = tatami_stats::variances::by_row(dense.get(), vopt);
        EXPECT_TRUE(rref2.size() > 0);
        EXPECT_TRUE(is_all_nan(rref2));
    }

    // One observation.
    {
        auto dense = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(10, 1, std::vector<double>(10)));

        auto rref = tatami_stats::variances::by_row(dense.get());
        EXPECT_TRUE(rref.size() > 0);
        EXPECT_TRUE(is_all_nan(rref));

        auto rref2 = tatami_stats::variances::by_row(dense.get(), vopt);
        EXPECT_TRUE(rref2.size() > 0);
        EXPECT_TRUE(is_all_nan(rref2));
    }
}
