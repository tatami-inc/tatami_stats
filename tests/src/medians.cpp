#include <gtest/gtest.h>

#include <vector>

#ifdef CUSTOM_PARALLEL_TEST
// Put this before any tatami imports.
#include "custom_parallel.h"
#endif

#include "tatami_stats/medians.hpp"
#include "tatami_test/tatami_test.hpp"

TEST(ComputeMedians, Dense) {
    {
        std::vector<int> vec { 2, 1, 4, 5, 3 };
        int vsize = vec.size();
        EXPECT_EQ(tatami_stats::medians::direct<double>(vec.data(), vsize, false), 3);
        EXPECT_EQ(tatami_stats::medians::direct<double>(vec.data() + 1, vsize - 1, false), 3.5);
    }

    // Now with NaN stripping.
    {
        std::vector<double> vec { 2, 1, std::numeric_limits<double>::quiet_NaN(), 5, 3 };
        int vsize = vec.size();
        EXPECT_EQ(tatami_stats::medians::direct<double>(vec.data(), vsize, true), 2.5);
    }

    EXPECT_TRUE(std::isnan(tatami_stats::medians::direct(static_cast<double*>(NULL), 0, false)));
}

TEST(ComputeMedians, Sparse) {
    {
        std::vector<int> vec { 2, 1, 4, 5, 3 };
        int vsize = vec.size();
        EXPECT_EQ(tatami_stats::medians::direct<double>(vec.data(), vsize, 5, false), 3);
        EXPECT_EQ(tatami_stats::medians::direct<double>(vec.data(), vsize, 11, false), 0);
        EXPECT_EQ(tatami_stats::medians::direct<double>(vec.data(), vsize, 10, false), 0.5);
        EXPECT_EQ(tatami_stats::medians::direct<double>(vec.data(), vsize, 9, false), 1);
        EXPECT_EQ(tatami_stats::medians::direct<double>(vec.data(), vsize, 8, false), 1.5);
    }

    {
        std::vector<int> vec { -2, -1, -4, -5, -3 };
        int vsize = vec.size();
        EXPECT_EQ(tatami_stats::medians::direct<double>(vec.data(), vsize, 5, false), -3);
        EXPECT_EQ(tatami_stats::medians::direct<double>(vec.data(), vsize, 11, false), 0);
        EXPECT_EQ(tatami_stats::medians::direct<double>(vec.data(), vsize, 10, false), -0.5);
        EXPECT_EQ(tatami_stats::medians::direct<double>(vec.data(), vsize, 9, false), -1);
        EXPECT_EQ(tatami_stats::medians::direct<double>(vec.data(), vsize, 8, false), -1.5);
    }

    // Various mixed flavors.
    {
        std::vector<double> vec { 2.5, -1, 4, -5, 3 };
        int vsize = vec.size();
        EXPECT_EQ(tatami_stats::medians::direct<double>(vec.data(), vsize, 5, false), 2.5);
        EXPECT_EQ(tatami_stats::medians::direct<double>(vec.data(), vsize, 11, false), 0);
        EXPECT_EQ(tatami_stats::medians::direct<double>(vec.data(), vsize, 10, false), 0);
        EXPECT_EQ(tatami_stats::medians::direct<double>(vec.data(), vsize, 6, false), 1.25);
        EXPECT_EQ(tatami_stats::medians::direct<double>(vec.data(), vsize, 7, false), 0);
    }

    {
        std::vector<double> vec { -2.5, 1, -4, 5, -3 };
        int vsize = vec.size();
        EXPECT_EQ(tatami_stats::medians::direct(vec.data(), vsize, 5, false), -2.5);
        EXPECT_EQ(tatami_stats::medians::direct(vec.data(), vsize, 11, false), 0);
        EXPECT_EQ(tatami_stats::medians::direct(vec.data(), vsize, 10, false), 0);
        EXPECT_EQ(tatami_stats::medians::direct(vec.data(), vsize, 6, false), -1.25);
        EXPECT_EQ(tatami_stats::medians::direct(vec.data(), vsize, 7, false), 0);
    }

    // Plus missing values.
    {
        std::vector<double> vec { 2, 1, std::numeric_limits<double>::quiet_NaN(), 5, 3 };
        int vsize = vec.size();
        EXPECT_EQ(tatami_stats::medians::direct<double>(vec.data(), vsize, 8, true), 1);
        EXPECT_EQ(tatami_stats::medians::direct<double>(vec.data(), vsize, 9, true), 0.5);
    }

    EXPECT_TRUE(std::isnan(tatami_stats::medians::direct(static_cast<double*>(NULL), 0, 0, false)));
}

