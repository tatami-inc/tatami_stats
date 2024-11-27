#include <gtest/gtest.h>

#include <vector>

#include "tatami_stats/grouped_variances.hpp"
#include "tatami_stats/variances.hpp"
#include "tatami_test/tatami_test.hpp"
#include "utils.h"

template<class L_, class R_>
void compare_double_vectors_of_vectors(const L_& left, const R_& right) {
    ASSERT_EQ(left.size(), right.size());
    for (size_t i = 0; i < left.size(); ++i) {
        compare_double_vectors(left[i], right[i]);
    }
}

TEST(GroupedVariances, ByRow) {
    size_t NR = 99, NC = 155;

    auto simulated = tatami_test::simulate_vector<double>(NR * NC, []{
        tatami_test::SimulateVectorOptions opt;
        opt.density = 0.2;
        opt.seed = 1298191;
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
        expected[g] = tatami_stats::variances::by_row(sub.get());
    }

    compare_double_vectors_of_vectors(expected, tatami_stats::grouped_variances::by_row(dense_row.get(), cgroups.data()));
    compare_double_vectors_of_vectors(expected, tatami_stats::grouped_variances::by_row(dense_column.get(), cgroups.data()));
    compare_double_vectors_of_vectors(expected, tatami_stats::grouped_variances::by_row(sparse_row.get(), cgroups.data()));
    compare_double_vectors_of_vectors(expected, tatami_stats::grouped_variances::by_row(sparse_column.get(), cgroups.data()));

    // Checking that the parallel code is the same.
    tatami_stats::grouped_variances::Options sopt;
    sopt.num_threads = 3;
    compare_double_vectors_of_vectors(expected, tatami_stats::grouped_variances::by_row(dense_row.get(), cgroups.data(), sopt));
    compare_double_vectors_of_vectors(expected, tatami_stats::grouped_variances::by_row(dense_column.get(), cgroups.data(), sopt));
    compare_double_vectors_of_vectors(expected, tatami_stats::grouped_variances::by_row(sparse_row.get(), cgroups.data(), sopt));
    compare_double_vectors_of_vectors(expected, tatami_stats::grouped_variances::by_row(sparse_column.get(), cgroups.data(), sopt));

    // Checking same results from matrices that can yield unsorted indices.
    std::shared_ptr<tatami::NumericMatrix> unsorted_row(new tatami_test::ReversedIndicesWrapper<double, int>(sparse_row));
    compare_double_vectors_of_vectors(expected, tatami_stats::grouped_variances::by_row(unsorted_row.get(), cgroups.data()));
    std::shared_ptr<tatami::NumericMatrix> unsorted_column(new tatami_test::ReversedIndicesWrapper<double, int>(sparse_column));
    compare_double_vectors_of_vectors(expected, tatami_stats::grouped_variances::by_row(unsorted_column.get(), cgroups.data()));
}

TEST(GroupedVariances, ByRowWithNan) {
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
        tatami_stats::variances::Options mopt;
        mopt.skip_nan = true;
        expected[g] = tatami_stats::variances::by_row(sub.get(), mopt);
    }

    tatami_stats::grouped_variances::Options mopt;
    mopt.skip_nan = true;
    compare_double_vectors_of_vectors(expected, tatami_stats::grouped_variances::by_row(dense_column.get(), cgroups.data(), mopt));
    compare_double_vectors_of_vectors(expected, tatami_stats::grouped_variances::by_row(dense_column.get(), cgroups.data(), mopt));
    compare_double_vectors_of_vectors(expected, tatami_stats::grouped_variances::by_row(sparse_row.get(), cgroups.data(), mopt));
    compare_double_vectors_of_vectors(expected, tatami_stats::grouped_variances::by_row(sparse_column.get(), cgroups.data(), mopt));
}

