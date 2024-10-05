#include <gtest/gtest.h>

#include <vector>

#include "tatami_stats/medians.hpp"
#include "tatami_test/tatami_test.hpp"

class ComputeMediansTest : public ::testing::Test {
protected:
    template<typename Value_>
    static double direct_medians(const Value_* vec, size_t n, bool skip_nan) {
        std::vector<Value_> copy(vec, vec + n);
        return tatami_stats::medians::direct<double>(copy.data(), copy.size(), skip_nan);
    }

    template<typename Value_, typename Index_>
    static double direct_medians(const Value_* vec, Index_ num_nonzero, Index_ num_all, bool skip_nan) {
        std::vector<Value_> copy(vec, vec + num_nonzero);
        return tatami_stats::medians::direct<double>(copy.data(), num_nonzero, num_all, skip_nan);
    }
};

TEST_F(ComputeMediansTest, DenseBasic) {
    std::vector<int> vec { 2, 1, 4, 5, 3 };
    int vsize = vec.size();
    EXPECT_EQ(direct_medians(vec.data(), vsize, false), 3);
    EXPECT_EQ(direct_medians(vec.data() + 1, vsize - 1, false), 3.5);
    EXPECT_EQ(direct_medians(vec.data(), vsize - 1, false), 3);

    EXPECT_TRUE(std::isnan(tatami_stats::medians::direct(static_cast<double*>(NULL), 0, false)));
}

TEST_F(ComputeMediansTest, DenseTies) {
    std::vector<int> vec { 1, 2, 3, 1, 2, 1 };
    int vsize = vec.size();
    EXPECT_EQ(direct_medians(vec.data(), vsize, false), 1.5);
    EXPECT_EQ(direct_medians(vec.data() + 1, vsize - 1, false), 2);

    // Make sure we get identical results when the midpoints are tied floating-point values.
    std::vector<double> frac_vec { 1.0/3, 2.0/10, 3.0/7, 1.0/3, 2.0/10, 1.0/3 };
    int fvsize = frac_vec.size();
    EXPECT_EQ(direct_medians(frac_vec.data(), fvsize, false), 1.0/3);
    EXPECT_EQ(direct_medians(frac_vec.data() + 1, fvsize - 1, false), 1.0/3);
}

TEST_F(ComputeMediansTest, DenseRealistic) {
    for (size_t n = 10; n < 100; n += 10) {
        std::mt19937_64 rng(n);
        std::vector<double> contents;
        std::normal_distribution dist;
        for (size_t i = 0; i < n; ++i) {
            contents.push_back(dist(rng));
        }

        // Even
        {
            auto copy = contents;
            std::sort(copy.begin(), copy.end());
            EXPECT_FLOAT_EQ(direct_medians(contents.data(), contents.size(), false), (copy[copy.size() / 2] + copy[copy.size() / 2 - 1]) / 2);
        }

        // Odd
        {
            auto copy = contents;
            copy.pop_back();
            std::sort(copy.begin(), copy.end());
            EXPECT_EQ(direct_medians(contents.data(), contents.size() - 1, false), copy[copy.size() / 2]);
        }
    }
}

TEST_F(ComputeMediansTest, DenseNaN) {
    std::vector<double> vec { 2, 1, std::numeric_limits<double>::quiet_NaN(), 5, 3 };
    int vsize = vec.size();
    EXPECT_EQ(direct_medians(vec.data(), vsize, true), 2.5);

    vec[0] = std::numeric_limits<double>::quiet_NaN();
    EXPECT_EQ(direct_medians(vec.data(), vsize, true), 3);

    vec[4] = std::numeric_limits<double>::quiet_NaN();
    EXPECT_EQ(direct_medians(vec.data(), vsize, true), 3);

    std::fill(vec.begin(), vec.end(), std::numeric_limits<double>::quiet_NaN());
    EXPECT_TRUE(std::isnan(direct_medians(vec.data(), vsize, true)));
}

TEST_F(ComputeMediansTest, SparseAllPositive) {
    std::vector<int> vec { 2, 1, 4, 5, 3 };
    int vsize = vec.size();
    EXPECT_EQ(direct_medians(vec.data(), vsize, 5, false), 3);
    EXPECT_EQ(direct_medians(vec.data(), vsize, 11, false), 0);
    EXPECT_EQ(direct_medians(vec.data(), vsize, 10, false), 0.5);
    EXPECT_EQ(direct_medians(vec.data(), vsize, 9, false), 1);
    EXPECT_EQ(direct_medians(vec.data(), vsize, 8, false), 1.5);

    EXPECT_TRUE(std::isnan(tatami_stats::medians::direct(static_cast<double*>(NULL), 0, 0, false)));

    // Make sure we get identical results when the midpoints are tied floating-point values.
    std::vector<double> frac_vec { 1.0/3, 2.0/9, 3.0/7, 1.0/3, 2.0/9, 1.0/3 };
    int fvsize = frac_vec.size();
    EXPECT_EQ(direct_medians(frac_vec.data(), fvsize, 6, false), 1.0/3);
    EXPECT_EQ(direct_medians(frac_vec.data(), fvsize, 7, false), 1.0/3);
    EXPECT_EQ(direct_medians(frac_vec.data(), fvsize, 8, false), (1.0/3 + 2.0/9) / 2);
    EXPECT_EQ(direct_medians(frac_vec.data(), fvsize, 9, false), 2.0/9);
    EXPECT_EQ(direct_medians(frac_vec.data(), fvsize, 10, false), 2.0/9);
}