TEST(ComputingDimMedians, SparseMedians) {
    size_t NR = 111, NC = 222;

    // We use a density of 0.5 so that we some of the median calculations will
    // need to use the structural zeros.  We also put all non-zero values on
    // one side of zero, otherwise the structural zeros will dominate the
    // median; in this case, we choose all-positive values.
    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double>(NR, NC, tatami_test::simulate_sparse_vector<double>(NR * NC, 0.5, 1, 10)));
    auto dense_column = tatami::convert_to_dense<false>(dense_row.get());
    auto sparse_row = tatami::convert_to_compressed_sparse<true>(dense_row.get());
    auto sparse_column = tatami::convert_to_compressed_sparse<false>(dense_row.get());

    auto rref = tatami_stats::medians::by_row(dense_row.get());
    EXPECT_EQ(rref.size(), NR);
    EXPECT_EQ(rref, tatami_stats::medians::by_row(dense_column.get()));
    EXPECT_EQ(rref, tatami_stats::medians::by_row(sparse_row.get()));
    EXPECT_EQ(rref, tatami_stats::medians::by_row(sparse_column.get()));

    auto cref = tatami_stats::medians::by_column(dense_row.get());
    EXPECT_EQ(cref.size(), NC);
    EXPECT_EQ(cref, tatami_stats::medians::by_column(dense_column.get()));
    EXPECT_EQ(cref, tatami_stats::medians::by_column(sparse_row.get()));
    EXPECT_EQ(cref, tatami_stats::medians::by_column(sparse_column.get()));

    // Checking that the parallel code is the same.
    tatami_stats::medians::Options mopt;
    mopt.num_threads = 3;

    EXPECT_EQ(rref, tatami_stats::medians::by_row(dense_row.get(), mopt));
    EXPECT_EQ(rref, tatami_stats::medians::by_row(dense_column.get(), mopt));
    EXPECT_EQ(rref, tatami_stats::medians::by_row(sparse_row.get(), mopt));
    EXPECT_EQ(rref, tatami_stats::medians::by_row(sparse_column.get(), mopt));

    EXPECT_EQ(cref, tatami_stats::medians::by_column(dense_row.get(), mopt));
    EXPECT_EQ(cref, tatami_stats::medians::by_column(dense_column.get(), mopt));
    EXPECT_EQ(cref, tatami_stats::medians::by_column(sparse_row.get(), mopt));
    EXPECT_EQ(cref, tatami_stats::medians::by_column(sparse_column.get(), mopt));

    // Checking same results from matrices that can yield unsorted indices.
    std::shared_ptr<tatami::NumericMatrix> unsorted_row(new tatami_test::UnsortedWrapper<double, int>(sparse_row));
    EXPECT_EQ(rref, tatami_stats::medians::by_row(unsorted_row.get()));
    EXPECT_EQ(cref, tatami_stats::medians::by_column(unsorted_row.get()));
    std::shared_ptr<tatami::NumericMatrix> unsorted_column(new tatami_test::UnsortedWrapper<double, int>(sparse_column));
    EXPECT_EQ(rref, tatami_stats::medians::by_row(unsorted_column.get()));
    EXPECT_EQ(cref, tatami_stats::medians::by_column(unsorted_column.get()));
}

TEST(ComputingMedians, WithNan) {
    size_t NR = 152, NC = 183;

    {
        auto dump = tatami_test::simulate_sparse_vector<double>(NR * NC, 0.5);
        for (size_t c = 0; c < NC; ++c) { // Injecting an NaN at the start of each column.
            dump[c] = std::numeric_limits<double>::quiet_NaN();
        }

        auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double>(NR, NC, dump));
        auto dense_column = tatami::convert_to_dense<false>(dense_row.get());
        auto sparse_row = tatami::convert_to_compressed_sparse<true>(dense_row.get());
        auto sparse_column = tatami::convert_to_compressed_sparse<false>(dense_row.get());

        std::vector<double> skip(dump.begin() + NC, dump.end());
        auto ref = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double>(NR - 1, NC, std::move(skip)));
        auto cref = tatami_stats::medians::by_column(ref.get());

        tatami_stats::medians::Options mopt;
        mopt.skip_nan = true;
        EXPECT_EQ(cref, tatami_stats::medians::by_column(dense_row.get(), mopt));
        EXPECT_EQ(cref, tatami_stats::medians::by_column(dense_column.get(), mopt));
        EXPECT_EQ(cref, tatami_stats::medians::by_column(sparse_row.get(), mopt));
        EXPECT_EQ(cref, tatami_stats::medians::by_column(sparse_column.get(), mopt));
    }

    {
        auto dump = tatami_test::simulate_sparse_vector<double>(NR * NC, 0.5);
        for (size_t r = 0; r < NR; ++r) { // Injecting an NaN at the start of each row.
            dump[r * NC] = std::numeric_limits<double>::quiet_NaN();
        }

        auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double>(NR, NC, dump));
        auto dense_column = tatami::convert_to_dense<false>(dense_row.get());
        auto sparse_row = tatami::convert_to_compressed_sparse<true>(dense_row.get());
        auto sparse_column = tatami::convert_to_compressed_sparse<false>(dense_row.get());

        std::vector<double> skip;
        skip.reserve(NR * (NC - 1));
        for (size_t r = 0; r < NR; ++r) {
            auto start = dump.begin() + r * NC;
            skip.insert(skip.end(), start + 1, start + NC);
        }
        auto ref = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double>(NR, NC - 1, std::move(skip)));
        auto rref = tatami_stats::medians::by_row(ref.get());

        tatami_stats::medians::Options mopt;
        mopt.skip_nan = true;
        EXPECT_EQ(rref, tatami_stats::medians::by_row(dense_row.get(), mopt));
        EXPECT_EQ(rref, tatami_stats::medians::by_row(dense_column.get(), mopt));
        EXPECT_EQ(rref, tatami_stats::medians::by_row(sparse_row.get(), mopt));
        EXPECT_EQ(rref, tatami_stats::medians::by_row(sparse_column.get(), mopt));
    }
}

