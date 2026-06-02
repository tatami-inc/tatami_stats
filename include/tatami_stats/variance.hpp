#ifndef TATAMI_STATS_VARIANCE_HPP
#define TATAMI_STATS_VARIANCE_HPP

#include "utils.hpp"

#include <vector>
#include <cmath>
#include <numeric>
#include <limits>
#include <algorithm>
#include <cstddef>
#include <optional>

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
    std::optional<std::vector<std::optional<std::vector<Output_> > > > all_partial_mean, all_partial_rss;
    std::optional<std::vector<Index_> > all_partial_count;
    if (do_parallel) {
        // -1, as we'll repurpose the RSS output buffer to store the partial RSS of the first thread.
        all_partial_rss.emplace(sanisizer::cast<I<decltype(all_partial_rss->size())> >(vopt.num_threads - 1));
        all_partial_mean.emplace(sanisizer::cast<I<decltype(all_partial_mean->size())> >(vopt.num_threads));
        all_partial_count.emplace(sanisizer::cast<I<decltype(all_partial_count->size())> >(vopt.num_threads));
    }

    std::fill_n(output.variance, dim, 0);
    std::fill_n(output.mean, dim, 0);

    const int nused = tatami::parallelize([&](int thread, Index_ s, Index_ l) -> void {
        Output_* rss_ptr;
        Output_* mean_ptr;
        std::optional<std::vector<Output_> > cur_rss, cur_mean;

        if (!do_parallel) {
            // Storing mean and RSS directly in the output vector to cut down two allocations if we're not working in parallel.
            rss_ptr = output.variance;
            mean_ptr = output.mean;
        } else {
            // Storing the partial RSS directly in the output vector to save ourselves an allocation if we're in the first thread.
            // We can't do the same for the mean, though, as we need to keep the partial mean and the global mean separate for the reduction.
            cur_mean.emplace(tatami::cast_Index_to_container_size<std::vector<Output_> >(dim));
            mean_ptr = cur_mean->data();
            if (thread == 0) {
                rss_ptr = output.variance;
            } else {
                cur_rss.emplace(tatami::cast_Index_to_container_size<std::vector<Output_> >(dim));
                rss_ptr = cur_rss->data();
            }
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

        if (do_parallel) {
            (*all_partial_count)[thread] = l;
            (*all_partial_mean)[thread] = std::move(cur_mean);
            if (thread > 0) {
                (*all_partial_rss)[thread - 1] = std::move(cur_rss);
            }
        }
    }, otherdim, vopt.num_threads);

    // Don't check nused > 1, as it's possible for do_parallel = true with nused = 1 if not all threads are used.
    // This would cause us to leave output.mean and output.variance empty.
    if (do_parallel) {
        const auto& ap_count = *all_partial_count;
        const auto& ap_mean = *all_partial_mean;
        const auto& ap_rss = *all_partial_rss;

        // Computing the global mean.
        for (int u = 0; u < nused; ++u) {
            const Output_ mult = static_cast<Output_>(ap_count[u]) / static_cast<Output_>(otherdim);
            const auto& cur_mean = *(ap_mean[u]);
            for (Index_ d = 0; d < dim; ++d) {
                output.mean[d] += cur_mean[d] * mult;
            }
        }

        // Combining the RSS. We can use recenter_rss_unsafe() as we are guaranteed that cur_count > 0,
        // as parallelize() will only ever split into non-empty ranges if those ranges are used.
        for (int u = 0; u < nused; ++u) {
            const auto cur_count = ap_count[u];
            const auto& cur_mean = *(ap_mean[u]);
            if (u == 0) {
                for (Index_ d = 0; d < dim; ++d) {
                    output.variance[d] = quickstats::recenter_rss_unsafe(cur_count, output.variance[d], cur_mean[d], output.mean[d]); 
                }
            } else {
                const auto& cur_rss = *(ap_rss[u - 1]);
                for (Index_ d = 0; d < dim; ++d) {
                    output.variance[d] += quickstats::recenter_rss_unsafe(cur_count, cur_rss[d], cur_mean[d], output.mean[d]); 
                }
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
    std::optional<std::vector<std::optional<std::vector<Output_> > > > all_partial_mean, all_partial_rss;
    if (do_parallel) {
        // -1, as we'll repurpose the output buffers to store the output of the first thread.
        all_partial_rss.emplace(sanisizer::cast<I<decltype(all_partial_rss->size())> >(vopt.num_threads - 1));
        all_partial_mean.emplace(sanisizer::cast<I<decltype(all_partial_mean->size())> >(vopt.num_threads));
    }
    auto all_partial_count = sanisizer::create<std::vector<std::optional<std::vector<Index_> > > >(vopt.num_threads);

    std::fill_n(output.variance, dim, 0);
    std::fill_n(output.mean, dim, 0);

    const int nused = tatami::parallelize([&](int thread, Index_ s, Index_ l) -> void {
        Output_* rss_ptr;
        Output_* mean_ptr;
        std::optional<std::vector<Output_> > cur_rss, cur_mean;

        if (!do_parallel) {
            // Storing mean and RSS directly in the output vector to cut down two allocations if we're not working in parallel.
            rss_ptr = output.variance;
            mean_ptr = output.mean;
        } else {
            // Storing the partial RSS directly in the output vector to save ourselves an allocation if we're in the first thread.
            // We can't do the same for the mean, though, as we need to keep the partial mean and the global mean separate for the reduction.
            cur_mean.emplace(tatami::cast_Index_to_container_size<std::vector<Output_> >(dim));
            mean_ptr = cur_mean->data();
            if (thread == 0) {
                rss_ptr = output.variance;
            } else {
                cur_rss.emplace(tatami::cast_Index_to_container_size<std::vector<Output_> >(dim));
                rss_ptr = cur_rss->data();
            }
        }

        auto cur_count = tatami::create_container_of_Index_size<std::vector<Index_> >(dim);

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

        // Moving results to the main containers.
        all_partial_count[thread] = std::move(cur_count);
        if (do_parallel) {
            (*all_partial_mean)[thread] = std::move(cur_mean);
            if (thread > 0) {
                (*all_partial_rss)[thread - 1] = std::move(cur_rss);
            }
        }
    }, otherdim, vopt.num_threads);

    // Don't check nused > 1, as it's possible for do_parallel = true with nused = 1 if not all threads are used.
    // This would cause us to leave output.mean and output.variance empty.
    if (!do_parallel) {
        quickstats::rss_to_variance(dim, all_partial_count.front()->data(), output.variance);

    } else {
        const auto& ap_mean = *all_partial_mean;
        const auto& ap_rss = *all_partial_rss;

        // Computing the global total.
        auto global_count = tatami::create_container_of_Index_size<std::vector<Index_> >(dim);
        for (int u = 0; u < nused; ++u) {
            const auto& cur_count = *(all_partial_count[u]);
            for (Index_ d = 0; d < dim; ++d) {
                global_count[d] += cur_count[d];
            }
        }

        // Computing the global mean.
        for (int u = 0; u < nused; ++u) {
            const auto& cur_count = *(all_partial_count[u]);
            const auto& cur_mean = *(ap_mean[u]);
            for (Index_ d = 0; d < dim; ++d) {
                const auto mult = static_cast<Output_>(cur_count[u]) / static_cast<Output_>(global_count[u]);
                output.mean[d] += cur_mean[d] * mult;
            }
        }

        // Combining the RSS. This time, we need to use the safe version as we don't know whether all elements were skipped in a thread.
        for (int u = 0; u < nused; ++u) {
            const auto& cur_count = *(all_partial_count[u]);
            const auto& cur_mean = *(ap_mean[u]);
            if (u == 0) {
                for (Index_ d = 0; d < dim; ++d) {
                    output.variance[d] = quickstats::recenter_rss(cur_count[d], output.variance[d], cur_mean[d], output.mean[d]); 
                }
            } else {
                const auto& cur_rss = *(ap_rss[u - 1]);
                for (Index_ d = 0; d < dim; ++d) {
                    output.variance[d] += quickstats::recenter_rss(cur_count[d], cur_rss[d], cur_mean[d], output.mean[d]); 
                }
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
