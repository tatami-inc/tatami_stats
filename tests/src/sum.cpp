#include <gtest/gtest.h>

#include <vector>

#include "tatami_stats/sum.hpp"
#include "tatami_test/tatami_test.hpp"
#include "utils.h"

TEST(Sum, RowSimple) {
    size_t NR = 99, NC = 152;
    auto dump = tatami_test::simulate_vector<double>(NR * NC, []{
        tatami_test::SimulateVectorOptions opt;
        opt.density = 0.1;
        opt.seed = 238947239;
        return opt;
    }());

    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, dump));
    auto dense_column = tatami::convert_to_dense<double, int>(*dense_row, false, {});
    auto sparse_row = tatami::convert_to_compressed_sparse<double, int>(*dense_row, true, {});
    auto sparse_column = tatami::convert_to_compressed_sparse<double, int>(*dense_row, false, {});

    // Naive calculation.
    std::vector<double> ref(NR);
    for (size_t r = 0; r < NR; ++r) {
        for (size_t c = 0; c < NC; ++c) {
            ref[r] += dump[c + r * NC];
        }
    }

    compare_double_vectors(ref, tatami_stats::sum(true, *dense_row, {}));
    compare_double_vectors(ref, tatami_stats::sum(true, *dense_column, {}));
    compare_double_vectors(ref, tatami_stats::sum(true, *sparse_row, {}));
    compare_double_vectors(ref, tatami_stats::sum(true, *sparse_column, {}));

    // Checking same results from parallel code.
    tatami_stats::SumOptions sopt;
    sopt.num_threads = 3;
    compare_double_vectors(ref, tatami_stats::sum(true, *dense_row, sopt));
    compare_double_vectors(ref, tatami_stats::sum(true, *dense_column, sopt));
    compare_double_vectors(ref, tatami_stats::sum(true, *sparse_row, sopt));
    compare_double_vectors(ref, tatami_stats::sum(true, *sparse_column, sopt));

    // Same results with NaN skipping.
    sopt.num_threads = 1;
    sopt.skip_nan = true;
    compare_double_vectors(ref, tatami_stats::sum(true, *dense_row, sopt));
    compare_double_vectors(ref, tatami_stats::sum(true, *dense_column, sopt));
    compare_double_vectors(ref, tatami_stats::sum(true, *sparse_row, sopt));
    compare_double_vectors(ref, tatami_stats::sum(true, *sparse_column, sopt));
}

TEST(Sum, RowSkipNan) {
    size_t NR = 52, NC = 83;
    auto dump = tatami_test::simulate_vector<double>(NR * NC, []{ 
        tatami_test::SimulateVectorOptions opt;
        opt.density = 0.1;
        opt.seed = 992929;
        return opt;
    }());
    for (size_t r = 0; r < NR; ++r) { // Injecting an NaN at the start.
        dump[r * NC] = std::numeric_limits<double>::quiet_NaN();
    }

    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, dump));
    auto dense_column = tatami::convert_to_dense<double, int>(*dense_row, false, {});
    auto sparse_row = tatami::convert_to_compressed_sparse<double, int>(*dense_row, true, {});
    auto sparse_column = tatami::convert_to_compressed_sparse<double, int>(*dense_row, false, {});

    std::vector<double> ref(NR);
    for (size_t r = 0; r < NR; ++r) {
        for (size_t c = 1; c < NC; ++c) { // skipping the first element.
            ref[r] += dump[c + r * NC];
        }
    }

    tatami_stats::SumOptions sopt;
    sopt.skip_nan = true;
    compare_double_vectors(ref, tatami_stats::sum(true, *dense_row, sopt));
    compare_double_vectors(ref, tatami_stats::sum(true, *dense_column, sopt));
    compare_double_vectors(ref, tatami_stats::sum(true, *sparse_row, sopt));
    compare_double_vectors(ref, tatami_stats::sum(true, *sparse_column, sopt));

    sopt.num_threads = 3;
    compare_double_vectors(ref, tatami_stats::sum(true, *dense_row, sopt));
    compare_double_vectors(ref, tatami_stats::sum(true, *dense_column, sopt));
    compare_double_vectors(ref, tatami_stats::sum(true, *sparse_row, sopt));
    compare_double_vectors(ref, tatami_stats::sum(true, *sparse_column, sopt));
}

