#ifndef TATAMI_STATS__MEDIANS_HPP
#define TATAMI_STATS__MEDIANS_HPP

#include "tatami/tatami.hpp"
#include "utils.hpp"

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
 * @namespace tatami_stats::medians
 */
namespace medians {

/**
 * @brief Median calculation options.
 */
struct Options {
    /**
     * Whether to check for NaNs in the input, and skip them.
     * If false, NaNs are assumed to be absent, and the behavior of the median calculation in the presence of NaNs is undefined.
     */
    bool skip_nan = false;

    /**
     * Number of threads to use when computing medians across a `tatami::Matrix`.
     * See `tatami::parallelize()` for more details on the parallelization mechanism.
     */
    int num_threads = 1;
};

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
 * Directly compute the median from a dense objective vector.
 *
 * @param[in] ptr Pointer to an array of length `num`, containing the values of the objective vector.
 * This may be modified on output.
 * @param num Length of the objective vector, i.e., length of the array at `ptr`.
 * @param skip_nan See `Options::skip_nan` for details.
 *
 * @tparam Output_ Type of the output value.
 * This should be floating-point to store potential averages.
 * @tparam Value_ Type of the input values.
 * @tparam Index_ Type of the row/column indices.
 *
 * @return The median of values in `[ptr, ptr + n)`.
 */
template<typename Output_ = double, typename Value_, typename Index_>
Output_ direct(Value_* ptr, Index_ num, bool skip_nan) {
    ::tatami_stats::internal::nanable_ifelse<Value_>(
        skip_nan,
        [&]() {
            auto lost = internal::translocate_nans(ptr, num);
            ptr += lost;
            num -= lost;
        },
        [&]() {}
    );

    if (num == 0) {
        return std::numeric_limits<Output_>::quiet_NaN();
    }

    size_t halfway = num / 2;
    bool is_even = (num % 2 == 0);

    std::nth_element(ptr, ptr + halfway, ptr + num);
    Output_ medtmp = *(ptr + halfway);
    if (!is_even) {
        return medtmp;
    }

    // 'nth_element()' reorganizes 'ptr' so that everything below 'halfway' is
    // less than or equal to 'ptr[halfway]', while everything above 'halfway'
    // is greater than or equal to 'ptr[halfway]'. Thus, to get the element
    // immediately before 'halfway' in the sort order, we just need to find the
    // maximum from '[0, halfway)'.
    Output_ other = *std::max_element(ptr, ptr + halfway);
    return (medtmp + other)/2;
}

/**
 * Directly compute the median from a sparse objective vector.
 *
 * @param[in] value Pointer to an array of length `num_nonzero`, containing values of the structural non-zeroes.
 * This may be modified on output.
 * @param num_nonzero Number of structural non-zeros in the objective vector.
 * @param num_all Length of the obejctive vector, including the structural zeros,
 * i.e., `num_all - num_nonzero` is the number of zeros.
 * @param skip_nan See `Options::skip_nan` for details.
 *
 * @tparam Output_ Type of the output value.
 * This should be floating-point to store potential averages.
 * @tparam Value_ Type of the input values.
 * @tparam Index_ Type of the row/column indices.
 *
 * @return The median of values in the sparse vector.
 */
template<typename Output_ = double, typename Value_, typename Index_>
Output_ direct(Value_* value, Index_ num_nonzero, Index_ num_all, bool skip_nan) {
    // Fallback to the dense code if there are no structural zeros. This is not
    // just for efficiency as the downstream averaging code assumes that there
    // is at least one structural zero when considering its scenarios.
    if (num_nonzero == num_all) {
        return direct<Output_>(value, num_all, skip_nan);
    }

    ::tatami_stats::internal::nanable_ifelse<Value_>(
        skip_nan,
        [&]() {
            auto lost = internal::translocate_nans(value, num_nonzero);
            value += lost;
            num_nonzero -= lost;
            num_all -= lost;
        },
        [&]() {}
    );

    // Is the number of non-zeros less than the number of zeros?
    // If so, the median must be zero. Note that we calculate it
    // in this way to avoid overflow from 'num_nonzero * 2'.
    if (num_nonzero < num_all - num_nonzero) {
        return 0;
    } 
    
    size_t halfway = num_all / 2;
    bool is_even = (num_all % 2 == 0);

    size_t num_zero = num_all - num_nonzero;
    size_t num_negative = 0;
    for (Index_ i = 0; i < num_nonzero; ++i) {
        num_negative += (value[i] < 0);
    }

    if (!is_even) {
        if (num_negative > halfway) {
            std::nth_element(value, value + halfway, value + num_nonzero);
            return value[halfway];

        } else if (halfway >= num_negative + num_zero) {
            size_t skip_zeros = halfway - num_zero;
            std::nth_element(value, value + skip_zeros, value + num_nonzero);
            return value[skip_zeros];

        } else {
            return 0;
        }
    }

    Output_ tmp = 0;
    if (num_negative > halfway) { // both halves of the median are negative.
        std::nth_element(value, value + halfway, value + num_nonzero);
        tmp = value[halfway] + *(std::max_element(value, value + halfway)); // max_element gets the sorted value at halfway - 1, see explanation for the dense case.

    } else if (num_negative == halfway) { // the upper half is guaranteed to be zero.
        size_t below_halfway = halfway - 1;
        std::nth_element(value, value + below_halfway, value + num_nonzero);
        tmp = value[below_halfway];

    } else if (num_negative < halfway && num_negative + num_zero > halfway) { // both halves are zero, so zero is the median.
        ;

    } else if (num_negative + num_zero == halfway) { // the lower half is guaranteed to be zero.
        size_t skip_zeros = halfway - num_zero;
        std::nth_element(value, value + skip_zeros, value + num_nonzero);
        tmp = value[skip_zeros];

    } else { // both halves of the median are non-negative.
        size_t skip_zeros = halfway - num_zero;
        std::nth_element(value, value + skip_zeros, value + num_nonzero);
        tmp = value[skip_zeros] + *(std::max_element(value, value + skip_zeros)); // max_element gets the sorted value at skip_zeros - 1, see explanation for the dense case.
    }

    return tmp / 2;
}

/**
 * Compute medians for each element of a chosen dimension of a `tatami::Matrix`.
 *
 * @tparam Value_ Type of the matrix value, should be numeric.
 * @tparam Index_ Type of the row/column indices.
 * @tparam Output_ Type of the output value.
 * This should be floating-point to store potential averages.
 *
 * @param row Whether to compute the median for each row.
 * If false, the median is computed for each column instead.
 * @param p Pointer to a `tatami::Matrix`.
 * @param[out] output Pointer to an array of length equal to the number of rows (if `row = true`) or columns (otherwise).
 * On output, this will contain the row/column medians.
 * @param mopt Median calculation options.
 */
template<typename Value_, typename Index_, typename Output_>
void apply(bool row, const tatami::Matrix<Value_, Index_>* p, Output_* output, const medians::Options& mopt) {
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
                output[x + s] = medians::direct<Output_>(vbuffer, range.number, otherdim, mopt.skip_nan);
            }
        }, dim, mopt.num_threads);

    } else {
        tatami::parallelize([&](int, Index_ s, Index_ l) -> void {
            std::vector<Value_> buffer(otherdim);
            auto ext = tatami::consecutive_extractor<false>(p, row, s, l);
            for (Index_ x = 0; x < l; ++x) {
                auto ptr = ext->fetch(buffer.data());
                tatami::copy_n(ptr, otherdim, buffer.data());
                output[x + s] = medians::direct<Output_>(buffer.data(), otherdim, mopt.skip_nan);
            }
        }, dim, mopt.num_threads);
    }
}

