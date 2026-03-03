#ifndef TATAMI_STATS_QUANTILES_HPP
#define TATAMI_STATS_QUANTILES_HPP

#include "utils.hpp"

#include <cmath>
#include <vector>
#include <algorithm>
#include <limits>
#include <type_traits>

#include "tatami/tatami.hpp"
#include "sanisizer/sanisizer.hpp"
#include "quickstats/quickstats.hpp"

/**
 * @file quantiles.hpp
 *
 * @brief Compute row and column quantiles from a `tatami::Matrix`.
 */

namespace tatami_stats {

/**
 * @brief Functions for computing dimension-wise quantiles.
 * @namespace tatami_stats::quantiles
 */
namespace quantiles {

/**
 * @brief Quantile calculation options.
 */
struct Options {
    /**
     * Whether to check for NaNs in the input, and skip them.
     * If false, NaNs are assumed to be absent, and the behavior of the quantile calculation in the presence of NaNs is undefined.
     */
    bool skip_nan = false;

    /**
     * Number of threads to use when computing quantiles across a `tatami::Matrix`.
     * See `tatami::parallelize()` for more details on the parallelization mechanism.
     */
    int num_threads = 1;
};

/**
 * Compute quantiles for each element of a chosen dimension of a `tatami::Matrix`.
 *
 * @tparam Value_ Numeric type of the input values.
 * @tparam Index_ Integer type of the row/column indices.
 * @tparam Output_ Floating-point type of the output value.
 * This should be capable of storing NaNs.
 *
 * @param row Whether to compute the quantile for each row.
 * If false, the quantile is computed for each column instead.
 * @param mat Instance of a `tatami::Matrix`.
 * @param quantile Probability of the quantile to compute.
 * This should be in \f$[0, 1]\f$.
 * @param[out] output Pointer to an array of length equal to the number of rows (if `row = true`) or columns (otherwise).
 * On output, this will contain the row/column quantiles.
 * @param qopt Quantile calculation options.
 */
template<typename Value_, typename Index_, typename Output_>
void apply(bool row, const tatami::Matrix<Value_, Index_>& mat, const double quantile, Output_* output, const Options& qopt) {
    auto dim = (row ? mat.nrow() : mat.ncol());
    auto otherdim = (row ? mat.ncol() : mat.nrow());

    if (otherdim == 0) {
        // Prevent initalize() from constructing with a fixed instance with otherdim == 0.
        std::fill_n(output, dim, std::numeric_limits<Output_>::quiet_NaN());
        return;
    }

    auto initialize = [&](
        std::optional<quickstats::SingleQuantileFixedNumber<Output_, Index_> >& fixed,
        std::optional<quickstats::SingleQuantileVariableNumber<Output_, Index_> >& variable
    ) -> void {
        if (qopt.skip_nan) {
            variable.emplace(otherdim, quantile);
        } else {
            fixed.emplace(otherdim, quantile);
        }
    };

    if (mat.sparse()) {
        tatami::Options opt;
        opt.sparse_extract_index = false;
        opt.sparse_ordered_index = false; // we'll be sorting by value anyway.

        tatami::parallelize([&](int, Index_ s, Index_ l) -> void {
            auto ext = tatami::consecutive_extractor<true>(mat, row, s, l, opt);
            auto buffer = tatami::create_container_of_Index_size<std::vector<Value_> >(otherdim);
            auto vbuffer = buffer.data();

            std::optional<quickstats::SingleQuantileFixedNumber<Output_, Index_> > qcalcs_fixed;
            std::optional<quickstats::SingleQuantileVariableNumber<Output_, Index_> > qcalcs_var;
            initialize(qcalcs_fixed, qcalcs_var);

            for (Index_ x = 0; x < l; ++x) {
                auto range = ext->fetch(vbuffer, NULL);
                tatami::copy_n(range.value, range.number, vbuffer);

                ::tatami_stats::internal::nanable_ifelse<Value_>(
                    qopt.skip_nan,
                    [&]() -> void {
                        auto lost = shift_nans(vbuffer, range.number);
                        output[x + s] = (*qcalcs_var)(otherdim - lost, range.number - lost, vbuffer + lost);
                    },
                    [&]() -> void {
                        output[x + s] = (*qcalcs_fixed)(range.number, vbuffer);
                    }
                );

            }
        }, dim, qopt.num_threads);

    } else {
        tatami::parallelize([&](int, Index_ s, Index_ l) -> void {
            auto buffer = tatami::create_container_of_Index_size<std::vector<Value_> >(otherdim);
            auto ext = tatami::consecutive_extractor<false>(mat, row, s, l);

            std::optional<quickstats::SingleQuantileFixedNumber<Output_, Index_> > qcalcs_fixed;
            std::optional<quickstats::SingleQuantileVariableNumber<Output_, Index_> > qcalcs_var;
            initialize(qcalcs_fixed, qcalcs_var);

            for (Index_ x = 0; x < l; ++x) {
                auto bufptr = buffer.data();
                auto raw = ext->fetch(bufptr);
                tatami::copy_n(raw, otherdim, bufptr);

                ::tatami_stats::internal::nanable_ifelse<Value_>(
                    qopt.skip_nan,
                    [&]() -> void {
                        auto lost = shift_nans(bufptr, otherdim);
                        output[x + s] = (*qcalcs_var)(otherdim - lost, bufptr + lost);
                    },
                    [&]() -> void {
                        output[x + s] = (*qcalcs_fixed)(bufptr);
                    }
                );
            }
        }, dim, qopt.num_threads);
    }
}

/**
 * Wrapper around `apply()` for column quantiles.
 *
 * @tparam Output_ Floating-point type of the output value.
 * This should be capable of storing NaNs.
 * @tparam Value_ Numeric type of the input values.
 * @tparam Index_ Integer type of the row/column indices.
 *
 * @param mat Instance of a `tatami::Matrix`.
 * @param quantile Probability of the quantile to compute.
 * This should be in \f$[0, 1]\f$.
 * @param qopt Quantile calculation options.
 *
 * @return A vector of length equal to the number of columns, containing the column quantiles.
 */
template<typename Output_ = double, typename Value_, typename Index_>
std::vector<Output_> by_column(const tatami::Matrix<Value_, Index_>& mat, const double quantile, const Options& qopt) {
    auto output = tatami::create_container_of_Index_size<std::vector<Output_> >(mat.ncol());
    apply(false, mat, quantile, output.data(), qopt);
    return output;
}

/**
 * Wrapper around `apply()` for row quantiles.
 *
 * @tparam Output_ Floating-point type of the output value.
 * This should be capable of storing NaNs.
 * @tparam Value_ Numeric type of the input values.
 * @tparam Index_ Integer type of the row/column indices.
 *
 * @param mat Instance of a `tatami::Matrix`.
 * @param quantile Probability of the quantile to compute.
 * This should be in \f$[0, 1]\f$.
 * @param qopt Quantile calculation options.
 *
 * @return A vector of length equal to the number of rows, containing the row quantiles.
 */
template<typename Output_ = double, typename Value_, typename Index_>
std::vector<Output_> by_row(const tatami::Matrix<Value_, Index_>& mat, const double quantile, const Options& qopt) {
    auto output = tatami::create_container_of_Index_size<std::vector<Output_> >(mat.nrow());
    apply(true, mat, quantile, output.data(), qopt);
    return output;
}

}

}

#endif
