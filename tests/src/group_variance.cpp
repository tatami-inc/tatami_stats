#include <gtest/gtest.h>

#include <vector>

#include "tatami_stats/group_variance.hpp"
#include "tatami_stats/variance.hpp"
#include "tatami_test/tatami_test.hpp"
#include "utils.h"

static void compare_result(
    const tatami_stats::GroupVarianceResult<double>& res,
    const std::vector<std::vector<double> >& expected_mean,
    const std::vector<std::vector<double> >& expected_variance
) {
    compare_double_vectors_of_vectors(expected_variance, res.variance);
    compare_double_vectors_of_vectors(expected_mean, res.mean);
}

class GroupVarianceBasicTest : public ::testing::TestWithParam<std::tuple<int, int, bool> > {
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

TEST_P(GroupVarianceBasicTest, Row) {
    std::size_t NR = 99, NC = 155;
    auto params = GetParam();
    const int num_threads = std::get<0>(params);
    const int ngroup = std::get<1>(params);
    const bool interleaved = std::get<2>(params);

    auto simulated = tatami_test::simulate_vector<double>(NR * NC, [&]{
        tatami_test::SimulateVectorOptions opt;
        opt.density = 0.2;
        opt.seed = 1298191 + num_threads + ngroup + interleaved;
        return opt;
    }());

    auto dense_row = std::shared_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, std::move(simulated)));
    auto dense_column = tatami::convert_to_dense<double, int>(*dense_row, false, {});
    auto sparse_row = tatami::convert_to_compressed_sparse<double, int>(dense_row.get(), true, {});
    auto sparse_column = tatami::convert_to_compressed_sparse<double, int>(dense_row.get(), false, {});

    auto cgroups = generate_groups(interleaved, ngroup, NC);
    auto subsets = create_subsets(ngroup, cgroups);
    std::vector<std::vector<double> > expected_v(ngroup), expected_m(ngroup);
    for (int g = 0; g < ngroup; ++g) {
        auto sub = tatami::make_DelayedSubset<1>(dense_row, subsets[g]);
        auto res = tatami_stats::variance(true, *sub, {});
        expected_m[g] = std::move(res.mean);
        expected_v[g] = std::move(res.variance);
    }

    tatami_stats::GroupVarianceOptions vopt;
    vopt.num_threads = num_threads;

    compare_result(tatami_stats::group_variance(true, *dense_row, cgroups.data(), ngroup, vopt), expected_m, expected_v);
    compare_result(tatami_stats::group_variance(true, *dense_column, cgroups.data(), ngroup, vopt), expected_m, expected_v);
    compare_result(tatami_stats::group_variance(true, *sparse_row, cgroups.data(), ngroup, vopt), expected_m, expected_v);
    compare_result(tatami_stats::group_variance(true, *sparse_column, cgroups.data(), ngroup, vopt), expected_m, expected_v);

    // Checking same results from matrices that can yield unsorted indices.
    std::shared_ptr<tatami::NumericMatrix> unsorted_row(new tatami_test::ReversedIndicesWrapper<double, int>(sparse_row));
    compare_result(tatami_stats::group_variance(true, *unsorted_row, cgroups.data(), ngroup, vopt), expected_m, expected_v);
    std::shared_ptr<tatami::NumericMatrix> unsorted_column(new tatami_test::ReversedIndicesWrapper<double, int>(sparse_column));
    compare_result(tatami_stats::group_variance(true, *unsorted_column, cgroups.data(), ngroup, vopt), expected_m, expected_v);

    // Checking that we get the same result after skipping NaNs.
    vopt.skip_nan = true;
    compare_result(tatami_stats::group_variance(true, *dense_row, cgroups.data(), ngroup, vopt), expected_m, expected_v);
    compare_result(tatami_stats::group_variance(true, *dense_column, cgroups.data(), ngroup, vopt), expected_m, expected_v);
    compare_result(tatami_stats::group_variance(true, *sparse_row, cgroups.data(), ngroup, vopt), expected_m, expected_v);
    compare_result(tatami_stats::group_variance(true, *sparse_column, cgroups.data(), ngroup, vopt), expected_m, expected_v);
}

