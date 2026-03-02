#include <gtest/gtest.h>

#include <vector>

#include "tatami_stats/medians.hpp"
#include "tatami_test/tatami_test.hpp"

#include "utils.h"

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

TEST_F(ComputeMediansTest, DenseInf) {
    auto inf = std::numeric_limits<double>::infinity();
    std::vector<double> vec { inf, inf, -inf, -inf };
    EXPECT_EQ(direct_medians(vec.data(), 2, false), inf);
    EXPECT_EQ(direct_medians(vec.data() + 2, 2, false), -inf);
    EXPECT_EQ(direct_medians(vec.data(), 3, false), inf);
    EXPECT_TRUE(std::isnan(direct_medians(vec.data(), 4, false)));
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

TEST_F(ComputeMediansTest, SparseInf) {
    auto inf = std::numeric_limits<double>::infinity();
    std::vector<double> vec { inf, inf, inf, -inf, -inf, -inf };
    EXPECT_EQ(direct_medians(vec.data(), 3, 3, false), inf);
    EXPECT_EQ(direct_medians(vec.data(), 3, 4, false), inf);
    EXPECT_EQ(direct_medians(vec.data() + 3, 3, false), -inf);
    EXPECT_EQ(direct_medians(vec.data() + 3, 4, false), -inf);
    EXPECT_EQ(direct_medians(vec.data(), 4, 5, false), inf);
    EXPECT_TRUE(std::isnan(direct_medians(vec.data(), 6, 6, false)));
    EXPECT_EQ(direct_medians(vec.data(), 6, 8, false), 0);
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

/***************************************/

class ComputingDimMediansTest : public ::testing::TestWithParam<std::tuple<std::pair<size_t, size_t>, int> > {};

TEST_P(ComputingDimMediansTest, Basic) {
    auto params = GetParam();
    auto dims = std::get<0>(params);
    auto NR = dims.first, NC = dims.second;
    auto status = std::get<1>(params);

    auto vec = tatami_test::simulate_vector<double>(NR * NC, [&]{
        tatami_test::SimulateVectorOptions opt;

        if (status == -1) {
            opt.lower = -10;
            opt.upper = -1;
        } else if (status == 0) {
            opt.lower = -10;
            opt.upper = 10;
        } else {
            opt.lower = 1;
            opt.upper = 10;
        }

        opt.seed = NR * NC + status;
        return opt;
    }());

    std::mt19937_64 rng(NR * NC + 20 + status);

    {
        auto vecr = vec;
        inject_variable_zeros(NR, NC, vecr, rng);

        auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, std::move(vecr)));
        auto dense_column = tatami::convert_to_dense(dense_row.get(), false);
        auto sparse_row = tatami::convert_to_compressed_sparse(dense_row.get(), true);
        auto sparse_column = tatami::convert_to_compressed_sparse(dense_row.get(), false);

        auto rref = tatami_stats::medians::by_row(dense_row.get());
        EXPECT_EQ(rref.size(), NR);
        EXPECT_EQ(rref, tatami_stats::medians::by_row(dense_column.get()));
        EXPECT_EQ(rref, tatami_stats::medians::by_row(sparse_row.get()));
        EXPECT_EQ(rref, tatami_stats::medians::by_row(sparse_column.get()));

        // Checking that the parallel code is the same.
        tatami_stats::medians::Options mopt;
        mopt.num_threads = 3;
        EXPECT_EQ(rref, tatami_stats::medians::by_row(dense_row.get(), mopt));
        EXPECT_EQ(rref, tatami_stats::medians::by_row(dense_column.get(), mopt));
        EXPECT_EQ(rref, tatami_stats::medians::by_row(sparse_row.get(), mopt));
        EXPECT_EQ(rref, tatami_stats::medians::by_row(sparse_column.get(), mopt));

        // Checking same results from matrices that can yield unsorted indices.
        std::shared_ptr<tatami::NumericMatrix> unsorted_row(new tatami_test::ReversedIndicesWrapper<double, int>(sparse_row));
        EXPECT_EQ(rref, tatami_stats::medians::by_row(unsorted_row.get()));
        std::shared_ptr<tatami::NumericMatrix> unsorted_column(new tatami_test::ReversedIndicesWrapper<double, int>(sparse_column));
        EXPECT_EQ(rref, tatami_stats::medians::by_row(unsorted_column.get()));
    }

    {
        auto vecc = vec;
        inject_variable_zeros(NC, NR, vecc, rng);

        auto dense_column = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseColumnMatrix<double, int>(NR, NC, std::move(vecc)));
        auto dense_row = tatami::convert_to_dense(dense_column.get(), true);
        auto sparse_row = tatami::convert_to_compressed_sparse(dense_row.get(), true);
        auto sparse_column = tatami::convert_to_compressed_sparse(dense_row.get(), false);

        auto cref = tatami_stats::medians::by_column(dense_row.get());
        EXPECT_EQ(cref.size(), NC);
        EXPECT_EQ(cref, tatami_stats::medians::by_column(dense_column.get()));
        EXPECT_EQ(cref, tatami_stats::medians::by_column(sparse_row.get()));
        EXPECT_EQ(cref, tatami_stats::medians::by_column(sparse_column.get()));

        tatami_stats::medians::Options mopt;
        mopt.num_threads = 3;
        EXPECT_EQ(cref, tatami_stats::medians::by_column(dense_row.get(), mopt));
        EXPECT_EQ(cref, tatami_stats::medians::by_column(dense_column.get(), mopt));
        EXPECT_EQ(cref, tatami_stats::medians::by_column(sparse_row.get(), mopt));
        EXPECT_EQ(cref, tatami_stats::medians::by_column(sparse_column.get(), mopt));

        std::shared_ptr<tatami::NumericMatrix> unsorted_row(new tatami_test::ReversedIndicesWrapper<double, int>(sparse_row));
        EXPECT_EQ(cref, tatami_stats::medians::by_column(unsorted_row.get()));
        std::shared_ptr<tatami::NumericMatrix> unsorted_column(new tatami_test::ReversedIndicesWrapper<double, int>(sparse_column));
        EXPECT_EQ(cref, tatami_stats::medians::by_column(unsorted_column.get()));
    }
}

