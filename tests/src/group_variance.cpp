#include <gtest/gtest.h>

#include <vector>

#include "tatami_stats/group_variance.hpp"
#include "tatami_stats/variance.hpp"
#include "tatami_test/tatami_test.hpp"
#include "utils.h"

template<class L_, class R_>
static void compare_double_vectors_of_vectors(const L_& left, const R_& right) {
    ASSERT_EQ(left.size(), right.size());
    for (size_t i = 0; i < left.size(); ++i) {
        compare_double_vectors(left[i], right[i]);
    }
}

static void compare_result(
    const tatami_stats::GroupVarianceResult<double>& res,
    const std::vector<std::vector<double> >& expected_mean,
    const std::vector<std::vector<double> >& expected_variance
) {
    compare_double_vectors_of_vectors(expected_variance, res.variance);
    compare_double_vectors_of_vectors(expected_mean, res.mean);
}

TEST(GroupVariance, ByRow) {
    size_t NR = 99, NC = 155;

    auto simulated = tatami_test::simulate_vector<double>(NR * NC, []{
        tatami_test::SimulateVectorOptions opt;
        opt.density = 0.2;
        opt.seed = 1298191;
        return opt;
    }());

    auto dense_row = std::shared_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, std::move(simulated)));
    auto dense_column = tatami::convert_to_dense<double, int>(*dense_row, false, {});
    auto sparse_row = tatami::convert_to_compressed_sparse<double, int>(dense_row.get(), true, {});
    auto sparse_column = tatami::convert_to_compressed_sparse<double, int>(dense_row.get(), false, {});

    std::vector<int> cgroups(NC);
    int ngroup = 3; 
    std::vector<std::vector<int> > subsets(ngroup);
    for (size_t c = 0; c < NC; ++c) {
        cgroups[c] = c % ngroup;
        subsets[cgroups[c]].push_back(c);
    }

    std::vector<std::vector<double> > expected_v(ngroup), expected_m(ngroup);
    for (int g = 0; g < ngroup; ++g) {
        auto sub = tatami::make_DelayedSubset<1>(dense_row, subsets[g]);
        auto res = tatami_stats::variance(true, *sub, {});
        expected_m[g] = std::move(res.mean);
        expected_v[g] = std::move(res.variance);
    }

    compare_result(tatami_stats::group_variance(true, *dense_row, cgroups.data(), ngroup, {}), expected_m, expected_v);
    compare_result(tatami_stats::group_variance(true, *dense_column, cgroups.data(), ngroup, {}), expected_m, expected_v);
    compare_result(tatami_stats::group_variance(true, *sparse_row, cgroups.data(), ngroup, {}), expected_m, expected_v);
    compare_result(tatami_stats::group_variance(true, *sparse_column, cgroups.data(), ngroup, {}), expected_m, expected_v);

    // Checking that the parallel code is the same.
    tatami_stats::GroupVarianceOptions vopt;
    vopt.num_threads = 3;
    compare_result(tatami_stats::group_variance(true, *dense_row, cgroups.data(), ngroup, vopt), expected_m, expected_v);
    compare_result(tatami_stats::group_variance(true, *dense_column, cgroups.data(), ngroup, vopt), expected_m, expected_v);
    compare_result(tatami_stats::group_variance(true, *sparse_row, cgroups.data(), ngroup, vopt), expected_m, expected_v);
    compare_result(tatami_stats::group_variance(true, *sparse_column, cgroups.data(), ngroup, vopt), expected_m, expected_v);

    // Checking that we get the same result after skipping NaNs.
    vopt.num_threads = 1;
    vopt.skip_nan = true;
    compare_result(tatami_stats::group_variance(true, *dense_row, cgroups.data(), ngroup, vopt), expected_m, expected_v);
    compare_result(tatami_stats::group_variance(true, *dense_column, cgroups.data(), ngroup, vopt), expected_m, expected_v);
    compare_result(tatami_stats::group_variance(true, *sparse_row, cgroups.data(), ngroup, vopt), expected_m, expected_v);
    compare_result(tatami_stats::group_variance(true, *sparse_column, cgroups.data(), ngroup, vopt), expected_m, expected_v);

    // Checking same results from matrices that can yield unsorted indices.
    std::shared_ptr<tatami::NumericMatrix> unsorted_row(new tatami_test::ReversedIndicesWrapper<double, int>(sparse_row));
    compare_result(tatami_stats::group_variance(true, *unsorted_row, cgroups.data(), ngroup, {}), expected_m, expected_v);
    std::shared_ptr<tatami::NumericMatrix> unsorted_column(new tatami_test::ReversedIndicesWrapper<double, int>(sparse_column));
    compare_result(tatami_stats::group_variance(true, *unsorted_column, cgroups.data(), ngroup, {}), expected_m, expected_v);
}