TEST_P(GroupVarianceBasicTest, RowSkipNan) {
    size_t NR = 99, NC = 155;
    auto params = GetParam();
    const int num_threads = std::get<0>(params);
    const int ngroup = std::get<1>(params);
    const bool interleaved = std::get<2>(params);

    // Sprinkling in some NaNs.
    auto simulated = tatami_test::simulate_vector<double>(NR * NC, [&]{ 
        tatami_test::SimulateVectorOptions opt;
        opt.density = 0.2;
        opt.lower = -10;
        opt.upper = -2;
        opt.seed = 52827 + num_threads + ngroup + interleaved;
        return opt;
    }());
    for (size_t r = 0; r < NR; ++r) {
        // Invalidating a bunch of observations at the start of each row.
        std::fill_n(simulated.data() + r * NC, r % 20 + 1, std::numeric_limits<double>::quiet_NaN());
    }

    auto dense_row = std::shared_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, std::move(simulated)));
    auto dense_column = tatami::convert_to_dense<double, int>(*dense_row, false, {});
    auto sparse_row = tatami::convert_to_compressed_sparse<double, int>(*dense_row, true, {});
    auto sparse_column = tatami::convert_to_compressed_sparse<double, int>(*dense_row, false, {});

    auto cgroups = generate_groups(interleaved, ngroup, NC);
    auto subsets = create_subsets(ngroup, cgroups);
    std::vector<std::vector<double> > expected_m(ngroup), expected_v(ngroup);
    for (int g = 0; g < ngroup; ++g) {
        auto sub = tatami::make_DelayedSubset<1>(dense_row, subsets[g]);
        tatami_stats::VarianceOptions mopt;
        mopt.skip_nan = true;
        auto res = tatami_stats::variance(true, *sub, mopt);
        expected_m[g] = std::move(res.mean);
        expected_v[g] = std::move(res.variance);
    }

    tatami_stats::GroupVarianceOptions mopt;
    mopt.num_threads = num_threads;
    mopt.skip_nan = true;

    compare_result(tatami_stats::group_variance(true, *dense_column, cgroups.data(), ngroup, mopt), expected_m, expected_v);
    compare_result(tatami_stats::group_variance(true, *dense_column, cgroups.data(), ngroup, mopt), expected_m, expected_v);
    compare_result(tatami_stats::group_variance(true, *sparse_row, cgroups.data(), ngroup, mopt), expected_m, expected_v);
    compare_result(tatami_stats::group_variance(true, *sparse_column, cgroups.data(), ngroup, mopt), expected_m, expected_v);
}

