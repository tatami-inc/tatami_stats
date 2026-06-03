#include <gtest/gtest.h>

#include <vector>
#include <cmath>
#include <algorithm>
#include <cstdint>
#include <cstddef>

#include "tatami_stats/quantile.hpp"
#include "tatami_test/tatami_test.hpp"

#include "utils.h"

class QuantileTest : public ::testing::TestWithParam<std::tuple<double, double> > {};

TEST_P(QuantileTest, RowSimple) {
    size_t NR = 1234, NC = 222;
    const auto params = GetParam();
    const auto prob = std::get<0>(params);
    const auto status = std::get<1>(params);

    auto vec = tatami_test::simulate_vector<double>(NR * NC, [&]{
        tatami_test::SimulateVectorOptions opt;

        if (status == -1) {
            opt.lower = -10;
            opt.upper = -1;
        } else if (status == 0) {
            opt.lower = -10;
            opt.upper = 10;
        } else {
            opt.lower = 1;
            opt.upper = 10;
        }

        opt.seed = 734686273 * prob + status * 10;
        return opt;
    }());

    std::mt19937_64 rng(6969 * prob + status);
    inject_variable_zeros(NR, NC, vec, rng);

    auto dense_row = std::make_unique<tatami::DenseRowMatrix<double, int> >(NR, NC, std::move(vec));
    auto dense_column = tatami::convert_to_dense<double, int>(*dense_row, false, {});
    auto sparse_row = tatami::convert_to_compressed_sparse<double, int>(*dense_row, true, {});
    auto sparse_column = tatami::convert_to_compressed_sparse<double, int>(*dense_row, false, {});

    auto rref = tatami_stats::quantile::apply(true, *dense_row, prob, {});
    EXPECT_EQ(rref.size(), NR);
    EXPECT_EQ(rref, tatami_stats::quantile::apply(true, *dense_column, prob, {}));
    EXPECT_EQ(rref, tatami_stats::quantile::apply(true, *sparse_row, prob, {}));
    EXPECT_EQ(rref, tatami_stats::quantile::apply(true, *sparse_column, prob, {}));

    // Checking that the parallel code is the same.
    tatami_stats::quantile::Options qopt;
    qopt.num_threads = 3;
    EXPECT_EQ(rref, tatami_stats::quantile::apply(true, *dense_row, prob, qopt));
    EXPECT_EQ(rref, tatami_stats::quantile::apply(true, *dense_column, prob, qopt));
    EXPECT_EQ(rref, tatami_stats::quantile::apply(true, *sparse_row, prob, qopt));
    EXPECT_EQ(rref, tatami_stats::quantile::apply(true, *sparse_column, prob, qopt));

    // Checking that the results are the same with NaN skipping.
    qopt.num_threads = 1;
    qopt.skip_nan = true;
    EXPECT_EQ(rref, tatami_stats::quantile::apply(true, *dense_row, prob, qopt));
    EXPECT_EQ(rref, tatami_stats::quantile::apply(true, *dense_column, prob, qopt));
    EXPECT_EQ(rref, tatami_stats::quantile::apply(true, *sparse_row, prob, qopt));
    EXPECT_EQ(rref, tatami_stats::quantile::apply(true, *sparse_column, prob, qopt));

    // Checking same results from matrices that can yield unsorted indices.
    auto unsorted_row = std::make_unique<tatami_test::ReversedIndicesWrapper<double, int> >(sparse_row);
    EXPECT_EQ(rref, tatami_stats::quantile::apply(true, *unsorted_row, prob, {}));
    auto unsorted_column = std::make_unique<tatami_test::ReversedIndicesWrapper<double, int> >(sparse_column);
    EXPECT_EQ(rref, tatami_stats::quantile::apply(true, *unsorted_column, prob, {}));
}

