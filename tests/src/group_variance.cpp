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

static std::vector<std::vector<int> > create_subsets(const int ngroups, const std::vector<int>& groups) {
    std::vector<std::vector<int> > subsets(ngroups);
    const std::size_t length = groups.size();
    for (std::size_t i = 0; i < length; ++i) {
        subsets[groups[i]].push_back(i);
    }
    return subsets;
}

// COMMENT: most of the heavy lifting is done by group_rss() and skip_nan::group_rss().
// So, to avoid redundant work, we don't test as much here - we just check that the denominator is applied correctly.

TEST(GroupVariance, Row) {
    std::size_t NR = 99, NC = 155;

    auto simulated = tatami_test::simulate_vector<double>(NR * NC, [&]{
        tatami_test::SimulateVectorOptions opt;
        opt.density = 0.2;
        opt.seed = 1298191; 
        return opt;
    }());

    int ngroups = 4;
    std::vector<int> cgroups(NC);
    for (std::size_t i = 0; i < NC; ++i) {
        cgroups[i] = i % ngroups;
    }

    auto dense_row = std::shared_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, std::move(simulated)));
    auto dense_column = tatami::convert_to_dense<double, int>(*dense_row, false, {});
    auto sparse_row = tatami::convert_to_compressed_sparse<double, int>(dense_row.get(), true, {});
    auto sparse_column = tatami::convert_to_compressed_sparse<double, int>(dense_row.get(), false, {});

    auto subsets = create_subsets(ngroups, cgroups);
    std::vector<std::vector<double> > expected_v(ngroups), expected_m(ngroups);
    for (int g = 0; g < ngroups; ++g) {
        auto sub = tatami::make_DelayedSubset<1>(dense_row, subsets[g]);
        auto res = tatami_stats::variance(true, *sub, {});
        expected_m[g] = std::move(res.mean);
        expected_v[g] = std::move(res.variance);
    }

    compare_result(tatami_stats::group_variance(true, *dense_row, cgroups.data(), ngroups, {}), expected_m, expected_v);
    compare_result(tatami_stats::group_variance(true, *dense_column, cgroups.data(), ngroups, {}), expected_m, expected_v);
    compare_result(tatami_stats::group_variance(true, *sparse_row, cgroups.data(), ngroups, {}), expected_m, expected_v);
    compare_result(tatami_stats::group_variance(true, *sparse_column, cgroups.data(), ngroups, {}), expected_m, expected_v);
}

TEST(GroupVariance, RowSkipNan) {
    size_t NR = 99, NC = 155;

    // Sprinkling in some NaNs.
    auto simulated = tatami_test::simulate_vector<double>(NR * NC, [&]{ 
        tatami_test::SimulateVectorOptions opt;
        opt.density = 0.2;
        opt.lower = -10;
        opt.upper = -2;
        opt.seed = 52827; 
        return opt;
    }());
    for (size_t r = 0; r < NR; ++r) {
        // Invalidating a bunch of observations at the start of each row.
        std::fill_n(simulated.data() + r * NC, r % 20 + 1, std::numeric_limits<double>::quiet_NaN());
    }

    int ngroups = 3;
    std::vector<int> cgroups(NC);
    for (std::size_t i = 0; i < NC; ++i) {
        cgroups[i] = i % ngroups;
    }

    auto dense_row = std::shared_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, std::move(simulated)));
    auto dense_column = tatami::convert_to_dense<double, int>(*dense_row, false, {});
    auto sparse_row = tatami::convert_to_compressed_sparse<double, int>(*dense_row, true, {});
    auto sparse_column = tatami::convert_to_compressed_sparse<double, int>(*dense_row, false, {});

    auto subsets = create_subsets(ngroups, cgroups);
    std::vector<std::vector<double> > expected_m(ngroups), expected_v(ngroups);
    for (int g = 0; g < ngroups; ++g) {
        auto sub = tatami::make_DelayedSubset<1>(dense_row, subsets[g]);
        tatami_stats::VarianceOptions mopt;
        mopt.skip_nan = true;
        auto res = tatami_stats::variance(true, *sub, mopt);
        expected_m[g] = std::move(res.mean);
        expected_v[g] = std::move(res.variance);
    }

    tatami_stats::GroupVarianceOptions mopt;
    mopt.skip_nan = true;

    compare_result(tatami_stats::group_variance(true, *dense_column, cgroups.data(), ngroups, mopt), expected_m, expected_v);
    compare_result(tatami_stats::group_variance(true, *dense_column, cgroups.data(), ngroups, mopt), expected_m, expected_v);
    compare_result(tatami_stats::group_variance(true, *sparse_row, cgroups.data(), ngroups, mopt), expected_m, expected_v);
    compare_result(tatami_stats::group_variance(true, *sparse_column, cgroups.data(), ngroups, mopt), expected_m, expected_v);
}

