#include <gtest/gtest.h>

#include <vector>

#include "tatami_stats/group_sum.hpp"
#include "tatami_stats/sum.hpp"
#include "tatami_test/tatami_test.hpp"

#include "utils.h"

TEST(GroupSum, RowSimple) {
    size_t NR = 99, NC = 155;
    auto simulated = tatami_test::simulate_vector<double>(NR * NC, []{
        tatami_test::SimulateVectorOptions opt;
        opt.density = 0.2;
        opt.seed = 239847612;
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
        expected[g] = tatami_stats::sum(true, *sub, {});
    }

    compare_double_vectors_of_vectors(expected, tatami_stats::group_sum(true, *dense_row, cgroups.data(), ngroup, {}));
    compare_double_vectors_of_vectors(expected, tatami_stats::group_sum(true, *dense_column, cgroups.data(), ngroup, {}));
    compare_double_vectors_of_vectors(expected, tatami_stats::group_sum(true, *sparse_row, cgroups.data(), ngroup, {}));
    compare_double_vectors_of_vectors(expected, tatami_stats::group_sum(true, *sparse_column, cgroups.data(), ngroup, {}));

    // Checking that the parallel code is the same.
    tatami_stats::GroupSumOptions sopt;
    sopt.num_threads = 3;
    compare_double_vectors_of_vectors(expected, tatami_stats::group_sum(true, *dense_row, cgroups.data(), ngroup, sopt));
    compare_double_vectors_of_vectors(expected, tatami_stats::group_sum(true, *dense_column, cgroups.data(), ngroup, sopt));
    compare_double_vectors_of_vectors(expected, tatami_stats::group_sum(true, *sparse_row, cgroups.data(), ngroup, sopt));
    compare_double_vectors_of_vectors(expected, tatami_stats::group_sum(true, *sparse_column, cgroups.data(), ngroup, sopt));

    // Checking that the parallel code is the same.
    sopt.num_threads = 1;
    sopt.skip_nan = true;
    compare_double_vectors_of_vectors(expected, tatami_stats::group_sum(true, *dense_row, cgroups.data(), ngroup, sopt));
    compare_double_vectors_of_vectors(expected, tatami_stats::group_sum(true, *dense_column, cgroups.data(), ngroup, sopt));
    compare_double_vectors_of_vectors(expected, tatami_stats::group_sum(true, *sparse_row, cgroups.data(), ngroup, sopt));
    compare_double_vectors_of_vectors(expected, tatami_stats::group_sum(true, *sparse_column, cgroups.data(), ngroup, sopt));

    // Checking same results from matrices that can yield unsorted indices.
    std::shared_ptr<tatami::NumericMatrix> unsorted_row(new tatami_test::ReversedIndicesWrapper<double, int>(sparse_row));
    compare_double_vectors_of_vectors(expected, tatami_stats::group_sum(true, *unsorted_row, cgroups.data(), ngroup, sopt));
    std::shared_ptr<tatami::NumericMatrix> unsorted_column(new tatami_test::ReversedIndicesWrapper<double, int>(sparse_column));
    compare_double_vectors_of_vectors(expected, tatami_stats::group_sum(true, *unsorted_column, cgroups.data(), ngroup, sopt));
}

TEST(GroupSum, RowSkipNan) {
    size_t NR = 99, NC = 155;

    // Sprinkling in some NaNs.
    auto simulated = tatami_test::simulate_vector<double>(NR * NC, []{
        tatami_test::SimulateVectorOptions opt;
        opt.density = 0.15;
        opt.lower = -10;
        opt.upper = -2;
        opt.seed = 23098472;
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

    std::vector<std::vector<double> > expected(ngroup);
    for (int g = 0; g < ngroup; ++g) {
        auto sub = tatami::make_DelayedSubset<1>(dense_row, subsets[g]);
        tatami_stats::SumOptions mopt;
        mopt.skip_nan = true;
        expected[g] = tatami_stats::sum(true, *sub, mopt);
    }

    tatami_stats::GroupSumOptions mopt;
    mopt.skip_nan = true;
    compare_double_vectors_of_vectors(expected, tatami_stats::group_sum(true, *dense_column, cgroups.data(), ngroup, mopt));
    compare_double_vectors_of_vectors(expected, tatami_stats::group_sum(true, *dense_column, cgroups.data(), ngroup, mopt));
    compare_double_vectors_of_vectors(expected, tatami_stats::group_sum(true, *sparse_row, cgroups.data(), ngroup, mopt));
    compare_double_vectors_of_vectors(expected, tatami_stats::group_sum(true, *sparse_column, cgroups.data(), ngroup, mopt));

    mopt.num_threads = 3;
    compare_double_vectors_of_vectors(expected, tatami_stats::group_sum(true, *dense_column, cgroups.data(), ngroup, mopt));
    compare_double_vectors_of_vectors(expected, tatami_stats::group_sum(true, *dense_column, cgroups.data(), ngroup, mopt));
    compare_double_vectors_of_vectors(expected, tatami_stats::group_sum(true, *sparse_row, cgroups.data(), ngroup, mopt));
    compare_double_vectors_of_vectors(expected, tatami_stats::group_sum(true, *sparse_column, cgroups.data(), ngroup, mopt));
}

TEST(GroupSum, ColumnSimple) {
    size_t NR = 56, NC = 179;

    auto simulated = tatami_test::simulate_vector<double>(NR * NC, []{
        tatami_test::SimulateVectorOptions opt;
        opt.density = 0.25;
        opt.seed = 593871;
        return opt;
    }());

    auto dense_row = std::shared_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, std::move(simulated)));
    auto dense_column = tatami::convert_to_dense(dense_row.get(), false);
    auto sparse_row = tatami::convert_to_compressed_sparse(dense_row.get(), true);
    auto sparse_column = tatami::convert_to_compressed_sparse(dense_row.get(), false);

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
        expected[g] = tatami_stats::sum(false, *sub, {});
    }

    compare_double_vectors_of_vectors(expected, tatami_stats::group_sum(false, *dense_row, rgroups.data(), ngroup, {}));
    compare_double_vectors_of_vectors(expected, tatami_stats::group_sum(false, *dense_column, rgroups.data(), ngroup, {}));
    compare_double_vectors_of_vectors(expected, tatami_stats::group_sum(false, *sparse_row, rgroups.data(), ngroup, {}));
    compare_double_vectors_of_vectors(expected, tatami_stats::group_sum(false, *sparse_column, rgroups.data(), ngroup, {}));

    // Checking that the parallel code is the same.
    tatami_stats::GroupSumOptions sopt;
    sopt.num_threads = 3;
    compare_double_vectors_of_vectors(expected, tatami_stats::group_sum(false, *dense_row, rgroups.data(), ngroup, sopt));
    compare_double_vectors_of_vectors(expected, tatami_stats::group_sum(false, *dense_column, rgroups.data(), ngroup, sopt));
    compare_double_vectors_of_vectors(expected, tatami_stats::group_sum(false, *sparse_row, rgroups.data(), ngroup, sopt));
    compare_double_vectors_of_vectors(expected, tatami_stats::group_sum(false, *sparse_column, rgroups.data(), ngroup, sopt));

    // Same results with skipping NaNs.
    sopt.num_threads = 1;
    sopt.skip_nan = true;
    compare_double_vectors_of_vectors(expected, tatami_stats::group_sum(false, *dense_row, rgroups.data(), ngroup, sopt));
    compare_double_vectors_of_vectors(expected, tatami_stats::group_sum(false, *dense_column, rgroups.data(), ngroup, sopt));
    compare_double_vectors_of_vectors(expected, tatami_stats::group_sum(false, *sparse_row, rgroups.data(), ngroup, sopt));
    compare_double_vectors_of_vectors(expected, tatami_stats::group_sum(false, *sparse_column, rgroups.data(), ngroup, sopt));

    // Checking same results from matrices that can yield unsorted indices.
    std::shared_ptr<tatami::NumericMatrix> unsorted_row(new tatami_test::ReversedIndicesWrapper<double, int>(sparse_row));
    compare_double_vectors_of_vectors(expected, tatami_stats::group_sum(false, *unsorted_row, rgroups.data(), ngroup, {}));
    std::shared_ptr<tatami::NumericMatrix> unsorted_column(new tatami_test::ReversedIndicesWrapper<double, int>(sparse_column));
    compare_double_vectors_of_vectors(expected, tatami_stats::group_sum(false, *unsorted_column, rgroups.data(), ngroup, {}));
}