TEST_F(ComputeMediansTest, SparseAllNegative) {
    std::vector<int> vec { -2, -1, -4, -5, -3 };
    int vsize = vec.size();
    EXPECT_EQ(direct_medians(vec.data(), vsize, 5, false), -3);
    EXPECT_EQ(direct_medians(vec.data(), vsize, 11, false), 0);
    EXPECT_EQ(direct_medians(vec.data(), vsize, 10, false), -0.5);
    EXPECT_EQ(direct_medians(vec.data(), vsize, 9, false), -1);
    EXPECT_EQ(direct_medians(vec.data(), vsize, 8, false), -1.5);
}

TEST_F(ComputeMediansTest, SparseMixed) {
    // Mostly positive.
    {
        std::vector<double> vec { 2.5, -1, 4, -5, 3 };
        int vsize = vec.size();
        EXPECT_EQ(direct_medians(vec.data(), vsize, 5, false), 2.5);
        EXPECT_EQ(direct_medians(vec.data(), vsize, 11, false), 0);
        EXPECT_EQ(direct_medians(vec.data(), vsize, 10, false), 0);
        EXPECT_EQ(direct_medians(vec.data(), vsize, 6, false), 1.25);
        EXPECT_EQ(direct_medians(vec.data(), vsize, 7, false), 0);
    }

    // Mostly negative.
    {
        std::vector<double> vec { -2.5, 1, -4, 5, -3 };
        int vsize = vec.size();
        EXPECT_EQ(direct_medians(vec.data(), vsize, 5, false), -2.5);
        EXPECT_EQ(direct_medians(vec.data(), vsize, 11, false), 0);
        EXPECT_EQ(direct_medians(vec.data(), vsize, 10, false), 0);
        EXPECT_EQ(direct_medians(vec.data(), vsize, 6, false), -1.25);
        EXPECT_EQ(direct_medians(vec.data(), vsize, 7, false), 0);
    }

    // Equal numbers of positive and negative.
    {
        std::vector<double> vec { -2.5, 1, -4, 5, -3, 6 };
        int vsize = vec.size();
        EXPECT_FLOAT_EQ(direct_medians(vec.data(), vsize, 6, false), -0.75);
        EXPECT_EQ(direct_medians(vec.data(), vsize, 13, false), 0);
        EXPECT_EQ(direct_medians(vec.data(), vsize, 12, false), 0);
        EXPECT_EQ(direct_medians(vec.data(), vsize, 7, false), 0);
        EXPECT_EQ(direct_medians(vec.data(), vsize, 8, false), 0);
    }
}

TEST_F(ComputeMediansTest, SparseNaN) {
    std::vector<double> vec { 2, 1, std::numeric_limits<double>::quiet_NaN(), 5, 3 };
    int vsize = vec.size();
    EXPECT_EQ(direct_medians(vec.data(), vsize, 8, true), 1);
    EXPECT_EQ(direct_medians(vec.data(), vsize, 9, true), 0.5);
}

TEST_F(ComputeMediansTest, SparseRealistic) {
    for (int n = 10; n < 100; n += 5) {
        std::mt19937_64 rng(n);
        std::vector<double> contents;
        std::normal_distribution dist;
        for (int i = 0; i < n; ++i) {
            contents.push_back(dist(rng));
        }

        {
            auto ref = direct_medians(contents.data(), n, n, false);
            EXPECT_EQ(ref, direct_medians(contents.data(), n, false));
        }

        // Replacing the back with a zero.
        {
            auto ref = direct_medians(contents.data(), n - 1, n, false);
            auto copy = contents;
            copy.back() = 0;
            EXPECT_EQ(ref, direct_medians(copy.data(), n, false));
        }

        // Adding an extra zero.
        {
            auto ref = direct_medians(contents.data(), n, n + 1, false);
            auto copy = contents;
            copy.push_back(0);
            EXPECT_EQ(ref, direct_medians(copy.data(), n + 1, false));
        }

        // Adding two extra zeros.
        {
            auto ref = direct_medians(contents.data(), n, n + 2, false);
            auto copy = contents;
            copy.push_back(0);
            copy.push_back(0);
            EXPECT_EQ(ref, direct_medians(copy.data(), n + 2, false));
        }
    }
}