TEST(GroupVariance, Column) {
    size_t NR = 56, NC = 179;

    auto simulated = tatami_test::simulate_vector<double>(NR * NC, [&]{
        tatami_test::SimulateVectorOptions opt;
        opt.density = 0.25;
        opt.seed = 83828; 
        return opt;
    }());

    int ngroups = 5;
    std::vector<int> rgroups(NR);
    for (std::size_t i = 0; i < NR; ++i) {
        rgroups[i] = i % ngroups;
    }

    auto dense_row = std::shared_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, std::move(simulated)));
    auto dense_column = tatami::convert_to_dense<double, int>(*dense_row, false, {});
    auto sparse_row = tatami::convert_to_compressed_sparse<double, int>(*dense_row, true, {});
    auto sparse_column = tatami::convert_to_compressed_sparse<double, int>(*dense_row, false, {});

    auto subsets = create_subsets(ngroups, rgroups);
    std::vector<std::vector<double> > expected_m(ngroups), expected_v(ngroups);
    for (int g = 0; g < ngroups; ++g) {
        auto sub = tatami::make_DelayedSubset<0>(dense_row, subsets[g]);
        auto res = tatami_stats::variance(false, *sub, {});
        expected_m[g] = std::move(res.mean);
        expected_v[g] = std::move(res.variance);
    }

    compare_result(tatami_stats::group_variance(false, *dense_row, rgroups.data(), ngroups, {}), expected_m, expected_v);
    compare_result(tatami_stats::group_variance(false, *dense_column, rgroups.data(), ngroups, {}), expected_m, expected_v);
    compare_result(tatami_stats::group_variance(false, *sparse_row, rgroups.data(), ngroups, {}), expected_m, expected_v);
    compare_result(tatami_stats::group_variance(false, *sparse_column, rgroups.data(), ngroups, {}), expected_m, expected_v);
}

TEST(GroupVariance, ColumnSkipNan) {
    size_t NR = 99, NC = 155;

    // Sprinkling in some NaNs.
    auto simulated = tatami_test::simulate_vector<double>(NR * NC, [&]{ 
        tatami_test::SimulateVectorOptions opt;
        opt.density = 0.3;
        opt.lower = 1;
        opt.upper = 2;
        opt.seed = 191188;
        return opt;
    }());
    for (size_t c = 0; c < NC; ++c) {
        const std::size_t rstart = NR - c % 20 - 1;
        for (size_t r = rstart; r < NR; ++r) {
            // Invalidating a bunch of observations at the end of each column.
            simulated[c + r * NC] = std::numeric_limits<double>::quiet_NaN();
        }
    }

    int ngroups = 4;
    std::vector<int> rgroups(NR);
    for (std::size_t i = 0; i < NR; ++i) {
        rgroups[i] = i % ngroups;
    }

    auto dense_row = std::shared_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, std::move(simulated)));
    auto dense_column = tatami::convert_to_dense<double, int>(*dense_row, false, {});
    auto sparse_row = tatami::convert_to_compressed_sparse<double, int>(*dense_row, true, {});
    auto sparse_column = tatami::convert_to_compressed_sparse<double, int>(*dense_row, false, {});

    auto subsets = create_subsets(ngroups, rgroups);
    std::vector<std::vector<double> > expected_m(ngroups), expected_v(ngroups);
    for (int g = 0; g < ngroups; ++g) {
        auto sub = tatami::make_DelayedSubset<0>(dense_row, subsets[g]);
        tatami_stats::VarianceOptions mopt;
        mopt.skip_nan = true;
        auto res = tatami_stats::variance(false, *sub, mopt);
        expected_m[g] = std::move(res.mean);
        expected_v[g] = std::move(res.variance);
    }

    tatami_stats::GroupVarianceOptions vopt;
    vopt.skip_nan = true;

    compare_result(tatami_stats::group_variance(false, *dense_row, rgroups.data(), ngroups, vopt), expected_m, expected_v);
    compare_result(tatami_stats::group_variance(false, *dense_column, rgroups.data(), ngroups, vopt), expected_m, expected_v);
    compare_result(tatami_stats::group_variance(false, *sparse_row, rgroups.data(), ngroups, vopt), expected_m, expected_v);
    compare_result(tatami_stats::group_variance(false, *sparse_column, rgroups.data(), ngroups, vopt), expected_m, expected_v);
}

