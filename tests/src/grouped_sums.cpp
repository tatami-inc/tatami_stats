#include <gtest/gtest.h>

#include <vector>

#include "tatami_stats/grouped_sums.hpp"
#include "tatami_stats/sums.hpp"
#include "tatami_test/tatami_test.hpp"
#include "utils.h"

TEST(GroupedSums, ByRow) {
    size_t NR = 99, NC = 155;
    auto simulated = tatami_test::simulate_vector<double>(NR * NC, []{
        tatami_test::SimulateVectorOptions opt;
        opt.density = 0.2;
        opt.seed = 239847612;
        return opt;
    }());

    auto dense_row = std::shared_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, std::move(simulated)));
    auto dense_column = tatami::convert_to_dense(dense_row.get(), false);
    auto sparse_row = tatami::convert_to_compressed_sparse(dense_row.get(), true);
    auto sparse_column = tatami::convert_to_compressed_sparse(dense_row.get(), false);

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
        expected[g] = tatami_stats::sums::by_row(sub.get());
    }

    EXPECT_EQ(expected, tatami_stats::grouped_sums::by_row(dense_row.get(), cgroups.data()));
    EXPECT_EQ(expected, tatami_stats::grouped_sums::by_row(dense_column.get(), cgroups.data()));
    EXPECT_EQ(expected, tatami_stats::grouped_sums::by_row(sparse_row.get(), cgroups.data()));
    EXPECT_EQ(expected, tatami_stats::grouped_sums::by_row(sparse_column.get(), cgroups.data()));

    // Checking that the parallel code is the same.
    tatami_stats::grouped_sums::Options sopt;
    sopt.num_threads = 3;
    EXPECT_EQ(expected, tatami_stats::grouped_sums::by_row(dense_row.get(), cgroups.data(), sopt));
    EXPECT_EQ(expected, tatami_stats::grouped_sums::by_row(dense_column.get(), cgroups.data(), sopt));
    EXPECT_EQ(expected, tatami_stats::grouped_sums::by_row(sparse_row.get(), cgroups.data(), sopt));
    EXPECT_EQ(expected, tatami_stats::grouped_sums::by_row(sparse_column.get(), cgroups.data(), sopt));

    // Checking same results from matrices that can yield unsorted indices.
    std::shared_ptr<tatami::NumericMatrix> unsorted_row(new tatami_test::ReversedIndicesWrapper<double, int>(sparse_row));
    EXPECT_EQ(expected, tatami_stats::grouped_sums::by_row(unsorted_row.get(), cgroups.data()));
    std::shared_ptr<tatami::NumericMatrix> unsorted_column(new tatami_test::ReversedIndicesWrapper<double, int>(sparse_column));
    EXPECT_EQ(expected, tatami_stats::grouped_sums::by_row(unsorted_column.get(), cgroups.data()));
}

TEST(GroupedSums, ByRowWithNan) {
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
    auto dense_column = tatami::convert_to_dense(dense_row.get(), false);
    auto sparse_row = tatami::convert_to_compressed_sparse(dense_row.get(), true);
    auto sparse_column = tatami::convert_to_compressed_sparse(dense_row.get(), false);

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
        tatami_stats::sums::Options mopt;
        mopt.skip_nan = true;
        expected[g] = tatami_stats::sums::by_row(sub.get(), mopt);
    }

    tatami_stats::grouped_sums::Options mopt;
    mopt.skip_nan = true;
    EXPECT_EQ(expected, tatami_stats::grouped_sums::by_row(dense_column.get(), cgroups.data(), mopt));
    EXPECT_EQ(expected, tatami_stats::grouped_sums::by_row(dense_column.get(), cgroups.data(), mopt));
    EXPECT_EQ(expected, tatami_stats::grouped_sums::by_row(sparse_row.get(), cgroups.data(), mopt));
    EXPECT_EQ(expected, tatami_stats::grouped_sums::by_row(sparse_column.get(), cgroups.data(), mopt));
}

