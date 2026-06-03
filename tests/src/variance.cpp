#include <gtest/gtest.h>

#include <cstdint>
#include <vector>

#include "utils.h"
#include "tatami_stats/variance.hpp"
#include "tatami_test/tatami_test.hpp"

static void compare_result(
    const tatami_stats::variance::Result<double>& res,
    const std::vector<double>& expected_mean,
    const std::vector<double>& expected_variance
) {
    compare_double_vectors(expected_variance, res.variance);
    compare_double_vectors(expected_mean, res.mean);
}

TEST(Variance, RowVariances) {
    size_t NR = 109, NC = 82;
    auto dump = tatami_test::simulate_vector<double>(NR * NC, []{
        tatami_test::SimulateVectorOptions opt;
        opt.density = 0.1;
        opt.seed = 1987191;
        return opt;
    }());

    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, dump));
    auto dense_column = tatami::convert_to_dense<double, int>(*dense_row, false, {});
    auto sparse_row = tatami::convert_to_compressed_sparse<double, int>(*dense_row, true, {});
    auto sparse_column = tatami::convert_to_compressed_sparse<double, int>(*dense_row, false, {});

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

    compare_result(tatami_stats::variance::apply(true, *dense_row, {}), expectedm, ref);
    compare_result(tatami_stats::variance::apply(true, *dense_column, {}), expectedm, ref);
    compare_result(tatami_stats::variance::apply(true, *sparse_row, {}), expectedm, ref);
    compare_result(tatami_stats::variance::apply(true, *sparse_column, {}), expectedm, ref);

    // Same results from parallel code.
    tatami_stats::variance::Options vopt;
    vopt.num_threads = 3;
    compare_result(tatami_stats::variance::apply(true, *dense_row, vopt), expectedm, ref);
    compare_result(tatami_stats::variance::apply(true, *dense_column, vopt), expectedm, ref);
    compare_result(tatami_stats::variance::apply(true, *sparse_row, vopt), expectedm, ref);
    compare_result(tatami_stats::variance::apply(true, *sparse_column, vopt), expectedm, ref);

    // Same results if NaN skipping is enabled.
    vopt.num_threads = 1;
    vopt.skip_nan = true;
    compare_result(tatami_stats::variance::apply(true, *dense_row, vopt), expectedm, ref);
    compare_result(tatami_stats::variance::apply(true, *dense_column, vopt), expectedm, ref);
    compare_result(tatami_stats::variance::apply(true, *sparse_row, vopt), expectedm, ref);
    compare_result(tatami_stats::variance::apply(true, *sparse_column, vopt), expectedm, ref);
}

TEST(Variance, RowVariancesWithNan) {
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

    tatami_stats::variance::Options vopt;
    vopt.skip_nan = true;
    compare_result(tatami_stats::variance::apply(true, *dense_row, vopt), expectedm, ref);
    compare_result(tatami_stats::variance::apply(true, *dense_column, vopt), expectedm, ref);
    compare_result(tatami_stats::variance::apply(true, *sparse_row, vopt), expectedm, ref);
    compare_result(tatami_stats::variance::apply(true, *sparse_column, vopt), expectedm, ref);

    vopt.num_threads = 3;
    compare_result(tatami_stats::variance::apply(true, *dense_row, vopt), expectedm, ref);
    compare_result(tatami_stats::variance::apply(true, *dense_column, vopt), expectedm, ref);
    compare_result(tatami_stats::variance::apply(true, *sparse_row, vopt), expectedm, ref);
    compare_result(tatami_stats::variance::apply(true, *sparse_column, vopt), expectedm, ref);
}

TEST(Variance, ColumnVariances) {
    size_t NR = 99, NC = 92;
    auto dump = tatami_test::simulate_vector<double>(NR * NC, []{
        tatami_test::SimulateVectorOptions opt;
        opt.density = 0.1;
        opt.seed = 21823818;
        return opt;
    }());

    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, dump));
    auto dense_column = tatami::convert_to_dense<double, int>(*dense_row, false, {});
    auto sparse_row = tatami::convert_to_compressed_sparse<double, int>(*dense_row, true, {});
    auto sparse_column = tatami::convert_to_compressed_sparse<double, int>(*dense_row, false, {});

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

    compare_result(tatami_stats::variance::apply(false, *dense_row, {}), expectedm, ref);
    compare_result(tatami_stats::variance::apply(false, *dense_column, {}), expectedm, ref);
    compare_result(tatami_stats::variance::apply(false, *sparse_row, {}), expectedm, ref);
    compare_result(tatami_stats::variance::apply(false, *sparse_column, {}), expectedm, ref);

    // Same results from parallel code.
    tatami_stats::variance::Options vopt;
    vopt.num_threads = 3;
    compare_result(tatami_stats::variance::apply(false, *dense_row, vopt), expectedm, ref);
    compare_result(tatami_stats::variance::apply(false, *dense_column, vopt), expectedm, ref);
    compare_result(tatami_stats::variance::apply(false, *sparse_row, vopt), expectedm, ref);
    compare_result(tatami_stats::variance::apply(false, *sparse_column, vopt), expectedm, ref);

    // Same results if NaN skipping is enabled.
    vopt.num_threads = 1;
    vopt.skip_nan = true;
    compare_result(tatami_stats::variance::apply(false, *dense_row, vopt), expectedm, ref);
    compare_result(tatami_stats::variance::apply(false, *dense_column, vopt), expectedm, ref);
    compare_result(tatami_stats::variance::apply(false, *sparse_row, vopt), expectedm, ref);
    compare_result(tatami_stats::variance::apply(false, *sparse_column, vopt), expectedm, ref);
}

