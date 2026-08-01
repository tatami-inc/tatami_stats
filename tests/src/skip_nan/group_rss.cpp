#include <gtest/gtest.h>

#include <vector>

#include "tatami_stats/skip_nan/group_rss.hpp"
#include "tatami_stats/skip_nan/rss.hpp"
#include "tatami_test/tatami_test.hpp"

#include "../utils.h"

static void compare_result(
    const tatami_stats::skip_nan::GroupRssResult<double, int>& res,
    const std::vector<std::vector<double> >& expected_mean,
    const std::vector<std::vector<double> >& expected_rss,
    const std::vector<std::vector<int> >& expected_count 
) {
    compare_double_vectors_of_vectors(expected_rss, res.rss);
    compare_double_vectors_of_vectors(expected_mean, res.mean);
    EXPECT_EQ(res.count, expected_count);
}

class SkipNanGroupRssBasicTest : public ::testing::TestWithParam<std::tuple<int, int, bool, bool> > {
public:
    static std::vector<int> generate_groups(const bool interleaved, const int num_groups, const std::size_t length) {
        std::vector<int> groups(length);

        // We check different group layouts because the parallelization now splits along the observed vectors.
        // This means that different threads might get different groups; we need to check that everything is merged correctly.
        if (interleaved) {
            for (std::size_t i = 0; i < length; ++i) {
                groups[i] = i % num_groups;
            }
        } else {
            const auto per_group = length / num_groups;
            const int remainder = length % num_groups;
            std::size_t counter = 0;
            for (int g = 0; g < num_groups; ++g) {
                const auto group_size = per_group + (g < remainder);
                std::fill_n(groups.begin() + counter, group_size, g);
                counter += group_size;
            }
        }

        return groups;
    }

    static std::vector<std::vector<int> > create_subsets(const int num_groups, const std::vector<int>& groups) {
        std::vector<std::vector<int> > subsets(num_groups);
        const std::size_t length = groups.size();
        for (std::size_t i = 0; i < length; ++i) {
            subsets[groups[i]].push_back(i);
        }
        return subsets;
    }
};

TEST_P(SkipNanGroupRssBasicTest, Row) {
    size_t NR = 99, NC = 155;
    auto params = GetParam();
    const int num_threads = std::get<0>(params);
    const int ngroup = std::get<1>(params);
    const bool interleaved = std::get<2>(params);
    const bool random_nan = std::get<3>(params);

    // Sprinkling in some NaNs.
    auto simulated = tatami_test::simulate_vector<double>(NR * NC, [&]{ 
        tatami_test::SimulateVectorOptions opt;
        opt.density = 0.2;
        opt.lower = -10;
        opt.upper = -2;
        opt.seed = 52827 + num_threads + ngroup + interleaved + 5 * random_nan;
        return opt;
    }());

    auto cgroups = generate_groups(interleaved, ngroup, NC);
    auto subsets = create_subsets(ngroup, cgroups);

    std::mt19937_64 rng(num_threads + 2 * ngroup + 4 * interleaved + 8 * random_nan);
    if (random_nan) {
        std::uniform_real_distribution runif;
        for (size_t r = 0; r < NR; ++r) {
            for (size_t c = 0; c < NC; ++c) {
                if (runif(rng)) {
                    simulated[r * NC + c] = std::numeric_limits<double>::quiet_NaN();
                }
            }
        }
    } else {
        std::uniform_int_distribution runif(0, ngroup - 1);
        for (size_t r = 0; r < NR; ++r) {
            auto chosen = runif(rng);
            for (auto c : subsets[chosen]) {
                simulated[r * NC + c] = std::numeric_limits<double>::quiet_NaN();
            }
        }
    }

    auto dense_row = std::shared_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, std::move(simulated)));
    auto dense_column = tatami::convert_to_dense<double, int>(*dense_row, false, {});
    auto sparse_row = tatami::convert_to_compressed_sparse<double, int>(*dense_row, true, {});
    auto sparse_column = tatami::convert_to_compressed_sparse<double, int>(*dense_row, false, {});

    std::vector<std::vector<double> > expected_m(ngroup), expected_v(ngroup);
    std::vector<std::vector<int> > expected_c(ngroup);
    for (int g = 0; g < ngroup; ++g) {
        auto sub = tatami::make_DelayedSubset<1>(dense_row, subsets[g]);
        auto res = tatami_stats::skip_nan::rss<double, int>(true, *sub, {});
        expected_m[g] = std::move(res.mean);
        expected_v[g] = std::move(res.rss);
        expected_c[g] = std::move(res.count);
    }

    tatami_stats::skip_nan::GroupRssOptions vopt;
    vopt.num_threads = num_threads;

    compare_result(tatami_stats::skip_nan::group_rss<double, int>(true, *dense_row, cgroups.data(), ngroup, vopt), expected_m, expected_v, expected_c);
    compare_result(tatami_stats::skip_nan::group_rss<double, int>(true, *dense_column, cgroups.data(), ngroup, vopt), expected_m, expected_v, expected_c);
    compare_result(tatami_stats::skip_nan::group_rss<double, int>(true, *sparse_row, cgroups.data(), ngroup, vopt), expected_m, expected_v, expected_c);
    compare_result(tatami_stats::skip_nan::group_rss<double, int>(true, *sparse_column, cgroups.data(), ngroup, vopt), expected_m, expected_v, expected_c);

    // Checking same results from matrices that can yield unsorted indices.
    std::shared_ptr<tatami::NumericMatrix> unsorted_row(new tatami_test::ReversedIndicesWrapper<double, int>(sparse_row));
    compare_result(tatami_stats::skip_nan::group_rss<double, int>(true, *unsorted_row, cgroups.data(), ngroup, vopt), expected_m, expected_v, expected_c);
    std::shared_ptr<tatami::NumericMatrix> unsorted_column(new tatami_test::ReversedIndicesWrapper<double, int>(sparse_column));
    compare_result(tatami_stats::skip_nan::group_rss<double, int>(true, *unsorted_column, cgroups.data(), ngroup, vopt), expected_m, expected_v, expected_c);
}

