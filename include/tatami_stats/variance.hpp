#ifndef TATAMI_STATS_VARIANCE_HPP
#define TATAMI_STATS_VARIANCE_HPP

#include "utils.hpp"

#include <vector>
#include <cmath>
#include <numeric>
#include <limits>
#include <algorithm>
#include <cstddef>

#include "tatami/tatami.hpp"
#include "sanisizer/sanisizer.hpp"
#include "quickstats/quickstats.hpp"

/**
 * @file variance.hpp
 *
 * @brief Compute row and column variances from a `tatami::Matrix`.
 */

namespace tatami_stats {

/**
 * @brief Functions for computing dimension-wise variances.
 * @namespace tatami_stats::variances
 */
namespace variance {

/**
 * @brief Variance calculation options.
 */
struct Options {
    /**
     * Whether to check for NaNs in the input, and skip them.
     * If false, NaNs are assumed to be absent, and the behavior of the variance calculation in the presence of NaNs is undefined.
     */
    bool skip_nan = false;

    /**
     * Number of threads to use when computing variances across a `tatami::Matrix`.
     * See `tatami::parallelize()` for more details on the parallelization mechanism.
     */
    int num_threads = 1;
};

/**
 * @brief Result buffers for `apply()`.
 *
 * @tparam Output_ Floating-point type of the output data.
 * This should be capable of storing NaNs.
 */
template<typename Output_>
struct Buffers {
    /**
     * Pointer to an array of length equal to the appropriate dimension extent (rows for `row = true`, columns otherwise).
     * After `apply()`, this is filled with the sample mean of each row/column.
     */
    Output_* mean;

