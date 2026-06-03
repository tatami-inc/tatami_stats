#include <gtest/gtest.h>

#include <vector>

#include "tatami/tatami.hpp"
#include "tatami_stats/group_median.hpp"
#include "tatami_test/tatami_test.hpp"

TEST(GroupMedian, RowSimple) {
    size_t NR = 99, NC = 155;

    // We use a density of 0.5 so that some of the median calculations will
    // need to use the structural zeros. We also put all non-zero values on
    // one side of zero, otherwise the structural zeros will dominate the
    // median; in this case, we choose all-negative values.
    auto simulated = tatami_test::simulate_vector<double>(NR * NC, []{
        tatami_test::SimulateVectorOptions opt;
        opt.density = 0.5;
        opt.lower = -10;
        opt.upper = -2;
        opt.seed = 192836;
        return opt;
    }());

    auto dense_row = std::shared_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, std::move(simulated)));
    auto dense_column = tatami::convert_to_dense<double, int>(*dense_row, false, {});
    auto sparse_row = tatami::convert_to_compressed_sparse<double, int>(*dense_row, true, {});
    auto sparse_column = tatami::convert_to_compressed_sparse<double, int>(*dense_row, false, {});

    std::vector<int> cgroups(NC);
    int ngroup = 3; 
    std::vector<std::vector<int> > subsets(ngroup);
    for (size_t c = 0; c < NC; ++c) {
        cgroups[c] = c % ngroup;
        subsets[cgroups[c]].push_back(c);
    }

    std::vector<std::vector<double> > expected(ngroup);
    for (int g = 0; g < ngroup; ++g) {
        auto sub = tatami::make_DelayedSubset<1>(dense_row, subsets[g]);
        expected[g] = tatami_stats::median::apply(true, *sub, {});
    }

    EXPECT_EQ(expected, tatami_stats::group_median::apply(true, *dense_column, cgroups.data(), ngroup, {}));
    EXPECT_EQ(expected, tatami_stats::group_median::apply(true, *dense_column, cgroups.data(), ngroup, {}));
    EXPECT_EQ(expected, tatami_stats::group_median::apply(true, *sparse_row, cgroups.data(), ngroup, {}));
    EXPECT_EQ(expected, tatami_stats::group_median::apply(true, *sparse_column, cgroups.data(), ngroup, {}));

    // Checking that the parallel code is the same.
    tatami_stats::group_median::Options mopt;
    mopt.num_threads = 3;
    EXPECT_EQ(expected, tatami_stats::group_median::apply(true, *dense_row, cgroups.data(), ngroup, mopt));
    EXPECT_EQ(expected, tatami_stats::group_median::apply(true, *dense_column, cgroups.data(), ngroup, mopt));
    EXPECT_EQ(expected, tatami_stats::group_median::apply(true, *sparse_row, cgroups.data(), ngroup, mopt));
    EXPECT_EQ(expected, tatami_stats::group_median::apply(true, *sparse_column, cgroups.data(), ngroup, mopt));

    // Checking that we get the same results when skipping NaNs.
    mopt.num_threads = 1;
    mopt.skip_nan = true;
    EXPECT_EQ(expected, tatami_stats::group_median::apply(true, *dense_row, cgroups.data(), ngroup, mopt));
    EXPECT_EQ(expected, tatami_stats::group_median::apply(true, *dense_column, cgroups.data(), ngroup, mopt));
    EXPECT_EQ(expected, tatami_stats::group_median::apply(true, *sparse_row, cgroups.data(), ngroup, mopt));
    EXPECT_EQ(expected, tatami_stats::group_median::apply(true, *sparse_column, cgroups.data(), ngroup, mopt));

    // Checking same results from matrices that can yield unsorted indices.
    std::shared_ptr<tatami::NumericMatrix> unsorted_row(new tatami_test::ReversedIndicesWrapper<double, int>(sparse_row));
    EXPECT_EQ(expected, tatami_stats::group_median::apply(true, *unsorted_row, cgroups.data(), ngroup, {}));
    std::shared_ptr<tatami::NumericMatrix> unsorted_column(new tatami_test::ReversedIndicesWrapper<double, int>(sparse_column));
    EXPECT_EQ(expected, tatami_stats::group_median::apply(true, *unsorted_column, cgroups.data(), ngroup, {}));
}