TEST(GroupSum, ColumnSkipNan) {
    size_t NR = 99, NC = 155;

    // Sprinkling in some NaNs.
    auto simulated = tatami_test::simulate_vector<double>(NR * NC, []{
        tatami_test::SimulateVectorOptions opt;
        opt.density = 0.18;
        opt.lower = 1;
        opt.upper = 2;
        opt.seed = 1923871;
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
        tatami_stats::SumOptions mopt;
        mopt.skip_nan = true;
        expected[g] = tatami_stats::sum(false, *sub, mopt);
    }

    tatami_stats::GroupSumOptions mopt;
    mopt.skip_nan = true;
    compare_double_vectors_of_vectors(expected, tatami_stats::group_sum(false, *dense_row, rgroups.data(), ngroup, mopt));
    compare_double_vectors_of_vectors(expected, tatami_stats::group_sum(false, *dense_column, rgroups.data(), ngroup, mopt));
    compare_double_vectors_of_vectors(expected, tatami_stats::group_sum(false, *sparse_row, rgroups.data(), ngroup, mopt));
    compare_double_vectors_of_vectors(expected, tatami_stats::group_sum(false, *sparse_column, rgroups.data(), ngroup, mopt));

    mopt.num_threads = 3;
    compare_double_vectors_of_vectors(expected, tatami_stats::group_sum(false, *dense_row, rgroups.data(), ngroup, mopt));
    compare_double_vectors_of_vectors(expected, tatami_stats::group_sum(false, *dense_column, rgroups.data(), ngroup, mopt));
    compare_double_vectors_of_vectors(expected, tatami_stats::group_sum(false, *sparse_row, rgroups.data(), ngroup, mopt));
    compare_double_vectors_of_vectors(expected, tatami_stats::group_sum(false, *sparse_column, rgroups.data(), ngroup, mopt));
}

TEST(GroupSum, EdgeCases) {
    tatami::DenseRowMatrix<double, int> empty1(0, 10, std::vector<double>());
    tatami::DenseRowMatrix<double, int> empty2(10, 0, std::vector<double>());

    std::vector<int> grouping { 0, 1, 2, 0, 1, 2, 0, 1, 2, 0 };
    auto rout = tatami_stats::group_sum(true, empty1, grouping.data(), 3, {});
    EXPECT_EQ(rout.size(), 3);
    EXPECT_TRUE(rout[0].empty());

    rout = tatami_stats::group_sum(false, empty2, grouping.data(), 3, {});
    EXPECT_EQ(rout.size(), 3);
    EXPECT_TRUE(rout[0].empty());

    grouping.clear();
    EXPECT_TRUE(tatami_stats::group_sum(false, empty1, grouping.data(), 0, {}).empty());
    EXPECT_TRUE(tatami_stats::group_sum(true, empty2, grouping.data(), 0, {}).empty());
}

TEST(GroupSum, NewType) {
    size_t NR = 98, NC = 152;
    auto dump = tatami_test::simulate_vector<double>(NR * NC, []{
        tatami_test::SimulateVectorOptions opt;
        opt.density = 0.1;
        opt.lower = 1;
        opt.upper = 100;
        opt.seed = 717879;
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
    auto rexpected = tatami_stats::group_sum(true, *ref, cgrouping.data(), cgroup, {});
    auto cexpected = tatami_stats::group_sum(false, *ref, rgrouping.data(), rgroup, {});

    std::vector<std::int8_t> ivec(dump.begin(), dump.end());
    auto dense_row = std::make_shared<tatami::DenseRowMatrix<std::int8_t, std::uint8_t> >(NR, NC, std::move(ivec));
    auto dense_column = tatami::convert_to_dense<std::int8_t, std::uint8_t>(*dense_row, false, {});
    auto sparse_row = tatami::convert_to_compressed_sparse<std::int8_t, std::uint8_t>(*dense_row, true, {});
    auto sparse_column = tatami::convert_to_compressed_sparse<std::int8_t, std::uint8_t>(*dense_row, false, {});

    compare_double_vectors_of_vectors(rexpected, tatami_stats::group_sum(true, *dense_row, cgrouping.data(), cgroup, {}));
    compare_double_vectors_of_vectors(rexpected, tatami_stats::group_sum(true, *dense_column, cgrouping.data(), cgroup, {}));
    compare_double_vectors_of_vectors(rexpected, tatami_stats::group_sum(true, *sparse_row, cgrouping.data(), cgroup, {}));
    compare_double_vectors_of_vectors(rexpected, tatami_stats::group_sum(true, *sparse_column, cgrouping.data(), cgroup, {}));

    compare_double_vectors_of_vectors(cexpected, tatami_stats::group_sum(false, *dense_row, rgrouping.data(), rgroup, {}));
    compare_double_vectors_of_vectors(cexpected, tatami_stats::group_sum(false, *dense_column, rgrouping.data(), rgroup, {}));
    compare_double_vectors_of_vectors(cexpected, tatami_stats::group_sum(false, *sparse_row, rgrouping.data(), rgroup, {}));
    compare_double_vectors_of_vectors(cexpected, tatami_stats::group_sum(false, *sparse_column, rgrouping.data(), rgroup, {}));

    // Trying with skipping as well.
    tatami_stats::GroupSumOptions opt;
    opt.skip_nan = true;

    compare_double_vectors_of_vectors(rexpected, tatami_stats::group_sum(true, *dense_row, cgrouping.data(), cgroup, opt));
    compare_double_vectors_of_vectors(rexpected, tatami_stats::group_sum(true, *dense_column, cgrouping.data(), cgroup, opt));
    compare_double_vectors_of_vectors(rexpected, tatami_stats::group_sum(true, *sparse_row, cgrouping.data(), cgroup, opt));
    compare_double_vectors_of_vectors(rexpected, tatami_stats::group_sum(true, *sparse_column, cgrouping.data(), cgroup, opt));

    compare_double_vectors_of_vectors(cexpected, tatami_stats::group_sum(false, *dense_row, rgrouping.data(), rgroup, opt));
    compare_double_vectors_of_vectors(cexpected, tatami_stats::group_sum(false, *dense_column, rgrouping.data(), rgroup, opt));
    compare_double_vectors_of_vectors(cexpected, tatami_stats::group_sum(false, *sparse_row, rgrouping.data(), rgroup, opt));
    compare_double_vectors_of_vectors(cexpected, tatami_stats::group_sum(false, *sparse_column, rgrouping.data(), rgroup, opt));
}