TEST_P(GroupVarianceBasicTest, Column) {
    size_t NR = 56, NC = 179;
    auto params = GetParam();
    const int num_threads = std::get<0>(params);
    const int ngroup = std::get<1>(params);
    const bool interleaved = std::get<2>(params);

    auto simulated = tatami_test::simulate_vector<double>(NR * NC, [&]{
        tatami_test::SimulateVectorOptions opt;
        opt.density = 0.25;
        opt.seed = 83828 + num_threads + ngroup + interleaved;
        return opt;
    }());

    auto dense_row = std::shared_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, std::move(simulated)));
    auto dense_column = tatami::convert_to_dense<double, int>(*dense_row, false, {});
    auto sparse_row = tatami::convert_to_compressed_sparse<double, int>(*dense_row, true, {});
    auto sparse_column = tatami::convert_to_compressed_sparse<double, int>(*dense_row, false, {});

    auto rgroups = generate_groups(interleaved, ngroup, NR);
    auto subsets = create_subsets(ngroup, rgroups);
    std::vector<std::vector<double> > expected_m(ngroup), expected_v(ngroup);
    for (int g = 0; g < ngroup; ++g) {
        auto sub = tatami::make_DelayedSubset<0>(dense_row, subsets[g]);
        auto res = tatami_stats::variance(false, *sub, {});
        expected_m[g] = std::move(res.mean);
        expected_v[g] = std::move(res.variance);
    }

    tatami_stats::GroupVarianceOptions vopt;
    vopt.num_threads = num_threads;

    compare_result(tatami_stats::group_variance(false, *dense_row, rgroups.data(), ngroup, vopt), expected_m, expected_v);
    compare_result(tatami_stats::group_variance(false, *dense_column, rgroups.data(), ngroup, vopt), expected_m, expected_v);
    compare_result(tatami_stats::group_variance(false, *sparse_row, rgroups.data(), ngroup, vopt), expected_m, expected_v);
    compare_result(tatami_stats::group_variance(false, *sparse_column, rgroups.data(), ngroup, vopt), expected_m, expected_v);

    // Checking same results from matrices that can yield unsorted indices.
    std::shared_ptr<tatami::NumericMatrix> unsorted_row(new tatami_test::ReversedIndicesWrapper<double, int>(sparse_row));
    compare_result(tatami_stats::group_variance(false, *unsorted_row, rgroups.data(), ngroup, vopt), expected_m, expected_v);
    std::shared_ptr<tatami::NumericMatrix> unsorted_column(new tatami_test::ReversedIndicesWrapper<double, int>(sparse_column));
    compare_result(tatami_stats::group_variance(false, *unsorted_column, rgroups.data(), ngroup, vopt), expected_m, expected_v);

    // Checking that we get the same result after skipping NaNs.
    vopt.skip_nan = true;
    compare_result(tatami_stats::group_variance(false, *dense_row, rgroups.data(), ngroup, vopt), expected_m, expected_v);
    compare_result(tatami_stats::group_variance(false, *dense_column, rgroups.data(), ngroup, vopt), expected_m, expected_v);
    compare_result(tatami_stats::group_variance(false, *sparse_row, rgroups.data(), ngroup, vopt), expected_m, expected_v);
    compare_result(tatami_stats::group_variance(false, *sparse_column, rgroups.data(), ngroup, vopt), expected_m, expected_v);
}

TEST_P(GroupVarianceBasicTest, ColumnSkipNan) {
    size_t NR = 99, NC = 155;
    auto params = GetParam();
    const int num_threads = std::get<0>(params);
    const int ngroup = std::get<1>(params);
    const bool interleaved = std::get<2>(params);

    // Sprinkling in some NaNs.
    auto simulated = tatami_test::simulate_vector<double>(NR * NC, [&]{ 
        tatami_test::SimulateVectorOptions opt;
        opt.density = 0.3;
        opt.lower = 1;
        opt.upper = 2;
        opt.seed = 191188 + num_threads + ngroup + interleaved;
        return opt;
    }());
    for (size_t c = 0; c < NC; ++c) {
        const std::size_t rstart = NR - c % 20 - 1;
        for (size_t r = rstart; r < NR; ++r) {
            // Invalidating a bunch of observations at the end of each column.
            simulated[c + r * NC] = std::numeric_limits<double>::quiet_NaN();
        }
    }

    auto dense_row = std::shared_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, std::move(simulated)));
    auto dense_column = tatami::convert_to_dense<double, int>(*dense_row, false, {});
    auto sparse_row = tatami::convert_to_compressed_sparse<double, int>(*dense_row, true, {});
    auto sparse_column = tatami::convert_to_compressed_sparse<double, int>(*dense_row, false, {});

    auto rgroups = generate_groups(interleaved, ngroup, NR);
    auto subsets = create_subsets(ngroup, rgroups);
    std::vector<std::vector<double> > expected_m(ngroup), expected_v(ngroup);
    for (int g = 0; g < ngroup; ++g) {
        auto sub = tatami::make_DelayedSubset<0>(dense_row, subsets[g]);
        tatami_stats::VarianceOptions mopt;
        mopt.skip_nan = true;
        auto res = tatami_stats::variance(false, *sub, mopt);
        expected_m[g] = std::move(res.mean);
        expected_v[g] = std::move(res.variance);
    }

    tatami_stats::GroupVarianceOptions vopt;
    vopt.skip_nan = true;
    vopt.num_threads = num_threads;

    compare_result(tatami_stats::group_variance(false, *dense_row, rgroups.data(), ngroup, vopt), expected_m, expected_v);
    compare_result(tatami_stats::group_variance(false, *dense_column, rgroups.data(), ngroup, vopt), expected_m, expected_v);
    compare_result(tatami_stats::group_variance(false, *sparse_row, rgroups.data(), ngroup, vopt), expected_m, expected_v);
    compare_result(tatami_stats::group_variance(false, *sparse_column, rgroups.data(), ngroup, vopt), expected_m, expected_v);
}