TEST(ComputingDimMedians, AllZero) {
    size_t NR = 55, NC = 22;
    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double>(NR, NC, std::vector<double>(NR * NC)));
    auto dense_column = tatami::convert_to_dense<false>(dense_row.get());
    auto sparse_row = tatami::convert_to_compressed_sparse<true>(dense_row.get());
    auto sparse_column = tatami::convert_to_compressed_sparse<false>(dense_row.get());

    auto ref = tatami_stats::medians::by_row(dense_row.get());
    EXPECT_EQ(ref, std::vector<double>(NR));
    EXPECT_EQ(ref, tatami_stats::medians::by_row(dense_column.get()));
    EXPECT_EQ(ref, tatami_stats::medians::by_row(sparse_row.get()));
    EXPECT_EQ(ref, tatami_stats::medians::by_row(sparse_column.get()));

    ref = tatami_stats::medians::by_column(dense_row.get());
    EXPECT_EQ(ref, std::vector<double>(NC));
    EXPECT_EQ(ref, tatami_stats::medians::by_column(dense_column.get()));
    EXPECT_EQ(ref, tatami_stats::medians::by_column(sparse_row.get()));
    EXPECT_EQ(ref, tatami_stats::medians::by_column(sparse_column.get()));
}

/* Lots of additional checks necessary to account for the 
 * many conditional branches in the sparse case. We create
 * a triangular matrix to ensure that we get some good coverage
 * of all possible zero/non-zero combinations.
 */

class MedianTriangularTest : public ::testing::TestWithParam<int> {
protected:
    void triangularize(size_t order, std::vector<double>& values) {
        for (size_t r = 0; r < order; ++r) {
            for (size_t c = r + 1; c < order; ++c) {
                values[r * order + c] = 0; // wiping out the upper triangular.
            }
        }
    }
};

TEST_P(MedianTriangularTest, Positive) {
    size_t order = GetParam();
    auto dump = tatami_test::simulate_dense_vector<double>(order * order, 0.1, 1);
    triangularize(order, dump);

    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double>(order, order, dump));
    auto dense_column = tatami::convert_to_dense<false>(dense_row.get());
    auto sparse_row = tatami::convert_to_compressed_sparse<true>(dense_row.get());
    auto sparse_column = tatami::convert_to_compressed_sparse<false>(dense_row.get());

    auto ref = tatami_stats::medians::by_row(dense_row.get());
    EXPECT_EQ(ref.size(), order);
    EXPECT_EQ(ref, tatami_stats::medians::by_row(dense_column.get()));
    EXPECT_EQ(ref, tatami_stats::medians::by_row(sparse_row.get()));
    EXPECT_EQ(ref, tatami_stats::medians::by_row(sparse_column.get()));

    ref = tatami_stats::medians::by_column(dense_row.get());
    EXPECT_EQ(ref.size(), order);
    EXPECT_EQ(ref, tatami_stats::medians::by_column(dense_column.get()));
    EXPECT_EQ(ref, tatami_stats::medians::by_column(sparse_row.get()));
    EXPECT_EQ(ref, tatami_stats::medians::by_column(sparse_column.get()));
}

