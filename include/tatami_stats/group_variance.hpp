#ifndef TATAMI_STATS_GROUP_VARIANCE_HPP
#define TATAMI_STATS_GROUP_VARIANCE_HPP

#include "utils.hpp"

#include <vector>
#include <algorithm>
#include <cstddef>
#include <optional>
#include <cassert>

#include "tatami/tatami.hpp"
#include "sanisizer/sanisizer.hpp"

/**
 * @file group_variance.hpp
 *
 * @brief Compute group-wise variances from a `tatami::Matrix`.
 */

namespace tatami_stats {

/**
 * @brief Functions for computing dimension-wise per-group variances.
 * @namespace tatami_stats::group_variance
 */
namespace group_variance {

/**
 * @brief Grouped summation options.
 */
struct Options {
    /**
     * Whether to check for NaNs in the input, and skip them.
     * If false, NaNs are assumed to be absent, and the behavior of the summation in the presence of NaNs is undefined.
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
     * Vector of length equal to the number of groups.
     * Each element is a pointer to an array of length equal to the appropriate dimension extent (rows for `row = true`, columns otherwise).
     * After `apply()`, this is filled with the sample mean of each row/column for the corresponding group.
     */
    std::vector<Output_*> mean;

    /**
     * Vector of length equal to the number of groups.
     * Each element is a pointer to an array of length equal to the appropriate dimension extent (rows for `row = true`, columns otherwise).
     * After `apply()`, this is filled with the sample variance of each row/column for the corresponding group.
     */
    std::vector<Output_*> variance;
};

/**
 * @cond
 */
template<typename Index_, typename Output_>
void finish_means(const std::size_t num_groups, const std::vector<Index_>& group_size, std::vector<Output_>& means, const Index_ i, std::vector<Output_*>& output_means) {
    for (std::size_t b = 0; b < num_groups; ++b) {
        if (group_size[b]) {
            means[b] /= group_size[b];
        } else {
            means[b] = std::numeric_limits<Output_>::quiet_NaN();
        }
        output_means[b][i] = means[b];
    }
}

template<typename Value_, typename Index_, typename Group_, typename Output_>
void apply_direct_noskip(
    const bool row,
    const tatami::Matrix<Value_, Index_>& mat, 
    const Group_* const group, 
    const std::size_t num_groups, 
    Buffers<Output_>& output,
    const Options& vopt
) {
    const auto dim = (row ? mat.nrow() : mat.ncol());
    const auto otherdim = (row ? mat.ncol() : mat.nrow());

    auto group_sizes = sanisizer::create<std::vector<Index_> >(num_groups);
    for (Index_ i = 0; i < otherdim; ++i) {
        group_sizes[group[i]] += 1;
    }

    if (mat.sparse()) {
        tatami::parallelize([&](int, Index_ s, Index_ l) -> void {
            auto ext = tatami::consecutive_extractor<true>(mat, row, s, l);
            auto vbuffer = tatami::create_container_of_Index_size<std::vector<Value_> >(otherdim);
            auto ibuffer = tatami::create_container_of_Index_size<std::vector<Index_> >(otherdim);
            auto cur_means = sanisizer::create<std::vector<Output_> >(num_groups);
            auto cur_rss = sanisizer::create<std::vector<Output_> >(num_groups);
            auto cur_non_zeros = sanisizer::create<std::vector<Index_> >(num_groups);

            for (Index_ x = 0; x < l; ++x) {
                auto range = ext->fetch(vbuffer.data(), ibuffer.data());

                // Computing the mean first.
                for (Index_ i = 0; i < range.number; ++i) {
                    const auto g = group[range.index[i]];
                    cur_means[g] += range.value[i];
                    ++cur_non_zeros[g];
                }
                finish_means(num_groups, group_sizes, cur_means, static_cast<Index_>(s + x), output.mean);

                // Now computing the RSS.
                for (Index_ i = 0; i < range.number; ++i) {
                    const auto g = group[range.index[i]];
                    const auto delta = range.value[i] - cur_means[g];
                    cur_rss[g] += delta * delta;
                }
                for (std::size_t g = 0; g < num_groups; ++g) {
                    const Output_ my_rss = cur_rss[g] + cur_means[g] * cur_means[g] * (group_sizes[g] - cur_non_zeros[g]);
                    output.variance[g][s + x] = quickstats::rss_to_variance(group_sizes[g], my_rss);
                }

                std::fill(cur_means.begin(), cur_means.end(), 0);
                std::fill(cur_rss.begin(), cur_rss.end(), 0);
                std::fill(cur_non_zeros.begin(), cur_non_zeros.end(), 0);
            }
        }, dim, vopt.num_threads);

    } else {
        tatami::parallelize([&](int, Index_ s, Index_ l) -> void {
            auto ext = tatami::consecutive_extractor<false>(mat, row, s, l);
            auto buffer = tatami::create_container_of_Index_size<std::vector<Value_> >(otherdim);
            auto cur_means = sanisizer::create<std::vector<Output_> >(num_groups);
            auto cur_rss = sanisizer::create<std::vector<Output_> >(num_groups);

            for (Index_ x = 0; x < l; ++x) {
                auto ptr = ext->fetch(buffer.data());

                // Computing the mean first.
                for (Index_ j = 0; j < otherdim; ++j) {
                    cur_means[group[j]] += ptr[j];
                }
                finish_means(num_groups, group_sizes, cur_means, static_cast<Index_>(s + x), output.mean);

                // Now computing the RSS.
                for (Index_ j = 0; j < otherdim; ++j) {
                    const auto g = group[j];
                    const auto delta = ptr[j] - cur_means[g];
                    cur_rss[g] += delta * delta;
                }
                for (std::size_t g = 0; g < num_groups; ++g) {
                    output.variance[g][s + x] = quickstats::rss_to_variance(group_sizes[g], cur_rss[g]);
                }

                std::fill(cur_means.begin(), cur_means.end(), 0);
                std::fill(cur_rss.begin(), cur_rss.end(), 0);
            }
        }, dim, vopt.num_threads);
    }
}

template<typename Value_, typename Index_, typename Group_, typename Output_>
void apply_direct_skip(
    const bool row,
    const tatami::Matrix<Value_, Index_>& mat, 
    const Group_* const group, 
    const std::size_t num_groups, 
    Buffers<Output_>& output,
    const Options& vopt
) {
    const auto dim = (row ? mat.nrow() : mat.ncol());
    const auto otherdim = (row ? mat.ncol() : mat.nrow());

    if (mat.sparse()) {
        auto full_group_sizes = sanisizer::create<std::vector<Index_> >(num_groups);
        for (Index_ i = 0; i < otherdim; ++i) {
            full_group_sizes[group[i]] += 1;
        }

        tatami::parallelize([&](int, Index_ s, Index_ l) -> void {
            auto ext = tatami::consecutive_extractor<true>(mat, row, s, l);
            auto vbuffer = tatami::create_container_of_Index_size<std::vector<Value_> >(otherdim);
            auto ibuffer = tatami::create_container_of_Index_size<std::vector<Index_> >(otherdim);
            auto cur_means = sanisizer::create<std::vector<Output_> >(num_groups);
            auto cur_rss = sanisizer::create<std::vector<Output_> >(num_groups);
            auto cur_non_zeros = sanisizer::create<std::vector<Index_> >(num_groups);
            auto cur_sizes = sanisizer::create<std::vector<Index_> >(num_groups);

            for (Index_ x = 0; x < l; ++x) {
                auto range = ext->fetch(vbuffer.data(), ibuffer.data());

                // Computing the mean first.
                for (Index_ i = 0; i < range.number; ++i) {
                    const auto val = range.value[i];
                    const auto b = group[range.index[i]];
                    if (!std::isnan(val)) {
                        ++cur_non_zeros[b];
                        cur_means[b] += val;
                    } else {
                        ++cur_sizes[b];
                    }
                }
                for (std::size_t g = 0; g < num_groups; ++g) {
                    cur_sizes[g] = full_group_sizes[g] - cur_sizes[g];
                }
                finish_means(num_groups, cur_sizes, cur_means, static_cast<Index_>(s + x), output.mean);

                // Computing the variance first.
                for (Index_ i = 0; i < range.number; ++i) {
                    const auto val = range.value[i];
                    if (!std::isnan(val)) {
                        const auto g = group[range.index[i]];
                        const auto delta = val - cur_means[g];
                        cur_rss[g] += delta * delta;
                    }
                }
                for (std::size_t g = 0; g < num_groups; ++g) {
                    const Output_ my_rss = cur_rss[g] + cur_means[g] * cur_means[g] * (cur_sizes[g] - cur_non_zeros[g]);
                    output.variance[g][s + x] = quickstats::rss_to_variance(cur_sizes[g], my_rss);
                }

                std::fill(cur_means.begin(), cur_means.end(), 0);
                std::fill(cur_rss.begin(), cur_rss.end(), 0);
                std::fill(cur_non_zeros.begin(), cur_non_zeros.end(), 0);
                std::fill(cur_sizes.begin(), cur_sizes.end(), 0);
            }
        }, dim, vopt.num_threads);

    } else {
        tatami::parallelize([&](int, Index_ s, Index_ l) -> void {
            auto ext = tatami::consecutive_extractor<false>(mat, row, s, l);
            auto buffer = tatami::create_container_of_Index_size<std::vector<Value_> >(otherdim);
            auto cur_means = sanisizer::create<std::vector<Output_> >(num_groups);
            auto cur_rss = sanisizer::create<std::vector<Output_> >(num_groups);
            auto cur_sizes = sanisizer::create<std::vector<Index_> >(num_groups);

            for (Index_ x = 0; x < l; ++x) {
                auto ptr = ext->fetch(buffer.data());

                // Computing the mean first.
                for (Index_ j = 0; j < otherdim; ++j) {
                    const auto val = ptr[j];
                    if (!std::isnan(val)) {
                        const auto g = group[j];
                        cur_means[g] += val;
                        ++cur_sizes[g];
                    }
                }
                finish_means(num_groups, cur_sizes, cur_means, static_cast<Index_>(s + x), output.mean);

                // Computing the variance first.
                for (Index_ j = 0; j < otherdim; ++j) {
                    const auto val = ptr[j];
                    if (!std::isnan(val)) {
                        const auto g = group[j];
                        const auto delta = val - cur_means[g];
                        cur_rss[g] += delta * delta;
                    }
                }
                for (std::size_t g = 0; g < num_groups; ++g) {
                    output.variance[g][s + x] = quickstats::rss_to_variance(cur_sizes[g], cur_rss[g]);
                }

                std::fill(cur_means.begin(), cur_means.end(), 0);
                std::fill(cur_rss.begin(), cur_rss.end(), 0);
                std::fill(cur_sizes.begin(), cur_sizes.end(), 0);
            }
        }, dim, vopt.num_threads);
    }
}

template<typename Value_, typename Index_, typename Group_, typename Output_>
void apply_running_noskip(
    const bool row,
    const tatami::Matrix<Value_, Index_>& mat,
    const Group_* const group, 
    const std::size_t num_groups, 
    Buffers<Output_>& output,
    const Options& vopt
) {
    const auto dim = (row ? mat.nrow() : mat.ncol());
    const auto otherdim = (row ? mat.ncol() : mat.nrow());
    const bool is_sparse = mat.is_sparse();

    const bool do_parallel = vopt.num_threads > 1;
    std::vector<std::vector<std::vector<Output_> > > all_partial_mean, all_partial_rss;
    if (do_parallel) {
        sanisizer::resize(all_partial_mean, vopt.num_threads);
        sanisizer::resize(all_partial_rss, vopt.num_threads);
    }
    auto all_partial_count = sanisizer::create<std::vector<std::vector<Index_> > >(vopt.num_threads);

    const int nused = tatami::parallelize([&](int thread, Index_ s, Index_ l) -> void {
        Output_** mean_ptrs, **rss_ptrs;
        std::optional<std::vector<Output_*> > tmp_mean_ptrs, tmp_rss_ptrs;
        if (!do_parallel) {
            mean_ptrs = output.mean.data();
            rss_ptrs = output.variance.data();
            for (std::size_t g = 0; g < num_groups; ++g) {
                std::fill_n(mean_ptrs[g], dim, 0);
                std::fill_n(rss_ptrs[g], dim, 0);
            }
        } else {
            auto& cur_mean = all_partial_mean[thread];
            sanisizer::resize(cur_mean, num_groups);
            auto& cur_rss = all_partial_rss[thread];
            sanisizer::resize(cur_rss, num_groups);

            tmp_mean_ptrs.emplace(sanisizer::cast<I<decltype(tmp_mean_ptrs->size())> >(num_groups));
            tmp_rss_ptrs.emplace(sanisizer::cast<I<decltype(tmp_rss_ptrs->size())> >(num_groups));
            for (std::size_t g = 0; g < num_groups; ++g) {
                tatami::resize_container_to_Index_size(cur_mean[g], dim);
                (*tmp_mean_ptrs)[g] = cur_mean[g].data();
                tatami::resize_container_to_Index_size(cur_rss[g], dim);
                (*tmp_rss_ptrs)[g] = cur_rss[g].data();
            }
            mean_ptrs = tmp_mean_ptrs->data();
            rss_ptrs = tmp_rss_ptrs->data();
        }

        auto& cur_count = all_partial_count[thread];
        sanisizer::resize(cur_count, num_groups);
        for (Index_ x = 0; x < l; ++x) {
            cur_count[group[x + s]] += 1;
        }

        if (is_sparse) {
            auto ext = tatami::consecutive_extractor<true>(mat, !row, s, l);
            auto vbuffer = tatami::create_container_of_Index_size<std::vector<Value_> >(dim);
            auto ibuffer = tatami::create_container_of_Index_size<std::vector<Index_> >(dim);

            auto nonzeros = sanisizer::create<std::vector<std::vector<Index_> > >(num_groups);
            std::vector<quickstats::RssRunningSparse<Index_, Value_, Output_> > runners;
            sanisizer::reserve(runners, num_groups);
            for (std::size_t g = 0; g < num_groups; ++g) {
                tatami::resize_container_to_Index_size(nonzeros[g], dim);
                runners.emplace_back(dim, mean_ptrs[g], rss_ptrs[g], nonzeros[g].data());
            }

            for (Index_ x = 0; x < l; ++x) {
                auto out = ext->fetch(vbuffer.data(), ibuffer.data());
                runners[group[x + s]].add(out.number, out.value, out.index);
            }

            for (std::size_t g = 0; g < num_groups; ++g) {
                runners[g].finish();
            }

        } else {
            auto ext = tatami::consecutive_extractor<false>(mat, !row, s, l);
            auto buffer = tatami::create_container_of_Index_size<std::vector<Value_> >(dim);

            std::vector<quickstats::RssRunningDense<Value_, Output_> > runners;
            sanisizer::reserve(runners, num_groups);
            for (std::size_t g = 0; g < num_groups; ++g) {
                runners.emplace_back(dim, mean_ptrs[g], rss_ptrs[g]);
            }

            for (Index_ x = 0; x < l; ++x) {
                auto out = ext->fetch(buffer.data());
                runners[group[x + s]].add(out);
            }

            for (std::size_t g = 0; g < num_groups; ++g) {
                runners[g].finish();
            }
        }
    }, otherdim, vopt.num_threads);

    if (!do_parallel) {
        const auto& cur_count = all_partial_count[0];
        for (std::size_t g = 0; g < num_groups; ++g) {
            quickstats::rss_to_variance(dim, cur_count[g], output.variance[g]);
        }

    } else {
        auto global_count = sanisizer::create<std::vector<Index_> >(num_groups);
        for (int u = 0; u < nused; ++u) {
            const auto& cur_count = all_partial_count[u];
            for (std::size_t g = 0; g < num_groups; ++g) {
                global_count[g] += cur_count[g];
            }
        }

        // Computing the global mean.
        for (std::size_t g = 0; g < num_groups; ++g) {
            const auto cur_output = output.mean[g];
            for (int u = 0; u < nused; ++u) {
                const auto& cur_mean = all_partial_mean[u][g];
                const Output_ mult = static_cast<Output_>(all_partial_count[u][g]) / static_cast<Output_>(global_count[g]);
                for (Index_ d = 0; d < dim; ++d) {
                    cur_output[d] += cur_mean[d] * mult;
                }
            }
        }

        // Combining the RSS. We need to use the safe variant of recenter_rss(), just to protect against the
        // case where a group has no observations within a particular thread. 
        for (std::size_t g = 0; g < num_groups; ++g) {
            const auto& cur_global = output.mean[g];
            const auto cur_output = output.variance[g];
            for (int u = 0; u < nused; ++u) {
                const auto cur_count = all_partial_count[u][g];
                const auto& cur_mean = all_partial_mean[u][g];
                const auto& cur_rss = all_partial_rss[u][g];
                for (Index_ d = 0; d < dim; ++d) {
                    cur_output[d] += quickstats::recenter_rss(cur_count, cur_rss[d], cur_mean[d], cur_global[d]); 
                }
            }
            quickstats::rss_to_variance(dim, global_count[g], cur_output);
        }
    }
}

template<typename Value_, typename Index_, typename Group_, typename Output_>
void apply_running_skip(
    const bool row,
    const tatami::Matrix<Value_, Index_>& mat,
    const Group_* const group, 
    const std::size_t num_groups, 
    Buffers<Output_>& output,
    const Options& vopt
) {
    const auto dim = (row ? mat.nrow() : mat.ncol());
    const auto otherdim = (row ? mat.ncol() : mat.nrow());
    const bool is_sparse = mat.is_sparse();

    const bool do_parallel = vopt.num_threads > 1;
    std::vector<std::vector<std::vector<Output_> > > all_partial_mean, all_partial_rss;
    if (do_parallel) {
        sanisizer::resize(all_partial_mean, vopt.num_threads);
        sanisizer::resize(all_partial_rss, vopt.num_threads);
    }
    auto all_partial_count = sanisizer::create<std::vector<std::vector<std::vector<Index_> > > >(vopt.num_threads);

    const int nused = tatami::parallelize([&](int thread, Index_ s, Index_ l) -> void {
        Output_** mean_ptrs, **rss_ptrs;
        std::optional<std::vector<Output_*> > tmp_mean_ptrs, tmp_rss_ptrs;
        if (!do_parallel) {
            mean_ptrs = output.mean.data();
            rss_ptrs = output.variance.data();
            for (std::size_t g = 0; g < num_groups; ++g) {
                std::fill_n(mean_ptrs[g], dim, 0);
                std::fill_n(rss_ptrs[g], dim, 0);
            }
        } else {
            auto& cur_mean = all_partial_mean[thread];
            sanisizer::resize(cur_mean, num_groups);
            auto& cur_rss = all_partial_rss[thread];
            sanisizer::resize(cur_rss, num_groups);

            tmp_mean_ptrs.emplace(sanisizer::cast<I<decltype(tmp_mean_ptrs->size())> >(num_groups));
            tmp_rss_ptrs.emplace(sanisizer::cast<I<decltype(tmp_rss_ptrs->size())> >(num_groups));
            for (std::size_t g = 0; g < num_groups; ++g) {
                tatami::resize_container_to_Index_size(cur_mean[g], dim);
                (*tmp_mean_ptrs)[g] = cur_mean[g].data();
                tatami::resize_container_to_Index_size(cur_rss[g], dim);
                (*tmp_rss_ptrs)[g] = cur_rss[g].data();
            }
            mean_ptrs = tmp_mean_ptrs->data();
            rss_ptrs = tmp_rss_ptrs->data();
        }

        auto& cur_count = all_partial_count[thread];
        sanisizer::resize(cur_count, num_groups);
        for (std::size_t g = 0; g < num_groups; ++g) {
            tatami::resize_container_to_Index_size(cur_count[g], dim);
        }

        if (is_sparse) {
            auto ext = tatami::consecutive_extractor<true>(mat, !row, s, l);
            auto vbuffer = tatami::create_container_of_Index_size<std::vector<Value_> >(dim);
            auto ibuffer = tatami::create_container_of_Index_size<std::vector<Index_> >(dim);

            auto nonzeros = sanisizer::create<std::vector<std::vector<Index_> > >(num_groups);
            std::vector<quickstats::RssRunningSparseSkip<Index_, Value_, Output_> > runners;
            sanisizer::reserve(runners, num_groups);
            for (std::size_t g = 0; g < num_groups; ++g) {
                tatami::resize_container_to_Index_size(nonzeros[g], dim);
                runners.emplace_back(dim, mean_ptrs[g], rss_ptrs[g], nonzeros[g].data(), cur_count[g].data());
            }

            for (Index_ x = 0; x < l; ++x) {
                auto out = ext->fetch(vbuffer.data(), ibuffer.data());
                runners[group[x + s]].add(
                    out.number,
                    out.value,
                    out.index,
                    [](const std::size_t, const Value_ val) -> bool {
                        return std::isnan(val);
                    }
                );
            }

            for (std::size_t g = 0; g < num_groups; ++g) {
                runners[g].finish();
            }

        } else {
            auto ext = tatami::consecutive_extractor<false>(mat, !row, s, l);
            auto buffer = tatami::create_container_of_Index_size<std::vector<Value_> >(dim);

            std::vector<quickstats::RssRunningDenseSkip<Index_, Value_, Output_> > runners;
            sanisizer::reserve(runners, num_groups);
            for (std::size_t g = 0; g < num_groups; ++g) {
                runners.emplace_back(dim, mean_ptrs[g], rss_ptrs[g], cur_count[g].data());
            }

            for (Index_ x = 0; x < l; ++x) {
                auto out = ext->fetch(buffer.data());
                runners[group[x + s]].add(
                    out,
                    [](const std::size_t, const Value_ val) -> bool {
                        return std::isnan(val);
                    }
                );
            }

            for (std::size_t g = 0; g < num_groups; ++g) {
                runners[g].finish();
            }
        }
    }, otherdim, vopt.num_threads);

    if (!do_parallel) {
        const auto& cur_count = all_partial_count[0];
        for (std::size_t g = 0; g < num_groups; ++g) {
            quickstats::rss_to_variance(dim, cur_count[g].data(), output.variance[g]);
        }

    } else {
        auto global_count = sanisizer::create<std::vector<std::vector<Index_> > >(num_groups);
        for (std::size_t g = 0; g < num_groups; ++g) {
            auto& cur_global_count = global_count[g];
            tatami::resize_container_to_Index_size(cur_global_count, dim);
            for (int u = 0; u < nused; ++u) {
                const auto& cur_count = all_partial_count[u][g];
                for (Index_ d = 0; d < dim; ++d) {
                    cur_global_count[d] += cur_count[d];
                }
            }
        }

        // Computing the global mean.
        for (std::size_t g = 0; g < num_groups; ++g) {
            const auto& cur_global_count = global_count[g];
            const auto cur_global_mean = output.mean[g];
            for (int u = 0; u < nused; ++u) {
                const auto& cur_mean = all_partial_mean[u][g];
                const auto& cur_count = all_partial_count[u][g];
                for (Index_ d = 0; d < dim; ++d) {
                    const auto mult = static_cast<Output_>(cur_count[d]) / static_cast<Output_>(cur_global_count[d]);
                    cur_global_mean[d] += cur_mean[d] * mult;
                }
            }
        }

        // Combining the RSS. We need to use the safe variant of recenter_rss(), just to protect against the
        // case where a group has no observations within a particular thread. 
        for (std::size_t g = 0; g < num_groups; ++g) {
            const auto cur_global_mean = output.mean[g];
            const auto cur_output = output.variance[g];
            for (int u = 0; u < nused; ++u) {
                const auto& cur_mean = all_partial_mean[u][g];
                const auto& cur_count = all_partial_count[u][g];
                const auto& cur_rss = all_partial_rss[u][g];
                for (Index_ d = 0; d < dim; ++d) {
                    cur_output[d] += quickstats::recenter_rss(cur_count[d], cur_rss[d], cur_mean[d], cur_global_mean[d]); 
                }
            }

            quickstats::rss_to_variance(dim, global_count[g].data(), output.variance[g]);
        }
    }
}
/**
 * @endcond
 */

/**
 * Compute per-group variances for each element of a chosen dimension of a `tatami::Matrix`.
 *
 * @tparam Value_ Numeric type of the matrix value.
 * @tparam Index_ Integer type of the row/column indices.
 * @tparam Group_ Integer type of the group assignments for each row/column.
 * @tparam Output_ Floating-point type of the output value.
 * This should be capable of storing NaNs.
 *
 * @param row Whether to compute variances for the rows.
 * @param mat Instance of a `tatami::Matrix`.
 * @param[in] group Pointer to an array of length equal to the number of columns (if `row = true`) or rows (otherwise).
 * Each value should be an integer that specifies the group assignment.
 * Values should lie in \f$[0, N)\f$ where \f$N\f$ is the number of unique groups.
 * @param num_groups Number of groups, i.e., \f$N\f$.
 * @param[out] output Pointer to an array of pointers of length equal to the number of groups.
 * Each inner pointer should reference an array of length equal to the number of rows (if `row = true`) or columns (otherwise).
 * On output, this will contain the row/column variances for each group (indexed according to the assignment in `group`).
 * @param opt Further options.
 */
template<typename Value_, typename Index_, typename Group_, typename Output_>
void apply(
    bool row,
    const tatami::Matrix<Value_, Index_>& mat,
    const Group_* group,
    std::size_t num_groups,
    Buffers<Output_>& output,
    const Options& opt
) {
    assert(sanisizer::is_equal(num_groups, output.mean.size()));
    assert(sanisizer::is_equal(num_groups, output.variance.size()));

    internal::nanable_ifelse<Value_>(
        opt.skip_nan,
        [&]() -> void {
            if (mat.prefer_rows() == row) {
                apply_direct_skip(row, mat, group, num_groups, output, opt);
            } else {
                apply_running_skip(row, mat, group, num_groups, output, opt);
            }
        },
        [&]() -> void {
            if (mat.prefer_rows() == row) {
                apply_direct_noskip(row, mat, group, num_groups, output, opt);
            } else {
                apply_running_noskip(row, mat, group, num_groups, output, opt);
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
     * Vector of length equal to the number of groups.
     * Each element is a vector of length equal to the appropriate dimension extent (rows for `row = true`, columns otherwise),
     * containing the sample mean of each row/column.
     */
    std::vector<std::vector<Output_> > mean;

    /**
     * Vector of length equal to the number of groups.
     * Each element is a vector of length equal to the appropriate dimension extent (rows for `row = true`, columns otherwise),
     * containing the sample variance of each row/column.
     */
    std::vector<std::vector<Output_> > variance;
};

/**
 * Compute per-group variances for each element of a chosen dimension of a `tatami::Matrix`.
 *
 * @tparam Output_ Floating-point type of the output value.
 * This should be capable of storing NaNs.
 * @tparam Value_ Numeric type of the matrix value.
 * @tparam Index_ Integer type of the row/column indices.
 * @tparam Group_ Integer type of the group assignments for each row/column.
 *
 * @param row Whether to compute variances for the rows.
 * @param mat Instance of a `tatami::Matrix`.
 * @param[in] group Pointer to an array of length equal to the number of columns (if `row = true`) or rows (otherwise).
 * Each value should be an integer that specifies the group assignment.
 * Values should lie in \f$[0, N)\f$ where \f$N\f$ is the number of unique groups.
 * @param num_groups Number of groups, i.e., \f$N\f$.
 * @param[out] output Pointer to an array of pointers of length equal to the number of groups.
 * Each inner pointer should reference an array of length equal to the number of rows (if `row = true`) or columns (otherwise).
 * On output, this will contain the row/column variances for each group (indexed according to the assignment in `group`).
 * @param opt Further options.
 */
template<typename Output_ = double, typename Value_, typename Index_, typename Group_> 
Result<Output_> apply(
    bool row,
    const tatami::Matrix<Value_, Index_>& mat,
    const Group_* group,
    std::size_t num_groups,
    const Options& opt
) {
    Result<Output_> output;
    sanisizer::resize(output.mean, num_groups);
    sanisizer::resize(output.variance, num_groups);

    Buffers<Output_> buffers;
    sanisizer::resize(buffers.mean, num_groups);
    sanisizer::resize(buffers.variance, num_groups);
    const auto dim = (row ? mat.nrow() : mat.ncol());

    for (std::size_t g = 0; g < num_groups; ++g) {
        tatami::resize_container_to_Index_size(output.mean[g], dim); 
        buffers.mean[g] = output.mean[g].data();
        tatami::resize_container_to_Index_size(output.variance[g], dim); 
        buffers.variance[g] = output.variance[g].data();
    }

    apply(row, mat, group, num_groups, buffers, opt);
    return output;
}

}

}

#endif