TEST(Sum, ColumnSimple) {
    size_t NR = 79, NC = 62;
    auto dump = tatami_test::simulate_vector<double>(NR * NC, []{
        tatami_test::SimulateVectorOptions opt;
        opt.density = 0.1;
        opt.seed = 12919010;
        return opt;
    }());

    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, dump));
    auto dense_column = tatami::convert_to_dense<double, int>(*dense_row, false, {});
    auto sparse_row = tatami::convert_to_compressed_sparse<double, int>(*dense_row, true, {});
    auto sparse_column = tatami::convert_to_compressed_sparse<double, int>(*dense_row, false, {});

    std::vector<double> ref(NC);
    for (size_t c = 0; c < NC; ++c) {
        for (size_t r = 0; r < NR; ++r) {
            ref[c] += dump[c + r * NC];
        }
    }

    compare_double_vectors(ref, tatami_stats::sum(false, *dense_row, {}));
    compare_double_vectors(ref, tatami_stats::sum(false, *dense_column, {}));
    compare_double_vectors(ref, tatami_stats::sum(false, *sparse_row, {}));
    compare_double_vectors(ref, tatami_stats::sum(false, *sparse_column, {}));

    // Checking same results from parallel code.
    tatami_stats::SumOptions sopt;
    sopt.num_threads = 3;
    compare_double_vectors(ref, tatami_stats::sum(false, *dense_column, sopt));
    compare_double_vectors(ref, tatami_stats::sum(false, *dense_column, sopt));
    compare_double_vectors(ref, tatami_stats::sum(false, *sparse_column, sopt));
    compare_double_vectors(ref, tatami_stats::sum(false, *sparse_column, sopt));

    // Same results with NaN skipping.
    sopt.num_threads = 1;
    sopt.skip_nan = true;
    compare_double_vectors(ref, tatami_stats::sum(false, *dense_column, sopt));
    compare_double_vectors(ref, tatami_stats::sum(false, *dense_column, sopt));
    compare_double_vectors(ref, tatami_stats::sum(false, *sparse_column, sopt));
    compare_double_vectors(ref, tatami_stats::sum(false, *sparse_column, sopt));
}

TEST(Sum, ColumnSkipNan) {
    size_t NR = 82, NC = 33;
    auto dump = tatami_test::simulate_vector<double>(NR * NC, []{
        tatami_test::SimulateVectorOptions opt;
        opt.density = 0.1;
        opt.seed = 8768372;
        return opt;
    }());
    for (size_t c = 0; c < NC; ++c) { // Injecting an NaN at the start.
        dump[c] = std::numeric_limits<double>::quiet_NaN();
    }

    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, dump));
    auto dense_column = tatami::convert_to_dense<double, int>(*dense_row, false, {});
    auto sparse_row = tatami::convert_to_compressed_sparse<double, int>(*dense_row, true, {});
    auto sparse_column = tatami::convert_to_compressed_sparse<double, int>(*dense_row, false, {});

    std::vector<double> ref(NC), expectedm(NC);
    for (size_t c = 0; c < NC; ++c) {
        for (size_t r = 1; r < NR; ++r) { // skipping the first row.
            ref[c] += dump[c + r * NC];
        }
    }

    tatami_stats::SumOptions sopt;
    sopt.skip_nan = true;
    compare_double_vectors(ref, tatami_stats::sum(false, *dense_row, sopt));
    compare_double_vectors(ref, tatami_stats::sum(false, *dense_column, sopt));
    compare_double_vectors(ref, tatami_stats::sum(false, *sparse_row, sopt));
    compare_double_vectors(ref, tatami_stats::sum(false, *sparse_column, sopt));

    sopt.num_threads = 3;
    compare_double_vectors(ref, tatami_stats::sum(false, *dense_row, sopt));
    compare_double_vectors(ref, tatami_stats::sum(false, *dense_column, sopt));
    compare_double_vectors(ref, tatami_stats::sum(false, *sparse_row, sopt));
    compare_double_vectors(ref, tatami_stats::sum(false, *sparse_column, sopt));
}

