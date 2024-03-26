#ifndef TATAMI_STATS__SUMS_HPP
#define TATAMI_STATS__SUMS_HPP

#include "tatami/tatami.hpp"

#include <vector>
#include <numeric>
#include <algorithm>

/**
 * @file sums.hpp
 *
 * @brief Compute row and column sums from a `tatami::Matrix`.
 */

namespace tatami_stats {

/**
 * @brief Functions for computing dimension-wise sums.
 * @namespace tatami_stats::sum
 */
namespace sum {

/**
 * Sum an array of values using naive accumulation.
 * This is best used with a sufficiently high-precision `Output_`, hence the default of `double`.
 *
 * @tparam skip_nan_ Whether to check for (and skip) NaNs.
 * @tparam Output_ Type of the output data.
 * @tparam Value_ Type of the input data.
 * @tparam Index_ Type of the row/column index.
 *
 * @param[in] ptr Pointer to an array of values of length `num`.
 * @param num Size of the array.
 * @return The sum.
 */
template<bool skip_nan_  = false, typename Output_ = double, typename Value_, typename Index_>
Output_ compute(const Value_* ptr, Index_ num) {
    if constexpr(skip_nan_) {
        Output_ sum = 0;
        for (Index_ i = 0; i < num; ++i) {
            auto val = ptr[i];
            if (std::isnan(val)) {
                continue;
            }
            sum += val;
        }
        return sum;
    } else {
        return std::accumulate(ptr, ptr + num, static_cast<Output_>(0));
    }
}

/**
 * @brief Running sums from dense data.
 *
 * This considers a scenario with a set of equilength "target" vectors [V1, V2, V3, ..., Vn],
 * but data are only available for "observed" vectors [P1, P2, P3, ..., Pm],
 * where Pi[j] contains the i-th element of target vector Vj.
 * The idea is to repeatedly call `add()` for `ptr` corresponding to observed vectors from 0 to m - 1,
 * and then finally call `finish()` to obtain the sum for each target vector.
 *
 * This class uses naive accumulation to obtain the sum for each target vector.
 * Callers should use a sufficiently high-precision `Output_` such as `double` to mitigate round-off errors.
 *
 * @tparam skip_nan_ Whether to check for (and skip) NaNs.
 * @tparam Output_ Type of the output data.
 * @tparam Index_ Type of the row/column indices.
 */
template<bool skip_nan_ = false, typename Output_ = double, typename Index_ = int>
struct RunningDense {
    /**
     * @param num Number of target vectors, i.e., n.
     * @param[out] sum Pointer to an output array of length `num`.
     * This should be zeroed on input, and will store the running sums after each `add()`.
     */
    RunningDense(Index_ num, Output_* sum) : num(num), sum(sum) {}

    /**
     * Add the next observed vector to the running sums.
     * @tparam Value_ Type of the input data.
     * @param[in] ptr Pointer to an array of values of length `num`, corresponding to an observed vector.
     */
    template<typename Value_>
    void add(const Value_* ptr) {
        for (Index_ i = 0; i < num; ++i) {
            auto val = ptr[i];
            if constexpr(skip_nan_) {
                if (std::isnan(val)) {
                    continue;
                }
            }
            sum[i] += ptr[i];
        }
    }

private:
    Index_ num;
    Output_* sum;
    std::vector<Output_> error;
};

/**
 * @brief Running sums from sparse data.
 *
 * Compute running sums from sparse data. 
 * This is the counterpart to `RunningDense`, but for sparse observed vectors.
 *
 * @tparam skip_nan_ Whether to check for (and skip) NaNs.
 * @tparam Output_ Type of the output data.
 * @tparam Index_ Type of the row/column indices.
 */
template<bool skip_nan_ = false, typename Output_ = double, typename Index_ = int>
struct RunningSparse {
    /**
     * @param num Number of target vectors.
     * @param[out] sum Pointer to an output array of length `num`.
     * This should be zeroed on input, and will store the running sums after each `add()`.
     * @param subtract Offset to subtract from each element of `index` before using it to index into `mean` and friends.
     * Only relevant if `mean` and friends hold statistics for a contiguous subset of target vectors,
     * e.g., during task allocation for parallelization.
     */
    RunningSparse(Index_ num, Output_* sum, Index_ subtract = 0) : num(num), sum(sum), subtract(subtract) {}

