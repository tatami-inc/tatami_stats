#include <gtest/gtest.h>

#include <cstdint>
#include <vector>

#include "utils.h"
#include "../utils.h"
#include "tatami_stats/skip_nan/rss.hpp"
#include "tatami_test/tatami_test.hpp"

static void compare_result(
    const tatami_stats::skip_nan::RssResult<double, int>& res,
    const std::vector<double>& expected_mean,
    const std::vector<double>& expected_rss,
    const std::vector<int>& expected_count 
) {
    compare_double_vectors(expected_rss, res.rss);
    compare_double_vectors(expected_mean, res.mean);
    EXPECT_EQ(expected_count, res.count);
}

/*******************************/

class SkipNanRssTest : public ::testing::TestWithParam<std::tuple<SkipNanSimulationType, int> > {};

TEST_P(SkipNanRssTest, Row) {
    const auto params = GetParam();
    const auto nan_type = std::get<0>(params);
    const auto num_threads = std::get<1>(params);

    size_t NR = 52, NC = 83;
    const unsigned long long seed = 44982197 + (int)nan_type + num_threads;
    auto dump = tatami_test::simulate_vector<double>(NR * NC, [&]{
        tatami_test::SimulateVectorOptions opt;
        opt.density = 0.1;
        opt.seed = seed;
        return opt;
    }());
    inject_nans_by_row(dump, NR, NC, nan_type, /* seed = */ 213 + seed);

    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, dump));
    auto dense_column = tatami::convert_to_dense(dense_row.get(), false);
    auto sparse_row = tatami::convert_to_compressed_sparse(dense_row.get(), true);
    auto sparse_column = tatami::convert_to_compressed_sparse(dense_row.get(), false);

    std::vector<double> expectedm(NR), refrss(NR);
    std::vector<int> count(NR);
    std::vector<double> current(NC);
    quickstats::RssWorkspace<double> wrk;
    for (size_t r = 0; r < NR; ++r) {
        std::copy_n(dump.data() + NC * r, NC, current.data());
        const auto unskip_size = quickstats::skip_values(current.size(), current.data(), [](const std::size_t, const double val) -> bool { return std::isnan(val); });
        const auto unskip_res = quickstats::rss(unskip_size, current.data(), wrk, quickstats::RssOptions());
        expectedm[r] = unskip_res.mean;
        refrss[r] = unskip_res.rss;
        count[r] = unskip_size;
    }

    tatami_stats::skip_nan::RssOptions vopt;
    vopt.num_threads = num_threads;
    compare_result(tatami_stats::skip_nan::rss<double, int>(true, *dense_row, vopt), expectedm, refrss, count);
    compare_result(tatami_stats::skip_nan::rss<double, int>(true, *dense_column, vopt), expectedm, refrss, count);
    compare_result(tatami_stats::skip_nan::rss<double, int>(true, *sparse_row, vopt), expectedm, refrss, count);
    compare_result(tatami_stats::skip_nan::rss<double, int>(true, *sparse_column, vopt), expectedm, refrss, count);
}

TEST_P(SkipNanRssTest, Column) {
    const auto params = GetParam();
    const auto nan_type = std::get<0>(params);
    const auto num_threads = std::get<1>(params);

    size_t NR = 82, NC = 33;
    const unsigned long long seed = num_threads + (int)nan_type;
    auto dump = tatami_test::simulate_vector<double>(NR * NC, [&]{
        tatami_test::SimulateVectorOptions opt;
        opt.density = 0.1;
        opt.seed = 191353 + seed;
        return opt;
    }());
    inject_nans_by_column(dump, NR, NC, nan_type, /* seed = */ 21 + seed);

    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, dump));
    auto dense_column = tatami::convert_to_dense<double, int>(*dense_row, false, {});
    auto sparse_row = tatami::convert_to_compressed_sparse<double, int>(*dense_row, true, {});
    auto sparse_column = tatami::convert_to_compressed_sparse<double, int>(*dense_row, false, {});

    std::vector<double> expectedm(NC), refrss(NC);
    std::vector<int> count(NC);
    std::vector<double> current(NR);
    quickstats::RssWorkspace<double> wrk;
    for (size_t c = 0; c < NC; ++c) {
        for (size_t r = 0; r < NR; ++r) {
            current[r] = dump[c + r * NC];
        }
        const auto unskip_size = quickstats::skip_values(current.size(), current.data(), [](const std::size_t, const double val) -> bool { return std::isnan(val); });
        const auto unskip_res = quickstats::rss(unskip_size, current.data(), wrk, quickstats::RssOptions());
        expectedm[c] = unskip_res.mean;
        refrss[c] = unskip_res.rss;
        count[c] = unskip_size;
    }

    tatami_stats::skip_nan::RssOptions vopt;
    vopt.num_threads = num_threads;
    compare_result(tatami_stats::skip_nan::rss<double, int>(false, *dense_row, vopt), expectedm, refrss, count);
    compare_result(tatami_stats::skip_nan::rss<double, int>(false, *dense_column, vopt), expectedm, refrss, count);
    compare_result(tatami_stats::skip_nan::rss<double, int>(false, *sparse_row, vopt), expectedm, refrss, count);
    compare_result(tatami_stats::skip_nan::rss<double, int>(false, *sparse_column, vopt), expectedm, refrss, count);
}