TEST_P(QuantileTest, ColumnSimple) {
    size_t NR = 1234, NC = 222;
    const auto params = GetParam();
    const auto prob = std::get<0>(params);
    const auto status = std::get<1>(params);

    auto vec = tatami_test::simulate_vector<double>(NR * NC, [&]{
        tatami_test::SimulateVectorOptions opt;

        if (status == -1) {
            opt.lower = -10;
            opt.upper = -1;
        } else if (status == 0) {
            opt.lower = -10;
            opt.upper = 10;
        } else {
            opt.lower = 1;
            opt.upper = 10;
        }

        opt.seed = 734686273 * prob + status * 10;
        return opt;
    }());

    std::mt19937_64 rng(6969 * prob + status);
    inject_variable_zeros(NC, NR, vec, rng);

    auto dense_column = std::make_unique<tatami::DenseColumnMatrix<double, int> >(NR, NC, std::move(vec));
    auto dense_row = tatami::convert_to_dense<double, int>(*dense_column, true, {});
    auto sparse_row = tatami::convert_to_compressed_sparse<double, int>(*dense_row, true, {});
    auto sparse_column = tatami::convert_to_compressed_sparse<double, int>(*dense_row, false, {});

    auto cref = tatami_stats::quantile::apply(false, *dense_row, prob, {});
    EXPECT_EQ(cref.size(), NC);
    EXPECT_EQ(cref, tatami_stats::quantile::apply(false, *dense_column, prob, {}));
    EXPECT_EQ(cref, tatami_stats::quantile::apply(false, *sparse_row, prob, {}));
    EXPECT_EQ(cref, tatami_stats::quantile::apply(false, *sparse_column, prob, {}));

    // Checking that the parallel code is the same.
    tatami_stats::quantile::Options qopt;
    qopt.num_threads = 3;
    EXPECT_EQ(cref, tatami_stats::quantile::apply(false, *dense_row, prob, qopt));
    EXPECT_EQ(cref, tatami_stats::quantile::apply(false, *dense_column, prob, qopt));
    EXPECT_EQ(cref, tatami_stats::quantile::apply(false, *sparse_row, prob, qopt));
    EXPECT_EQ(cref, tatami_stats::quantile::apply(false, *sparse_column, prob, qopt));

    // Checking that the results are the same when skipping NaNs.
    qopt.skip_nan = true;
    qopt.num_threads = 1;
    EXPECT_EQ(cref, tatami_stats::quantile::apply(false, *dense_row, prob, qopt));
    EXPECT_EQ(cref, tatami_stats::quantile::apply(false, *dense_column, prob, qopt));
    EXPECT_EQ(cref, tatami_stats::quantile::apply(false, *sparse_row, prob, qopt));
    EXPECT_EQ(cref, tatami_stats::quantile::apply(false, *sparse_column, prob, qopt));

    // Checking same results from matrices that can yield unsorted indices.
    auto unsorted_row = std::make_unique<tatami_test::ReversedIndicesWrapper<double, int> >(sparse_row);
    EXPECT_EQ(cref, tatami_stats::quantile::apply(false, *unsorted_row, prob, {}));
    auto unsorted_column = std::make_unique<tatami_test::ReversedIndicesWrapper<double, int> >(sparse_column);
    EXPECT_EQ(cref, tatami_stats::quantile::apply(false, *unsorted_column, prob, {}));
}

TEST_P(QuantileTest, RowWithNan) {
    size_t NR = 352, NC = 283;
    const auto params = GetParam();
    const auto prob = std::get<0>(params);
    const auto status = std::get<1>(params);

    auto vec = tatami_test::simulate_vector<double>(NR * NC, [&]{
        tatami_test::SimulateVectorOptions opt;

        if (status == -1) {
            opt.lower = -10;
            opt.upper = -1;
        } else if (status == 0) {
            opt.lower = -10;
            opt.upper = 10;
        } else {
            opt.lower = 1;
            opt.upper = 10;
        }

        opt.seed = 12317 * prob + status * 10;
        return opt;
    }());

    std::mt19937_64 rng(6969 * prob + status);
    inject_variable_zeros(NR, NC, vec, rng);
    for (size_t r = 0; r < NR; ++r) { // Injecting an NaN randomly into each row.
        const auto failed = rng() % NC;
        vec[failed + r * NC] = std::numeric_limits<double>::quiet_NaN();
    }

    std::vector<double> stripped;
    stripped.reserve(NR * (NC - 1));
    for (auto x : vec) {
        if (!std::isnan(x)) {
            stripped.push_back(x);
        }
    }

    auto dense_row = std::make_unique<tatami::DenseRowMatrix<double, int> >(NR, NC, std::move(vec));
    auto dense_column = tatami::convert_to_dense<double, int>(*dense_row, false, {});
    auto sparse_row = tatami::convert_to_compressed_sparse<double, int>(*dense_row, true, {});
    auto sparse_column = tatami::convert_to_compressed_sparse<double, int>(*dense_row, false, {});

    auto ref = std::make_unique<tatami::DenseRowMatrix<double, int> >(NR, NC - 1, std::move(stripped));
    auto rref = tatami_stats::quantile::apply(true, *ref, prob, {});
    auto cref = tatami_stats::quantile::apply(false, *ref, prob, {});

    tatami_stats::quantile::Options qopt;
    qopt.skip_nan = true;
    EXPECT_EQ(rref, tatami_stats::quantile::apply(true, *dense_row, prob, qopt));
    EXPECT_EQ(rref, tatami_stats::quantile::apply(true, *dense_column, prob, qopt));
    EXPECT_EQ(rref, tatami_stats::quantile::apply(true, *sparse_row, prob, qopt));
    EXPECT_EQ(rref, tatami_stats::quantile::apply(true, *sparse_column, prob, qopt));

    // Same results with parallelization.
    qopt.num_threads = 3;
    EXPECT_EQ(rref, tatami_stats::quantile::apply(true, *dense_row, prob, qopt));
    EXPECT_EQ(rref, tatami_stats::quantile::apply(true, *dense_column, prob, qopt));
    EXPECT_EQ(rref, tatami_stats::quantile::apply(true, *sparse_row, prob, qopt));
    EXPECT_EQ(rref, tatami_stats::quantile::apply(true, *sparse_column, prob, qopt));
}