INSTANTIATE_TEST_SUITE_P(
    GroupVariance,
    GroupVarianceBasicTest,
    ::testing::Combine(
        ::testing::Values(1, 3),
        ::testing::Values(2, 3, 5),
        ::testing::Values(false, true)
    )
);

/*****************************/

class GroupVarianceEdgeTest : public ::testing::TestWithParam<int> {};

TEST_P(GroupVarianceEdgeTest, EmptyExtent) {
    auto dense_row = std::shared_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(0, 10, std::vector<double>()));
    auto dense_column = tatami::convert_to_dense<double, int>(*dense_row, false, {});
    auto sparse_row = tatami::convert_to_compressed_sparse<double, int>(*dense_row, true, {});
    auto sparse_column = tatami::convert_to_compressed_sparse<double, int>(*dense_row, false, {});

    int ngroups = 3;
    std::vector<int> grouping { 0, 1, 2, 0, 1, 2, 0, 1, 2, 0 };

    auto check_ok = [&](const tatami_stats::GroupVarianceResult<double>& res) -> void {
        EXPECT_EQ(res.mean.size(), ngroups);
        EXPECT_EQ(res.variance.size(), ngroups);
        for (int g = 0; g < ngroups; ++g) {
            EXPECT_TRUE(res.mean[g].empty());
            EXPECT_TRUE(res.variance[g].empty());
        }
    };

    tatami_stats::GroupVarianceOptions vopt;
    vopt.num_threads = GetParam();

    check_ok(tatami_stats::group_variance(true, *dense_row, grouping.data(), ngroups, vopt));
    check_ok(tatami_stats::group_variance(true, *dense_column, grouping.data(), ngroups, vopt));
    check_ok(tatami_stats::group_variance(true, *sparse_row, grouping.data(), ngroups, vopt));
    check_ok(tatami_stats::group_variance(true, *sparse_column, grouping.data(), ngroups, vopt));

    vopt.skip_nan = true;
    check_ok(tatami_stats::group_variance(true, *dense_row, grouping.data(), ngroups, vopt));
    check_ok(tatami_stats::group_variance(true, *dense_column, grouping.data(), ngroups, vopt));
    check_ok(tatami_stats::group_variance(true, *sparse_row, grouping.data(), ngroups, vopt));
    check_ok(tatami_stats::group_variance(true, *sparse_column, grouping.data(), ngroups, vopt));
}

TEST_P(GroupVarianceEdgeTest, NoGroups) {
    auto dense_row = std::shared_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(0, 10, std::vector<double>()));
    auto dense_column = tatami::convert_to_dense<double, int>(*dense_row, false, {});
    auto sparse_row = tatami::convert_to_compressed_sparse<double, int>(*dense_row, true, {});
    auto sparse_column = tatami::convert_to_compressed_sparse<double, int>(*dense_row, false, {});

    auto check_ok = [&](const tatami_stats::GroupVarianceResult<double>& res) -> void {
        EXPECT_EQ(res.mean.size(), 0);
        EXPECT_EQ(res.variance.size(), 0);
    };

    tatami_stats::GroupVarianceOptions vopt;
    vopt.num_threads = GetParam();

    const int* group = NULL; 
    check_ok(tatami_stats::group_variance(false, *dense_row, group, 0, vopt));
    check_ok(tatami_stats::group_variance(false, *dense_column, group, 0, vopt));
    check_ok(tatami_stats::group_variance(false, *sparse_row, group, 0, vopt));
    check_ok(tatami_stats::group_variance(false, *sparse_column, group, 0, vopt));

    check_ok(tatami_stats::group_variance(false, *dense_row, group, 0, vopt));
    check_ok(tatami_stats::group_variance(false, *dense_column, group, 0, vopt));
    check_ok(tatami_stats::group_variance(false, *sparse_row, group, 0, vopt));
    check_ok(tatami_stats::group_variance(false, *sparse_column, group, 0, vopt));
}

