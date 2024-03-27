#ifndef TATAMI_STATS__MEDIANS_HPP
#define TATAMI_STATS__MEDIANS_HPP

#include "tatami/tatami.hpp"

#include <cmath>
#include <vector>
#include <algorithm>
#include <limits>
#include <iostream>

/**
 * @file medians.hpp
 *
 * @brief Compute row and column medians from a `tatami::Matrix`.
 */

namespace tatami_stats {

/**
 * @brief Functions for computing dimension-wise medians.
 * @namespace tatami_stats::median
 */
namespace median {

/**
 * @cond
 */
namespace internal {

template<typename Value_, typename Index_>
Index_ translocate_nans(Value_* ptr, Index_& num) {
    Index_ pos = 0;
    for (Index_ i = 0; i < num; ++i) {
        if (std::isnan(ptr[i])) {
            std::swap(ptr[i], ptr[pos]);
            ++pos;
        }
    }
    return pos;
}

}
/**
 * @endcond
 */

/**
 * Compute the median from a dense vector.
 *
 * @param[in] ptr Pointer to an array of values.
 * This may be modified on output.
 * @param n Length of the array.
 *
 * @tparam skip_nan_ Whether to check for (and skip) NaNs.
 * @tparam Output_ Type of the output value.
 * This should be a floating-point value for possible averaging.
 * @tparam Value_ Type of the input values.
 *
 * @return The median of values in `[ptr, ptr + n)`.
 */
template<bool skip_nan_ = false, typename Output_ = double, typename Value_, typename Index_>
Output_ compute(Value_* ptr, Index_ num) {
    if constexpr(skip_nan_) {
        auto lost = internal::translocate_nans(ptr, num);
        ptr += lost;
        num -= lost;
    }

    if (num == 0) {
        return std::numeric_limits<Output_>::quiet_NaN();
    }

    size_t halfway = num / 2;
    bool is_even = (num % 2 == 0);

    // At some point, I found two nth_element calls to be faster than partial_sort.
    std::nth_element(ptr, ptr + halfway, ptr + num);
    double medtmp = *(ptr + halfway);
    if (is_even) {
        std::nth_element(ptr, ptr + halfway - 1, ptr + num);
        return (medtmp + *(ptr + halfway - 1))/2;
    } else {
        return medtmp;
    }
}

/**
 * Compute the median from a sparse vector.
 *
 * @param[in] value Pointer to an array of structural non-zero values.
 * This may be modified on output.
 * @param num_nonzero Number of non-zero elements, i.e., the length of the array referenced by `ptr`.
 * @param num_all Total number of elements in the set,
 * i.e., `num_all - num_nonzero` is the number of zeros.
 *
 * @tparam skip_nan_ Whether to check for (and skip) NaNs.
 * @tparam Output_ Type of the output value.
 * This should be a floating-point value for possible averaging.
 * @tparam Value_ Type of the input values.
 *
 * @return The median of values in the sparse vector.
 */
template<bool skip_nan_ = false, typename Output_ = double, typename Value_, typename Index_>
Output_ compute(Value_* value, Index_ num_nonzero, Index_ num_all) {
    if constexpr(skip_nan_) {
        auto lost = internal::translocate_nans(value, num_nonzero);
        value += lost;
        num_nonzero -= lost;
        num_all -= lost;
    }

    if (num_nonzero == num_all) {
        return compute<skip_nan_, Output_>(value, num_all);
    }

    if (num_nonzero * 2 < num_all) {
        return 0; // zero is the median if there are too many zeroes.
    } 
    
    size_t halfway = num_all / 2;
    bool is_even = (num_all % 2 == 0);

    auto vend = value + num_nonzero;
    std::sort(value, vend);
    size_t zeropos = std::lower_bound(value, vend, 0) - value;
    size_t nzero = num_all - num_nonzero;

    if (!is_even) {
        if (zeropos > halfway) {
            return value[halfway];
        } else if (halfway >= zeropos + nzero) {
            return value[halfway - nzero];
        } else {
            return 0; // zero is the median.
        }
    }

    double tmp = 0;
    if (zeropos > halfway) {
        tmp = value[halfway] + value[halfway - 1];
    } else if (zeropos == halfway) {
        // guaranteed to be at least 1 zero.
        tmp += value[halfway - 1];
    } else if (zeropos < halfway && zeropos + nzero > halfway) {
        ; // zero is the median.
    } else if (zeropos + nzero == halfway) {
        // guaranteed to be at least 1 zero.
        tmp += value[halfway - nzero];
    } else {
        tmp = value[halfway - nzero] + value[halfway - nzero - 1];
    }
    return tmp / 2;
}

}

/**
 * Compute medians for each element of a chosen dimension of a `tatami::Matrix`.
 *
 * @tparam skip_nan_ Whether to check for (and skip) NaNs.
 * @tparam Value_ Type of the matrix value, should be numeric.
 * @tparam Index_ Type of the row/column indices.
 * @tparam Output_ Type of the output value.
 *
 * @param row Whether to compute medians for the rows.
 * @param p Pointer to a `tatami::Matrix`.
 * @param[out] output Pointer to an array of length equal to the number of rows (if `row = true`) or columns (otherwise).
 * On output, this will contain the row/column medians.
 * @param threads Number of threads to use.
 */
template<bool skip_nan_, typename Value_, typename Index_, typename Output_>
void medians(bool row, const tatami::Matrix<Value_, Index_>* p, Output_* output, int threads) {
    auto dim = (row ? p->nrow() : p->ncol());
    auto otherdim = (row ? p->ncol() : p->nrow());

    if (p->sparse()) {
        tatami::Options opt;
        opt.sparse_extract_index = false;
        opt.sparse_ordered_index = false; // we'll be sorting by value anyway.

        tatami::parallelize([&](int, Index_ s, Index_ l) -> void {
            auto ext = tatami::consecutive_extractor<true>(p, row, s, l, opt);
            std::vector<Value_> buffer(otherdim);
            auto vbuffer = buffer.data();
            for (Index_ x = 0; x < l; ++x) {
                auto range = ext->fetch(vbuffer, NULL);
                tatami::copy_n(range.value, range.number, vbuffer);
                output[x + s] = median::compute<skip_nan_, Output_>(vbuffer, range.number, otherdim);
            }
        }, dim, threads);

    } else {
        tatami::parallelize([&](int, Index_ s, Index_ l) -> void {
            std::vector<Value_> buffer(otherdim);
            auto ext = tatami::consecutive_extractor<false>(p, row, s, l);
            for (Index_ x = 0; x < l; ++x) {
                auto ptr = ext->fetch(buffer.data());
                tatami::copy_n(ptr, otherdim, buffer.data());
                output[x + s] = median::compute<skip_nan_, Output_>(buffer.data(), otherdim);
            }
        }, dim, threads);
    }
}

/**
 * @tparam skip_nan_ Whether to check for (and skip) NaNs.
 * @tparam Value_ Type of the matrix value.
 * @tparam Index_ Type of the row/column indices.
 * @tparam Output_ Type of the output.
 *
 * @param p Shared pointer to a `tatami::Matrix`.
 * @param[out] output Pointer to an array of length equal to the number of columns.
 * On output, this will contain the column medians.
 * @param threads Number of threads to use.
 */
template<bool skip_nan_, typename Value_, typename Index_, typename Output_>
void column_medians(const tatami::Matrix<Value_, Index_>* p, Output_* output, int threads = 1) {
    medians<skip_nan_>(false, p, output, threads);
}

/**
 * @tparam skip_nan_ Whether to check for (and skip) NaNs.
 * @tparam Output_ Type of the output.
 * @tparam Value_ Type of the matrix value.
 * @tparam Index_ Type of the row/column indices.
 *
 * @param p Shared pointer to a `tatami::Matrix`.
 * @param threads Number of threads to use.
 *
 * @return A vector of length equal to the number of columns, containing the column medians.
 */
template<bool skip_nan_ = false, typename Output_ = double, typename Value_, typename Index_>
std::vector<Output_> column_medians(const tatami::Matrix<Value_, Index_>* p, int threads = 1) {
    std::vector<Output_> output(p->ncol());
    tatami_stats::column_medians<skip_nan_>(p, output.data(), threads);
    return output;
}

/**
 * @tparam skip_nan_ Whether to check for (and skip) NaNs.
 * @tparam Value_ Type of the matrix value.
 * @tparam Index_ Type of the row/column indices.
 * @tparam Output_ Type of the output.
 *
 * @param p Shared pointer to a `tatami::Matrix`.
 * @param[out] output Pointer to an array of length equal to the number of rows.
 * On output, this will contain the row medians.
 * @param threads Number of threads to use.
 */
template<bool skip_nan_ = false, typename Value_, typename Index_, typename Output_>
void row_medians(const tatami::Matrix<Value_, Index_>* p, Output_* output, int threads = 1) {
    medians<skip_nan_>(true, p, output, threads);
}

/**
 * @tparam skip_nan_ Whether to check for (and skip) NaNs.
 * @tparam Output_ Type of the output.
 * @tparam Value_ Type of the matrix value.
 * @tparam Index_ Type of the row/column indices.
 *
 * @param p Shared pointer to a `tatami::Matrix`.
 * @param threads Number of threads to use.
 *
 * @return A vector of length equal to the number of rows, containing the row medians.
 */
template<bool skip_nan_ = false, typename Output_ = double, typename Value_, typename Index_>
std::vector<Output_> row_medians(const tatami::Matrix<Value_, Index_>* p, int threads = 1) {
    std::vector<Output_> output(p->nrow());
    tatami_stats::row_medians<skip_nan_>(p, output.data(), threads);
    return output;
}

}

#endif