INSTANTIATE_TEST_SUITE_P(
    SkipNanRss,
    SkipNanRssTest,
    ::testing::Combine(
        ::testing::Values(NONE, RANDOM, BLOCK),
        ::testing::Values(1, 3)
    )
);

/*******************************/

TEST(SkipNanRss, NewType) {
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
    auto rexpected = tatami_stats::skip_nan::rss<double, int>(true, *ref, {});
    auto cexpected = tatami_stats::skip_nan::rss<double, int>(false, *ref, {});

    std::vector<std::int8_t> ivec(dump.begin(), dump.end());
    auto dense_row = std::make_shared<tatami::DenseRowMatrix<std::int8_t, std::uint8_t> >(NR, NC, std::move(ivec));
    auto dense_column = tatami::convert_to_dense<double, int>(*dense_row, false, {});
    auto sparse_row = tatami::convert_to_compressed_sparse<double, int>(*dense_row, true, {});
    auto sparse_column = tatami::convert_to_compressed_sparse<double, int>(*dense_row, false, {});

    compare_result(tatami_stats::skip_nan::rss<double, int>(true, *dense_row, {}), rexpected.mean, rexpected.rss, rexpected.count);
    compare_result(tatami_stats::skip_nan::rss<double, int>(true, *dense_column, {}), rexpected.mean, rexpected.rss, rexpected.count);
    compare_result(tatami_stats::skip_nan::rss<double, int>(true, *sparse_row, {}), rexpected.mean, rexpected.rss, rexpected.count);
    compare_result(tatami_stats::skip_nan::rss<double, int>(true, *sparse_column, {}), rexpected.mean, rexpected.rss, rexpected.count);

    compare_result(tatami_stats::skip_nan::rss<double, int>(false, *dense_row, {}), cexpected.mean, cexpected.rss, cexpected.count);
    compare_result(tatami_stats::skip_nan::rss<double, int>(false, *dense_column, {}), cexpected.mean, cexpected.rss, cexpected.count);
    compare_result(tatami_stats::skip_nan::rss<double, int>(false, *sparse_row, {}), cexpected.mean, cexpected.rss, cexpected.count);
    compare_result(tatami_stats::skip_nan::rss<double, int>(false, *sparse_column, {}), cexpected.mean, cexpected.rss, cexpected.count);
}

/*******************************/

class SkipNanRssEdgeTest : public ::testing::TestWithParam<int> {};

TEST_P(SkipNanRssEdgeTest, NoObservations) {
    auto dense_row = std::shared_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(111, 0, std::vector<double>()));
    auto dense_column = tatami::convert_to_dense<double, int>(*dense_row, false, {});
    auto sparse_row = tatami::convert_to_compressed_sparse<double, int>(*dense_row, true, {});
    auto sparse_column = tatami::convert_to_compressed_sparse<double, int>(*dense_row, false, {});

    tatami_stats::skip_nan::RssOptions vopt;
    vopt.num_threads = GetParam();

    auto check_ok = [&](const tatami_stats::skip_nan::RssResult<double, int>& res) -> void {
        EXPECT_EQ(res.rss.size(), 111);
        EXPECT_TRUE(is_all_nan(res.mean));
        EXPECT_EQ(res.rss, std::vector<double>(111));
        EXPECT_EQ(res.count, std::vector<int>(111));
    };

    check_ok(tatami_stats::skip_nan::rss<double, int>(true, *dense_row, vopt));
    check_ok(tatami_stats::skip_nan::rss<double, int>(true, *dense_column, vopt));
    check_ok(tatami_stats::skip_nan::rss<double, int>(true, *sparse_row, vopt));
    check_ok(tatami_stats::skip_nan::rss<double, int>(true, *sparse_column, vopt));
}

TEST_P(SkipNanRssEdgeTest, ZeroExtent) {
    auto dense_row = std::shared_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(0, 99, std::vector<double>()));
    auto dense_column = tatami::convert_to_dense<double, int>(*dense_row, false, {});
    auto sparse_row = tatami::convert_to_compressed_sparse<double, int>(*dense_row, true, {});
    auto sparse_column = tatami::convert_to_compressed_sparse<double, int>(*dense_row, false, {});

    tatami_stats::skip_nan::RssOptions vopt;
    vopt.num_threads = GetParam();

    auto check_ok = [&](const tatami_stats::skip_nan::RssResult<double, int>& res) -> void {
        EXPECT_EQ(res.mean.size(), 0);
        EXPECT_EQ(res.rss.size(), 0);
        EXPECT_EQ(res.count.size(), 0);
    };

    check_ok(tatami_stats::skip_nan::rss<double, int>(true, *dense_row, vopt));
    check_ok(tatami_stats::skip_nan::rss<double, int>(true, *dense_column, vopt));
    check_ok(tatami_stats::skip_nan::rss<double, int>(true, *sparse_row, vopt));
    check_ok(tatami_stats::skip_nan::rss<double, int>(true, *sparse_column, vopt));
}

