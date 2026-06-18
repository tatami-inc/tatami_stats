#ifndef TATAMI_STATS_RSS_HPP
#define TATAMI_STATS_RSS_HPP

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
 * @file rss.hpp
 *
 * @brief Compute row and column residual sum of squares from a `tatami::Matrix`.
 */

namespace tatami_stats {

/**
 * @brief Options for `rss()`.
 */
struct RssOptions {
    /**
     * Number of threads to use for iterating over a `tatami::Matrix`.
     * See `tatami::parallelize()` for more details on the parallelization mechanism.
     */
    int num_threads = 1;
};

/**
 * @brief Result buffers for `rss()`.
 *
 * @tparam Output_ Floating-point type of the output data.
 * This should be capable of storing NaNs.
 */
template<typename Output_>
struct RssBuffers {
    /**
     * Pointer to an array of length equal to the appropriate dimension extent (rows for `row = true`, columns otherwise).
     * After `rss()`, this is filled with the sample mean of each row/column.
     */
    Output_* mean;

    /**
     * Pointer to an array of length equal to the appropriate dimension extent (rows for `row = true`, columns otherwise).
     * After `rss()`, this is filled with the RSS of each row/column.
     */
    Output_* rss;
};

/**
 * @cond
 */
template<typename Value_, typename Index_, typename Output_>
void rss_direct(bool row, const tatami::Matrix<Value_, Index_>& mat, RssBuffers<Output_>& output, const RssOptions& opt) {
    const auto dim = (row ? mat.nrow() : mat.ncol());
    const auto otherdim = (row ? mat.ncol() : mat.nrow());

    if (mat.sparse()) {
        tatami::Options topt;
        topt.sparse_extract_index = false;

        tatami::parallelize([&](int, Index_ s, Index_ l) -> void {
            auto ext = tatami::consecutive_extractor<true>(mat, row, s, l, topt);
            auto vbuffer = tatami::create_container_of_Index_size<std::vector<Value_> >(otherdim);
            quickstats::RssWorkspace<Output_> work;
            for (Index_ x = 0; x < l; ++x) {
                auto out = ext->fetch(vbuffer.data(), NULL);
                const auto res = quickstats::rss(otherdim, out.number, out.value, work);
                output.mean[x + s] = res.mean;
                output.rss[x + s] = res.rss;
            }
        }, dim, opt.num_threads);

    } else {
        tatami::parallelize([&](int, Index_ s, Index_ l) -> void {
            auto ext = tatami::consecutive_extractor<false>(mat, row, s, l);
            auto buffer = tatami::create_container_of_Index_size<std::vector<Value_> >(otherdim);
            quickstats::RssWorkspace<Output_> work;
            for (Index_ x = 0; x < l; ++x) {
                auto out = ext->fetch(buffer.data());
                const auto res = quickstats::rss(otherdim, out, work);
                output.mean[x + s] = res.mean;
                output.rss[x + s] = res.rss;
            }
        }, dim, opt.num_threads);
    }
}

template<typename Value_, typename Index_, typename Output_>
void rss_running(bool row, const tatami::Matrix<Value_, Index_>& mat, RssBuffers<Output_>& output, const RssOptions& opt) {
    const auto dim = (row ? mat.nrow() : mat.ncol());
    const auto otherdim = (row ? mat.ncol() : mat.nrow());
    const bool is_sparse = mat.is_sparse();

    if (otherdim == 0) {
        std::fill_n(output.mean, dim, std::numeric_limits<Output_>::quiet_NaN());
        std::fill_n(output.rss, dim, 0);
        return;
    }

    assert(opt.num_threads > 0);
    const bool do_parallel = opt.num_threads > 1;
    std::optional<std::vector<std::optional<std::vector<Output_> > > > all_partial_mean, all_partial_rss;
    std::optional<std::vector<Index_> > all_partial_count;
    if (do_parallel) {
        // -1, as we'll repurpose the RSS output buffer to store the partial RSS of the first thread.
        all_partial_rss.emplace(sanisizer::cast<I<decltype(all_partial_rss->size())> >(opt.num_threads - 1));
        all_partial_mean.emplace(sanisizer::cast<I<decltype(all_partial_mean->size())> >(opt.num_threads));
        all_partial_count.emplace(sanisizer::cast<I<decltype(all_partial_count->size())> >(opt.num_threads));
    }

    std::fill_n(output.rss, dim, 0);
    std::fill_n(output.mean, dim, 0);

    const int nused = tatami::parallelize([&](int thread, Index_ s, Index_ l) -> void {
        Output_* rss_ptr;
        Output_* mean_ptr;
        std::optional<std::vector<Output_> > cur_rss, cur_mean;

        if (!do_parallel) {
            // Storing mean and RSS directly in the output vector to cut down two allocations if we're not working in parallel.
            rss_ptr = output.rss;
            mean_ptr = output.mean;
        } else {
            // Storing the partial RSS directly in the output vector to save ourselves an allocation if we're in the first thread.
            // We can't do the same for the mean, though, as we need to keep the partial mean and the global mean separate for the reduction.
            cur_mean.emplace(tatami::cast_Index_to_container_size<std::vector<Output_> >(dim));
            mean_ptr = cur_mean->data();
            if (thread == 0) {
                rss_ptr = output.rss;
            } else {
                cur_rss.emplace(tatami::cast_Index_to_container_size<std::vector<Output_> >(dim));
                rss_ptr = cur_rss->data();
            }
        }

        if (is_sparse) {
            tatami::Options topt;
            topt.sparse_ordered_index = false;
            auto ext = tatami::consecutive_extractor<true>(mat, !row, s, l, topt);
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
    }, otherdim, opt.num_threads);
    assert(nused > 0);

    // Don't check nused > 1, as it's possible for do_parallel = true with nused = 1 if not all threads are used.
    // This would cause us to leave output.mean and output.rss empty.
    if (do_parallel) {
        const auto& ap_count = *all_partial_count;
        const auto& ap_mean = *all_partial_mean;
        const auto& ap_rss = *all_partial_rss;

        // Computing the global mean. All ap_count is positive so we don't have to worry about cur_mean[d] being NaN.
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
                    output.rss[d] = quickstats::recenter_rss_unsafe(cur_count, output.rss[d], cur_mean[d], output.mean[d]); 
                }
            } else {
                const auto& cur_rss = *(ap_rss[u - 1]);
                for (Index_ d = 0; d < dim; ++d) {
                    output.rss[d] += quickstats::recenter_rss_unsafe(cur_count, cur_rss[d], cur_mean[d], output.mean[d]); 
                }
            }
        }
    }
}
/**
 * @endcond
 */