/**
 * Wrapper around `apply()` for column medians.
 *
 * @tparam Output_ Type of the output.
 * This should be floating-point to store potential averages.
 * @tparam Value_ Type of the matrix value.
 * @tparam Index_ Type of the row/column indices.
 *
 * @param p Pointer to a `tatami::Matrix`.
 * @param mopt Median calculation options.
 *
 * @return A vector of length equal to the number of columns, containing the column medians.
 */
template<typename Output_ = double, typename Value_, typename Index_>
std::vector<Output_> by_column(const tatami::Matrix<Value_, Index_>* p, const Options& mopt) {
    std::vector<Output_> output(p->ncol());
    apply(false, p, output.data(), mopt);
    return output;
}

/**
 * Overload with default options.
 *
 * @tparam Output_ Type of the output.
 * This should be floating-point to store potential averages.
 * @tparam Value_ Type of the matrix value.
 * @tparam Index_ Type of the row/column indices.
 *
 * @param p Pointer to a `tatami::Matrix`.
 *
 * @return A vector of length equal to the number of columns, containing the column medians.
 */
template<typename Output_ = double, typename Value_, typename Index_>
std::vector<Output_> by_column(const tatami::Matrix<Value_, Index_>* p) {
    return by_column(p, Options());
}

/**
 * Wrapper around `apply()` for row medians.
 *
 * @tparam Output_ Type of the output.
 * This should be floating-point to store potential averages.
 * @tparam Value_ Type of the matrix value.
 * @tparam Index_ Type of the row/column indices.
 *
 * @param p Pointer to a `tatami::Matrix`.
 * @param mopt Median calculation options.
 *
 * @return A vector of length equal to the number of rows, containing the row medians.
 */
template<typename Output_ = double, typename Value_, typename Index_>
std::vector<Output_> by_row(const tatami::Matrix<Value_, Index_>* p, const Options& mopt) {
    std::vector<Output_> output(p->nrow());
    apply(true, p, output.data(), mopt);
    return output;
}

/**
 * Overload with default options.
 *
 * @tparam Output_ Type of the output.
 * This should be floating-point to store potential averages.
 * @tparam Value_ Type of the matrix value.
 * @tparam Index_ Type of the row/column indices.
 *
 * @param p Pointer to a `tatami::Matrix`.
 *
 * @return A vector of length equal to the number of rows, containing the row medians.
 */
template<typename Output_ = double, typename Value_, typename Index_>
std::vector<Output_> by_row(const tatami::Matrix<Value_, Index_>* p) {
    return by_row(p, Options());
}

}

}

#endif