/*****************************/

TEST(GroupVariance, EmptyExtent) {
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

    check_ok(tatami_stats::group_variance(true, *dense_row, grouping.data(), ngroups, {}));
    check_ok(tatami_stats::group_variance(true, *dense_column, grouping.data(), ngroups, {}));
    check_ok(tatami_stats::group_variance(true, *sparse_row, grouping.data(), ngroups, {}));
    check_ok(tatami_stats::group_variance(true, *sparse_column, grouping.data(), ngroups, {}));

    tatami_stats::GroupVarianceOptions vopt;
    vopt.skip_nan = true;
    check_ok(tatami_stats::group_variance(true, *dense_row, grouping.data(), ngroups, vopt));
    check_ok(tatami_stats::group_variance(true, *dense_column, grouping.data(), ngroups, vopt));
    check_ok(tatami_stats::group_variance(true, *sparse_row, grouping.data(), ngroups, vopt));
    check_ok(tatami_stats::group_variance(true, *sparse_column, grouping.data(), ngroups, vopt));
}

TEST(GroupVariance, NoGroups) {
    auto dense_row = std::shared_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(0, 10, std::vector<double>()));
    auto dense_column = tatami::convert_to_dense<double, int>(*dense_row, false, {});
    auto sparse_row = tatami::convert_to_compressed_sparse<double, int>(*dense_row, true, {});
    auto sparse_column = tatami::convert_to_compressed_sparse<double, int>(*dense_row, false, {});

    auto check_ok = [&](const tatami_stats::GroupVarianceResult<double>& res) -> void {
        EXPECT_EQ(res.mean.size(), 0);
        EXPECT_EQ(res.variance.size(), 0);
    };

    const int* group = NULL; 
    check_ok(tatami_stats::group_variance(false, *dense_row, group, 0, {}));
    check_ok(tatami_stats::group_variance(false, *dense_column, group, 0, {}));
    check_ok(tatami_stats::group_variance(false, *sparse_row, group, 0, {}));
    check_ok(tatami_stats::group_variance(false, *sparse_column, group, 0, {}));

    tatami_stats::GroupVarianceOptions vopt;
    vopt.skip_nan = true;
    check_ok(tatami_stats::group_variance(false, *dense_row, group, 0, vopt));
    check_ok(tatami_stats::group_variance(false, *dense_column, group, 0, vopt));
    check_ok(tatami_stats::group_variance(false, *sparse_row, group, 0, vopt));
    check_ok(tatami_stats::group_variance(false, *sparse_column, group, 0, vopt));
}

TEST(GroupVariance, AllEmptyGroups) {
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

    const int* group = NULL; 
    check_ok(tatami_stats::group_variance(false, *dense_row, group, ngroups, {}));
    check_ok(tatami_stats::group_variance(false, *dense_column, group, ngroups, {}));
    check_ok(tatami_stats::group_variance(false, *sparse_row, group, ngroups, {}));
    check_ok(tatami_stats::group_variance(false, *sparse_column, group, ngroups, {}));

    tatami_stats::GroupVarianceOptions vopt;
    vopt.skip_nan = true;
    check_ok(tatami_stats::group_variance(false, *dense_row, group, ngroups, vopt));
    check_ok(tatami_stats::group_variance(false, *dense_column, group, ngroups, vopt));
    check_ok(tatami_stats::group_variance(false, *sparse_row, group, ngroups, vopt));
    check_ok(tatami_stats::group_variance(false, *sparse_column, group, ngroups, vopt));
}

TEST(GroupVariance, SomeEmptyGroups) {
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

    check_ok(tatami_stats::group_variance(true, *dense_row, grouping.data(), ngroups, {}));
    check_ok(tatami_stats::group_variance(true, *dense_column, grouping.data(), ngroups, {}));
    check_ok(tatami_stats::group_variance(true, *sparse_row, grouping.data(), ngroups, {}));
    check_ok(tatami_stats::group_variance(true, *sparse_column, grouping.data(), ngroups, {}));

    tatami_stats::GroupVarianceOptions vopt;
    vopt.skip_nan = true;
    check_ok(tatami_stats::group_variance(true, *dense_row, grouping.data(), ngroups, vopt));
    check_ok(tatami_stats::group_variance(true, *dense_column, grouping.data(), ngroups, vopt));
    check_ok(tatami_stats::group_variance(true, *sparse_row, grouping.data(), ngroups, vopt));
    check_ok(tatami_stats::group_variance(true, *sparse_column, grouping.data(), ngroups, vopt));
}
