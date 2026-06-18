#include <gtest/gtest.h>

#include <cstdint>
#include <vector>

#include "utils.h"
#include "tatami_stats/rss.hpp"
#include "tatami_test/tatami_test.hpp"

static void compare_result(
    const tatami_stats::RssResult<double>& res,
    const std::vector<double>& expected_mean,
    const std::vector<double>& expected_rss
) {
    compare_double_vectors(expected_rss, res.rss);
    compare_double_vectors(expected_mean, res.mean);
}

/*******************************/

class RssTest : public ::testing::TestWithParam<int> {};

TEST_P(RssTest, Row) {
    const auto num_threads = GetParam();

    size_t NR = 109, NC = 82;
    auto dump = tatami_test::simulate_vector<double>(NR * NC, [&]{
        tatami_test::SimulateVectorOptions opt;
        opt.density = 0.1;
        opt.seed = 1987191 + num_threads;
        return opt;
    }());

    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, dump));
    auto dense_column = tatami::convert_to_dense<double, int>(*dense_row, false, {});
    auto sparse_row = tatami::convert_to_compressed_sparse<double, int>(*dense_row, true, {});
    auto sparse_column = tatami::convert_to_compressed_sparse<double, int>(*dense_row, false, {});

    // Doing the difference of squares as a quick-and-dirty reference.
    std::vector<double> expectedm(NR), refrss(NR);
    for (size_t r = 0; r < NR; ++r) {
        for (size_t c = 0; c < NC; ++c) {
            double x = dump[c + r * NC];
            expectedm[r] += x;
            refrss[r] += x * x;
        }
        expectedm[r] /= NC;
        refrss[r] -= expectedm[r] * expectedm[r] * NC;
    }

    tatami_stats::RssOptions ropt;
    ropt.num_threads = num_threads;
    compare_result(tatami_stats::rss(true, *dense_row, ropt), expectedm, refrss);
    compare_result(tatami_stats::rss(true, *dense_column, ropt), expectedm, refrss);
    compare_result(tatami_stats::rss(true, *sparse_row, ropt), expectedm, refrss);
    compare_result(tatami_stats::rss(true, *sparse_column, ropt), expectedm, refrss);
}

TEST_P(RssTest, Column) {
    const auto num_threads = GetParam();

    size_t NR = 99, NC = 92;
    auto dump = tatami_test::simulate_vector<double>(NR * NC, [&]{
        tatami_test::SimulateVectorOptions opt;
        opt.density = 0.1;
        opt.seed = 21823818 + num_threads;
        return opt;
    }());

    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, dump));
    auto dense_column = tatami::convert_to_dense<double, int>(*dense_row, false, {});
    auto sparse_row = tatami::convert_to_compressed_sparse<double, int>(*dense_row, true, {});
    auto sparse_column = tatami::convert_to_compressed_sparse<double, int>(*dense_row, false, {});

    // Doing the difference of squares as a quick-and-dirty reference.
    std::vector<double> expectedm(NC), refrss(NC);
    for (size_t c = 0; c < NC; ++c) {
        for (size_t r = 0; r < NR; ++r) {
            double x = dump[c + r * NC];
            expectedm[c] += x;
            refrss[c] += x * x;
        }
        expectedm[c] /= NR;
        refrss[c] -= NR * expectedm[c] * expectedm[c];
    }

    tatami_stats::RssOptions ropt;
    ropt.num_threads = num_threads;
    compare_result(tatami_stats::rss(false, *dense_row, ropt), expectedm, refrss);
    compare_result(tatami_stats::rss(false, *dense_column, ropt), expectedm, refrss);
    compare_result(tatami_stats::rss(false, *sparse_row, ropt), expectedm, refrss);
    compare_result(tatami_stats::rss(false, *sparse_column, ropt), expectedm, refrss);
}

INSTANTIATE_TEST_SUITE_P(
    Rss,
    RssTest,
    ::testing::Values(1, 3)
);

/*******************************/