TEST(Variance, ColumnVariancesWithNan) {
    size_t NR = 82, NC = 33;
    auto dump = tatami_test::simulate_vector<double>(NR * NC, []{
        tatami_test::SimulateVectorOptions opt;
        opt.density = 0.1;
        opt.seed = 191353;
        return opt;
    }());
    for (std::size_t c = 0; c < NC; ++c) { // Injecting an NaN at the start.
        dump[c] = std::numeric_limits<double>::quiet_NaN();
    }

    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, dump));
    auto dense_column = tatami::convert_to_dense<double, int>(*dense_row, false, {});
    auto sparse_row = tatami::convert_to_compressed_sparse<double, int>(*dense_row, true, {});
    auto sparse_column = tatami::convert_to_compressed_sparse<double, int>(*dense_row, false, {});

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

    tatami_stats::variance::Options vopt;
    vopt.skip_nan = true;
    compare_result(tatami_stats::variance::apply(false, *dense_row, vopt), expectedm, ref);
    compare_result(tatami_stats::variance::apply(false, *dense_column, vopt), expectedm, ref);
    compare_result(tatami_stats::variance::apply(false, *sparse_row, vopt), expectedm, ref);
    compare_result(tatami_stats::variance::apply(false, *sparse_column, vopt), expectedm, ref);

    // Same results with parallelization.
    vopt.num_threads = 3;
    compare_result(tatami_stats::variance::apply(false, *dense_row, vopt), expectedm, ref);
    compare_result(tatami_stats::variance::apply(false, *dense_column, vopt), expectedm, ref);
    compare_result(tatami_stats::variance::apply(false, *sparse_row, vopt), expectedm, ref);
    compare_result(tatami_stats::variance::apply(false, *sparse_column, vopt), expectedm, ref);
}

TEST(Variance, NewType) {
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
    auto rexpected = tatami_stats::variance::apply(true, *ref, {});
    auto cexpected = tatami_stats::variance::apply(false, *ref, {});

    std::vector<std::int8_t> ivec(dump.begin(), dump.end());
    auto dense_row = std::make_shared<tatami::DenseRowMatrix<std::int8_t, std::uint8_t> >(NR, NC, std::move(ivec));
    auto dense_column = tatami::convert_to_dense<double, int>(*dense_row, false, {});
    auto sparse_row = tatami::convert_to_compressed_sparse<double, int>(*dense_row, true, {});
    auto sparse_column = tatami::convert_to_compressed_sparse<double, int>(*dense_row, false, {});

    compare_result(tatami_stats::variance::apply(true, *dense_row, {}), rexpected.mean, rexpected.variance);
    compare_result(tatami_stats::variance::apply(true, *dense_column, {}), rexpected.mean, rexpected.variance);
    compare_result(tatami_stats::variance::apply(true, *sparse_row, {}), rexpected.mean, rexpected.variance);
    compare_result(tatami_stats::variance::apply(true, *sparse_column, {}), rexpected.mean, rexpected.variance);

    compare_result(tatami_stats::variance::apply(false, *dense_row, {}), cexpected.mean, cexpected.variance);
    compare_result(tatami_stats::variance::apply(false, *dense_column, {}), cexpected.mean, cexpected.variance);
    compare_result(tatami_stats::variance::apply(false, *sparse_row, {}), cexpected.mean, cexpected.variance);
    compare_result(tatami_stats::variance::apply(false, *sparse_column, {}), cexpected.mean, cexpected.variance);

    // Still behaves with NaN skipping.
    tatami_stats::variance::Options vopt;
    vopt.skip_nan = true;
    compare_result(tatami_stats::variance::apply(true, *dense_row, vopt), rexpected.mean, rexpected.variance);
    compare_result(tatami_stats::variance::apply(true, *dense_column, vopt), rexpected.mean, rexpected.variance);
    compare_result(tatami_stats::variance::apply(true, *sparse_row, vopt), rexpected.mean, rexpected.variance);
    compare_result(tatami_stats::variance::apply(true, *sparse_column, vopt), rexpected.mean, rexpected.variance);

    compare_result(tatami_stats::variance::apply(false, *dense_row, vopt), cexpected.mean, cexpected.variance);
    compare_result(tatami_stats::variance::apply(false, *dense_column, vopt), cexpected.mean, cexpected.variance);
    compare_result(tatami_stats::variance::apply(false, *sparse_row, vopt), cexpected.mean, cexpected.variance);
    compare_result(tatami_stats::variance::apply(false, *sparse_column, vopt), cexpected.mean, cexpected.variance);
}

