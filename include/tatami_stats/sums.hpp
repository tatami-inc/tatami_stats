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
 * @cond
 */
namespace sum_internal {

template<bool row_, typename Value_, typename Index_, typename Output_>
void dimension_sums(const tatami::Matrix<Value_, Index_>* p, Output_* output, int threads) {
    auto dim = (row_ ? p->nrow() : p->ncol());
    auto otherdim = (row_ ? p->ncol() : p->nrow());
    const bool direct = p->prefer_rows() == row_;

    if (p->sparse()) {
        if (direct) {
            tatami::Options opt;
            opt.sparse_extract_index = false;
            tatami::parallelize([&](size_t, Index_ s, Index_ l) {
                auto ext = tatami::consecutive_extractor<true>(p, row_, s, l, opt);
                std::vector<Value_> vbuffer(otherdim);
                for (Index_ x = 0; x < l; ++x) {
                    auto out = ext->fetch(vbuffer.data(), NULL);
                    output[x + s] = std::accumulate(out.value, out.value + out.number, static_cast<Output_>(0));
                }
            }, dim, threads);

        } else {
            std::fill(output, output + dim, static_cast<Output_>(0));

            tatami::parallelize([&](size_t, Index_ s, Index_ l) {
                auto ext = tatami::consecutive_extractor<true>(p, !row_, 0, otherdim, s, l);
                std::vector<Value_> vbuffer(l);
                std::vector<Index_> ibuffer(l);
                for (Index_ x = 0; x < otherdim; ++x) {
                    auto out = ext->fetch(vbuffer.data(), ibuffer.data());
                    for (Index_ j = 0; j < out.number; ++j) {
                        output[out.index[j]] += out.value[j];
                    }
                }
            }, dim, threads);
        }

    } else {
        if (direct) {
            tatami::parallelize([&](size_t, Index_ s, Index_ l) {
                auto ext = tatami::consecutive_extractor<false>(p, row_, s, l);
                std::vector<Value_> buffer(otherdim);
                for (Index_ x = 0; x < l; ++x) {
                    auto out = ext->fetch(buffer.data());
                    output[x + s] = std::accumulate(out, out + otherdim, static_cast<Output_>(0));
                }
            }, dim, threads);

        } else {
            std::fill(output, output + dim, static_cast<Output_>(0));

            tatami::parallelize([&](size_t, Index_ s, Index_ l) {
                auto ext = tatami::consecutive_extractor<false>(p, !row_, 0, otherdim, s, l);
                std::vector<Value_> buffer(l);
                for (Index_ x = 0; x < otherdim; ++x) {
                    auto out = ext->fetch(buffer.data());
                    for (Index_ j = 0; j < l; ++j) {
                        output[s + j] += out[j];
                    }
                }
            }, dim, threads);
        }
    }

    return;
}

}
/**
 * @endcond
 */

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
template<typename Value_, typename Index_, typename Output_>
void column_sums(const tatami::Matrix<Value_, Index_>* p, Output_* output, int threads = 1) {
    sum_internal::dimension_sums<false>(p, output, threads);
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
template<typename Output_ = double, typename Value_, typename Index_>
std::vector<Output_> column_sums(const tatami::Matrix<Value_, Index_>* p, int threads = 1) {
    std::vector<Output_> output(p->ncol());
    tatami_stats::column_sums(p, output.data(), threads);
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
template<typename Output_ = double, typename Value_, typename Index_>
void row_sums(const tatami::Matrix<Value_, Index_>* p, Output_* output, int threads = 1) {
    sum_internal::dimension_sums<true>(p, output, threads);
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
template<typename Output_ = double, typename Value_, typename Index_>
std::vector<Output_> row_sums(const tatami::Matrix<Value_, Index_>* p, int threads = 1) {
    std::vector<Output_> output(p->nrow());
    tatami_stats::row_sums(p, output.data(), threads);
    return output;
}

}

#endif