TEST_P(GroupVarianceEdgeTest, AllEmptyGroups) {
    auto dense_row = std::shared_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(0, 10, std::vector<double>()));
    auto dense_column = tatami::convert_to_dense<double, int>(*dense_row, false, {});
    auto sparse_row = tatami::convert_to_compressed_sparse<double, int>(*dense_row, true, {});
    auto sparse_column = tatami::convert_to_compressed_sparse<double, int>(*dense_row, false, {});

    int ngroups = 5;
    auto check_ok = [&](const tatami_stats::GroupVarianceResult<double>& res) -> void {
        EXPECT_EQ(res.mean.size(), ngroups);
        EXPECT_EQ(res.variance.size(), ngroups);
        for (int g = 0; g < ngroups; ++g) {
            EXPECT_TRUE(is_all_nan(res.mean[g]));
            EXPECT_TRUE(is_all_nan(res.variance[g]));
        }
    };

    tatami_stats::GroupVarianceOptions vopt;
    vopt.num_threads = GetParam();

    const int* group = NULL; 
    check_ok(tatami_stats::group_variance(false, *dense_row, group, ngroups, vopt));
    check_ok(tatami_stats::group_variance(false, *dense_column, group, ngroups, vopt));
    check_ok(tatami_stats::group_variance(false, *sparse_row, group, ngroups, vopt));
    check_ok(tatami_stats::group_variance(false, *sparse_column, group, ngroups, vopt));

    vopt.skip_nan = true;
    check_ok(tatami_stats::group_variance(false, *dense_row, group, ngroups, vopt));
    check_ok(tatami_stats::group_variance(false, *dense_column, group, ngroups, vopt));
    check_ok(tatami_stats::group_variance(false, *sparse_row, group, ngroups, vopt));
    check_ok(tatami_stats::group_variance(false, *sparse_column, group, ngroups, vopt));
}

TEST_P(GroupVarianceEdgeTest, SomeEmptyGroups) {
    int nrow = 20;
    auto dense_row = std::shared_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(nrow, 10, std::vector<double>(nrow * 10)));
    auto dense_column = tatami::convert_to_dense<double, int>(*dense_row, false, {});
    auto sparse_row = tatami::convert_to_compressed_sparse<double, int>(*dense_row, true, {});
    auto sparse_column = tatami::convert_to_compressed_sparse<double, int>(*dense_row, false, {});

    int ngroups = 4;
    std::vector<int> grouping(10, ngroups - 1);

    auto check_ok = [&](const tatami_stats::GroupVarianceResult<double>& res) -> void {
        EXPECT_EQ(res.mean.size(), ngroups);
        EXPECT_EQ(res.variance.size(), ngroups);
        for (int i = 0; i < ngroups - 1; ++i) {
            EXPECT_TRUE(is_all_nan(res.mean[i]));
            EXPECT_TRUE(is_all_nan(res.variance[i]));
        }
        for (int j = 0; j < nrow; ++j) {
            EXPECT_EQ(res.mean[ngroups - 1][j], 0);
            EXPECT_EQ(res.variance[ngroups - 1][j], 0);
        }
    };

    tatami_stats::GroupVarianceOptions vopt;
    vopt.num_threads = GetParam();

    check_ok(tatami_stats::group_variance(true, *dense_row, grouping.data(), ngroups, vopt));
    check_ok(tatami_stats::group_variance(true, *dense_column, grouping.data(), ngroups, vopt));
    check_ok(tatami_stats::group_variance(true, *sparse_row, grouping.data(), ngroups, vopt));
    check_ok(tatami_stats::group_variance(true, *sparse_column, grouping.data(), ngroups, vopt));

    vopt.skip_nan = true;
    check_ok(tatami_stats::group_variance(true, *dense_row, grouping.data(), ngroups, vopt));
    check_ok(tatami_stats::group_variance(true, *dense_column, grouping.data(), ngroups, vopt));
    check_ok(tatami_stats::group_variance(true, *sparse_row, grouping.data(), ngroups, vopt));
    check_ok(tatami_stats::group_variance(true, *sparse_column, grouping.data(), ngroups, vopt));
}