TEST_P(SkipNanGroupRssBasicTest, Column) {
    size_t NR = 99, NC = 155;
    auto params = GetParam();
    const int num_threads = std::get<0>(params);
    const int ngroup = std::get<1>(params);
    const bool interleaved = std::get<2>(params);
    const bool random_nan = std::get<3>(params);

    // Sprinkling in some NaNs.
    auto simulated = tatami_test::simulate_vector<double>(NR * NC, [&]{ 
        tatami_test::SimulateVectorOptions opt;
        opt.density = 0.3;
        opt.lower = 1;
        opt.upper = 2;
        opt.seed = 191188 + num_threads + ngroup + interleaved;
        return opt;
    }());

    auto rgroups = generate_groups(interleaved, ngroup, NR);
    auto subsets = create_subsets(ngroup, rgroups);

    std::mt19937_64 rng(num_threads + 2 * ngroup + 4 * interleaved + 8 * random_nan);
    if (random_nan) {
        std::uniform_real_distribution runif;
        for (size_t c = 0; c < NC; ++c) {
            for (size_t r = 0; r < NR; ++r) {
                if (runif(rng)) {
                    simulated[r * NC + c] = std::numeric_limits<double>::quiet_NaN();
                }
            }
        }
    } else {
        std::uniform_int_distribution runif(0, ngroup - 1);
        for (size_t c = 0; c < NC; ++c) {
            auto chosen = runif(rng);
            for (auto r : subsets[chosen]) {
                simulated[r * NC + c] = std::numeric_limits<double>::quiet_NaN();
            }
        }
    }

    auto dense_row = std::shared_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, std::move(simulated)));
    auto dense_column = tatami::convert_to_dense<double, int>(*dense_row, false, {});
    auto sparse_row = tatami::convert_to_compressed_sparse<double, int>(*dense_row, true, {});
    auto sparse_column = tatami::convert_to_compressed_sparse<double, int>(*dense_row, false, {});

    std::vector<std::vector<double> > expected_m(ngroup), expected_v(ngroup);
    std::vector<std::vector<int> > expected_c(ngroup);
    for (int g = 0; g < ngroup; ++g) {
        auto sub = tatami::make_DelayedSubset<0>(dense_row, subsets[g]);
        auto res = tatami_stats::skip_nan::rss<double, int>(false, *sub, {});
        expected_m[g] = std::move(res.mean);
        expected_v[g] = std::move(res.rss);
        expected_c[g] = std::move(res.count);
    }

    tatami_stats::skip_nan::GroupRssOptions vopt;
    vopt.num_threads = num_threads;

    compare_result(tatami_stats::skip_nan::group_rss<double, int>(false, *dense_row, rgroups.data(), ngroup, vopt), expected_m, expected_v, expected_c);
    compare_result(tatami_stats::skip_nan::group_rss<double, int>(false, *dense_column, rgroups.data(), ngroup, vopt), expected_m, expected_v, expected_c);
    compare_result(tatami_stats::skip_nan::group_rss<double, int>(false, *sparse_row, rgroups.data(), ngroup, vopt), expected_m, expected_v, expected_c);
    compare_result(tatami_stats::skip_nan::group_rss<double, int>(false, *sparse_column, rgroups.data(), ngroup, vopt), expected_m, expected_v, expected_c);

    // Checking same results from matrices that can yield unsorted indices.
    std::shared_ptr<tatami::NumericMatrix> unsorted_row(new tatami_test::ReversedIndicesWrapper<double, int>(sparse_row));
    compare_result(tatami_stats::skip_nan::group_rss<double, int>(false, *unsorted_row, rgroups.data(), ngroup, vopt), expected_m, expected_v, expected_c);
    std::shared_ptr<tatami::NumericMatrix> unsorted_column(new tatami_test::ReversedIndicesWrapper<double, int>(sparse_column));
    compare_result(tatami_stats::skip_nan::group_rss<double, int>(false, *unsorted_column, rgroups.data(), ngroup, vopt), expected_m, expected_v, expected_c);
}

