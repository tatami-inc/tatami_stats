#include <gtest/gtest.h>

#include <vector>

#include "tatami_stats/count.hpp"
#include "tatami_test/tatami_test.hpp"

template<typename Value_, typename Index_, class Condition_>
std::vector<int> apply(const bool row, const tatami::Matrix<Value_, Index_>& mat, Condition_ condition, const tatami_stats::count::Options& opt) {
    return tatami_stats::count::apply<int>(row, mat, std::move(condition), opt);
}

TEST(Count, ByRow) {
    size_t NR = 99, NC = 152;
    auto dump = tatami_test::simulate_vector<double>(NR * NC, []{
        tatami_test::SimulateVectorOptions opt;
        opt.density = 0.1;
        opt.seed = 324987;
        return opt;
    }());

    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, dump));
    auto dense_column = tatami::convert_to_dense<double, int>(*dense_row, false, {});
    auto sparse_row = tatami::convert_to_compressed_sparse<double, int>(*dense_row, true, {});
    auto sparse_column = tatami::convert_to_compressed_sparse<double, int>(*dense_row, false, {});

    auto cond = [](double val) -> bool { return val > 0; };
    std::vector<int> ref(NR);
    for (size_t r = 0; r < NR; ++r) {
        for (size_t c = 0; c < NC; ++c) {
            ref[r] += cond(dump[c + r * NC]);
        }
    }

    EXPECT_EQ(ref, apply(true, *dense_row, cond, {}));
    EXPECT_EQ(ref, apply(true, *dense_column, cond, {}));
    EXPECT_EQ(ref, apply(true, *sparse_row, cond, {}));
    EXPECT_EQ(ref, apply(true, *sparse_column, cond, {}));

    // Checking same results from parallel code.
    tatami_stats::count::Options nopt;
    nopt.num_threads = 3;
    EXPECT_EQ(ref, apply(true, *dense_row, cond, nopt));
    EXPECT_EQ(ref, apply(true, *dense_column, cond, nopt));
    EXPECT_EQ(ref, apply(true, *sparse_row, cond, nopt));
    EXPECT_EQ(ref, apply(true, *sparse_column, cond, nopt));

    // Checking same results from matrices that can yield unsorted indices.
    std::shared_ptr<tatami::NumericMatrix> unsorted_row(new tatami_test::ReversedIndicesWrapper<double, int>(sparse_row));
    EXPECT_EQ(ref, apply(true, *unsorted_row, cond, {}));
    std::shared_ptr<tatami::NumericMatrix> unsorted_column(new tatami_test::ReversedIndicesWrapper<double, int>(sparse_column));
    EXPECT_EQ(ref, apply(true, *unsorted_column, cond, {}));
}

TEST(Count, ByColumn) {
    size_t NR = 79, NC = 62;
    auto dump = tatami_test::simulate_vector<double>(NR * NC, []{
        tatami_test::SimulateVectorOptions opt;
        opt.density = 0.1;
        opt.seed = 28374928;
        return opt;
    }());

    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, dump));
    auto dense_column = tatami::convert_to_dense<double, int>(*dense_row, false, {});
    auto sparse_row = tatami::convert_to_compressed_sparse<double, int>(*dense_row, true, {});
    auto sparse_column = tatami::convert_to_compressed_sparse<double, int>(*dense_row, false, {});

    auto cond = [](double val) -> bool { return val > 0; };
    std::vector<int> ref(NC);
    for (size_t c = 0; c < NC; ++c) {
        for (size_t r = 0; r < NR; ++r) {
            ref[c] += cond(dump[c + r * NC]);
        }
    }

    EXPECT_EQ(ref, apply(false, *dense_row, cond, {}));
    EXPECT_EQ(ref, apply(false, *dense_column, cond, {}));
    EXPECT_EQ(ref, apply(false, *sparse_row, cond, {}));
    EXPECT_EQ(ref, apply(false, *sparse_column, cond, {}));

    // Checking same results from parallel code.
    tatami_stats::count::Options nopt;
    nopt.num_threads = 3;
    EXPECT_EQ(ref, apply(false, *dense_column, cond, nopt));
    EXPECT_EQ(ref, apply(false, *dense_column, cond, nopt));
    EXPECT_EQ(ref, apply(false, *sparse_column, cond, nopt));
    EXPECT_EQ(ref, apply(false, *sparse_column, cond, nopt));

    // Checking same results from matrices that can yield unsorted indices.
    std::shared_ptr<tatami::NumericMatrix> unsorted_row(new tatami_test::ReversedIndicesWrapper<double, int>(sparse_row));
    EXPECT_EQ(ref, apply(false, *unsorted_row, cond, {}));
    std::shared_ptr<tatami::NumericMatrix> unsorted_column(new tatami_test::ReversedIndicesWrapper<double, int>(sparse_column));
    EXPECT_EQ(ref, apply(false, *unsorted_column, cond, {}));
}

TEST(Count, EmptyCounts) {
    size_t NR = 0, NC = 10;
    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, std::vector<double>()));
    auto dense_column = tatami::convert_to_dense<double, int>(*dense_row, false, {});
    auto sparse_row = tatami::convert_to_compressed_sparse<double, int>(*dense_row, true, {});
    auto sparse_column = tatami::convert_to_compressed_sparse<double, int>(*dense_row, false, {});

    auto cond = [](double val) -> bool { return val > 0; };
    tatami_stats::count::Options nopt;
    nopt.num_threads = 3;

    std::vector<int> empty_c(NC);
    EXPECT_EQ(empty_c, apply(false, *dense_row, cond, {}));
    EXPECT_EQ(empty_c, apply(false, *dense_column, cond, {}));
    EXPECT_EQ(empty_c, apply(false, *sparse_row, cond, {}));
    EXPECT_EQ(empty_c, apply(false, *sparse_column, cond, {}));

    EXPECT_EQ(empty_c, apply(false, *dense_row, cond, nopt));
    EXPECT_EQ(empty_c, apply(false, *dense_column, cond, nopt));
    EXPECT_EQ(empty_c, apply(false, *sparse_row, cond, nopt));
    EXPECT_EQ(empty_c, apply(false, *sparse_column, cond, nopt));

    std::vector<int> empty_r(NR);
    EXPECT_EQ(empty_r, apply(true, *dense_row, cond, {}));
    EXPECT_EQ(empty_r, apply(true, *dense_column, cond, {}));
    EXPECT_EQ(empty_r, apply(true, *sparse_row, cond, {}));
    EXPECT_EQ(empty_r, apply(true, *sparse_column, cond, {}));

    EXPECT_EQ(empty_r, apply(true, *dense_row, cond, nopt));
    EXPECT_EQ(empty_r, apply(true, *dense_column, cond, nopt));
    EXPECT_EQ(empty_r, apply(true, *sparse_row, cond, nopt));
    EXPECT_EQ(empty_r, apply(true, *sparse_column, cond, nopt));
}