    /**
     * Add the next observed vector to the running sums.
     * @tparam Value_ Type of the input value.
     * @param[in] value Value of structural non-zero elements.
     * @param[in] index Index of structural non-zero elements.
     * This does not have to be sorted.
     * @param number Number of non-zero elements in `value` and `index`.
     */
    template<typename Value_>
    void add(const Value_* value, const Index_* index, Index_ number) {
        for (Index_ i = 0; i < number; ++i) {
            auto val = value[i];
            if constexpr(skip_nan_) {
                if (std::isnan(val)) {
                    continue;
                }
            }
            sum[index[i] - subtract] += val;
        }
    }

private:
    Index_ num;
    Output_* sum;
    Index_ subtract;
};

}

/**
 * Compute sums for each element of a chosen dimension of a `tatami::Matrix`.
 * This is done using `compute()`, `RunningDense` or `RunningSparse`, 
 * depending on the requested dimension in `row` and the preferred dimension for access in `p`.
 * Note that all sums are obtained using naive accumulation,
 * so it is best to use a sufficiently high-precision `Output_` to mitigate round-off errors.
 *
 * @tparam skip_nan_ Whether to check for (and skip) NaNs.
 * @tparam Value_ Type of the matrix value, should be numeric.
 * @tparam Index_ Type of the row/column indices.
 * @tparam Output_ Type of the output value.
 *
 * @param row Whether to compute variances for the rows.
 * @param p Pointer to a `tatami::Matrix`.
 * @param[out] output Pointer to an array of length equal to the number of rows (if `row = true`) or columns (otherwise).
 * On output, this will contain the row/column variances.
 * @param threads Number of threads to use.
 */
template<bool skip_nan_, typename Value_, typename Index_, typename Output_>
void sums(bool row, const tatami::Matrix<Value_, Index_>* p, Output_* output, int threads) {
    auto dim = (row ? p->nrow() : p->ncol());
    auto otherdim = (row ? p->ncol() : p->nrow());
    const bool direct = p->prefer_rows() == row;

    if (p->sparse()) {
        if (direct) {
            tatami::Options opt;
            opt.sparse_extract_index = false;

            tatami::parallelize([&](size_t, Index_ s, Index_ l) {
                auto ext = tatami::consecutive_extractor<true>(p, row, s, l, opt);
                std::vector<Value_> vbuffer(otherdim);
                for (Index_ x = 0; x < l; ++x) {
                    auto out = ext->fetch(vbuffer.data(), NULL);
                    output[x + s] = sum::compute<skip_nan_, Output_>(out.value, out.number);
                }
            }, dim, threads);

        } else {
            tatami::Options opt;
            opt.sparse_ordered_index = false;
            std::fill(output, output + dim, static_cast<Output_>(0));

            tatami::parallelize([&](size_t, Index_ s, Index_ l) {
                auto ext = tatami::consecutive_extractor<true>(p, !row, 0, otherdim, s, l, opt);
                std::vector<Value_> vbuffer(l);
                std::vector<Index_> ibuffer(l);
                sum::RunningSparse<skip_nan_, Output_, Index_> runner(l, output + s, s);

                for (Index_ x = 0; x < otherdim; ++x) {
                    auto out = ext->fetch(vbuffer.data(), ibuffer.data());
                    runner.add(out.value, out.index, out.number);
                }
            }, dim, threads);
        }

    } else {
        if (direct) {
            tatami::parallelize([&](size_t, Index_ s, Index_ l) {
                auto ext = tatami::consecutive_extractor<false>(p, row, s, l);
                std::vector<Value_> buffer(otherdim);
                for (Index_ x = 0; x < l; ++x) {
                    auto out = ext->fetch(buffer.data());
                    output[x + s] = sum::compute<skip_nan_, Output_>(out, otherdim);
                }
            }, dim, threads);

        } else {
            std::fill(output, output + dim, static_cast<Output_>(0));

            tatami::parallelize([&](size_t, Index_ s, Index_ l) {
                auto ext = tatami::consecutive_extractor<false>(p, !row, 0, otherdim, s, l);
                std::vector<Value_> buffer(l);
                sum::RunningDense<skip_nan_, Output_, Index_> runner(l, output + s);

                for (Index_ x = 0; x < otherdim; ++x) {
                    auto out = ext->fetch(buffer.data());
                    runner.add(out);
                }
            }, dim, threads);
        }
    }

    return;
}

/**
 * @tparam Value_ Type of the matrix value, should be summable.
 * @tparam Index_ Type of the row/column indices.
 * @tparam Output_ Type of the output value.
 *
 * @param p Pointer to a `tatami::Matrix`.
 * @param[out] output Pointer to an array of length equal to the number of columns.
 * On output, this will store the sum of values for each column.
 * @param threads Number of threads to use.
 */
template<bool skip_nan_ = false, typename Value_, typename Index_, typename Output_>
void column_sums(const tatami::Matrix<Value_, Index_>* p, Output_* output, int threads = 1) {
    sums<skip_nan_>(false, p, output, threads);
    return;
}

/**
 * @tparam Output_ Type of the output value.
 * @tparam Value_ Type of the matrix value, should be summable.
 * @tparam Index_ Type of the row/column indices.
 *
 * @param p Pointer to a `tatami::Matrix`.
 * @param threads Number of threads to use.
 *
 * @return A vector of length equal to the number of columns, containing the column sums.
 */
template<bool skip_nan_ = false, typename Output_ = double, typename Value_, typename Index_>
std::vector<Output_> column_sums(const tatami::Matrix<Value_, Index_>* p, int threads = 1) {
    std::vector<Output_> output(p->ncol());
    tatami_stats::column_sums<skip_nan_>(p, output.data(), threads);
    return output;
}

/**
 * @tparam Output_ Type of the output value.
 * @tparam Value_ Type of the matrix value, should be summable.
 * @tparam Index_ Type of the row/column indices.
 *
 * @param p Pointer to a `tatami::Matrix`.
 * @param[out] output Pointer to an array of length equal to the number of rows.
 * On output, this will contain the row sums.
 * @param threads Number of threads to use.
 */
template<bool skip_nan_ = false, typename Output_ = double, typename Value_, typename Index_>
void row_sums(const tatami::Matrix<Value_, Index_>* p, Output_* output, int threads = 1) {
    sums<skip_nan_>(true, p, output, threads);
    return;
}

/**
 * @tparam Output_ Type of the output value.
 * @tparam Value_ Type of the matrix value, should be summable.
 * @tparam Index_ Type of the row/column indices.
 *
 * @param p Pointer to a `tatami::Matrix`.
 * @param threads Number of threads to use.
 *
 * @return A vector of length equal to the number of rows, containing the row sums.
 */
template<bool skip_nan_ = false, typename Output_ = double, typename Value_, typename Index_>
std::vector<Output_> row_sums(const tatami::Matrix<Value_, Index_>* p, int threads = 1) {
    std::vector<Output_> output(p->nrow());
    tatami_stats::row_sums<skip_nan_>(p, output.data(), threads);
    return output;
}

}

#endif