TEST(Variance, DirtyOutput) {
    size_t NR = 99, NC = 152;
    auto dump = tatami_test::simulate_vector<double>(NR * NC, []{ 
        tatami_test::SimulateVectorOptions opt;
        opt.density = 0.1;
        opt.seed = 457633478;
        return opt;
    }());

    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, dump));
    auto dense_column = tatami::convert_to_dense<double, int>(*dense_row, false, {});
    auto sparse_row = tatami::convert_to_compressed_sparse<double, int>(*dense_row, true, {});
    auto sparse_column = tatami::convert_to_compressed_sparse<double, int>(*dense_row, false, {});

    auto ref = tatami_stats::variance::apply(true, *dense_row, {});

    // Works when the input vector is a bit dirty.
    std::vector<double> dirtym(NR, -1), dirtyv(NR, -1);
    tatami_stats::variance::Buffers<double> buf;
    buf.mean = dirtym.data();
    buf.variance = dirtyv.data();
    tatami_stats::variance::apply(true, *dense_row, buf, {});
    compare_result(ref, dirtym, dirtyv);

    std::fill(dirtym.begin(), dirtym.end(), -1);
    std::fill(dirtyv.begin(), dirtyv.end(), -1);
    tatami_stats::variance::apply(true, *dense_column, buf, {});
    compare_result(ref, dirtym, dirtyv);

    std::fill(dirtym.begin(), dirtym.end(), -1);
    std::fill(dirtyv.begin(), dirtyv.end(), -1);
    tatami_stats::variance::apply(true, *sparse_row, buf, {});
    compare_result(ref, dirtym, dirtyv);

    std::fill(dirtym.begin(), dirtym.end(), -1);
    std::fill(dirtyv.begin(), dirtyv.end(), -1);
    tatami_stats::variance::apply(true, *sparse_column, buf, {});
    compare_result(ref, dirtym, dirtyv);

    // Still behaves with NaN skipping.
    tatami_stats::variance::Options vopt;
    vopt.skip_nan = true;

    std::fill(dirtym.begin(), dirtym.end(), -1);
    std::fill(dirtyv.begin(), dirtyv.end(), -1);
    tatami_stats::variance::apply(true, *dense_row, buf, vopt);
    compare_result(ref, dirtym, dirtyv);

    std::fill(dirtym.begin(), dirtym.end(), -1);
    std::fill(dirtyv.begin(), dirtyv.end(), -1);
    tatami_stats::variance::apply(true, *dense_column, buf, vopt);
    compare_result(ref, dirtym, dirtyv);

    std::fill(dirtym.begin(), dirtym.end(), -1);
    std::fill(dirtyv.begin(), dirtyv.end(), -1);
    tatami_stats::variance::apply(true, *sparse_row, buf, vopt);
    compare_result(ref, dirtym, dirtyv);

    std::fill(dirtym.begin(), dirtym.end(), -1);
    std::fill(dirtyv.begin(), dirtyv.end(), -1);
    tatami_stats::variance::apply(true, *sparse_column, buf, vopt);
    compare_result(ref, dirtym, dirtyv);
}

TEST(Variance, InvalidVariances) {
    tatami_stats::variance::Options vopt;
    vopt.skip_nan = true;

    // No observations.
    {
        auto dense = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(111, 0, std::vector<double>()));

        auto cref = tatami_stats::variance::apply(false, *dense, {});
        EXPECT_EQ(cref.mean.size(), 0);
        EXPECT_EQ(cref.variance.size(), 0);

        auto cref2 = tatami_stats::variance::apply(false, *dense, vopt);
        EXPECT_EQ(cref2.mean.size(), 0);
        EXPECT_EQ(cref2.variance.size(), 0);

        auto rref = tatami_stats::variance::apply(true, *dense, {});
        EXPECT_EQ(rref.variance.size(), dense->nrow());
        EXPECT_TRUE(is_all_nan(rref.mean));
        EXPECT_TRUE(is_all_nan(rref.variance));

        auto rref2 = tatami_stats::variance::apply(true, *dense, vopt);
        EXPECT_EQ(rref2.variance.size(), dense->nrow());
        EXPECT_TRUE(is_all_nan(rref2.mean));
        EXPECT_TRUE(is_all_nan(rref2.variance));
    }

    // One observation.
    {
        auto dense = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(10, 1, std::vector<double>(10)));

        auto rref = tatami_stats::variance::apply(true, *dense, {});
        EXPECT_EQ(rref.mean, std::vector<double>(10));
        EXPECT_EQ(rref.variance.size(), dense->nrow());
        EXPECT_TRUE(is_all_nan(rref.variance));

        auto rref2 = tatami_stats::variance::apply(true, *dense, vopt);
        EXPECT_EQ(rref2.mean, std::vector<double>(10));
        EXPECT_EQ(rref2.variance.size(), dense->nrow());
        EXPECT_TRUE(is_all_nan(rref2.variance));
    }
}