TEST_P(MedianTriangularTest, Negative) {
    // Seeing what happens if all non-zeros are less than zero.
    size_t order = GetParam();
    auto dump = tatami_test::simulate_dense_vector<double>(order * order, -2, -0.1);
    triangularize(order, dump);

    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double>(order, order, dump));
    auto dense_column = tatami::convert_to_dense<false>(dense_row.get());
    auto sparse_row = tatami::convert_to_compressed_sparse<true>(dense_row.get());
    auto sparse_column = tatami::convert_to_compressed_sparse<false>(dense_row.get());

    auto ref = tatami_stats::medians::by_row(dense_row.get());
    EXPECT_EQ(ref.size(), order);
    EXPECT_EQ(ref, tatami_stats::medians::by_row(dense_column.get()));
    EXPECT_EQ(ref, tatami_stats::medians::by_row(sparse_row.get()));
    EXPECT_EQ(ref, tatami_stats::medians::by_row(sparse_column.get()));

    ref = tatami_stats::medians::by_column(dense_row.get());
    EXPECT_EQ(ref.size(), order);
    EXPECT_EQ(ref, tatami_stats::medians::by_column(dense_column.get()));
    EXPECT_EQ(ref, tatami_stats::medians::by_column(sparse_row.get()));
    EXPECT_EQ(ref, tatami_stats::medians::by_column(sparse_column.get()));
}

TEST_P(MedianTriangularTest, Mixed) {
    // Mixing up the ratios of non-zeros on both sides of zero.
    size_t order = GetParam();
    auto dump = tatami_test::simulate_dense_vector<double>(order * order, -2, 2);
    triangularize(order, dump);

    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double>(order, order, dump));
    auto dense_column = tatami::convert_to_dense<false>(dense_row.get());
    auto sparse_row = tatami::convert_to_compressed_sparse<true>(dense_row.get());
    auto sparse_column = tatami::convert_to_compressed_sparse<false>(dense_row.get());

    auto ref = tatami_stats::medians::by_row(dense_row.get());
    EXPECT_EQ(ref.size(), order);
    EXPECT_EQ(ref, tatami_stats::medians::by_row(dense_column.get()));
    EXPECT_EQ(ref, tatami_stats::medians::by_row(sparse_row.get()));
    EXPECT_EQ(ref, tatami_stats::medians::by_row(sparse_column.get()));

    ref = tatami_stats::medians::by_column(dense_row.get());
    EXPECT_EQ(ref.size(), order);
    EXPECT_EQ(ref, tatami_stats::medians::by_column(dense_column.get()));
    EXPECT_EQ(ref, tatami_stats::medians::by_column(sparse_row.get()));
    EXPECT_EQ(ref, tatami_stats::medians::by_column(sparse_column.get()));
}

INSTANTIATE_TEST_SUITE_P(
    ComputingDimMedians,
    MedianTriangularTest,
    ::testing::Values(13, 22, 51, 80) // mix of even and odd numbers
);

TEST(ComputingDimMedians, RowMediansNaN) {
    auto dense = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double>(111, 0, std::vector<double>()));

    auto cref = tatami_stats::medians::by_column(dense.get());
    EXPECT_EQ(cref.size(), 0);

    auto rref = tatami_stats::medians::by_row(dense.get());
    EXPECT_TRUE(rref.size() > 0);
    EXPECT_TRUE(std::isnan(rref.front()));
    EXPECT_TRUE(std::isnan(rref.back()));
}

TEST(ComputingDimMedians, DirtyOutput) {
    size_t NR = 99, NC = 152;
    auto dump = tatami_test::simulate_sparse_vector<double>(NR * NC, 0.5, 1, 10); // see comments above about why we use 0.5.
    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double>(NR, NC, dump));
    auto dense_column = tatami::convert_to_dense<false>(dense_row.get());
    auto sparse_row = tatami::convert_to_compressed_sparse<true>(dense_row.get());
    auto sparse_column = tatami::convert_to_compressed_sparse<false>(dense_row.get());

    auto ref = tatami_stats::medians::by_row(dense_row.get());

    tatami_stats::medians::Options mopt;

    // Works when the input vector is a bit dirty.
    std::vector<double> dirty(NR, -1);
    tatami_stats::medians::apply(true, dense_row.get(), dirty.data(), mopt);
    EXPECT_EQ(ref, dirty);

    std::fill(dirty.begin(), dirty.end(), -1);
    tatami_stats::medians::apply(true, dense_column.get(), dirty.data(), mopt);
    EXPECT_EQ(ref, dirty);

    std::fill(dirty.begin(), dirty.end(), -1);
    tatami_stats::medians::apply(true, sparse_row.get(), dirty.data(), mopt);
    EXPECT_EQ(ref, dirty);

    std::fill(dirty.begin(), dirty.end(), -1);
    tatami_stats::medians::apply(true, sparse_column.get(), dirty.data(), mopt);
    EXPECT_EQ(ref, dirty);
}