INSTANTIATE_TEST_SUITE_P(
    GroupVariance,
    GroupVarianceEdgeTest,
    ::testing::Values(1, 3)
);

/*****************************/

TEST(GroupVariance, NewType) {
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
    auto rexpected = tatami_stats::group_variance(true, *ref, cgrouping.data(), cgroup, {});
    auto cexpected = tatami_stats::group_variance(false, *ref, rgrouping.data(), rgroup, {});

    std::vector<std::int8_t> ivec(dump.begin(), dump.end());
    auto dense_row = std::make_shared<tatami::DenseRowMatrix<std::int8_t, std::uint8_t> >(NR, NC, std::move(ivec));
    auto dense_column = tatami::convert_to_dense(dense_row.get(), false);
    auto sparse_row = tatami::convert_to_compressed_sparse(dense_row.get(), true);
    auto sparse_column = tatami::convert_to_compressed_sparse(dense_row.get(), false);

    compare_result(tatami_stats::group_variance(true, *dense_row, cgrouping.data(), cgroup, {}), rexpected.mean, rexpected.variance);
    compare_result(tatami_stats::group_variance(true, *dense_column, cgrouping.data(), cgroup, {}), rexpected.mean, rexpected.variance);
    compare_result(tatami_stats::group_variance(true, *sparse_row, cgrouping.data(), cgroup, {}), rexpected.mean, rexpected.variance);
    compare_result(tatami_stats::group_variance(true, *sparse_column, cgrouping.data(), cgroup, {}), rexpected.mean, rexpected.variance);

    compare_result(tatami_stats::group_variance(false, *dense_row, rgrouping.data(), rgroup, {}), cexpected.mean, cexpected.variance);
    compare_result(tatami_stats::group_variance(false, *dense_column, rgrouping.data(), rgroup, {}), cexpected.mean, cexpected.variance);
    compare_result(tatami_stats::group_variance(false, *sparse_row, rgrouping.data(), rgroup, {}), cexpected.mean, cexpected.variance);
    compare_result(tatami_stats::group_variance(false, *sparse_column, rgrouping.data(), rgroup, {}), cexpected.mean, cexpected.variance);

    // Still behaves with NaN skipping.
    tatami_stats::GroupVarianceOptions vopt;
    vopt.skip_nan = true;

    compare_result(tatami_stats::group_variance(true, *dense_row, cgrouping.data(), cgroup, vopt), rexpected.mean, rexpected.variance);
    compare_result(tatami_stats::group_variance(true, *dense_column, cgrouping.data(), cgroup, vopt), rexpected.mean, rexpected.variance);
    compare_result(tatami_stats::group_variance(true, *sparse_row, cgrouping.data(), cgroup, vopt), rexpected.mean, rexpected.variance);
    compare_result(tatami_stats::group_variance(true, *sparse_column, cgrouping.data(), cgroup, vopt), rexpected.mean, rexpected.variance);

    compare_result(tatami_stats::group_variance(false, *dense_row, rgrouping.data(), rgroup, vopt), cexpected.mean, cexpected.variance);
    compare_result(tatami_stats::group_variance(false, *dense_column, rgrouping.data(), rgroup, vopt), cexpected.mean, cexpected.variance);
    compare_result(tatami_stats::group_variance(false, *sparse_row, rgrouping.data(), rgroup, vopt), cexpected.mean, cexpected.variance);
    compare_result(tatami_stats::group_variance(false, *sparse_column, rgrouping.data(), rgroup, vopt), cexpected.mean, cexpected.variance);
}
