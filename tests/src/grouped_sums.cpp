#include <gtest/gtest.h>

#include <vector>

#ifdef CUSTOM_PARALLEL_TEST
// Put this before any tatami imports.
#include "custom_parallel.h"
#endif

#include "tatami_stats/grouped_sums.hpp"
#include "tatami_stats/sums.hpp"
#include "tatami_test/tatami_test.hpp"
#include "utils.h"

TEST(GroupedSums, ByRow) {
    size_t NR = 99, NC = 155;
    auto dense_row = std::shared_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double>(NR, NC, tatami_test::simulate_sparse_vector<double>(NR * NC, 0.5)));
    auto dense_column = tatami::convert_to_dense<false>(dense_row.get());
    auto sparse_row = tatami::convert_to_compressed_sparse<true>(dense_row.get());
    auto sparse_column = tatami::convert_to_compressed_sparse<false>(dense_row.get());

    std::vector<int> cgroups(NC);
    int ngroup = 3; 
    std::vector<std::vector<int> > subsets(ngroup);
    for (size_t c = 0; c < NC; ++c) {
        cgroups[c] = c % ngroup;
        subsets[cgroups[c]].push_back(c);
    }

    auto rref = tatami_stats::row_sums_by_group(dense_row.get(), cgroups.data());
    EXPECT_EQ(rref.size(), NR * ngroup);

    std::vector<double> expected(rref.size());
    for (int g = 0; g < ngroup; ++g) {
        auto sub = tatami::make_DelayedSubset<1>(dense_row, subsets[g]);
        auto per_group = tatami_stats::sums::by_row(sub.get());
        for (size_t r = 0; r < NR; ++r) {
            expected[g + r * ngroup] = per_group[r];
        }
    }
    compare_double_vectors(expected, rref);

    EXPECT_EQ(rref, tatami_stats::row_sums_by_group(dense_column.get(), cgroups.data()));
    EXPECT_EQ(rref, tatami_stats::row_sums_by_group(sparse_row.get(), cgroups.data()));
    EXPECT_EQ(rref, tatami_stats::row_sums_by_group(sparse_column.get(), cgroups.data()));

    // Checking that the parallel code is the same.
    EXPECT_EQ(rref, tatami_stats::row_sums_by_group(dense_row.get(), cgroups.data(), 3));
    EXPECT_EQ(rref, tatami_stats::row_sums_by_group(dense_column.get(), cgroups.data(), 3));
    EXPECT_EQ(rref, tatami_stats::row_sums_by_group(sparse_row.get(), cgroups.data(), 3));
    EXPECT_EQ(rref, tatami_stats::row_sums_by_group(sparse_column.get(), cgroups.data(), 3));

    // Checking same results from matrices that can yield unsorted indices.
    std::shared_ptr<tatami::NumericMatrix> unsorted_row(new tatami_test::UnsortedWrapper<double, int>(sparse_row));
    EXPECT_EQ(rref, tatami_stats::row_sums_by_group(unsorted_row.get(), cgroups.data()));
    std::shared_ptr<tatami::NumericMatrix> unsorted_column(new tatami_test::UnsortedWrapper<double, int>(sparse_column));
    EXPECT_EQ(rref, tatami_stats::row_sums_by_group(unsorted_column.get(), cgroups.data()));
}

