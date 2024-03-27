#ifndef TATAMI_STATS_COUNTS_HPP
#define TATAMI_STATS_COUNTS_HPP

#include "tatami/tatami.hpp"

#include <vector>
#include <algorithm>
#include <cmath>
#include <type_traits>

/**
 * @file counts.hpp
 *
 * @brief Compute row and column counts from a `tatami::Matrix`.
 */

namespace tatami_stats {

/**
 * Count the number of values in each dimension element that satisfy the `condition`.
 *
 * @tparam Value_ Type of the matrix value, should be numeric.
 * @tparam Index_ Type of the row/column indices.
 * @tparam Output_ Type of the output value.
 *
 * @param row Whether to count in each row.
 * @param p Pointer to a `tatami::Matrix`.
 * @param[out] output Pointer to an array of length equal to the number of rows (if `row = true`) or columns (otherwise).
 * On output, this will contain the row/column variances.
 * @param threads Number of threads to use.
 * @param condition Function that accepts a `Value_` and returns a boolean.
 * If NaNs might be present in `p`, this should be handled by `condition`.
 */
template<typename Value_, typename Index_, typename Output_, class Condition_>
void counts(bool row, const tatami::Matrix<Value_, Index_>* p, Output_* output, int threads, Condition_ condition) {
    auto dim = (row ? p->nrow() : p->ncol());
    auto otherdim = (row ? p->ncol() : p->nrow());
    std::fill(output, output + dim, 0);

    if (p->prefer_rows() == row) {
        if (p->sparse()) {
            tatami::Options opt;
            opt.sparse_ordered_index = false;
            bool count_zero = condition(0);

            tatami::parallelize([&](int, Index_ start, Index_ len) -> void {
                std::vector<Value_> xbuffer(otherdim);
                std::vector<Index_> ibuffer(otherdim);
                auto ext = tatami::consecutive_extractor<true>(p, row, start, len, opt);

                for (Index_ x = 0; x < len; ++x) {
                    auto range = ext->fetch(xbuffer.data(), ibuffer.data());
                    Output_ target = 0;
                    for (Index_ j = 0; j < range.number; ++j) {
                        target += condition(range.value[j]);
                    }
                    if (count_zero) {
                        target += otherdim - range.number;
                    }
                    output[x + start] = target;
                }
            }, dim, threads);

        } else {
            tatami::parallelize([&](int, Index_ start, Index_ len) -> void {
                std::vector<Value_> xbuffer(otherdim);
                auto ext = tatami::consecutive_extractor<false>(p, row, start, len);

                for (Index_ x = 0; x < len; ++x) {
                    auto ptr = ext->fetch(xbuffer.data());
                    Output_ target = 0;
                    for (Index_ j = 0; j < otherdim; ++j) {
                        target += condition(ptr[j]);
                    }
                    output[x + start] = target;
                }
            }, dim, threads);
        }

    } else {
        std::vector<Output_*> threaded_output_ptrs(threads, output);
        std::vector<std::vector<Output_> > threaded_output(threads - 1);
        for (int t = 1; t < threads; ++t) {
            auto& curout = threaded_output[t - 1];
            curout.resize(dim);
            threaded_output_ptrs[t] = curout.data();
        }

        if (p->sparse()) {
            tatami::Options opt;
            opt.sparse_ordered_index = false;
            bool count_zero = condition(0);

            tatami::parallelize([&](int t, Index_ start, Index_ len) -> void {
                std::vector<Value_> xbuffer(dim);
                std::vector<Index_> ibuffer(dim);
                auto ext = tatami::consecutive_extractor<true>(p, !row, start, len, opt);

                auto curoutput = threaded_output_ptrs[t];
                std::vector<Index_> nonzeros(dim);

                for (Index_ x = 0; x < len; ++x) {
                    auto range = ext->fetch(xbuffer.data(), ibuffer.data());
                    for (Index_ j = 0; j < range.number; ++j) {
                        auto idx = range.index[j];
                        curoutput[idx] += condition(range.value[j]);
                        ++(nonzeros[idx]);
                    }
                }

                if (count_zero) {
                    for (int d = 0; d < dim; ++d) {
                        curoutput[d] += len - nonzeros[d];
                    }
                }
            }, otherdim, threads);

        } else {
            tatami::parallelize([&](int t, Index_ start, Index_ len) -> void {
                std::vector<Value_> xbuffer(dim);
                auto ext = tatami::consecutive_extractor<false>(p, !row, start, len);
                auto curoutput = threaded_output_ptrs[t];

                for (Index_ x = 0; x < len; ++x) {
                    auto ptr = ext->fetch(xbuffer.data());
                    for (Index_ j = 0; j < dim; ++j) {
                        curoutput[j] += condition(ptr[j]);
                    }
                }
            }, otherdim, threads);
        }

        for (int t = 1; t < threads; ++t) {
            auto curoutput = threaded_output_ptrs[t];
            for (Index_ d = 0; d < dim; ++d) {
                output[d] += curoutput[d];
            }
        }
    }
}

/**
 * @tparam Value_ Type of the matrix value, should be summable.
 * @tparam Index_ Type of the row/column indices.
 * @tparam Output_ Type of the output value.
 *
 * @param p Pointer to a `tatami::Matrix`.
 * @param[out] output Pointer to an array of length equal to the number of rows.
 * On output, this will store the number of NaNs in each row.
 * @param threads Number of threads to use.
 */
template<typename Value_, typename Index_, typename Output_>
void row_nan_counts(const tatami::Matrix<Value_, Index_>* p, Output_* output, int threads = 1) {
    counts(true, p, output, threads, [](Value_ x) -> bool { return std::isnan(x); });
}

/**
 * @tparam Output_ Type of the output value.
 * @tparam Value_ Type of the matrix value, should be summable.
 * @tparam Index_ Type of the row/column indices.
 *
 * @param p Pointer to a `tatami::Matrix`.
 * @param threads Number of threads to use.
 *
 * @return A vector of length equal to the number of rows, containing the number of NaNs in each row.
 */
template<typename Output_ = int, typename Value_, typename Index_>
std::vector<Output_> row_nan_counts(const tatami::Matrix<Value_, Index_>* p, int threads = 1) {
    std::vector<Output_> output(p->nrow());
    row_nan_counts(p, output.data(), threads);
    return output;
}

/**
 * @tparam Value_ Type of the matrix value, should be summable.
 * @tparam Index_ Type of the row/column indices.
 * @tparam Output_ Type of the output value.
 *
 * @param p Pointer to a `tatami::Matrix`.
 * @param[out] output Pointer to an array of length equal to the number of columns.
 * On output, this will store the number of NaNs in each column.
 * @param threads Number of threads to use.
 */
template<typename Value_, typename Index_, typename Output_>
void column_nan_counts(const tatami::Matrix<Value_, Index_>* p, Output_* output, int threads = 1) {
    counts(false, p, output, threads, [](Value_ x) -> bool { return std::isnan(x); });
}

/**
 * @tparam Output_ Type of the output value.
 * @tparam Value_ Type of the matrix value, should be summable.
 * @tparam Index_ Type of the row/column indices.
 *
 * @param p Pointer to a `tatami::Matrix`.
 * @param threads Number of threads to use.
 *
 * @return A vector of length equal to the number of columns, containing the number of NaNs in each column.
 */
template<typename Output_ = int, typename Value_, typename Index_>
std::vector<Output_> column_nan_counts(const tatami::Matrix<Value_, Index_>* p, int threads = 1) {
    std::vector<Output_> output(p->ncol());
    column_nan_counts(p, output.data(), threads);
    return output;
}

/**
 * @tparam Value_ Type of the matrix value, should be summable.
 * @tparam Index_ Type of the row/column indices.
 * @tparam Output_ Type of the output value.
 *
 * @param p Pointer to a `tatami::Matrix`.
 * @param[out] output Pointer to an array of length equal to the number of rows.
 * On output, this will store the number of zeros in each row.
 * @param threads Number of threads to use.
 */
template<typename Value_, typename Index_, typename Output_>
void row_zero_counts(const tatami::Matrix<Value_, Index_>* p, Output_* output, int threads = 1) {
    counts(true, p, output, threads, [](Value_ x) -> bool { return x == 0; });
}

/**
 * @tparam Output_ Type of the output value.
 * @tparam Value_ Type of the matrix value, should be summable.
 * @tparam Index_ Type of the row/column indices.
 *
 * @param p Pointer to a `tatami::Matrix`.
 * @param threads Number of threads to use.
 *
 * @return A vector of length equal to the number of rows, containing the number of zeros in each row.
 */
template<typename Output_ = int, typename Value_, typename Index_>
std::vector<Output_> row_zero_counts(const tatami::Matrix<Value_, Index_>* p, int threads = 1) {
    std::vector<Output_> output(p->nrow());
    row_zero_counts(p, output.data(), threads);
    return output;
}

/**
 * @tparam Value_ Type of the matrix value, should be summable.
 * @tparam Index_ Type of the row/column indices.
 * @tparam Output_ Type of the output value.
 *
 * @param p Pointer to a `tatami::Matrix`.
 * @param[out] output Pointer to an array of length equal to the number of columns.
 * On output, this will store the number of zeros in each column.
 * @param threads Number of threads to use.
 */
template<typename Value_, typename Index_, typename Output_>
void column_zero_counts(const tatami::Matrix<Value_, Index_>* p, Output_* output, int threads = 1) {
    counts(false, p, output, threads, [](Value_ x) -> bool { return x == 0; });
}

/**
 * @tparam Output_ Type of the output value.
 * @tparam Value_ Type of the matrix value, should be summable.
 * @tparam Index_ Type of the row/column indices.
 *
 * @param p Pointer to a `tatami::Matrix`.
 * @param threads Number of threads to use.
 *
 * @return A vector of length equal to the number of columns, containing the number of zeros in each column.
 */
template<typename Output_ = int, typename Value_, typename Index_>
std::vector<Output_> column_zero_counts(const tatami::Matrix<Value_, Index_>* p, int threads = 1) {
    std::vector<Output_> output(p->ncol());
    column_zero_counts(p, output.data(), threads);
    return output;
}

}

#endif