INSTANTIATE_TEST_SUITE_P(
    SkipNanGroupRss,
    SkipNanGroupRssBasicTest,
    ::testing::Combine(
        ::testing::Values(1, 3),
        ::testing::Values(2, 3, 5),
        ::testing::Values(false, true), // interleaved
        ::testing::Values(false, true) // random NaNs or per-group NaNs
    )
);

/*****************************/

class SkipNanGroupRssEdgeTest : public ::testing::TestWithParam<int> {};

TEST_P(SkipNanGroupRssEdgeTest, EmptyExtent) {
    auto dense_row = std::shared_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(0, 10, std::vector<double>()));
    auto dense_column = tatami::convert_to_dense<double, int>(*dense_row, false, {});
    auto sparse_row = tatami::convert_to_compressed_sparse<double, int>(*dense_row, true, {});
    auto sparse_column = tatami::convert_to_compressed_sparse<double, int>(*dense_row, false, {});

    int ngroups = 3;
    std::vector<int> grouping { 0, 1, 2, 0, 1, 2, 0, 1, 2, 0 };

    auto check_ok = [&](const tatami_stats::skip_nan::GroupRssResult<double, int>& res) -> void {
        EXPECT_EQ(res.mean.size(), ngroups);
        EXPECT_EQ(res.rss.size(), ngroups);
        EXPECT_EQ(res.count.size(), ngroups);
        for (int g = 0; g < ngroups; ++g) {
            EXPECT_TRUE(res.mean[g].empty());
            EXPECT_TRUE(res.rss[g].empty());
            EXPECT_TRUE(res.count[g].empty());
        }
    };

    tatami_stats::skip_nan::GroupRssOptions vopt;
    vopt.num_threads = GetParam();

    check_ok(tatami_stats::skip_nan::group_rss<double, int>(true, *dense_row, grouping.data(), ngroups, vopt));
    check_ok(tatami_stats::skip_nan::group_rss<double, int>(true, *dense_column, grouping.data(), ngroups, vopt));
    check_ok(tatami_stats::skip_nan::group_rss<double, int>(true, *sparse_row, grouping.data(), ngroups, vopt));
    check_ok(tatami_stats::skip_nan::group_rss<double, int>(true, *sparse_column, grouping.data(), ngroups, vopt));
}