TEST_P(ComputingDimMediansTest, WithNan) {
    auto params = GetParam();
    auto dims = std::get<0>(params);
    auto NR = dims.first, NC = dims.second;
    auto status = std::get<1>(params);

    auto vec = tatami_test::simulate_vector<double>(NR * NC, [&]{
        tatami_test::SimulateVectorOptions opt;

        if (status == -1) {
            opt.lower = -10;
            opt.upper = -1;
        } else if (status == 0) {
            opt.lower = -10;
            opt.upper = 10;
        } else {
            opt.lower = 1;
            opt.upper = 10;
        }

        opt.seed = NR * NC + status;
        return opt;
    }());

    std::mt19937_64 rng(NR * NC + 20 + status);

    {
        auto copy = vec;
        inject_variable_zeros(NR, NC, vec, rng);

        for (size_t r = 0; r < NR; ++r) { // Injecting an NaN randomly into each row.
            const auto failed = rng() % NC;
            copy[failed + r * NC] = std::numeric_limits<double>::quiet_NaN();
        }

        std::vector<double> skip;
        skip.reserve(NR * (NC - 1));
        for (auto x : copy) {
            if (!std::isnan(x)) {
                skip.push_back(x);
            }
        }

        auto dense_row = std::make_unique<tatami::DenseRowMatrix<double, int> >(NR, NC, std::move(copy));
        auto dense_column = tatami::convert_to_dense(dense_row.get(), false);
        auto sparse_row = tatami::convert_to_compressed_sparse(dense_row.get(), true);
        auto sparse_column = tatami::convert_to_compressed_sparse(dense_row.get(), false);

        auto ref = std::make_unique<tatami::DenseRowMatrix<double, int> >(NR, NC - 1, std::move(skip));
        auto rref = tatami_stats::medians::by_row(ref.get());

        tatami_stats::medians::Options mopt;
        mopt.skip_nan = true;
        EXPECT_EQ(rref, tatami_stats::medians::by_row(dense_row.get(), mopt));
        EXPECT_EQ(rref, tatami_stats::medians::by_row(dense_column.get(), mopt));
        EXPECT_EQ(rref, tatami_stats::medians::by_row(sparse_row.get(), mopt));
        EXPECT_EQ(rref, tatami_stats::medians::by_row(sparse_column.get(), mopt));
    }

    {
        auto copy = vec;
        inject_variable_zeros(NC, NR, vec, rng);

        for (size_t c = 0; c < NC; ++c) { // Injecting an NaN randomly into each column.
            const auto failed = rng() % NR;
            copy[c * NR + failed] = std::numeric_limits<double>::quiet_NaN();
        }

        std::vector<double> skip;
        skip.reserve((NR - 1) * NC);
        for (auto x : copy) {
            if (!std::isnan(x)) {
                skip.push_back(x);
            }
        }

        auto dense_column = std::make_unique<tatami::DenseColumnMatrix<double, int> >(NR, NC, std::move(copy));
        auto dense_row = tatami::convert_to_dense(dense_column.get(), true);
        auto sparse_row = tatami::convert_to_compressed_sparse(dense_row.get(), true);
        auto sparse_column = tatami::convert_to_compressed_sparse(dense_row.get(), false);

        auto ref = std::make_unique<tatami::DenseColumnMatrix<double, int> >(NR - 1, NC, std::move(skip));
        auto cref = tatami_stats::medians::by_column(ref.get());

        tatami_stats::medians::Options mopt;
        mopt.skip_nan = true;
        EXPECT_EQ(cref, tatami_stats::medians::by_column(dense_row.get(), mopt));
        EXPECT_EQ(cref, tatami_stats::medians::by_column(dense_column.get(), mopt));
        EXPECT_EQ(cref, tatami_stats::medians::by_column(sparse_row.get(), mopt));
        EXPECT_EQ(cref, tatami_stats::medians::by_column(sparse_column.get(), mopt));
    }
}

INSTANTIATE_TEST_SUITE_P(
    ComputingDimMedians,
    ComputingDimMediansTest,
    ::testing::Combine(
        ::testing::Values(
            std::make_pair<size_t, size_t>(121, 220), // mix of evens and odds here.
            std::make_pair<size_t, size_t>(150, 131),
            std::make_pair<size_t, size_t>(178, 144),
            std::make_pair<size_t, size_t>(201, 179)
        ),
        ::testing::Values(-1, 0, 1)
    )
);

/***************************************/

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

TEST(ComputingDimMedians, Empty) {
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
    auto dump = tatami_test::simulate_vector<double>(NR * NC, []{
        tatami_test::SimulateVectorOptions opt;
        opt.density = 0.5;  // using 0.5 to make things interesting, see above.
        opt.lower = 1;
        opt.upper = 100;
        opt.seed = 293876423;
        return opt;
    }());
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
    auto dump = tatami_test::simulate_vector<double>(NR * NC, []{ 
        tatami_test::SimulateVectorOptions opt;
        opt.density = 0.5;  // using 0.5 to make things interesting, see above.
        opt.lower = 1;
        opt.upper = 10;
        opt.seed = 187181;
        return opt;
    }());

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