TEST(GroupVariance, ByRowWithNan) {
    size_t NR = 99, NC = 155;

    // Sprinkling in some NaNs.
    auto simulated = tatami_test::simulate_vector<double>(NR * NC, []{ 
        tatami_test::SimulateVectorOptions opt;
        opt.density = 0.2;
        opt.lower = -10;
        opt.upper = -2;
        opt.seed = 52827;
        return opt;
    }());
    for (size_t r = 0; r < NR; ++r) {
        simulated[r * NC + (r % NC)] = std::numeric_limits<double>::quiet_NaN();
    }

    auto dense_row = std::shared_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, std::move(simulated)));
    auto dense_column = tatami::convert_to_dense<double, int>(*dense_row, false, {});
    auto sparse_row = tatami::convert_to_compressed_sparse<double, int>(*dense_row, true, {});
    auto sparse_column = tatami::convert_to_compressed_sparse<double, int>(*dense_row, false, {});

    std::vector<int> cgroups(NC);
    int ngroup = 4; 
    std::vector<std::vector<int> > subsets(ngroup);
    for (size_t c = 0; c < NC; ++c) {
        cgroups[c] = c % ngroup;
        subsets[cgroups[c]].push_back(c);
    }

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
    mopt.skip_nan = true;
    compare_result(tatami_stats::group_variance(true, *dense_column, cgroups.data(), ngroup, mopt), expected_m, expected_v);
    compare_result(tatami_stats::group_variance(true, *dense_column, cgroups.data(), ngroup, mopt), expected_m, expected_v);
    compare_result(tatami_stats::group_variance(true, *sparse_row, cgroups.data(), ngroup, mopt), expected_m, expected_v);
    compare_result(tatami_stats::group_variance(true, *sparse_column, cgroups.data(), ngroup, mopt), expected_m, expected_v);

    mopt.num_threads = 3;
    compare_result(tatami_stats::group_variance(true, *dense_column, cgroups.data(), ngroup, mopt), expected_m, expected_v);
    compare_result(tatami_stats::group_variance(true, *dense_column, cgroups.data(), ngroup, mopt), expected_m, expected_v);
    compare_result(tatami_stats::group_variance(true, *sparse_row, cgroups.data(), ngroup, mopt), expected_m, expected_v);
    compare_result(tatami_stats::group_variance(true, *sparse_column, cgroups.data(), ngroup, mopt), expected_m, expected_v);
}