TEST(GroupMedian, RowSkipNan) {
    size_t NR = 99, NC = 155;

    // We use a density of 0.5 as described above, plus sprinkling in some NaNs.
    auto simulated = tatami_test::simulate_vector<double>(NR * NC, []{
        tatami_test::SimulateVectorOptions opt;
        opt.density = 0.5;
        opt.lower = -10;
        opt.upper = -2;
        opt.seed = 1297836;
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
    int ngroup = 3; 
    std::vector<std::vector<int> > subsets(ngroup);
    for (size_t c = 0; c < NC; ++c) {
        cgroups[c] = c % ngroup;
        subsets[cgroups[c]].push_back(c);
    }

    std::vector<std::vector<double> > expected(ngroup);
    for (int g = 0; g < ngroup; ++g) {
        auto sub = tatami::make_DelayedSubset<1>(dense_row, subsets[g]);
        tatami_stats::median::Options mopt;
        mopt.skip_nan = true;
        expected[g] = tatami_stats::median::apply(true, *sub, mopt);
    }

    tatami_stats::group_median::Options mopt;
    mopt.skip_nan = true;
    EXPECT_EQ(expected, tatami_stats::group_median::apply(true, *dense_column, cgroups.data(), ngroup, mopt));
    EXPECT_EQ(expected, tatami_stats::group_median::apply(true, *dense_column, cgroups.data(), ngroup, mopt));
    EXPECT_EQ(expected, tatami_stats::group_median::apply(true, *sparse_row, cgroups.data(), ngroup, mopt));
    EXPECT_EQ(expected, tatami_stats::group_median::apply(true, *sparse_column, cgroups.data(), ngroup, mopt));

    mopt.num_threads = 3;
    EXPECT_EQ(expected, tatami_stats::group_median::apply(true, *dense_column, cgroups.data(), ngroup, mopt));
    EXPECT_EQ(expected, tatami_stats::group_median::apply(true, *dense_column, cgroups.data(), ngroup, mopt));
    EXPECT_EQ(expected, tatami_stats::group_median::apply(true, *sparse_row, cgroups.data(), ngroup, mopt));
    EXPECT_EQ(expected, tatami_stats::group_median::apply(true, *sparse_column, cgroups.data(), ngroup, mopt));
}

TEST(GroupMedian, ColumnSimple) {
    size_t NR = 56, NC = 179;

    // See above for why we use a density of 0.5. This time, we use all-positive values.
    auto simulated = tatami_test::simulate_vector<double>(NR * NC, []{
        tatami_test::SimulateVectorOptions opt;
        opt.density = 0.5;
        opt.lower = 0.1;
        opt.upper = 2;
        opt.seed = 239847;
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

    std::vector<std::vector<double> > expected(ngroup);
    for (int g = 0; g < ngroup; ++g) {
        auto sub = tatami::make_DelayedSubset<0>(dense_row, subsets[g]);
        expected[g] = tatami_stats::median::apply(false, *sub, {});
    }

    EXPECT_EQ(expected, tatami_stats::group_median::apply(false, *dense_row, rgroups.data(), ngroup, {}));
    EXPECT_EQ(expected, tatami_stats::group_median::apply(false, *dense_column, rgroups.data(), ngroup, {}));
    EXPECT_EQ(expected, tatami_stats::group_median::apply(false, *sparse_row, rgroups.data(), ngroup, {}));
    EXPECT_EQ(expected, tatami_stats::group_median::apply(false, *sparse_column, rgroups.data(), ngroup, {}));

    // Checking that the parallel code is the same.
    tatami_stats::group_median::Options mopt;
    mopt.num_threads = 3;
    EXPECT_EQ(expected, tatami_stats::group_median::apply(false, *dense_row, rgroups.data(), ngroup, mopt));
    EXPECT_EQ(expected, tatami_stats::group_median::apply(false, *dense_column, rgroups.data(), ngroup, mopt));
    EXPECT_EQ(expected, tatami_stats::group_median::apply(false, *sparse_row, rgroups.data(), ngroup, mopt));
    EXPECT_EQ(expected, tatami_stats::group_median::apply(false, *sparse_column, rgroups.data(), ngroup, mopt));

    // Checking that we get the same results when skipping NaNs.
    mopt.num_threads = 1;
    mopt.skip_nan = true;
    EXPECT_EQ(expected, tatami_stats::group_median::apply(false, *dense_row, rgroups.data(), ngroup, mopt));
    EXPECT_EQ(expected, tatami_stats::group_median::apply(false, *dense_column, rgroups.data(), ngroup, mopt));
    EXPECT_EQ(expected, tatami_stats::group_median::apply(false, *sparse_row, rgroups.data(), ngroup, mopt));
    EXPECT_EQ(expected, tatami_stats::group_median::apply(false, *sparse_column, rgroups.data(), ngroup, mopt));

    // Checking same results from matrices that can yield unsorted indices.
    std::shared_ptr<tatami::NumericMatrix> unsorted_row(new tatami_test::ReversedIndicesWrapper<double, int>(sparse_row));
    EXPECT_EQ(expected, tatami_stats::group_median::apply(false, *unsorted_row, rgroups.data(), ngroup, {}));
    std::shared_ptr<tatami::NumericMatrix> unsorted_column(new tatami_test::ReversedIndicesWrapper<double, int>(sparse_column));
    EXPECT_EQ(expected, tatami_stats::group_median::apply(false, *unsorted_column, rgroups.data(), ngroup, {}));
}

TEST(GroupMedian, ColumnSkipNan) {
    size_t NR = 99, NC = 155;

    // We use a density of 0.5, plus sprinkling in some NaNs.
    auto simulated = tatami_test::simulate_vector<double>(NR * NC, []{
        tatami_test::SimulateVectorOptions opt;
        opt.density = 0.5;
        opt.lower = 1;
        opt.upper = 2;
        opt.seed = 9428378;
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
    int ngroup = 7; 
    std::vector<std::vector<int> > subsets(ngroup);
    for (size_t r = 0; r < NR; ++r) {
        rgroups[r] = r % ngroup;
        subsets[rgroups[r]].push_back(r);
    }

    std::vector<std::vector<double> > expected(ngroup);
    for (int g = 0; g < ngroup; ++g) {
        auto sub = tatami::make_DelayedSubset<0>(dense_row, subsets[g]);
        tatami_stats::median::Options mopt;
        mopt.skip_nan = true;
        expected[g] = tatami_stats::median::apply(false, *sub, mopt);
    }

    tatami_stats::group_median::Options mopt;
    mopt.skip_nan = true;
    EXPECT_EQ(expected, tatami_stats::group_median::apply(false, *dense_row, rgroups.data(), ngroup, mopt));
    EXPECT_EQ(expected, tatami_stats::group_median::apply(false, *dense_column, rgroups.data(), ngroup, mopt));
    EXPECT_EQ(expected, tatami_stats::group_median::apply(false, *sparse_row, rgroups.data(), ngroup, mopt));
    EXPECT_EQ(expected, tatami_stats::group_median::apply(false, *sparse_column, rgroups.data(), ngroup, mopt));

    mopt.num_threads = 3;
    EXPECT_EQ(expected, tatami_stats::group_median::apply(false, *dense_row, rgroups.data(), ngroup, mopt));
    EXPECT_EQ(expected, tatami_stats::group_median::apply(false, *dense_column, rgroups.data(), ngroup, mopt));
    EXPECT_EQ(expected, tatami_stats::group_median::apply(false, *sparse_row, rgroups.data(), ngroup, mopt));
    EXPECT_EQ(expected, tatami_stats::group_median::apply(false, *sparse_column, rgroups.data(), ngroup, mopt));
}

TEST(GroupMedian, EdgeCases) {
    tatami_stats::group_median::Options vopt;
    vopt.skip_nan = true;

    {
        auto dense_row = std::shared_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(0, 10, std::vector<double>()));
        auto dense_column = tatami::convert_to_dense<double, int>(*dense_row, false, {});
        auto sparse_row = tatami::convert_to_compressed_sparse<double, int>(*dense_row, true, {});
        auto sparse_column = tatami::convert_to_compressed_sparse<double, int>(*dense_row, false, {});

        {
            int ngroups = 3;
            std::vector<int> grouping { 0, 1, 2, 0, 1, 2, 0, 1, 2, 0 };

            auto check_ok = [&](const std::vector<std::vector<double> >& res) -> void {
                EXPECT_EQ(res.size(), ngroups);
                for (int g = 0; g < ngroups; ++g) {
                    EXPECT_TRUE(res[g].empty());
                }
            };

            check_ok(tatami_stats::group_median::apply(true, *dense_row, grouping.data(), ngroups, {}));
            check_ok(tatami_stats::group_median::apply(true, *dense_column, grouping.data(), ngroups, {}));
            check_ok(tatami_stats::group_median::apply(true, *sparse_row, grouping.data(), ngroups, {}));
            check_ok(tatami_stats::group_median::apply(true, *sparse_column, grouping.data(), ngroups, {}));

            check_ok(tatami_stats::group_median::apply(true, *dense_row, grouping.data(), ngroups, vopt));
            check_ok(tatami_stats::group_median::apply(true, *dense_column, grouping.data(), ngroups, vopt));
            check_ok(tatami_stats::group_median::apply(true, *sparse_row, grouping.data(), ngroups, vopt));
            check_ok(tatami_stats::group_median::apply(true, *sparse_column, grouping.data(), ngroups, vopt));
        }

        {
            const int* group = NULL; 
            EXPECT_EQ(tatami_stats::group_median::apply(false, *dense_row, group, 0, {}).size(), 0);
            EXPECT_EQ(tatami_stats::group_median::apply(false, *dense_column, group, 0, {}).size(), 0);
            EXPECT_EQ(tatami_stats::group_median::apply(false, *sparse_row, group, 0, {}).size(), 0);
            EXPECT_EQ(tatami_stats::group_median::apply(false, *sparse_column, group, 0, {}).size(), 0);

            EXPECT_EQ(tatami_stats::group_median::apply(false, *dense_row, group, 0, vopt).size(), 0);
            EXPECT_EQ(tatami_stats::group_median::apply(false, *dense_column, group, 0, vopt).size(), 0);
            EXPECT_EQ(tatami_stats::group_median::apply(false, *sparse_row, group, 0, vopt).size(), 0);
            EXPECT_EQ(tatami_stats::group_median::apply(false, *sparse_column, group, 0, vopt).size(), 0);
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

        auto check_ok = [&](const std::vector<std::vector<double> >& res) -> void {
            EXPECT_EQ(res.size(), ngroups);
            for (int i = 0; i < ngroups - 1; ++i) {
                for (int j = 0; j < nrow; ++j) {
                    EXPECT_TRUE(std::isnan(res[i][j]));
                }
            }
            for (int j = 0; j < nrow; ++j) {
                EXPECT_EQ(res[ngroups - 1][j], 0);
            }
        };

        check_ok(tatami_stats::group_median::apply(true, *dense_row, grouping.data(), ngroups, {}));
        check_ok(tatami_stats::group_median::apply(true, *dense_column, grouping.data(), ngroups, {}));
        check_ok(tatami_stats::group_median::apply(true, *sparse_row, grouping.data(), ngroups, {}));
        check_ok(tatami_stats::group_median::apply(true, *sparse_column, grouping.data(), ngroups, {}));

        check_ok(tatami_stats::group_median::apply(true, *dense_row, grouping.data(), ngroups, vopt));
        check_ok(tatami_stats::group_median::apply(true, *dense_column, grouping.data(), ngroups, vopt));
        check_ok(tatami_stats::group_median::apply(true, *sparse_row, grouping.data(), ngroups, vopt));
        check_ok(tatami_stats::group_median::apply(true, *sparse_column, grouping.data(), ngroups, vopt));
    }
}

TEST(GroupMedian, NewType) {
    size_t NR = 98, NC = 152;

    auto dump = tatami_test::simulate_vector<double>(NR * NC, []{
        tatami_test::SimulateVectorOptions opt;
        opt.density = 0.5; // see above for why we use a density of 0.5.
        opt.lower = 1;
        opt.upper = 100;
        opt.seed = 2398472;
        return opt;
    }());

    for (auto& d : dump) { 
        d = std::round(d);
    }
    auto ref = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, dump));

    const int cgroup = 5;
    std::vector<int> cgrouping;
    for (size_t c = 0; c < NC; ++c) {
        cgrouping.push_back(c % cgroup);
    }
    const int rgroup = 7;
    std::vector<int> rgrouping;
    for (size_t r = 0; r < NR; ++r) {
        rgrouping.push_back(r % rgroup);
    }
    auto rexpected = tatami_stats::group_median::apply(true, *ref, cgrouping.data(), cgroup, {});
    auto cexpected = tatami_stats::group_median::apply(false, *ref, rgrouping.data(), rgroup, {});

    std::vector<std::int8_t> ivec(dump.begin(), dump.end());
    auto dense_row = std::make_shared<tatami::DenseRowMatrix<std::int8_t, std::uint8_t> >(NR, NC, std::move(ivec));
    auto dense_column = tatami::convert_to_dense<std::int8_t, std::uint8_t>(*dense_row, false, {});
    auto sparse_row = tatami::convert_to_compressed_sparse<std::int8_t, std::uint8_t>(*dense_row, true, {});
    auto sparse_column = tatami::convert_to_compressed_sparse<std::int8_t, std::uint8_t>(*dense_row, false, {});

    EXPECT_EQ(tatami_stats::group_median::apply(true, *dense_row, cgrouping.data(), cgroup, {}), rexpected);
    EXPECT_EQ(tatami_stats::group_median::apply(true, *dense_column, cgrouping.data(), cgroup, {}), rexpected);
    EXPECT_EQ(tatami_stats::group_median::apply(true, *sparse_row, cgrouping.data(), cgroup, {}), rexpected);
    EXPECT_EQ(tatami_stats::group_median::apply(true, *sparse_column, cgrouping.data(), cgroup, {}), rexpected);

    EXPECT_EQ(tatami_stats::group_median::apply(false, *dense_row, rgrouping.data(), rgroup, {}), cexpected);
    EXPECT_EQ(tatami_stats::group_median::apply(false, *dense_column, rgrouping.data(), rgroup, {}), cexpected);
    EXPECT_EQ(tatami_stats::group_median::apply(false, *sparse_row, rgrouping.data(), rgroup, {}), cexpected);
    EXPECT_EQ(tatami_stats::group_median::apply(false, *sparse_column, rgrouping.data(), rgroup, {}), cexpected);

    // Checking that it works with skipping NaNs.
    tatami_stats::group_median::Options mopt;
    mopt.skip_nan = true;

    EXPECT_EQ(tatami_stats::group_median::apply(true, *dense_row, cgrouping.data(), cgroup, mopt), rexpected);
    EXPECT_EQ(tatami_stats::group_median::apply(true, *dense_column, cgrouping.data(), cgroup, mopt), rexpected);
    EXPECT_EQ(tatami_stats::group_median::apply(true, *sparse_row, cgrouping.data(), cgroup, mopt), rexpected);
    EXPECT_EQ(tatami_stats::group_median::apply(true, *sparse_column, cgrouping.data(), cgroup, mopt), rexpected);

    EXPECT_EQ(tatami_stats::group_median::apply(false, *dense_row, rgrouping.data(), rgroup, mopt), cexpected);
    EXPECT_EQ(tatami_stats::group_median::apply(false, *dense_column, rgrouping.data(), rgroup, mopt), cexpected);
    EXPECT_EQ(tatami_stats::group_median::apply(false, *sparse_row, rgrouping.data(), rgroup, mopt), cexpected);
    EXPECT_EQ(tatami_stats::group_median::apply(false, *sparse_column, rgrouping.data(), rgroup, mopt), cexpected);
}