TEST(ComputingDimMedians, SparseMedians) {
    size_t NR = 111, NC = 222;

    // We use a density of 0.5 so that we some of the median calculations will
    // need to use the structural zeros.  We also put all non-zero values on
    // one side of zero, otherwise the structural zeros will dominate the
    // median; in this case, we choose all-positive values.
    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, tatami_test::simulate_sparse_vector<double>(NR * NC, 0.5, 1, 10)));
    auto dense_column = tatami::convert_to_dense(dense_row.get(), false);
    auto sparse_row = tatami::convert_to_compressed_sparse(dense_row.get(), true);
    auto sparse_column = tatami::convert_to_compressed_sparse(dense_row.get(), false);

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

        auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, dump));
        auto dense_column = tatami::convert_to_dense(dense_row.get(), false);
        auto sparse_row = tatami::convert_to_compressed_sparse(dense_row.get(), true);
        auto sparse_column = tatami::convert_to_compressed_sparse(dense_row.get(), false);

        std::vector<double> skip(dump.begin() + NC, dump.end());
        auto ref = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR - 1, NC, std::move(skip)));
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

        auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, dump));
        auto dense_column = tatami::convert_to_dense(dense_row.get(), false);
        auto sparse_row = tatami::convert_to_compressed_sparse(dense_row.get(), true);
        auto sparse_column = tatami::convert_to_compressed_sparse(dense_row.get(), false);

        std::vector<double> skip;
        skip.reserve(NR * (NC - 1));
        for (size_t r = 0; r < NR; ++r) {
            auto start = dump.begin() + r * NC;
            skip.insert(skip.end(), start + 1, start + NC);
        }
        auto ref = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC - 1, std::move(skip)));
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
    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, std::vector<double>(NR * NC)));
    auto dense_column = tatami::convert_to_dense(dense_row.get(), false);
    auto sparse_row = tatami::convert_to_compressed_sparse(dense_row.get(), true);
    auto sparse_column = tatami::convert_to_compressed_sparse(dense_row.get(), false);

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

    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(order, order, dump));
    auto dense_column = tatami::convert_to_dense(dense_row.get(), false);
    auto sparse_row = tatami::convert_to_compressed_sparse(dense_row.get(), true);
    auto sparse_column = tatami::convert_to_compressed_sparse(dense_row.get(), false);

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

    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(order, order, dump));
    auto dense_column = tatami::convert_to_dense(dense_row.get(), false);
    auto sparse_row = tatami::convert_to_compressed_sparse(dense_row.get(), true);
    auto sparse_column = tatami::convert_to_compressed_sparse(dense_row.get(), false);

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

    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(order, order, dump));
    auto dense_column = tatami::convert_to_dense(dense_row.get(), false);
    auto sparse_row = tatami::convert_to_compressed_sparse(dense_row.get(), true);
    auto sparse_column = tatami::convert_to_compressed_sparse(dense_row.get(), false);

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
    auto dense = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(111, 0, std::vector<double>()));

    auto cref = tatami_stats::medians::by_column(dense.get());
    EXPECT_EQ(cref.size(), 0);

    auto rref = tatami_stats::medians::by_row(dense.get());
    EXPECT_TRUE(rref.size() > 0);
    EXPECT_TRUE(std::isnan(rref.front()));
    EXPECT_TRUE(std::isnan(rref.back()));
}

TEST(ComputingDimMedians, NewType) {
    size_t NR = 198, NC = 52;
    auto dump = tatami_test::simulate_sparse_vector<double>(NR * NC, 0.1, /* lower = */ 1, /* upper = */ 100);
    for (auto& d : dump) { 
        d = std::round(d);
    }
    auto ref = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, dump));
    auto rexpected = tatami_stats::medians::by_row(ref.get());
    auto cexpected = tatami_stats::medians::by_column(ref.get());

    std::vector<int8_t> ivec(dump.begin(), dump.end());
    auto dense_row = std::make_shared<tatami::DenseRowMatrix<int8_t, uint8_t> >(NR, NC, std::move(ivec));
    auto dense_column = tatami::convert_to_dense(dense_row.get(), false);
    auto sparse_row = tatami::convert_to_compressed_sparse(dense_row.get(), true);
    auto sparse_column = tatami::convert_to_compressed_sparse(dense_row.get(), false);

    EXPECT_EQ(tatami_stats::medians::by_row(dense_row.get()), rexpected);
    EXPECT_EQ(tatami_stats::medians::by_row(dense_column.get()), rexpected);
    EXPECT_EQ(tatami_stats::medians::by_row(sparse_row.get()), rexpected);
    EXPECT_EQ(tatami_stats::medians::by_row(sparse_column.get()), rexpected);

    EXPECT_EQ(tatami_stats::medians::by_column(dense_row.get()), cexpected);
    EXPECT_EQ(tatami_stats::medians::by_column(dense_column.get()), cexpected);
    EXPECT_EQ(tatami_stats::medians::by_column(sparse_row.get()), cexpected);
    EXPECT_EQ(tatami_stats::medians::by_column(sparse_column.get()), cexpected);
}

TEST(ComputingDimMedians, DirtyOutput) {
    size_t NR = 99, NC = 152;
    auto dump = tatami_test::simulate_sparse_vector<double>(NR * NC, 0.5, 1, 10); // see comments above about why we use 0.5.
    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, dump));
    auto dense_column = tatami::convert_to_dense(dense_row.get(), false);
    auto sparse_row = tatami::convert_to_compressed_sparse(dense_row.get(), true);
    auto sparse_column = tatami::convert_to_compressed_sparse(dense_row.get(), false);

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