TEST_P(QuantileTest, ColumnWithNan) {
    size_t NR = 352, NC = 283;
    const auto params = GetParam();
    const auto prob = std::get<0>(params);
    const auto status = std::get<1>(params);

    auto vec = tatami_test::simulate_vector<double>(NR * NC, [&]{
        tatami_test::SimulateVectorOptions opt;

        if (status == -1) {
            opt.lower = -10;
            opt.upper = -1;
        } else if (status == 0) {
            opt.lower = -10;
            opt.upper = 10;
        } else {
            opt.lower = 1;
            opt.upper = 10;
        }

        opt.seed = 12317 * prob + status * 10;
        return opt;
    }());

    std::mt19937_64 rng(696969 * prob + status);
    inject_variable_zeros(NC, NR, vec, rng);

    for (size_t c = 0; c < NC; ++c) { // Injecting an NaN randomly into each column.
        const auto failed = rng() % NR;
        vec[failed + c * NR] = std::numeric_limits<double>::quiet_NaN();
    }

    std::vector<double> stripped;
    stripped.reserve((NR - 1) * NC);
    for (auto x : vec) {
        if (!std::isnan(x)) {
            stripped.push_back(x);
        }
    }

    auto dense_column = std::make_unique<tatami::DenseColumnMatrix<double, int> >(NR, NC, std::move(vec));
    auto dense_row = tatami::convert_to_dense<double, int>(*dense_column, true, {});
    auto sparse_row = tatami::convert_to_compressed_sparse<double, int>(*dense_row, true, {});
    auto sparse_column = tatami::convert_to_compressed_sparse<double, int>(*dense_row, false, {});

    auto ref = std::make_unique<tatami::DenseColumnMatrix<double, int> >(NR - 1, NC, std::move(stripped));
    auto cref = tatami_stats::quantile::apply(false, *ref, prob, {});

    tatami_stats::quantile::Options qopt;
    qopt.skip_nan = true;
    EXPECT_EQ(cref, tatami_stats::quantile::apply(false, *dense_row, prob, qopt));
    EXPECT_EQ(cref, tatami_stats::quantile::apply(false, *dense_column, prob, qopt));
    EXPECT_EQ(cref, tatami_stats::quantile::apply(false, *sparse_row, prob, qopt));
    EXPECT_EQ(cref, tatami_stats::quantile::apply(false, *sparse_column, prob, qopt));

    // Same results with parallelization.
    qopt.num_threads = 3;
    EXPECT_EQ(cref, tatami_stats::quantile::apply(false, *dense_row, prob, qopt));
    EXPECT_EQ(cref, tatami_stats::quantile::apply(false, *dense_column, prob, qopt));
    EXPECT_EQ(cref, tatami_stats::quantile::apply(false, *sparse_row, prob, qopt));
    EXPECT_EQ(cref, tatami_stats::quantile::apply(false, *sparse_column, prob, qopt));
}

INSTANTIATE_TEST_SUITE_P(
    Quantile,
    QuantileTest,
    ::testing::Combine(
        ::testing::Values(0.0, 0.25, 0.5, 0.75, 1.0),
        ::testing::Values(-1, 0, 1)
    )
);

/****************************************/

