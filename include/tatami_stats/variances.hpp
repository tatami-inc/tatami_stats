#ifndef TATAMI_STATS_VARS_HPP
#define TATAMI_STATS_VARS_HPP

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
 * @file variances.hpp
 *
 * @brief Compute row and column variances from a `tatami::Matrix`.
 */

namespace tatami_stats {

/**
 * @brief Functions for computing dimension-wise variances.
 * @namespace tatami_stats::variances
 */
namespace variances {

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
 * @cond
 */
template<typename Value_, typename Index_, typename Output_>
void apply_direct_noskip(bool row, const tatami::Matrix<Value_, Index_>& mat, Output_* output, const Options& vopt) {
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
                output[x + s] = quickstats::rss_to_variance(otherdim, res.rss);
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
                output[x + s] = quickstats::rss_to_variance(otherdim, res.rss);
            }
        }, dim, vopt.num_threads);
    }
}

template<typename Value_, typename Index_, typename Output_>
void apply_direct_skip(bool row, const tatami::Matrix<Value_, Index_>& mat, Output_* output, const Options& vopt) {
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
                output[x + s] = quickstats::rss_to_variance(new_total, res.rss);
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
                output[x + s] = quickstats::rss_to_variance(new_total, res.rss);
            }
        }, dim, vopt.num_threads);
    }
}

template<typename Value_, typename Index_, typename Output_>
void apply_running_noskip(bool row, const tatami::Matrix<Value_, Index_>& mat, Output_* output, const Options& vopt) {
    const auto dim = (row ? mat.nrow() : mat.ncol());
    const auto otherdim = (row ? mat.ncol() : mat.nrow());
    const bool is_sparse = mat.is_sparse();

    auto all_partial_rss = sanisizer::create<std::vector<std::vector<Output_> > >(vopt.num_threads);
    auto all_partial_mean = sanisizer::create<std::vector<std::vector<Output_> > >(vopt.num_threads);
    auto all_partial_count = sanisizer::create<std::vector<Index_> >(vopt.num_threads);

    const int nused = tatami::parallelize([&](int thread, Index_ s, Index_ l) -> void {
        // Storing the RSS's for the first thread in the output vector to save ourselves an allocation.
        Output_* rss_ptr;
        if (thread == 0) {
            std::fill_n(output, dim, 0);
            rss_ptr = output;
        } else {
            auto& cur_rss = all_partial_rss[thread];
            tatami::resize_container_to_Index_size(cur_rss, dim);
            rss_ptr = cur_rss.data();
        }

        auto& cur_mean = all_partial_mean[thread];
        tatami::resize_container_to_Index_size(cur_mean, dim);
        all_partial_count[thread] = l;

        if (is_sparse) {
            auto ext = tatami::consecutive_extractor<true>(mat, !row, s, l);
            auto vbuffer = tatami::create_container_of_Index_size<std::vector<Value_> >(dim);
            auto ibuffer = tatami::create_container_of_Index_size<std::vector<Index_> >(dim);
            auto nonzeros = tatami::create_container_of_Index_size<std::vector<Index_> >(dim);

            quickstats::RssRunningSparse<Index_, Value_, Output_> runner(dim, cur_mean.data(), rss_ptr, nonzeros.data());
            for (Index_ x = 0; x < l; ++x) {
                auto out = ext->fetch(vbuffer.data(), ibuffer.data());
                runner.add(out.number, out.value, out.index);
            }
            runner.finish();

        } else {
            auto ext = tatami::consecutive_extractor<false>(mat, !row, s, l);
            auto buffer = tatami::create_container_of_Index_size<std::vector<Value_> >(dim);

            quickstats::RssRunningDense<Value_, Output_> runner(dim, cur_mean.data(), rss_ptr);
            for (Index_ x = 0; x < l; ++x) {
                auto out = ext->fetch(buffer.data());
                runner.add(out);
            }
            runner.finish();
        }

    }, otherdim, vopt.num_threads);

    if (nused > 1) {
        // Computing the global mean.
        auto global_mean = tatami::create_container_of_Index_size<std::vector<Output_> >(dim);
        for (int u = 0; u < nused; ++u) {
            const Output_ mult = static_cast<Output_>(all_partial_count[u]) / static_cast<Output_>(otherdim);
            const auto& cur_mean = all_partial_mean[u];
            for (Index_ d = 0; d < dim; ++d) {
                global_mean[d] += cur_mean[d] * mult;
            }
        }

        // Combining the RSS. We can use recenter_rss_unsafe() as we are guaranteed that cur_count > 0,
        // as parallelize() will only ever split into non-empty ranges if those ranges are used.
        for (int u = 0; u < nused; ++u) {
            const auto cur_count = all_partial_count[u];
            const auto& cur_mean = all_partial_mean[u];
            if (u == 0) {
                for (Index_ d = 0; d < dim; ++d) {
                    output[d] = quickstats::recenter_rss_unsafe(cur_count, output[d], cur_mean[d], global_mean[d]); 
                }
            } else {
                const auto& cur_rss = all_partial_rss[u];
                for (Index_ d = 0; d < dim; ++d) {
                    output[d] += quickstats::recenter_rss_unsafe(cur_count, cur_rss[d], cur_mean[d], global_mean[d]); 
                }
            }
        }
    }

    quickstats::rss_to_variance(dim, otherdim, output);
}

