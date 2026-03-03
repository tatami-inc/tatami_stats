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

TEST_F(ComputeMediansTest, SparseNaN) {
    std::vector<double> vec { 2, 1, std::numeric_limits<double>::quiet_NaN(), 5, 3 };
    int vsize = vec.size();
    EXPECT_EQ(direct_medians(vec.data(), vsize, 8, true), 1);
    EXPECT_EQ(direct_medians(vec.data(), vsize, 9, true), 0.5);
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