TEST(GroupVariance, ByColumn) {
    size_t NR = 56, NC = 179;

    auto simulated = tatami_test::simulate_vector<double>(NR * NC, []{
        tatami_test::SimulateVectorOptions opt;
        opt.density = 0.25;
        opt.seed = 83828;
        return opt;
    }());

    auto dense_row = std::shared_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, std::move(simulated)));
    auto dense_column = tatami::convert_to_dense<double, int>(*dense_row, false, {});
    auto sparse_row = tatami::convert_to_compressed_sparse<double, int>(*dense_row, true, {});
    auto sparse_column = tatami::convert_to_compressed_sparse<double, int>(*dense_row, false, {});

    std::vector<int> rgroups(NR);
    int ngroup = 7; 
    std::vector<std::vector<int> > subsets(ngroup);
    for (size_t r = 0; r < NR; ++r) {
        rgroups[r] = r % ngroup;
        subsets[rgroups[r]].push_back(r);
    }

    std::vector<std::vector<double> > expected_m(ngroup), expected_v(ngroup);
    for (int g = 0; g < ngroup; ++g) {
        auto sub = tatami::make_DelayedSubset<0>(dense_row, subsets[g]);
        auto res = tatami_stats::variance(false, *sub, {});
        expected_m[g] = std::move(res.mean);
        expected_v[g] = std::move(res.variance);
    }

    compare_result(tatami_stats::group_variance(false, *dense_row, rgroups.data(), ngroup, {}), expected_m, expected_v);
    compare_result(tatami_stats::group_variance(false, *dense_column, rgroups.data(), ngroup, {}), expected_m, expected_v);
    compare_result(tatami_stats::group_variance(false, *sparse_row, rgroups.data(), ngroup, {}), expected_m, expected_v);
    compare_result(tatami_stats::group_variance(false, *sparse_column, rgroups.data(), ngroup, {}), expected_m, expected_v);

    // Checking that the parallel code is the same.
    tatami_stats::GroupVarianceOptions sopt;
    sopt.num_threads = 3;
    compare_result(tatami_stats::group_variance(false, *dense_row, rgroups.data(), ngroup, sopt), expected_m, expected_v);
    compare_result(tatami_stats::group_variance(false, *dense_column, rgroups.data(), ngroup, sopt), expected_m, expected_v);
    compare_result(tatami_stats::group_variance(false, *sparse_row, rgroups.data(), ngroup, sopt), expected_m, expected_v);
    compare_result(tatami_stats::group_variance(false, *sparse_column, rgroups.data(), ngroup, sopt), expected_m, expected_v);

    // Checking same results from matrices that can yield unsorted indices.
    std::shared_ptr<tatami::NumericMatrix> unsorted_row(new tatami_test::ReversedIndicesWrapper<double, int>(sparse_row));
    compare_result(tatami_stats::group_variance(false, *unsorted_row, rgroups.data(), ngroup, {}), expected_m, expected_v);
    std::shared_ptr<tatami::NumericMatrix> unsorted_column(new tatami_test::ReversedIndicesWrapper<double, int>(sparse_column));
    compare_result(tatami_stats::group_variance(false, *unsorted_column, rgroups.data(), ngroup, {}), expected_m, expected_v);
}

TEST(GroupVariance, ByColumnWithNan) {
    size_t NR = 99, NC = 155;

    // Sprinkling in some NaNs.
    auto simulated = tatami_test::simulate_vector<double>(NR * NC, []{ 
        tatami_test::SimulateVectorOptions opt;
        opt.density = 0.3;
        opt.lower = 1;
        opt.upper = 2;
        opt.seed = 191188;
        return opt;
    }());
    for (size_t c = 0; c < NC; ++c) {
        simulated[(c % NR) * NC + c] = std::numeric_limits<double>::quiet_NaN();
    }

    auto dense_row = std::shared_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, std::move(simulated)));
    auto dense_column = tatami::convert_to_dense<double, int>(*dense_row, false, {});
    auto sparse_row = tatami::convert_to_compressed_sparse<double, int>(*dense_row, true, {});
    auto sparse_column = tatami::convert_to_compressed_sparse<double, int>(*dense_row, false, {});

    std::vector<int> rgroups(NR);
    int ngroup = 6; 
    std::vector<std::vector<int> > subsets(ngroup);
    for (size_t r = 0; r < NR; ++r) {
        rgroups[r] = r % ngroup;
        subsets[rgroups[r]].push_back(r);
    }

    std::vector<std::vector<double> > expected_m(ngroup), expected_v(ngroup);
    for (int g = 0; g < ngroup; ++g) {
        auto sub = tatami::make_DelayedSubset<0>(dense_row, subsets[g]);
        tatami_stats::VarianceOptions mopt;
        mopt.skip_nan = true;
        auto res = tatami_stats::variance(false, *sub, mopt);
        expected_m[g] = std::move(res.mean);
        expected_v[g] = std::move(res.variance);
    }

    tatami_stats::GroupVarianceOptions mopt;
    mopt.skip_nan = true;
    compare_result(tatami_stats::group_variance(false, *dense_row, rgroups.data(), ngroup, mopt), expected_m, expected_v);
    compare_result(tatami_stats::group_variance(false, *dense_column, rgroups.data(), ngroup, mopt), expected_m, expected_v);
    compare_result(tatami_stats::group_variance(false, *sparse_row, rgroups.data(), ngroup, mopt), expected_m, expected_v);
    compare_result(tatami_stats::group_variance(false, *sparse_column, rgroups.data(), ngroup, mopt), expected_m, expected_v);

    mopt.num_threads = 3;
    compare_result(tatami_stats::group_variance(false, *dense_column, rgroups.data(), ngroup, mopt), expected_m, expected_v);
    compare_result(tatami_stats::group_variance(false, *dense_column, rgroups.data(), ngroup, mopt), expected_m, expected_v);
    compare_result(tatami_stats::group_variance(false, *sparse_row, rgroups.data(), ngroup, mopt), expected_m, expected_v);
    compare_result(tatami_stats::group_variance(false, *sparse_column, rgroups.data(), ngroup, mopt), expected_m, expected_v);
}