TEST(Sum, NoObservations) {
    tatami_stats::SumOptions vopt;
    vopt.skip_nan = true;

    auto dense_row = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(111, 0, std::vector<double>()));

    {
        auto cref = tatami_stats::sum(false, *dense_row, {});
        EXPECT_EQ(cref.size(), 0);

        auto cref2 = tatami_stats::sum(false, *dense_row, vopt);
        EXPECT_EQ(cref2.size(), 0);

        auto rref = tatami_stats::sum(true, *dense_row, {});
        EXPECT_EQ(rref, std::vector<double>(111));

        auto rref2 = tatami_stats::sum(true, *dense_row, vopt);
        EXPECT_EQ(rref2, std::vector<double>(111));
    }

    // Also checking the other orientation.
    auto sparse_column = tatami::convert_to_compressed_sparse<double, int>(*dense_row, false, {});

    {
        auto cref = tatami_stats::sum(false, *sparse_column, {});
        EXPECT_EQ(cref.size(), 0);

        auto cref2 = tatami_stats::sum(false, *sparse_column, vopt);
        EXPECT_EQ(cref2.size(), 0);

        auto rref = tatami_stats::sum(true, *sparse_column, {});
        EXPECT_EQ(rref, std::vector<double>(111));

        auto rref2 = tatami_stats::sum(true, *sparse_column, vopt);
        EXPECT_EQ(rref2, std::vector<double>(111));
    }
}

TEST(Sum, NewType) {
    size_t NR = 198, NC = 52;
    auto dump = tatami_test::simulate_vector<double>(NR * NC, []{
        tatami_test::SimulateVectorOptions opt;
        opt.density = 0.1;
        opt.lower = 1;
        opt.upper = 100;
        opt.seed = 8768372;
        return opt;
    }());
    for (auto& d : dump) { 
        d = std::round(d);
    }

    auto ref = std::unique_ptr<tatami::NumericMatrix>(new tatami::DenseRowMatrix<double, int>(NR, NC, dump));
    auto rexpected = tatami_stats::sum(true, *ref, {});
    auto cexpected = tatami_stats::sum(false, *ref, {});

    std::vector<std::int8_t> ivec(dump.begin(), dump.end());
    auto dense_row = std::make_shared<tatami::DenseRowMatrix<std::int8_t, std::uint8_t> >(NR, NC, std::move(ivec));
    auto dense_column = tatami::convert_to_dense<std::int8_t, std::uint8_t>(*dense_row, false, {});
    auto sparse_row = tatami::convert_to_compressed_sparse<std::int8_t, std::uint8_t>(*dense_row, true, {});
    auto sparse_column = tatami::convert_to_compressed_sparse<std::int8_t, std::uint8_t>(*dense_row, false, {});

    compare_double_vectors(tatami_stats::sum(true, *dense_row, {}), rexpected);
    compare_double_vectors(tatami_stats::sum(true, *dense_column, {}), rexpected);
    compare_double_vectors(tatami_stats::sum(true, *sparse_row, {}), rexpected);
    compare_double_vectors(tatami_stats::sum(true, *sparse_column, {}), rexpected);

    compare_double_vectors(tatami_stats::sum(false, *dense_row, {}), cexpected);
    compare_double_vectors(tatami_stats::sum(false, *dense_column, {}), cexpected);
    compare_double_vectors(tatami_stats::sum(false, *sparse_row, {}), cexpected);
    compare_double_vectors(tatami_stats::sum(false, *sparse_column, {}), cexpected);

    // Skipping NaNs.
    tatami_stats::SumOptions sopt;
    sopt.skip_nan = true;    
    compare_double_vectors(tatami_stats::sum(true, *dense_row, sopt), rexpected);
    compare_double_vectors(tatami_stats::sum(true, *dense_column, sopt), rexpected);
    compare_double_vectors(tatami_stats::sum(true, *sparse_row, sopt), rexpected);
    compare_double_vectors(tatami_stats::sum(true, *sparse_column, sopt), rexpected);

    compare_double_vectors(tatami_stats::sum(false, *dense_row, sopt), cexpected);
    compare_double_vectors(tatami_stats::sum(false, *dense_column, sopt), cexpected);
    compare_double_vectors(tatami_stats::sum(false, *sparse_row, sopt), cexpected);
    compare_double_vectors(tatami_stats::sum(false, *sparse_column, sopt), cexpected);
}