TEST_P(SkipNanGroupRssEdgeTest, NoGroups) {
    auto dense_row = std::shared_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(0, 10, std::vector<double>()));
    auto dense_column = tatami::convert_to_dense<double, int>(*dense_row, false, {});
    auto sparse_row = tatami::convert_to_compressed_sparse<double, int>(*dense_row, true, {});
    auto sparse_column = tatami::convert_to_compressed_sparse<double, int>(*dense_row, false, {});

    auto check_ok = [&](const tatami_stats::skip_nan::GroupRssResult<double, int>& res) -> void {
        EXPECT_EQ(res.mean.size(), 0);
        EXPECT_EQ(res.rss.size(), 0);
        EXPECT_EQ(res.count.size(), 0);
    };

    tatami_stats::skip_nan::GroupRssOptions vopt;
    vopt.num_threads = GetParam();

    const int* group = NULL; 
    check_ok(tatami_stats::skip_nan::group_rss<double, int>(false, *dense_row, group, 0, vopt));
    check_ok(tatami_stats::skip_nan::group_rss<double, int>(false, *dense_column, group, 0, vopt));
    check_ok(tatami_stats::skip_nan::group_rss<double, int>(false, *sparse_row, group, 0, vopt));
    check_ok(tatami_stats::skip_nan::group_rss<double, int>(false, *sparse_column, group, 0, vopt));
}

TEST_P(SkipNanGroupRssEdgeTest, AllEmptyGroups) {
    auto dense_row = std::shared_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(0, 10, std::vector<double>()));
    auto dense_column = tatami::convert_to_dense<double, int>(*dense_row, false, {});
    auto sparse_row = tatami::convert_to_compressed_sparse<double, int>(*dense_row, true, {});
    auto sparse_column = tatami::convert_to_compressed_sparse<double, int>(*dense_row, false, {});

    int ngroups = 5;
    auto check_ok = [&](const tatami_stats::skip_nan::GroupRssResult<double, int>& res) -> void {
        EXPECT_EQ(res.mean.size(), ngroups);
        EXPECT_EQ(res.rss.size(), ngroups);
        EXPECT_EQ(res.count.size(), ngroups);
        for (int g = 0; g < ngroups; ++g) {
            EXPECT_TRUE(is_all_nan(res.mean[g]));
            EXPECT_EQ(res.rss[g], std::vector<double>(10));
            EXPECT_EQ(res.count[g], std::vector<int>(10));
        }
    };

    tatami_stats::skip_nan::GroupRssOptions vopt;
    vopt.num_threads = GetParam();

    const int* group = NULL; 
    check_ok(tatami_stats::skip_nan::group_rss<double, int>(false, *dense_row, group, ngroups, vopt));
    check_ok(tatami_stats::skip_nan::group_rss<double, int>(false, *dense_column, group, ngroups, vopt));
    check_ok(tatami_stats::skip_nan::group_rss<double, int>(false, *sparse_row, group, ngroups, vopt));
    check_ok(tatami_stats::skip_nan::group_rss<double, int>(false, *sparse_column, group, ngroups, vopt));
}

TEST_P(SkipNanGroupRssEdgeTest, SomeEmptyGroups) {
    tatami_stats::skip_nan::GroupRssOptions vopt;
    vopt.num_threads = GetParam();

    int NR = 200, NC = 50;
    auto simulated = tatami_test::simulate_vector<double>(NR * NC, [&]{
        tatami_test::SimulateVectorOptions opt;
        opt.density = 0.2;
        opt.seed = 112312; 
        return opt;
    }());

    auto dense_row = std::shared_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, std::move(simulated)));
    auto dense_column = tatami::convert_to_dense<double, int>(*dense_row, false, {});
    auto sparse_row = tatami::convert_to_compressed_sparse<double, int>(*dense_row, true, {});
    auto sparse_column = tatami::convert_to_compressed_sparse<double, int>(*dense_row, false, {});

    std::vector<int> grouping(NC, 2);
    int interval = NC / 3;
    std::fill_n(grouping.begin() + interval, interval, 1);
    std::fill(grouping.begin() + 2 * interval, grouping.end(), 0);
    auto ref = tatami_stats::skip_nan::group_rss<double, int>(true, *dense_row, grouping.data(), 3, vopt);

    const int ngroups = 7;
    for (auto& g : grouping) {
        g = 2 * g + 1;
    }

    auto check_ok = [&](const tatami_stats::skip_nan::GroupRssResult<double, int>& res) -> void {
        EXPECT_EQ(res.mean.size(), ngroups);
        EXPECT_EQ(res.rss.size(), ngroups);
        for (int i = 0; i < ngroups; ++i) {
            if (i % 2 == 0) {
                EXPECT_TRUE(is_all_nan(res.mean[i]));
                EXPECT_EQ(res.rss[i], std::vector<double>(NR));
                EXPECT_EQ(res.count[i], std::vector<int>(NR));
            } else {
                compare_double_vectors(ref.mean[(i - 1) / 2], res.mean[i]);
                compare_double_vectors(ref.rss[(i - 1) / 2], res.rss[i]);
                EXPECT_EQ(ref.count[(i - 1) / 2], res.count[i]);
            }
        }
    };

    check_ok(tatami_stats::skip_nan::group_rss<double, int>(true, *dense_row, grouping.data(), ngroups, vopt));
    check_ok(tatami_stats::skip_nan::group_rss<double, int>(true, *dense_column, grouping.data(), ngroups, vopt));
    check_ok(tatami_stats::skip_nan::group_rss<double, int>(true, *sparse_row, grouping.data(), ngroups, vopt));
    check_ok(tatami_stats::skip_nan::group_rss<double, int>(true, *sparse_column, grouping.data(), ngroups, vopt));
}