TEST(GroupVariance, EdgeCases) {
    tatami_stats::GroupVarianceOptions vopt;
    vopt.skip_nan = true;

    {
        auto dense_row = std::shared_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(0, 10, std::vector<double>()));
        auto dense_column = tatami::convert_to_dense<double, int>(*dense_row, false, {});
        auto sparse_row = tatami::convert_to_compressed_sparse<double, int>(*dense_row, true, {});
        auto sparse_column = tatami::convert_to_compressed_sparse<double, int>(*dense_row, false, {});

        {
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

            check_ok(tatami_stats::group_variance(true, *dense_row, grouping.data(), ngroups, vopt));
            check_ok(tatami_stats::group_variance(true, *dense_column, grouping.data(), ngroups, vopt));
            check_ok(tatami_stats::group_variance(true, *sparse_row, grouping.data(), ngroups, vopt));
            check_ok(tatami_stats::group_variance(true, *sparse_column, grouping.data(), ngroups, vopt));
        }

        {
            auto check_ok = [&](const tatami_stats::GroupVarianceResult<double>& res) -> void {
                EXPECT_EQ(res.mean.size(), 0);
                EXPECT_EQ(res.variance.size(), 0);
            };

            const int* group = NULL; 
            check_ok(tatami_stats::group_variance(false, *dense_row, group, 0, {}));
            check_ok(tatami_stats::group_variance(false, *dense_column, group, 0, {}));
            check_ok(tatami_stats::group_variance(false, *sparse_row, group, 0, {}));
            check_ok(tatami_stats::group_variance(false, *sparse_column, group, 0, {}));

            check_ok(tatami_stats::group_variance(false, *dense_row, group, 0, vopt));
            check_ok(tatami_stats::group_variance(false, *dense_column, group, 0, vopt));
            check_ok(tatami_stats::group_variance(false, *sparse_row, group, 0, vopt));
            check_ok(tatami_stats::group_variance(false, *sparse_column, group, 0, vopt));
        }
    }

    {
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
                for (int j = 0; j < nrow; ++j) {
                    EXPECT_TRUE(std::isnan(res.mean[i][j]));
                    EXPECT_TRUE(std::isnan(res.variance[i][j]));
                }
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

        check_ok(tatami_stats::group_variance(true, *dense_row, grouping.data(), ngroups, vopt));
        check_ok(tatami_stats::group_variance(true, *dense_column, grouping.data(), ngroups, vopt));
        check_ok(tatami_stats::group_variance(true, *sparse_row, grouping.data(), ngroups, vopt));
        check_ok(tatami_stats::group_variance(true, *sparse_column, grouping.data(), ngroups, vopt));
    }
}

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