template<typename Value_, typename Index_, typename Output_>
void apply_running_skip(bool row, const tatami::Matrix<Value_, Index_>& mat, Output_* output, const Options& vopt) {
    const auto dim = (row ? mat.nrow() : mat.ncol());
    const auto otherdim = (row ? mat.ncol() : mat.nrow());
    const bool is_sparse = mat.is_sparse();

    auto all_partial_rss = sanisizer::create<std::vector<std::vector<Output_> > >(vopt.num_threads);
    auto all_partial_mean = sanisizer::create<std::vector<std::vector<Output_> > >(vopt.num_threads);
    auto all_partial_count = sanisizer::create<std::vector<std::vector<Index_> > >(vopt.num_threads);

    const int nused = tatami::parallelize([&](int thread, Index_ s, Index_ l) -> void {
        auto& cur_mean = all_partial_mean[thread];
        tatami::resize_container_to_Index_size(cur_mean, dim);
        auto& cur_count = all_partial_count[thread];
        tatami::resize_container_to_Index_size(cur_count, dim);

        // Storing the RSS's for the first thread in the output vector to save ourselves an allocation.
        Output_* rss;
        if (thread == 0) {
            std::fill_n(output, dim, 0);
            rss = output;
        } else {
            auto& cur_rss = all_partial_rss[thread];
            tatami::resize_container_to_Index_size(cur_rss, dim);
            rss = cur_rss.data();
        }

        if (is_sparse) {
            auto ext = tatami::consecutive_extractor<true>(mat, !row, s, l);
            auto vbuffer = tatami::create_container_of_Index_size<std::vector<Value_> >(dim);
            auto ibuffer = tatami::create_container_of_Index_size<std::vector<Index_> >(dim);
            auto nonzeros = tatami::create_container_of_Index_size<std::vector<Index_> >(dim);

            quickstats::RssRunningSparseSkip<Index_, Value_, Output_> runner(dim, cur_mean.data(), rss, nonzeros.data(), cur_count.data());
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

            quickstats::RssRunningDenseSkip<Index_, Value_, Output_> runner(dim, cur_mean.data(), rss, cur_count.data());
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

    if (nused == 1) {
        quickstats::rss_to_variance(dim, all_partial_count.front().data(), output);

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
        auto global_mean = tatami::create_container_of_Index_size<std::vector<Output_> >(dim);
        for (int u = 0; u < nused; ++u) {
            const auto& cur_count = all_partial_count[u];
            const auto& cur_mean = all_partial_mean[u];
            for (Index_ d = 0; d < dim; ++d) {
                const auto mult = static_cast<Output_>(cur_count[u]) / static_cast<Output_>(global_count[u]);
                global_mean[d] += cur_mean[d] * mult;
            }
        }

        // Combining the RSS. This time, we need to use the safe version as we don't know whether all elements were skipped in a thread.
        for (int u = 0; u < nused; ++u) {
            const auto& cur_count = all_partial_count[u];
            const auto& cur_mean = all_partial_mean[u];
            if (u == 0) {
                for (Index_ d = 0; d < dim; ++d) {
                    output[d] = quickstats::recenter_rss(cur_count[d], output[d], cur_mean[d], global_mean[d]); 
                }
            } else {
                const auto& cur_rss = all_partial_rss[u];
                for (Index_ d = 0; d < dim; ++d) {
                    output[d] += quickstats::recenter_rss(cur_count[d], cur_rss[d], cur_mean[d], global_mean[d]); 
                }
            }
        }

        quickstats::rss_to_variance(dim, global_count.data(), output);
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
 * @param[out] output Pointer to an array of length equal to the number of rows (if `row = true`) or columns (otherwise).
 * On output, this will contain the row/column variances.
 * @param vopt Variance calculation options.
 */
template<typename Value_, typename Index_, typename Output_>
void apply(bool row, const tatami::Matrix<Value_, Index_>& mat, Output_* output, const Options& vopt) {
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
 * @cond
 */
// Back-compatibility.
template<typename Value_, typename Index_, typename Output_>
void apply(bool row, const tatami::Matrix<Value_, Index_>* p, Output_* output, const Options& vopt) {
    apply(row, *p, output, vopt);
}
/**
 * @endcond
 */

/**
 * Wrapper around `apply()` for column variances.
 *
 * @tparam Output_ Floating-point type of the output data.
 * This should be capable of storing NaNs.
 * @tparam Value_ Numeric type of the input data.
 * @tparam Index_ Integer type of the row/column indices.
 *
 * @param mat Instance of a `tatami::Matrix`.
 * @param vopt Variance calculation options.
 *
 * @return A vector of length equal to the number of columns, containing the column variances.
 */
template<typename Output_ = double, typename Value_, typename Index_>
std::vector<Output_> by_column(const tatami::Matrix<Value_, Index_>& mat, const Options& vopt) {
    auto output = tatami::create_container_of_Index_size<std::vector<Output_> >(mat.ncol());
    apply(false, mat, output.data(), vopt);
    return output;
}

/**
 * @cond
 */
// Back-compatibility.
template<typename Output_ = double, typename Value_, typename Index_>
std::vector<Output_> by_column(const tatami::Matrix<Value_, Index_>* p, const Options& vopt) {
    return by_column<Output_>(*p, vopt);
}

template<typename Output_ = double, typename Value_, typename Index_>
std::vector<Output_> by_column(const tatami::Matrix<Value_, Index_>& mat) {
    return by_column<Output_>(mat, {});
}

template<typename Output_ = double, typename Value_, typename Index_>
std::vector<Output_> by_column(const tatami::Matrix<Value_, Index_>* p) {
    return by_column<Output_>(*p);
}
/**
 * @endcond
 */

/**
 * Wrapper around `apply()` for column variances.
 *
 * @tparam Output_ Floating-point type of the output data.
 * This should be capable of storing NaNs.
 * @tparam Value_ Numeric type of the input data.
 * @tparam Index_ Integer type of the row/column indices.
 *
 * @param mat Instance of a `tatami::Matrix`.
 * @param vopt Variance calculation options.
 *
 * @return A vector of length equal to the number of rows, containing the row variances.
 */
template<typename Output_ = double, typename Value_, typename Index_>
std::vector<Output_> by_row(const tatami::Matrix<Value_, Index_>& mat, const Options& vopt) {
    auto output = tatami::create_container_of_Index_size<std::vector<Output_> >(mat.nrow());
    apply(true, mat, output.data(), vopt);
    return output;
}

/**
 * @cond
 */
// Back-compatibility.
template<typename Output_ = double, typename Value_, typename Index_>
std::vector<Output_> by_row(const tatami::Matrix<Value_, Index_>* p, const Options& vopt) {
    return by_row<Output_>(*p, vopt);
}

template<typename Output_ = double, typename Value_, typename Index_>
std::vector<Output_> by_row(const tatami::Matrix<Value_, Index_>& mat) {
    return by_row<Output_>(mat, {});
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