TEST(GroupedSums, ByColumn) {
    size_t NR = 56, NC = 179;
    auto dense_row = std::shared_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double>(NR, NC, tatami_test::simulate_sparse_vector<double>(NR * NC, 0.5)));
    auto dense_column = tatami::convert_to_dense<false>(dense_row.get());
    auto sparse_row = tatami::convert_to_compressed_sparse<true>(dense_row.get());
    auto sparse_column = tatami::convert_to_compressed_sparse<false>(dense_row.get());

    std::vector<int> rgroups(NR);
    int ngroup = 7; 
    std::vector<std::vector<int> > subsets(ngroup);
    for (size_t r = 0; r < NR; ++r) {
        rgroups[r] = r % ngroup;
        subsets[rgroups[r]].push_back(r);
    }

    auto cref = tatami_stats::column_sums_by_group(dense_row.get(), rgroups.data());
    EXPECT_EQ(cref.size(), NC * ngroup);

    std::vector<double> expected(cref.size());
    for (int g = 0; g < ngroup; ++g) {
        auto sub = tatami::make_DelayedSubset<0>(dense_row, subsets[g]);
        auto per_group = tatami_stats::sums::by_column(sub.get());
        for (size_t c = 0; c < NC; ++c) {
            expected[g + c * ngroup] = per_group[c];
        }
    }
    EXPECT_EQ(expected, cref);

    EXPECT_EQ(cref, tatami_stats::column_sums_by_group(dense_column.get(), rgroups.data()));
    EXPECT_EQ(cref, tatami_stats::column_sums_by_group(sparse_row.get(), rgroups.data()));
    EXPECT_EQ(cref, tatami_stats::column_sums_by_group(sparse_column.get(), rgroups.data()));

    // Checking that the parallel code is the same.
    EXPECT_EQ(cref, tatami_stats::column_sums_by_group(dense_row.get(), rgroups.data(), 3));
    EXPECT_EQ(cref, tatami_stats::column_sums_by_group(dense_column.get(), rgroups.data(), 3));
    EXPECT_EQ(cref, tatami_stats::column_sums_by_group(sparse_row.get(), rgroups.data(), 3));
    EXPECT_EQ(cref, tatami_stats::column_sums_by_group(sparse_column.get(), rgroups.data(), 3));

    // Checking same results from matrices that can yield unsorted indices.
    std::shared_ptr<tatami::NumericMatrix> unsorted_row(new tatami_test::UnsortedWrapper<double, int>(sparse_row));
    EXPECT_EQ(cref, tatami_stats::column_sums_by_group(unsorted_row.get(), rgroups.data()));
    std::shared_ptr<tatami::NumericMatrix> unsorted_column(new tatami_test::UnsortedWrapper<double, int>(sparse_column));
    EXPECT_EQ(cref, tatami_stats::column_sums_by_group(unsorted_column.get(), rgroups.data()));
}

TEST(GroupedSums, EdgeCases) {
    tatami::DenseRowMatrix<double, int> empty1(0, 10, std::vector<double>());
    tatami::DenseRowMatrix<double, int> empty2(10, 0, std::vector<double>());

    std::vector<int> grouping { 0, 1, 2, 0, 1, 2, 0, 1, 2, 0 };
    EXPECT_TRUE(tatami_stats::row_sums_by_group(&empty1, grouping.data()).empty());
    EXPECT_TRUE(tatami_stats::column_sums_by_group(&empty2, grouping.data()).empty());

    grouping.clear();
    EXPECT_TRUE(tatami_stats::row_sums_by_group(&empty2, grouping.data()).empty());
    EXPECT_TRUE(tatami_stats::column_sums_by_group(&empty1, grouping.data()).empty());
}

TEST(GroupedSums, DirtyOutputs) {
    size_t NR = 56, NC = 179;
    auto dense_row = std::shared_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double>(NR, NC, tatami_test::simulate_sparse_vector<double>(NR * NC, 0.5)));
    auto dense_column = tatami::convert_to_dense<false>(dense_row.get());
    auto sparse_row = tatami::convert_to_compressed_sparse<true>(dense_row.get());
    auto sparse_column = tatami::convert_to_compressed_sparse<false>(dense_row.get());

    int ngroup = 5; 
    std::vector<int> grouping;
    for (size_t r = 0; r < NR; ++r) {
        grouping.push_back(r % ngroup);
    }
    auto ref = tatami_stats::column_sums_by_group(dense_row.get(), grouping.data());

    std::vector<double> dirty(ngroup * NC, -1);
    tatami_stats::column_sums_by_group(dense_row.get(), grouping.data(), ngroup, dirty.data());
    EXPECT_EQ(ref, dirty);

    std::fill(dirty.begin(), dirty.end(), -1);
    tatami_stats::column_sums_by_group(dense_column.get(), grouping.data(), ngroup, dirty.data());
    EXPECT_EQ(ref, dirty);

    std::fill(dirty.begin(), dirty.end(), -1);
    tatami_stats::column_sums_by_group(sparse_row.get(), grouping.data(), ngroup, dirty.data());
    EXPECT_EQ(ref, dirty);

    std::fill(dirty.begin(), dirty.end(), -1);
    tatami_stats::column_sums_by_group(sparse_column.get(), grouping.data(), ngroup, dirty.data());
    EXPECT_EQ(ref, dirty);
}