/**
 * Compute residual sums of squares (RSS) for each element of a chosen dimension of a `tatami::Matrix`.
 * This may use either Welford's method or the standard two-pass method,
 * depending on the dimension in `row` and the preferred access dimension of `p`.
 *
 * @tparam Value_ Numeric type of the input data.
 * @tparam Index_ Integer type of the row/column indices.
 * @tparam Output_ Floating-point type of the output data.
 * This should be capable of storing NaNs.
 *
 * @param row Whether to compute the RSS for each row.
 * If false, the RSS is computed for each column instead.
 * @param mat Instance of a `tatami::Matrix`.
 * @param[out] output Buffers to output arrays.
 * On output, this will contain the row/column RSSs.
 * @param opt Further options.
 */
template<typename Value_, typename Index_, typename Output_>
void rss(bool row, const tatami::Matrix<Value_, Index_>& mat, RssBuffers<Output_>& output, const RssOptions& opt) {
    if (mat.prefer_rows() == row) {
        rss_direct(row, mat, output, opt);
    } else {
        rss_running(row, mat, output, opt);
    }
}

/**
 * @brief Results of `rss()`.
 *
 * @tparam Output_ Floating-point type of the output data.
 * This should be capable of storing NaNs.
 */
template<typename Output_>
struct RssResult {
    /**
     * Vector of length equal to the appropriate dimension extent (rows for `row = true`, columns otherwise),
     * containing the sample mean of each row/column.
     */
    std::vector<Output_> mean;

    /**
     * Vector of length equal to the appropriate dimension extent (rows for `row = true`, columns otherwise),
     * containing the RSS of each row/column.
     */
    std::vector<Output_> rss;
};

/**
 * Overload of `rss()` that allocates memory for the output arrays.
 *
 * @tparam Output_ Floating-point type of the output data.
 * This should be capable of storing NaNs.
 * @tparam Value_ Numeric type of the input data.
 * @tparam Index_ Integer type of the row/column indices.
 *
 * @param row Whether to compute the RSS for each row.
 * If false, the RSS is computed for each column instead.
 * @param mat Instance of a `tatami::Matrix`.
 * @param opt Further options.
 *
 * @return The mean and RSS of each row/column.
 */
template<typename Output_ = double, typename Value_, typename Index_>
RssResult<Output_> rss(bool row, const tatami::Matrix<Value_, Index_>& mat, const RssOptions& opt) {
    RssResult<Output_> output;
    const auto dim = (row ? mat.nrow() : mat.ncol());
    tatami::resize_container_to_Index_size(output.mean, dim
#ifdef TATAMI_STATS_TEST_DIRTY
        , -1
#endif
    );
    tatami::resize_container_to_Index_size(output.rss, dim
#ifdef TATAMI_STATS_TEST_DIRTY
        , -1
#endif
    );

    RssBuffers<Output_> buffers;
    buffers.mean = output.mean.data();
    buffers.rss = output.rss.data();

    rss(row, mat, buffers, opt);
    return output;
}

}

#endif
