#ifndef TATAMI_STATS_MEDIANS_HPP
#define TATAMI_STATS_MEDIANS_HPP

#include "utils.hpp"

#include <cmath>
#include <vector>
#include <algorithm>
#include <limits>

#include "tatami/tatami.hpp"
#include "sanisizer/sanisizer.hpp"
#include "quickstats/quickstats.hpp"

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
template<typename Output_ = double, typename Value_, typename Index_>
Output_ direct(Value_* ptr, Index_ num, bool skip_nan) {
    ::tatami_stats::internal::nanable_ifelse<Value_>(
        skip_nan,
        [&]() -> void {
            auto lost = shift_nans(ptr, num);
            ptr += lost;
            num -= lost;
        },
        []() -> void {}
    );

    return quickstats::median<Output_>(num, ptr);
}

template<typename Output_ = double, typename Value_, typename Index_>
Output_ direct(Value_* value, Index_ num_nonzero, Index_ num_all, bool skip_nan) {
    ::tatami_stats::internal::nanable_ifelse<Value_>(
        skip_nan,
        [&]() -> void {
            auto lost = shift_nans(value, num_nonzero);
            value += lost;
            num_nonzero -= lost;
            num_all -= lost;
        },
        []() -> void {}
    );

    return quickstats::median<Output_>(num_all, num_nonzero, value);
}
/**
 * @endcond
 */

/**
 * Compute medians for each element of a chosen dimension of a `tatami::Matrix`.
 *
 * @tparam Value_ Numeric type of the input values.
 * @tparam Index_ Integer type of the row/column indices.
 * @tparam Output_ Floating-point type of the output value.
 * This should be capable of storing NaNs.
 *
 * @param row Whether to compute the median for each row.
 * If false, the median is computed for each column instead.
 * @param mat Instance of a `tatami::Matrix`.
 * @param[out] output Pointer to an array of length equal to the number of rows (if `row = true`) or columns (otherwise).
 * On output, this will contain the row/column medians.
 * @param mopt Median calculation options.
 */
template<typename Value_, typename Index_, typename Output_>
void apply(bool row, const tatami::Matrix<Value_, Index_>& mat, Output_* output, const medians::Options& mopt) {
    auto dim = (row ? mat.nrow() : mat.ncol());
    auto otherdim = (row ? mat.ncol() : mat.nrow());

    if (mat.sparse()) {
        tatami::Options opt;
        opt.sparse_extract_index = false;
        opt.sparse_ordered_index = false; // we'll be sorting by value anyway.

        tatami::parallelize([&](int, Index_ s, Index_ l) -> void {
            auto ext = tatami::consecutive_extractor<true>(mat, row, s, l, opt);
            auto buffer = tatami::create_container_of_Index_size<std::vector<Value_> >(otherdim);
            auto vbuffer = buffer.data();
            for (Index_ x = 0; x < l; ++x) {
                auto range = ext->fetch(vbuffer, NULL);
                tatami::copy_n(range.value, range.number, vbuffer);
                output[x + s] = direct<Output_>(vbuffer, range.number, otherdim, mopt.skip_nan);
            }
        }, dim, mopt.num_threads);

    } else {
        tatami::parallelize([&](int, Index_ s, Index_ l) -> void {
            auto buffer = tatami::create_container_of_Index_size<std::vector<Value_> >(otherdim);
            auto ext = tatami::consecutive_extractor<false>(mat, row, s, l);
            for (Index_ x = 0; x < l; ++x) {
                auto ptr = ext->fetch(buffer.data());
                tatami::copy_n(ptr, otherdim, buffer.data());
                output[x + s] = direct<Output_>(buffer.data(), otherdim, mopt.skip_nan);
            }
        }, dim, mopt.num_threads);
    }
}

/**
 * @cond
 */
// Back-compatibility.
template<typename Value_, typename Index_, typename Output_>
void apply(bool row, const tatami::Matrix<Value_, Index_>* p, Output_* output, const medians::Options& mopt) {
    apply(row, *p, output, mopt);
}
/**
 * @endcond
 */

/**
 * Wrapper around `apply()` for column medians.
 *
 * @tparam Output_ Floating-point type of the output value.
 * This should be capable of storing NaNs.
 * @tparam Value_ Numeric type of the input values.
 * @tparam Index_ Integer type of the row/column indices.
 *
 * @param mat Instance of a `tatami::Matrix`.
 * @param mopt Median calculation options.
 *
 * @return A vector of length equal to the number of columns, containing the column medians.
 */
template<typename Output_ = double, typename Value_, typename Index_>
std::vector<Output_> by_column(const tatami::Matrix<Value_, Index_>& mat, const Options& mopt) {
    auto output = tatami::create_container_of_Index_size<std::vector<Output_> >(mat.ncol());
    apply(false, mat, output.data(), mopt);
    return output;
}

/**
 * @cond
 */
// Back-compatibility.
template<typename Output_ = double, typename Value_, typename Index_>
std::vector<Output_> by_column(const tatami::Matrix<Value_, Index_>* p, const Options& mopt) {
    return by_column<Output_>(*p, mopt);
}

template<typename Output_ = double, typename Value_, typename Index_>
std::vector<Output_> by_column(const tatami::Matrix<Value_, Index_>& mat) {
    return by_column<Output_>(mat, Options());
}

template<typename Output_ = double, typename Value_, typename Index_>
std::vector<Output_> by_column(const tatami::Matrix<Value_, Index_>* p) {
    return by_column<Output_>(*p);
}
/**
 * @endcond
 */

/**
 * Wrapper around `apply()` for row medians.
 *
 * @tparam Output_ Floating-point type of the output value.
 * This should be capable of storing NaNs.
 * @tparam Value_ Numeric type of the input values.
 * @tparam Index_ Integer type of the row/column indices.
 *
 * @param mat Instance of a `tatami::Matrix`.
 * @param mopt Median calculation options.
 *
 * @return A vector of length equal to the number of rows, containing the row medians.
 */
template<typename Output_ = double, typename Value_, typename Index_>
std::vector<Output_> by_row(const tatami::Matrix<Value_, Index_>& mat, const Options& mopt) {
    auto output = tatami::create_container_of_Index_size<std::vector<Output_> >(mat.nrow());
    apply(true, mat, output.data(), mopt);
    return output;
}

/**
 * @cond
 */
// Back-compatibility.
template<typename Output_ = double, typename Value_, typename Index_>
std::vector<Output_> by_row(const tatami::Matrix<Value_, Index_>* p, const Options& mopt) {
    return by_row<Output_>(*p, mopt);
}

template<typename Output_ = double, typename Value_, typename Index_>
std::vector<Output_> by_row(const tatami::Matrix<Value_, Index_>& mat) {
    return by_row<Output_>(mat, Options());
}

template<typename Output_ = double, typename Value_, typename Index_>
std::vector<Output_> by_row(const tatami::Matrix<Value_, Index_>* p) {
    return by_row<Output_>(*p);
}
/**
 * @endcond
 */

}

}

#endif