TEST(GroupedVariances, ByColumn) {
    size_t NR = 56, NC = 179;

    auto simulated = tatami_test::simulate_vector<double>(NR * NC, []{
        tatami_test::SimulateVectorOptions opt;
        opt.density = 0.25;
        opt.seed = 83828;
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
        expected[g] = tatami_stats::variances::by_column(sub.get());
    }

    compare_double_vectors_of_vectors(expected, tatami_stats::grouped_variances::by_column(dense_row.get(), rgroups.data()));
    compare_double_vectors_of_vectors(expected, tatami_stats::grouped_variances::by_column(dense_column.get(), rgroups.data()));
    compare_double_vectors_of_vectors(expected, tatami_stats::grouped_variances::by_column(sparse_row.get(), rgroups.data()));
    compare_double_vectors_of_vectors(expected, tatami_stats::grouped_variances::by_column(sparse_column.get(), rgroups.data()));

    // Checking that the parallel code is the same.
    tatami_stats::grouped_variances::Options sopt;
    sopt.num_threads = 3;
    compare_double_vectors_of_vectors(expected, tatami_stats::grouped_variances::by_column(dense_row.get(), rgroups.data(), sopt));
    compare_double_vectors_of_vectors(expected, tatami_stats::grouped_variances::by_column(dense_column.get(), rgroups.data(), sopt));
    compare_double_vectors_of_vectors(expected, tatami_stats::grouped_variances::by_column(sparse_row.get(), rgroups.data(), sopt));
    compare_double_vectors_of_vectors(expected, tatami_stats::grouped_variances::by_column(sparse_column.get(), rgroups.data(), sopt));

    // Checking same results from matrices that can yield unsorted indices.
    std::shared_ptr<tatami::NumericMatrix> unsorted_row(new tatami_test::ReversedIndicesWrapper<double, int>(sparse_row));
    compare_double_vectors_of_vectors(expected, tatami_stats::grouped_variances::by_column(unsorted_row.get(), rgroups.data()));
    std::shared_ptr<tatami::NumericMatrix> unsorted_column(new tatami_test::ReversedIndicesWrapper<double, int>(sparse_column));
    compare_double_vectors_of_vectors(expected, tatami_stats::grouped_variances::by_column(unsorted_column.get(), rgroups.data()));
}

TEST(GroupedVariances, ByColumnWithNan) {
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
        tatami_stats::variances::Options mopt;
        mopt.skip_nan = true;
        expected[g] = tatami_stats::variances::by_column(sub.get(), mopt);
    }

    tatami_stats::grouped_variances::Options mopt;
    mopt.skip_nan = true;
    compare_double_vectors_of_vectors(expected, tatami_stats::grouped_variances::by_column(dense_row.get(), rgroups.data(), mopt));
    compare_double_vectors_of_vectors(expected, tatami_stats::grouped_variances::by_column(dense_column.get(), rgroups.data(), mopt));
    compare_double_vectors_of_vectors(expected, tatami_stats::grouped_variances::by_column(sparse_row.get(), rgroups.data(), mopt));
    compare_double_vectors_of_vectors(expected, tatami_stats::grouped_variances::by_column(sparse_column.get(), rgroups.data(), mopt));
}

TEST(GroupedVariances, EdgeCases) {
    {
        tatami::DenseRowMatrix<double, int> empty1(0, 10, std::vector<double>());
        tatami::DenseRowMatrix<double, int> empty2(10, 0, std::vector<double>());

        std::vector<int> grouping { 0, 1, 2, 0, 1, 2, 0, 1, 2, 0 };
        auto rout = tatami_stats::grouped_variances::by_row(&empty1, grouping.data());
        EXPECT_EQ(rout.size(), 3);
        EXPECT_TRUE(rout[0].empty());
        rout = tatami_stats::grouped_variances::by_column(&empty2, grouping.data());
        EXPECT_EQ(rout.size(), 3);
        EXPECT_TRUE(rout[0].empty());

        grouping.clear();
        EXPECT_TRUE(tatami_stats::grouped_variances::by_row(&empty2, grouping.data()).empty());
        EXPECT_TRUE(tatami_stats::grouped_variances::by_column(&empty1, grouping.data()).empty());
    }

    {
        tatami::DenseRowMatrix<double, int> zeros(10, 10, std::vector<double>(100));

        std::vector<int> grouping(10, 3);
        auto rout = tatami_stats::grouped_variances::by_row(&zeros, grouping.data());
        EXPECT_EQ(rout.size(), 4);
        EXPECT_TRUE(std::isnan(rout[0].front()));
        EXPECT_TRUE(std::isnan(rout[2].back()));
        EXPECT_EQ(rout[3].front(), 0);
        EXPECT_EQ(rout[3].back(), 0);
    }
}