INSTANTIATE_TEST_SUITE_P(
    SkipNanGroupRss,
    SkipNanGroupRssEdgeTest,
    ::testing::Values(1, 3)
);

/*****************************/

TEST(SkipNanGroupRss, NewType) {
    size_t NR = 198, NC = 52;
    auto dump = tatami_test::simulate_vector<double>(NR * NC, []{
        tatami_test::SimulateVectorOptions opt;
        opt.density = 0.1;
        opt.lower = 1;
        opt.upper = 100;
        opt.seed = 28928289;
        return opt;
    }());
    for (auto& d : dump) { 
        d = std::round(d);
    }
    auto ref = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, dump));

    std::vector<int> cgrouping;
    int cgroup = 5;
    for (size_t c = 0; c < NC; ++c) {
        cgrouping.push_back(c % cgroup);
    }
    std::vector<int> rgrouping;
    int rgroup = 7;
    for (size_t r = 0; r < NR; ++r) {
        rgrouping.push_back(r % rgroup);
    }
    auto rexpected = tatami_stats::skip_nan::group_rss<double, int>(true, *ref, cgrouping.data(), cgroup, {});
    auto cexpected = tatami_stats::skip_nan::group_rss<double, int>(false, *ref, rgrouping.data(), rgroup, {});

    std::vector<std::int8_t> ivec(dump.begin(), dump.end());
    auto dense_row = std::make_shared<tatami::DenseRowMatrix<std::int8_t, std::uint8_t> >(NR, NC, std::move(ivec));
    auto dense_column = tatami::convert_to_dense(dense_row.get(), false);
    auto sparse_row = tatami::convert_to_compressed_sparse(dense_row.get(), true);
    auto sparse_column = tatami::convert_to_compressed_sparse(dense_row.get(), false);

    compare_result(tatami_stats::skip_nan::group_rss<double, int>(true, *dense_row, cgrouping.data(), cgroup, {}), rexpected.mean, rexpected.rss, rexpected.count);
    compare_result(tatami_stats::skip_nan::group_rss<double, int>(true, *dense_column, cgrouping.data(), cgroup, {}), rexpected.mean, rexpected.rss, rexpected.count);
    compare_result(tatami_stats::skip_nan::group_rss<double, int>(true, *sparse_row, cgrouping.data(), cgroup, {}), rexpected.mean, rexpected.rss, rexpected.count);
    compare_result(tatami_stats::skip_nan::group_rss<double, int>(true, *sparse_column, cgrouping.data(), cgroup, {}), rexpected.mean, rexpected.rss, rexpected.count);

    compare_result(tatami_stats::skip_nan::group_rss<double, int>(false, *dense_row, rgrouping.data(), rgroup, {}), cexpected.mean, cexpected.rss, cexpected.count);
    compare_result(tatami_stats::skip_nan::group_rss<double, int>(false, *dense_column, rgrouping.data(), rgroup, {}), cexpected.mean, cexpected.rss, cexpected.count);
    compare_result(tatami_stats::skip_nan::group_rss<double, int>(false, *sparse_row, rgrouping.data(), rgroup, {}), cexpected.mean, cexpected.rss, cexpected.count);
    compare_result(tatami_stats::skip_nan::group_rss<double, int>(false, *sparse_column, rgrouping.data(), rgroup, {}), cexpected.mean, cexpected.rss, cexpected.count);
}