    /**
     * Pointer to an array of length equal to the appropriate dimension extent (rows for `row = true`, columns otherwise).
     * After `apply()`, this is filled with the sample variance of each row/column.
     */
    Output_* variance;
};

/**
 * @cond
 */
template<typename Value_, typename Index_, typename Output_>
void apply_direct_noskip(bool row, const tatami::Matrix<Value_, Index_>& mat, Buffers<Output_>& output, const Options& vopt) {
    const auto dim = (row ? mat.nrow() : mat.ncol());
    const auto otherdim = (row ? mat.ncol() : mat.nrow());

    if (mat.sparse()) {
        tatami::Options opt;
        opt.sparse_extract_index = false;

        tatami::parallelize([&](int, Index_ s, Index_ l) -> void {
            auto ext = tatami::consecutive_extractor<true>(mat, row, s, l, opt);
            auto vbuffer = tatami::create_container_of_Index_size<std::vector<Value_> >(otherdim);
            quickstats::RssWorkspace<Output_> work;
            for (Index_ x = 0; x < l; ++x) {
                auto out = ext->fetch(vbuffer.data(), NULL);
                const auto res = quickstats::rss(otherdim, out.number, out.value, work);
                output.mean[x + s] = res.mean;
                output.variance[x + s] = quickstats::rss_to_variance(otherdim, res.rss);
            }
        }, dim, vopt.num_threads);

    } else {
        tatami::parallelize([&](int, Index_ s, Index_ l) -> void {
            auto ext = tatami::consecutive_extractor<false>(mat, row, s, l);
            auto buffer = tatami::create_container_of_Index_size<std::vector<Value_> >(otherdim);
            quickstats::RssWorkspace<Output_> work;
            for (Index_ x = 0; x < l; ++x) {
                auto out = ext->fetch(buffer.data());
                const auto res = quickstats::rss(otherdim, out, work);
                output.mean[x + s] = res.mean;
                output.variance[x + s] = quickstats::rss_to_variance(otherdim, res.rss);
            }
        }, dim, vopt.num_threads);
    }
}

template<typename Value_, typename Index_, typename Output_>
void apply_direct_skip(bool row, const tatami::Matrix<Value_, Index_>& mat, Buffers<Output_>& output, const Options& vopt) {
    const auto dim = (row ? mat.nrow() : mat.ncol());
    const auto otherdim = (row ? mat.ncol() : mat.nrow());

    if (mat.sparse()) {
        tatami::Options opt;
        opt.sparse_extract_index = false;

        tatami::parallelize([&](int, Index_ s, Index_ l) -> void {
            auto ext = tatami::consecutive_extractor<true>(mat, row, s, l, opt);
            auto vbuffer = tatami::create_container_of_Index_size<std::vector<Value_> >(otherdim);
            quickstats::RssWorkspace<Output_> work;
            for (Index_ x = 0; x < l; ++x) {
                auto out = ext->fetch(vbuffer.data(), NULL);
                tatami::copy_n(out.value, out.number, vbuffer.data());
                const auto new_number = shift_nans(vbuffer.data(), out.number);
                const Index_ new_total = otherdim - (out.number - new_number);
                const auto res = quickstats::rss(new_total, new_number, vbuffer.data(), work);
                output.mean[x + s] = res.mean;
                output.variance[x + s] = quickstats::rss_to_variance(new_total, res.rss);
            }
        }, dim, vopt.num_threads);

    } else {
        tatami::parallelize([&](int, Index_ s, Index_ l) -> void {
            auto ext = tatami::consecutive_extractor<false>(mat, row, s, l);
            auto buffer = tatami::create_container_of_Index_size<std::vector<Value_> >(otherdim);
            quickstats::RssWorkspace<Output_> work;
            for (Index_ x = 0; x < l; ++x) {
                auto out = ext->fetch(buffer.data());
                tatami::copy_n(out, otherdim, buffer.data());
                const auto new_total = shift_nans(buffer.data(), otherdim);
                const auto res = quickstats::rss(new_total, buffer.data(), work);
                output.mean[x + s] = res.mean;
                output.variance[x + s] = quickstats::rss_to_variance(new_total, res.rss);
            }
        }, dim, vopt.num_threads);
    }
}

template<typename Value_, typename Index_, typename Output_>
void apply_running_noskip(bool row, const tatami::Matrix<Value_, Index_>& mat, Buffers<Output_>& output, const Options& vopt) {
    const auto dim = (row ? mat.nrow() : mat.ncol());
    const auto otherdim = (row ? mat.ncol() : mat.nrow());
    const bool is_sparse = mat.is_sparse();

    const bool do_parallel = vopt.num_threads > 1;
    std::vector<std::vector<Output_> > all_partial_mean, all_partial_rss;
    std::vector<Index_> all_partial_count;
    if (do_parallel) {
        sanisizer::resize(all_partial_rss, vopt.num_threads);
        sanisizer::resize(all_partial_mean, vopt.num_threads);
        sanisizer::resize(all_partial_count, vopt.num_threads);
    }

    std::fill_n(output.variance, dim, 0);
    std::fill_n(output.mean, dim, 0);

    const int nused = tatami::parallelize([&](int thread, Index_ s, Index_ l) -> void {
        // Storing the results directly in the output vector to save ourselves an allocation if we're not working in parallel.
        Output_* rss_ptr;
        Output_* mean_ptr;
        if (!do_parallel) {
            rss_ptr = output.variance;
            mean_ptr = output.mean;
        } else {
            auto& cur_rss = all_partial_rss[thread];
            tatami::resize_container_to_Index_size(cur_rss, dim);
            rss_ptr = cur_rss.data();
            auto& cur_mean = all_partial_mean[thread];
            tatami::resize_container_to_Index_size(cur_mean, dim);
            mean_ptr = cur_mean.data();
            all_partial_count[thread] = l;
        }

        if (is_sparse) {
            tatami::Options opt;
            opt.sparse_ordered_index = false;
            auto ext = tatami::consecutive_extractor<true>(mat, !row, s, l, opt);
            auto vbuffer = tatami::create_container_of_Index_size<std::vector<Value_> >(dim);
            auto ibuffer = tatami::create_container_of_Index_size<std::vector<Index_> >(dim);
            auto nonzeros = tatami::create_container_of_Index_size<std::vector<Index_> >(dim);

            quickstats::RssRunningSparse<Index_, Value_, Output_> runner(dim, mean_ptr, rss_ptr, nonzeros.data());
            for (Index_ x = 0; x < l; ++x) {
                auto out = ext->fetch(vbuffer.data(), ibuffer.data());
                runner.add(out.number, out.value, out.index);
            }
            runner.finish();

        } else {
            auto ext = tatami::consecutive_extractor<false>(mat, !row, s, l);
            auto buffer = tatami::create_container_of_Index_size<std::vector<Value_> >(dim);

            quickstats::RssRunningDense<Value_, Output_> runner(dim, mean_ptr, rss_ptr);
            for (Index_ x = 0; x < l; ++x) {
                auto out = ext->fetch(buffer.data());
                runner.add(out);
            }
            runner.finish();
        }

    }, otherdim, vopt.num_threads);

    if (do_parallel) { // don't check nused > 1, as it's possible for do_parallel = true with nused = 1 if not all threads are used.
        // Computing the global mean.
        for (int u = 0; u < nused; ++u) {
            const Output_ mult = static_cast<Output_>(all_partial_count[u]) / static_cast<Output_>(otherdim);
            const auto& cur_mean = all_partial_mean[u];
            for (Index_ d = 0; d < dim; ++d) {
                output.mean[d] += cur_mean[d] * mult;
            }
        }

        // Combining the RSS. We can use recenter_rss_unsafe() as we are guaranteed that cur_count > 0,
        // as parallelize() will only ever split into non-empty ranges if those ranges are used.
        for (int u = 0; u < nused; ++u) {
            const auto cur_count = all_partial_count[u];
            const auto& cur_mean = all_partial_mean[u];
            const auto& cur_rss = all_partial_rss[u];
            for (Index_ d = 0; d < dim; ++d) {
                output.variance[d] += quickstats::recenter_rss_unsafe(cur_count, cur_rss[d], cur_mean[d], output.mean[d]); 
            }
        }
    }

    quickstats::rss_to_variance(dim, otherdim, output.variance);
}

template<typename Value_, typename Index_, typename Output_>
void apply_running_skip(bool row, const tatami::Matrix<Value_, Index_>& mat, Buffers<Output_>& output, const Options& vopt) {
    const auto dim = (row ? mat.nrow() : mat.ncol());
    const auto otherdim = (row ? mat.ncol() : mat.nrow());
    const bool is_sparse = mat.is_sparse();

    assert(vopt.num_threads > 0);
    const bool do_parallel = vopt.num_threads > 1;
    std::vector<std::vector<Output_> > all_partial_mean, all_partial_rss;
    if (do_parallel) {
        sanisizer::resize(all_partial_rss, vopt.num_threads);
        sanisizer::resize(all_partial_mean, vopt.num_threads);
    }
    auto all_partial_count = sanisizer::create<std::vector<std::vector<Index_> > >(vopt.num_threads);

    std::fill_n(output.variance, dim, 0);
    std::fill_n(output.mean, dim, 0);

    const int nused = tatami::parallelize([&](int thread, Index_ s, Index_ l) -> void {
        // Storing the results directly in the output vector to save ourselves an allocation if we're not working in parallel.
        Output_* rss_ptr;
        Output_* mean_ptr;
        if (!do_parallel) {
            rss_ptr = output.variance;
            mean_ptr = output.mean;
        } else {
            auto& cur_rss = all_partial_rss[thread];
            tatami::resize_container_to_Index_size(cur_rss, dim);
            rss_ptr = cur_rss.data();
            auto& cur_mean = all_partial_mean[thread];
            tatami::resize_container_to_Index_size(cur_mean, dim);
            mean_ptr = cur_mean.data();
        }

        auto& cur_count = all_partial_count[thread];
        tatami::resize_container_to_Index_size(cur_count, dim);

        if (is_sparse) {
            tatami::Options opt;
            opt.sparse_ordered_index = false;
            auto ext = tatami::consecutive_extractor<true>(mat, !row, s, l, opt);
            auto vbuffer = tatami::create_container_of_Index_size<std::vector<Value_> >(dim);
            auto ibuffer = tatami::create_container_of_Index_size<std::vector<Index_> >(dim);
            auto nonzeros = tatami::create_container_of_Index_size<std::vector<Index_> >(dim);

            quickstats::RssRunningSparseSkip<Index_, Value_, Output_> runner(dim, mean_ptr, rss_ptr, nonzeros.data(), cur_count.data());
            for (Index_ x = 0; x < l; ++x) {
                auto out = ext->fetch(vbuffer.data(), ibuffer.data());
                runner.add(
                    out.number,
                    out.value,
                    out.index,
                    [](std::size_t, const Value_ val) -> bool {
                        return std::isnan(val);
                    }
                );
            }
            runner.finish();

        } else {
            auto ext = tatami::consecutive_extractor<false>(mat, !row, s, l);
            auto buffer = tatami::create_container_of_Index_size<std::vector<Value_> >(dim);

            quickstats::RssRunningDenseSkip<Index_, Value_, Output_> runner(dim, mean_ptr, rss_ptr, cur_count.data());
            for (Index_ x = 0; x < l; ++x) {
                auto out = ext->fetch(buffer.data());
                runner.add(
                    out,
                    [](std::size_t, const Value_ val) -> bool {
                        return std::isnan(val);
                    }
                );
            }
            runner.finish();
        }

    }, otherdim, vopt.num_threads);

    if (!do_parallel) {
        quickstats::rss_to_variance(dim, all_partial_count.front().data(), output.variance);

    } else {
        // Computing the global total.
        auto global_count = tatami::create_container_of_Index_size<std::vector<Index_> >(dim);
        for (int u = 0; u < nused; ++u) {
            const auto& cur_count = all_partial_count[u];
            for (Index_ d = 0; d < dim; ++d) {
                global_count[d] += cur_count[d];
            }
        }

        // Computing the global mean.
        for (int u = 0; u < nused; ++u) {
            const auto& cur_count = all_partial_count[u];
            const auto& cur_mean = all_partial_mean[u];
            for (Index_ d = 0; d < dim; ++d) {
                const auto mult = static_cast<Output_>(cur_count[u]) / static_cast<Output_>(global_count[u]);
                output.mean[d] += cur_mean[d] * mult;
            }
        }

        // Combining the RSS. This time, we need to use the safe version as we don't know whether all elements were skipped in a thread.
        for (int u = 0; u < nused; ++u) {
            const auto& cur_count = all_partial_count[u];
            const auto& cur_mean = all_partial_mean[u];
            const auto& cur_rss = all_partial_rss[u];
            for (Index_ d = 0; d < dim; ++d) {
                output.variance[d] += quickstats::recenter_rss(cur_count[d], cur_rss[d], cur_mean[d], output.mean[d]); 
            }
        }

        quickstats::rss_to_variance(dim, global_count.data(), output.variance);
    }
}
/**
 * @endcond
 */

/**
 * Compute variances for each element of a chosen dimension of a `tatami::Matrix`.
 * This may use either Welford's method or the standard two-pass method,
 * depending on the dimension in `row` and the preferred access dimension of `p`.
 *
 * @tparam Value_ Numeric type of the input data.
 * @tparam Index_ Integer type of the row/column indices.
 * @tparam Output_ Floating-point type of the output data.
 * This should be capable of storing NaNs.
 *
 * @param row Whether to compute the variance for each row.
 * If false, the variance is computed for each column instead.
 * @param mat Instance of a `tatami::Matrix`.
 * @param[out] output Buffers to output arrays.
 * On output, this will contain the row/column variances.
 * @param vopt Variance calculation options.
 */
template<typename Value_, typename Index_, typename Output_>
void apply(bool row, const tatami::Matrix<Value_, Index_>& mat, Buffers<Output_>& output, const Options& vopt) {
    internal::nanable_ifelse<Value_>(
        vopt.skip_nan,
        [&]() -> void {
            if (mat.prefer_rows() == row) {
                apply_direct_skip(row, mat, output, vopt);
            } else {
                apply_running_skip(row, mat, output, vopt);
            }
        },
        [&]() -> void {
            if (mat.prefer_rows() == row) {
                apply_direct_noskip(row, mat, output, vopt);
            } else {
                apply_running_noskip(row, mat, output, vopt);
            }
        }
    );
}

/**
 * @brief Results of `apply()`.
 *
 * @tparam Output_ Floating-point type of the output data.
 * This should be capable of storing NaNs.
 */
template<typename Output_>
struct Result {
    /**
     * Vector of length equal to the appropriate dimension extent (rows for `row = true`, columns otherwise),
     * containing the sample mean of each row/column.
     */
    std::vector<Output_> mean;