TEST(Quantile, AllZero) {
    size_t NR = 55, NC = 22;
    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, std::vector<double>(NR * NC)));
    auto dense_column = tatami::convert_to_dense<double, int>(*dense_row, false, {});
    auto sparse_row = tatami::convert_to_compressed_sparse<double, int>(*dense_row, true, {});
    auto sparse_column = tatami::convert_to_compressed_sparse<double, int>(*dense_row, false, {});

    auto ref = tatami_stats::quantile::apply(true, *dense_row, 0.5, {});
    EXPECT_EQ(ref, std::vector<double>(NR));
    EXPECT_EQ(ref, tatami_stats::quantile::apply(true, *dense_column, 0.5, {}));
    EXPECT_EQ(ref, tatami_stats::quantile::apply(true, *sparse_row, 0.5, {}));
    EXPECT_EQ(ref, tatami_stats::quantile::apply(true, *sparse_column, 0.5, {}));

    ref = tatami_stats::quantile::apply(false, *dense_row, 0.5, {});
    EXPECT_EQ(ref, std::vector<double>(NC));
    EXPECT_EQ(ref, tatami_stats::quantile::apply(false, *dense_column, 0.5, {}));
    EXPECT_EQ(ref, tatami_stats::quantile::apply(false, *sparse_row, 0.5, {}));
    EXPECT_EQ(ref, tatami_stats::quantile::apply(false, *sparse_column, 0.5, {}));
}

TEST(Quantile, Empty) {
    auto dense = std::make_unique<tatami::DenseRowMatrix<double, int> >(111, 0, std::vector<double>());

    auto cref = tatami_stats::quantile::apply(false, *dense, 0.5, {});
    EXPECT_EQ(cref.size(), 0);

    auto rref = tatami_stats::quantile::apply(true, *dense, 0.5, {});
    EXPECT_TRUE(rref.size() > 0);
    for (auto r : rref) {
        EXPECT_TRUE(std::isnan(r));
    }
}

TEST(Quantile, NewType) {
    size_t NR = 198, NC = 52;
    auto dump = tatami_test::simulate_vector<double>(NR * NC, []{
        tatami_test::SimulateVectorOptions opt;
        opt.density = 0.5;  // using 0.5 to make things interesting with a 0.5 quantile probability.
        opt.lower = 1;
        opt.upper = 100;
        opt.seed = 293876423;
        return opt;
    }());
    for (auto& d : dump) { 
        d = std::round(d);
    }

    auto ref = std::make_unique<tatami::DenseRowMatrix<double, int> >(NR, NC, dump);
    auto rexpected = tatami_stats::quantile::apply(true, *ref, 0.5, {});
    auto cexpected = tatami_stats::quantile::apply(false, *ref, 0.5, {});

    std::vector<std::int8_t> ivec(dump.begin(), dump.end());
    auto dense_row = std::make_unique<tatami::DenseRowMatrix<std::int8_t, std::uint8_t> >(NR, NC, std::move(ivec));
    auto dense_column = tatami::convert_to_dense<double, int>(*dense_row, false, {});
    auto sparse_row = tatami::convert_to_compressed_sparse<double, int>(*dense_row, true, {});
    auto sparse_column = tatami::convert_to_compressed_sparse<double, int>(*dense_row, false, {});

    EXPECT_EQ(tatami_stats::quantile::apply(true, *dense_row, 0.5, {}), rexpected);
    EXPECT_EQ(tatami_stats::quantile::apply(true, *dense_column, 0.5, {}), rexpected);
    EXPECT_EQ(tatami_stats::quantile::apply(true, *sparse_row, 0.5, {}), rexpected);
    EXPECT_EQ(tatami_stats::quantile::apply(true, *sparse_column, 0.5, {}), rexpected);

    EXPECT_EQ(tatami_stats::quantile::apply(false, *dense_row, 0.5, {}), cexpected);
    EXPECT_EQ(tatami_stats::quantile::apply(false, *dense_column, 0.5, {}), cexpected);
    EXPECT_EQ(tatami_stats::quantile::apply(false, *sparse_row, 0.5, {}), cexpected);
    EXPECT_EQ(tatami_stats::quantile::apply(false, *sparse_column, 0.5, {}), cexpected);

    // Skipping NaN.
    tatami_stats::quantile::Options opt;
    opt.skip_nan = true;

    EXPECT_EQ(tatami_stats::quantile::apply(true, *dense_row, 0.5, opt), rexpected);
    EXPECT_EQ(tatami_stats::quantile::apply(true, *dense_column, 0.5, opt), rexpected);
    EXPECT_EQ(tatami_stats::quantile::apply(true, *sparse_row, 0.5, opt), rexpected);
    EXPECT_EQ(tatami_stats::quantile::apply(true, *sparse_column, 0.5, opt), rexpected);

    EXPECT_EQ(tatami_stats::quantile::apply(false, *dense_row, 0.5, opt), cexpected);
    EXPECT_EQ(tatami_stats::quantile::apply(false, *dense_column, 0.5, opt), cexpected);
    EXPECT_EQ(tatami_stats::quantile::apply(false, *sparse_row, 0.5, opt), cexpected);
    EXPECT_EQ(tatami_stats::quantile::apply(false, *sparse_column, 0.5, opt), cexpected);
}