TEST(GroupedVariances, NewType) {
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
    for (size_t c = 0; c < NC; ++c) {
        cgrouping.push_back(c % 5);
    }
    std::vector<int> rgrouping;
    for (size_t r = 0; r < NR; ++r) {
        rgrouping.push_back(r % 7);
    }
    auto rexpected = tatami_stats::grouped_variances::by_row(ref.get(), cgrouping.data());
    auto cexpected = tatami_stats::grouped_variances::by_column(ref.get(), rgrouping.data());

    std::vector<int8_t> ivec(dump.begin(), dump.end());
    auto dense_row = std::make_shared<tatami::DenseRowMatrix<int8_t, uint8_t> >(NR, NC, std::move(ivec));
    auto dense_column = tatami::convert_to_dense(dense_row.get(), false);
    auto sparse_row = tatami::convert_to_compressed_sparse(dense_row.get(), true);
    auto sparse_column = tatami::convert_to_compressed_sparse(dense_row.get(), false);

    compare_double_vectors_of_vectors(tatami_stats::grouped_variances::by_row(dense_row.get(), cgrouping.data()), rexpected);
    compare_double_vectors_of_vectors(tatami_stats::grouped_variances::by_row(dense_column.get(), cgrouping.data()), rexpected);
    compare_double_vectors_of_vectors(tatami_stats::grouped_variances::by_row(sparse_row.get(), cgrouping.data()), rexpected);
    compare_double_vectors_of_vectors(tatami_stats::grouped_variances::by_row(sparse_column.get(), cgrouping.data()), rexpected);

    compare_double_vectors_of_vectors(tatami_stats::grouped_variances::by_column(dense_row.get(), rgrouping.data()), cexpected);
    compare_double_vectors_of_vectors(tatami_stats::grouped_variances::by_column(dense_column.get(), rgrouping.data()), cexpected);
    compare_double_vectors_of_vectors(tatami_stats::grouped_variances::by_column(sparse_row.get(), rgrouping.data()), cexpected);
    compare_double_vectors_of_vectors(tatami_stats::grouped_variances::by_column(sparse_column.get(), rgrouping.data()), cexpected);
}

TEST(GroupedVariances, DirtyOutputs) {
    int NR = 56, NC = 179;

    auto simulated = tatami_test::simulate_vector<double>(NR * NC, []{
        tatami_test::SimulateVectorOptions opt;
        opt.density = 0.3;
        opt.seed = 18761283;
        return opt;
    }());

    auto dense_row = std::shared_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, std::move(simulated)));
    auto dense_column = tatami::convert_to_dense(dense_row.get(), false);
    auto sparse_row = tatami::convert_to_compressed_sparse(dense_row.get(), true);
    auto sparse_column = tatami::convert_to_compressed_sparse(dense_row.get(), false);

    int ngroup = 5; 
    std::vector<int> grouping;
    for (int r = 0; r < NR; ++r) {
        grouping.push_back(r % ngroup);
    }
    auto group_sizes = tatami_stats::tabulate_groups<int, int>(grouping.data(), NR);

    std::vector<double> dirty(ngroup * NC, -1);
    std::vector<double*> ptrs;
    ptrs.reserve(ngroup);
    for (int g = 0; g < ngroup; ++g) {
        ptrs.push_back(dirty.data() + g * NC);
    }

    auto ref = tatami_stats::grouped_variances::by_column(dense_row.get(), grouping.data());
    std::vector<double> expected;
    for (const auto& r : ref) {
        expected.insert(expected.end(), r.begin(), r.end());
    }

    tatami_stats::grouped_variances::Options mopt;
    tatami_stats::grouped_variances::apply(false, dense_row.get(), grouping.data(), ngroup, group_sizes.data(), ptrs.data(), mopt);
    compare_double_vectors(expected, dirty);

    std::fill(dirty.begin(), dirty.end(), -1);
    tatami_stats::grouped_variances::apply(false, dense_column.get(), grouping.data(), ngroup, group_sizes.data(), ptrs.data(), mopt);
    compare_double_vectors(expected, dirty);

    std::fill(dirty.begin(), dirty.end(), -1);
    tatami_stats::grouped_variances::apply(false, sparse_row.get(), grouping.data(), ngroup, group_sizes.data(), ptrs.data(), mopt);
    compare_double_vectors(expected, dirty);

    std::fill(dirty.begin(), dirty.end(), -1);
    tatami_stats::grouped_variances::apply(false, sparse_column.get(), grouping.data(), ngroup, group_sizes.data(), ptrs.data(), mopt);
    compare_double_vectors(expected, dirty);
}