    /**
     * Vector of length equal to the appropriate dimension extent (rows for `row = true`, columns otherwise),
     * containing the sample variance of each row/column.
     */
    std::vector<Output_> variance;
};

/**
 * Overload of `apply()` that allocates memory for the output arrays.
 *
 * @tparam Output_ Floating-point type of the output data.
 * This should be capable of storing NaNs.
 * @tparam Value_ Numeric type of the input data.
 * @tparam Index_ Integer type of the row/column indices.
 *
 * @param row Whether to compute the variance for each row.
 * If false, the variance is computed for each column instead.
 * @param mat Instance of a `tatami::Matrix`.
 * @param vopt Variance calculation options.
 *
 * @return The mean and variance of each row/column.
 */
template<typename Output_ = double, typename Value_, typename Index_>
Result<Output_> apply(bool row, const tatami::Matrix<Value_, Index_>& mat, const Options& vopt) {
    Result<Output_> output;
    const auto dim = (row ? mat.nrow() : mat.ncol());
    tatami::resize_container_to_Index_size(output.mean, dim);
    tatami::resize_container_to_Index_size(output.variance, dim);

    Buffers<Output_> buffers;
    buffers.mean = output.mean.data();
    buffers.variance = output.variance.data();

    apply(row, mat, buffers, vopt);
    return output;
}

}

}

#endif