TEST_P(SkipNanRssEdgeTest, OneObservation) {
    auto dense_row = std::shared_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(10, 1, std::vector<double>(10)));
    auto dense_column = tatami::convert_to_dense<double, int>(*dense_row, false, {});
    auto sparse_row = tatami::convert_to_compressed_sparse<double, int>(*dense_row, true, {});
    auto sparse_column = tatami::convert_to_compressed_sparse<double, int>(*dense_row, false, {});

    tatami_stats::skip_nan::RssOptions vopt;
    vopt.num_threads = GetParam();

    auto check_ok = [&](const tatami_stats::skip_nan::RssResult<double, int>& res) -> void {
        EXPECT_EQ(res.mean, std::vector<double>(10));
        EXPECT_EQ(res.rss.size(), 10);
        EXPECT_EQ(res.rss, std::vector<double>(10));
        EXPECT_EQ(res.count, std::vector<int>(10, 1));
    };

    check_ok(tatami_stats::skip_nan::rss<double, int>(true, *dense_row, vopt));
    check_ok(tatami_stats::skip_nan::rss<double, int>(true, *dense_column, vopt));
    check_ok(tatami_stats::skip_nan::rss<double, int>(true, *sparse_row, vopt));
    check_ok(tatami_stats::skip_nan::rss<double, int>(true, *sparse_column, vopt));
}

TEST_P(SkipNanRssEdgeTest, NoValidObservations) {
    const int NR = 50, NC = 40;
    std::vector<double> vec(NR * NC, std::numeric_limits<double>::quiet_NaN());

    auto dense_row = std::shared_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, std::move(vec)));
    auto dense_column = tatami::convert_to_dense<double, int>(*dense_row, false, {});
    auto sparse_row = tatami::convert_to_compressed_sparse<double, int>(*dense_row, true, {});
    auto sparse_column = tatami::convert_to_compressed_sparse<double, int>(*dense_row, false, {});

    tatami_stats::skip_nan::RssOptions vopt;
    vopt.num_threads = GetParam();

    auto check_ok = [&](const tatami_stats::skip_nan::RssResult<double, int>& res) -> void {
        for (int r = 0; r < NR; ++r) {
            EXPECT_TRUE(std::isnan(res.mean[r]));
            EXPECT_EQ(res.rss[r], 0);
            EXPECT_EQ(res.count[r], 0);
        }
    };

    check_ok(tatami_stats::skip_nan::rss<double, int>(true, *dense_row, vopt));
    check_ok(tatami_stats::skip_nan::rss<double, int>(true, *dense_column, vopt));
    check_ok(tatami_stats::skip_nan::rss<double, int>(true, *sparse_row, vopt));
    check_ok(tatami_stats::skip_nan::rss<double, int>(true, *sparse_column, vopt));
}

TEST_P(SkipNanRssEdgeTest, FewValidObservations) {
    const int NR = 50, NC = 40;
    std::vector<double> vec(NR * NC, std::numeric_limits<double>::quiet_NaN());
    for (int r = 0; r < NR; ++r) {
        vec[r * NC + r % NC] = r;
        vec[r * NC + (r + 1) % NC] = r + 2;
    }

    auto dense_row = std::shared_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, std::move(vec)));
    auto dense_column = tatami::convert_to_dense<double, int>(*dense_row, false, {});
    auto sparse_row = tatami::convert_to_compressed_sparse<double, int>(*dense_row, true, {});
    auto sparse_column = tatami::convert_to_compressed_sparse<double, int>(*dense_row, false, {});

    tatami_stats::skip_nan::RssOptions vopt;
    vopt.num_threads = GetParam();

    auto check_ok = [&](const tatami_stats::skip_nan::RssResult<double, int>& res) -> void {
        for (int r = 0; r < NR; ++r) {
            EXPECT_FLOAT_EQ(res.mean[r], r + 1);
            EXPECT_FLOAT_EQ(res.rss[r], 2);
            EXPECT_EQ(res.count[r], 2);
        }
    };

    check_ok(tatami_stats::skip_nan::rss<double, int>(true, *dense_row, vopt));
    check_ok(tatami_stats::skip_nan::rss<double, int>(true, *dense_column, vopt));
    check_ok(tatami_stats::skip_nan::rss<double, int>(true, *sparse_row, vopt));
    check_ok(tatami_stats::skip_nan::rss<double, int>(true, *sparse_column, vopt));
}

INSTANTIATE_TEST_SUITE_P(
    SkipNanRss,
    SkipNanRssEdgeTest,
    ::testing::Values(1, 3)
);