TEST(GroupedSums, ByColumn) {
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
        expected[g] = tatami_stats::sums::by_column(sub.get());
    }

    EXPECT_EQ(expected, tatami_stats::grouped_sums::by_column(dense_row.get(), rgroups.data()));
    EXPECT_EQ(expected, tatami_stats::grouped_sums::by_column(dense_column.get(), rgroups.data()));
    EXPECT_EQ(expected, tatami_stats::grouped_sums::by_column(sparse_row.get(), rgroups.data()));
    EXPECT_EQ(expected, tatami_stats::grouped_sums::by_column(sparse_column.get(), rgroups.data()));

    // Checking that the parallel code is the same.
    tatami_stats::grouped_sums::Options sopt;
    sopt.num_threads = 3;
    EXPECT_EQ(expected, tatami_stats::grouped_sums::by_column(dense_row.get(), rgroups.data(), sopt));
    EXPECT_EQ(expected, tatami_stats::grouped_sums::by_column(dense_column.get(), rgroups.data(), sopt));
    EXPECT_EQ(expected, tatami_stats::grouped_sums::by_column(sparse_row.get(), rgroups.data(), sopt));
    EXPECT_EQ(expected, tatami_stats::grouped_sums::by_column(sparse_column.get(), rgroups.data(), sopt));

    // Checking same results from matrices that can yield unsorted indices.
    std::shared_ptr<tatami::NumericMatrix> unsorted_row(new tatami_test::ReversedIndicesWrapper<double, int>(sparse_row));
    EXPECT_EQ(expected, tatami_stats::grouped_sums::by_column(unsorted_row.get(), rgroups.data()));
    std::shared_ptr<tatami::NumericMatrix> unsorted_column(new tatami_test::ReversedIndicesWrapper<double, int>(sparse_column));
    EXPECT_EQ(expected, tatami_stats::grouped_sums::by_column(unsorted_column.get(), rgroups.data()));
}

TEST(GroupedSums, ByColumnWithNan) {
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
        tatami_stats::sums::Options mopt;
        mopt.skip_nan = true;
        expected[g] = tatami_stats::sums::by_column(sub.get(), mopt);
    }

    tatami_stats::grouped_sums::Options mopt;
    mopt.skip_nan = true;
    EXPECT_EQ(expected, tatami_stats::grouped_sums::by_column(dense_row.get(), rgroups.data(), mopt));
    EXPECT_EQ(expected, tatami_stats::grouped_sums::by_column(dense_column.get(), rgroups.data(), mopt));
    EXPECT_EQ(expected, tatami_stats::grouped_sums::by_column(sparse_row.get(), rgroups.data(), mopt));
    EXPECT_EQ(expected, tatami_stats::grouped_sums::by_column(sparse_column.get(), rgroups.data(), mopt));
}

TEST(GroupedSums, EdgeCases) {
    tatami::DenseRowMatrix<double, int> empty1(0, 10, std::vector<double>());
    tatami::DenseRowMatrix<double, int> empty2(10, 0, std::vector<double>());

    std::vector<int> grouping { 0, 1, 2, 0, 1, 2, 0, 1, 2, 0 };
    auto rout = tatami_stats::grouped_sums::by_row(&empty1, grouping.data());
    EXPECT_EQ(rout.size(), 3);
    EXPECT_TRUE(rout[0].empty());
    rout = tatami_stats::grouped_sums::by_column(&empty2, grouping.data());
    EXPECT_EQ(rout.size(), 3);
    EXPECT_TRUE(rout[0].empty());

    grouping.clear();
    EXPECT_TRUE(tatami_stats::grouped_sums::by_row(&empty2, grouping.data()).empty());
    EXPECT_TRUE(tatami_stats::grouped_sums::by_column(&empty1, grouping.data()).empty());
}