TEST(Rss, NewType) {
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
    auto rexpected = tatami_stats::rss(true, *ref, {});
    auto cexpected = tatami_stats::rss(false, *ref, {});

    std::vector<std::int8_t> ivec(dump.begin(), dump.end());
    auto dense_row = std::make_shared<tatami::DenseRowMatrix<std::int8_t, std::uint8_t> >(NR, NC, std::move(ivec));
    auto dense_column = tatami::convert_to_dense<double, int>(*dense_row, false, {});
    auto sparse_row = tatami::convert_to_compressed_sparse<double, int>(*dense_row, true, {});
    auto sparse_column = tatami::convert_to_compressed_sparse<double, int>(*dense_row, false, {});

    compare_result(tatami_stats::rss(true, *dense_row, {}), rexpected.mean, rexpected.rss);
    compare_result(tatami_stats::rss(true, *dense_column, {}), rexpected.mean, rexpected.rss);
    compare_result(tatami_stats::rss(true, *sparse_row, {}), rexpected.mean, rexpected.rss);
    compare_result(tatami_stats::rss(true, *sparse_column, {}), rexpected.mean, rexpected.rss);

    compare_result(tatami_stats::rss(false, *dense_row, {}), cexpected.mean, cexpected.rss);
    compare_result(tatami_stats::rss(false, *dense_column, {}), cexpected.mean, cexpected.rss);
    compare_result(tatami_stats::rss(false, *sparse_row, {}), cexpected.mean, cexpected.rss);
    compare_result(tatami_stats::rss(false, *sparse_column, {}), cexpected.mean, cexpected.rss);
}

/*******************************/

class RssEdgeTest : public ::testing::TestWithParam<int> {};

TEST_P(RssEdgeTest, NoObservations) {
    auto dense_row = std::shared_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(111, 0, std::vector<double>()));
    auto dense_column = tatami::convert_to_dense<double, int>(*dense_row, false, {});
    auto sparse_row = tatami::convert_to_compressed_sparse<double, int>(*dense_row, true, {});
    auto sparse_column = tatami::convert_to_compressed_sparse<double, int>(*dense_row, false, {});

    tatami_stats::RssOptions vopt;
    vopt.num_threads = GetParam();

    auto check_ok = [&](const tatami_stats::RssResult<double>& res) -> void {
        EXPECT_EQ(res.rss.size(), 111);
        EXPECT_TRUE(is_all_nan(res.mean));
        EXPECT_EQ(res.rss, std::vector<double>(111));
    };

    check_ok(tatami_stats::rss(true, *dense_row, vopt));
    check_ok(tatami_stats::rss(true, *dense_column, vopt));
    check_ok(tatami_stats::rss(true, *sparse_row, vopt));
    check_ok(tatami_stats::rss(true, *sparse_column, vopt));
}

TEST_P(RssEdgeTest, ZeroExtent) {
    auto dense_row = std::shared_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(0, 99, std::vector<double>()));
    auto dense_column = tatami::convert_to_dense<double, int>(*dense_row, false, {});
    auto sparse_row = tatami::convert_to_compressed_sparse<double, int>(*dense_row, true, {});
    auto sparse_column = tatami::convert_to_compressed_sparse<double, int>(*dense_row, false, {});

    tatami_stats::RssOptions vopt;
    vopt.num_threads = GetParam();

    auto check_ok = [&](const tatami_stats::RssResult<double>& res) -> void {
        EXPECT_EQ(res.mean.size(), 0);
        EXPECT_EQ(res.rss.size(), 0);
    };

    check_ok(tatami_stats::rss(true, *dense_row, vopt));
    check_ok(tatami_stats::rss(true, *dense_column, vopt));
    check_ok(tatami_stats::rss(true, *sparse_row, vopt));
    check_ok(tatami_stats::rss(true, *sparse_column, vopt));
}

TEST_P(RssEdgeTest, OneObservation) {
    auto dense_row = std::shared_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(10, 1, std::vector<double>(10)));
    auto dense_column = tatami::convert_to_dense<double, int>(*dense_row, false, {});
    auto sparse_row = tatami::convert_to_compressed_sparse<double, int>(*dense_row, true, {});
    auto sparse_column = tatami::convert_to_compressed_sparse<double, int>(*dense_row, false, {});

    tatami_stats::RssOptions vopt;
    vopt.num_threads = GetParam();

    auto check_ok = [&](const tatami_stats::RssResult<double>& res) -> void {
        EXPECT_EQ(res.mean, std::vector<double>(10));
        EXPECT_EQ(res.rss, std::vector<double>(10));
    };

    check_ok(tatami_stats::rss(true, *dense_row, vopt));
    check_ok(tatami_stats::rss(true, *dense_column, vopt));
    check_ok(tatami_stats::rss(true, *sparse_row, vopt));
    check_ok(tatami_stats::rss(true, *sparse_column, vopt));
}

INSTANTIATE_TEST_SUITE_P(
    Rss,
    RssEdgeTest,
    ::testing::Values(1, 3)
);