TEST(GroupedSums, NewType) {
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

    std::vector<int> cgrouping;
    for (size_t c = 0; c < NC; ++c) {
        cgrouping.push_back(c % 5);
    }
    std::vector<int> rgrouping;
    for (size_t r = 0; r < NR; ++r) {
        rgrouping.push_back(r % 7);
    }
    auto rexpected = tatami_stats::grouped_sums::by_row(ref.get(), cgrouping.data());
    auto cexpected = tatami_stats::grouped_sums::by_column(ref.get(), rgrouping.data());

    std::vector<int8_t> ivec(dump.begin(), dump.end());
    auto dense_row = std::make_shared<tatami::DenseRowMatrix<int8_t, uint8_t> >(NR, NC, std::move(ivec));
    auto dense_column = tatami::convert_to_dense(dense_row.get(), false);
    auto sparse_row = tatami::convert_to_compressed_sparse(dense_row.get(), true);
    auto sparse_column = tatami::convert_to_compressed_sparse(dense_row.get(), false);

    EXPECT_EQ(tatami_stats::grouped_sums::by_row(dense_row.get(), cgrouping.data()), rexpected);
    EXPECT_EQ(tatami_stats::grouped_sums::by_row(dense_column.get(), cgrouping.data()), rexpected);
    EXPECT_EQ(tatami_stats::grouped_sums::by_row(sparse_row.get(), cgrouping.data()), rexpected);
    EXPECT_EQ(tatami_stats::grouped_sums::by_row(sparse_column.get(), cgrouping.data()), rexpected);

    EXPECT_EQ(tatami_stats::grouped_sums::by_column(dense_row.get(), rgrouping.data()), cexpected);
    EXPECT_EQ(tatami_stats::grouped_sums::by_column(dense_column.get(), rgrouping.data()), cexpected);
    EXPECT_EQ(tatami_stats::grouped_sums::by_column(sparse_row.get(), rgrouping.data()), cexpected);
    EXPECT_EQ(tatami_stats::grouped_sums::by_column(sparse_column.get(), rgrouping.data()), cexpected);
}

TEST(GroupedSums, DirtyOutputs) {
    size_t NR = 56, NC = 179;

    auto simulated = tatami_test::simulate_vector<double>(NR * NC, []{
        tatami_test::SimulateVectorOptions opt;
        opt.density = 0.3;
        opt.seed = 192191;
        return opt;
    }());

    auto dense_row = std::shared_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, std::move(simulated)));
    auto dense_column = tatami::convert_to_dense(dense_row.get(), false);
    auto sparse_row = tatami::convert_to_compressed_sparse(dense_row.get(), true);
    auto sparse_column = tatami::convert_to_compressed_sparse(dense_row.get(), false);

    int ngroup = 5; 
    std::vector<int> grouping;
    for (size_t r = 0; r < NR; ++r) {
        grouping.push_back(r % ngroup);
    }

    std::vector<double> dirty(ngroup * NC, -1);
    std::vector<double*> ptrs;
    ptrs.reserve(ngroup);
    for (int g = 0; g < ngroup; ++g) {
        ptrs.push_back(dirty.data() + g * NC);
    }

    auto ref = tatami_stats::grouped_sums::by_column(dense_row.get(), grouping.data());
    std::vector<double> expected;
    for (const auto& r : ref) {
        expected.insert(expected.end(), r.begin(), r.end());
    }

    tatami_stats::grouped_sums::Options mopt;
    tatami_stats::grouped_sums::apply(false, dense_row.get(), grouping.data(), ngroup, ptrs.data(), mopt);
    EXPECT_EQ(expected, dirty);

    std::fill(dirty.begin(), dirty.end(), -1);
    tatami_stats::grouped_sums::apply(false, dense_column.get(), grouping.data(), ngroup, ptrs.data(), mopt);
    EXPECT_EQ(expected, dirty);

    std::fill(dirty.begin(), dirty.end(), -1);
    tatami_stats::grouped_sums::apply(false, sparse_row.get(), grouping.data(), ngroup, ptrs.data(), mopt);
    EXPECT_EQ(expected, dirty);

    std::fill(dirty.begin(), dirty.end(), -1);
    tatami_stats::grouped_sums::apply(false, sparse_column.get(), grouping.data(), ngroup, ptrs.data(), mopt);
    EXPECT_EQ(expected, dirty);
}
