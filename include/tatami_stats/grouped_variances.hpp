#ifndef TATAMI_STATS_GROUPED_VARIANCES_HPP
#define TATAMI_STATS_GROUPED_VARIANCES_HPP

#include "utils.hpp"
#include "variances.hpp"

#include <vector>
#include <algorithm>
#include <cstddef>

#include "tatami/tatami.hpp"
#include "sanisizer/sanisizer.hpp"

/**
 * @file grouped_variances.hpp
 *
 * @brief Compute group-wise variances from a `tatami::Matrix`.
 */

namespace tatami_stats {

/**
 * @brief Functions for computing dimension-wise grouped variances.
 * @namespace tatami_stats::grouped_variances
 */
namespace grouped_variances {

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
 * @cond
 */
template<typename Index_, typename Output_>
void finish_means(const std::size_t num_groups, const std::vector<Index_>& group_size, Output_* output_means) {
    for (std::size_t b = 0; b < num_groups; ++b) {
        if (group_size[b]) {
            output_means[b] /= group_size[b];
        } else {
            output_means[b] = std::numeric_limits<Output_>::quiet_NaN();
        }
    }
}

template<typename Index_, typename Output_>
void finish_variances(const std::size_t num_groups, const std::vector<Index_>& group_size, const std::vector<Output_>& rss, const Index_ i, Output_** output_variances) {
   for (std::size_t  b = 0; b < num_groups; ++b) {
       if (group_size[b] > 1) {
           output_variances[b][i] = rss[b] / static_cast<Output_>(group_size[b] - 1);
       } else {
           output_variances[b][i] = std::numeric_limits<Output_>::quiet_NaN();
       }
   }
}

template<typename Value_, typename Index_, typename Group_, typename Output_>
void apply_direct_noskip(
    const bool row,
    const tatami::Matrix<Value_, Index_>& mat, 
    const Group_* const group, 
    const std::size_t num_groups, 
    Output_** const output_variances,
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
            auto cur_variances = sanisizer::create<std::vector<Output_> >(num_groups);
            auto cur_non_zeros = sanisizer::create<std::vector<Index_> >(num_groups);

            for (Index_ x = 0; x < l; ++x) {
                auto range = ext->fetch(vbuffer.data(), ibuffer.data());

                // Computing the mean first.
                for (Index_ i = 0; i < range.number; ++i) {
                    const auto b = group[range.index[i]];
                    cur_means[b] += range.value[i];
                    ++cur_non_zeros[b];
                }
                finish_means(num_groups, group_sizes, cur_means.data());

                // Now computing the RSS.
                for (Index_ i = 0; i < range.number; ++i) {
                    const auto b = group[range.index[i]];
                    const auto delta = range.value[i] - cur_means[b];
                    cur_variances[b] += delta * delta;
                }
                for (std::size_t b = 0; b < num_groups; ++b) {
                    cur_variances[b] += cur_means[b] * cur_means[b] * (group_sizes[b] - cur_non_zeros[b]);
                }
                finish_variances(num_groups, group_sizes, cur_variances, static_cast<Index_>(s + x), output_variances);

                for (std::size_t g = 0; g < num_groups; ++g) {
                    std::fill(cur_means.begin(), cur_means.end(), 0);
                    std::fill(cur_variances.begin(), cur_variances.end(), 0);
                    std::fill(cur_non_zeros.begin(), cur_non_zeros.end(), 0);
                }
            }
        }, dim, vopt.num_threads);

    } else {
        tatami::parallelize([&](int, Index_ s, Index_ l) -> void {
            auto ext = tatami::consecutive_extractor<false>(mat, row, s, l);
            auto buffer = tatami::create_container_of_Index_size<std::vector<Value_> >(otherdim);
            auto cur_means = sanisizer::create<std::vector<Output_> >(num_groups);
            auto cur_variances = sanisizer::create<std::vector<Output_> >(num_groups);

            for (Index_ x = 0; x < l; ++x) {
                auto ptr = ext->fetch(buffer.data());

                // Computing the mean first.
                for (Index_ j = 0; j < otherdim; ++j) {
                    cur_means[group[j]] += ptr[j];
                }
                finish_means(num_groups, group_sizes, cur_means.data());

                // Now computing the RSS.
                for (Index_ j = 0; j < otherdim; ++j) {
                    const auto b = group[j];
                    const auto delta = ptr[j] - cur_means[b];
                    cur_variances[b] += delta * delta;
                }
                finish_variances(num_groups, group_sizes, cur_variances, static_cast<Index_>(s + x), output_variances);

                for (std::size_t g = 0; g < num_groups; ++g) {
                    std::fill(cur_means.begin(), cur_means.end(), 0);
                    std::fill(cur_variances.begin(), cur_variances.end(), 0);
                }
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
    Output_** const output_variances,
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
            auto cur_variances = sanisizer::create<std::vector<Output_> >(num_groups);
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
                finish_means(num_groups, cur_sizes, cur_means.data());

                // Computing the variance first.
                for (Index_ i = 0; i < range.number; ++i) {
                    const auto val = range.value[i];
                    if (!std::isnan(val)) {
                        const auto b = group[range.index[i]];
                        const auto delta = val - cur_means[b];
                        cur_variances[b] += delta * delta;
                    }
                }
                for (std::size_t b = 0; b < num_groups; ++b) {
                    cur_variances[b] += cur_means[b] * cur_means[b] * (cur_sizes[b] - cur_non_zeros[b]);
                }
                finish_variances(num_groups, cur_sizes, cur_variances, static_cast<Index_>(s + x), output_variances);

                for (std::size_t g = 0; g < num_groups; ++g) {
                    std::fill(cur_means.begin(), cur_means.end(), 0);
                    std::fill(cur_variances.begin(), cur_variances.end(), 0);
                    std::fill(cur_non_zeros.begin(), cur_non_zeros.end(), 0);
                    std::fill(cur_sizes.begin(), cur_sizes.end(), 0);
                }
            }
        }, dim, vopt.num_threads);

    } else {
        tatami::parallelize([&](int, Index_ s, Index_ l) -> void {
            auto ext = tatami::consecutive_extractor<false>(mat, row, s, l);
            auto buffer = tatami::create_container_of_Index_size<std::vector<Value_> >(otherdim);
            auto cur_means = sanisizer::create<std::vector<Output_> >(num_groups);
            auto cur_variances = sanisizer::create<std::vector<Output_> >(num_groups);
            auto cur_sizes = sanisizer::create<std::vector<Index_> >(num_groups);

            for (Index_ x = 0; x < l; ++x) {
                auto ptr = ext->fetch(buffer.data());

                // Computing the mean first.
                for (Index_ j = 0; j < otherdim; ++j) {
                    const auto val = ptr[j];
                    if (!std::isnan(val)) {
                        const auto b = group[j];
                        cur_means[b] += val;
                        ++cur_sizes[b];
                    }
                }
                finish_means(num_groups, cur_sizes, cur_means.data());

                // Computing the variance first.
                for (Index_ j = 0; j < otherdim; ++j) {
                    const auto val = ptr[j];
                    if (!std::isnan(val)) {
                        const auto b = group[j];
                        const auto delta = val - cur_means[b];
                        cur_variances[b] += delta * delta;
                    }
                }
                finish_variances(num_groups, cur_sizes, cur_variances, static_cast<Index_>(s + x), output_variances);

                for (std::size_t g = 0; g < num_groups; ++g) {
                    std::fill(cur_means.begin(), cur_means.end(), 0);
                    std::fill(cur_variances.begin(), cur_variances.end(), 0);
                    std::fill(cur_sizes.begin(), cur_sizes.end(), 0);
                }
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
    Output_** const output_variances,
    const Options& vopt
) {
    const auto dim = (row ? mat.nrow() : mat.ncol());
    const auto otherdim = (row ? mat.ncol() : mat.nrow());
    const bool is_sparse = mat.is_sparse();

    auto all_partial_rss = sanisizer::create<std::vector<std::vector<std::vector<Output_> > > >(vopt.num_threads);
    auto all_partial_mean = sanisizer::create<std::vector<std::vector<std::vector<Output_> > > >(vopt.num_threads);
    auto all_partial_count = sanisizer::create<std::vector<std::vector<Index_> > >(vopt.num_threads);

    const int nused = tatami::parallelize([&](int thread, Index_ s, Index_ l) -> void {
        // Storing the RSS's for the first thread in the output vector to save ourselves an allocation.
        Output_** rss_ptr;
        std::optional<std::vector<Output_*> >rss_ptrs;
        if (thread == 0) {
            for (std::size_t g = 0; g < num_groups; ++g) {
                std::fill_n(output_variances[g], dim, 0);
            }
            rss_ptr = output_variances;
        } else {
            rss_ptrs.emplace(sanisizer::cast<I<decltype(rss_ptrs->size())> >(num_groups));
            auto& cur_rss = all_partial_rss[thread];
            sanisizer::resize(cur_rss, num_groups);
            for (std::size_t g = 0; g < num_groups; ++g) {
                tatami::resize_container_to_Index_size(cur_rss[g], dim);
                (*rss_ptrs)[g] = cur_rss[g].data();
            }
            rss_ptr = rss_ptrs->data();
        }

        auto& cur_mean = all_partial_mean[thread];
        sanisizer::resize(cur_mean, num_groups);
        for (std::size_t g = 0; g < num_groups; ++g) {
            tatami::resize_container_to_Index_size(cur_mean[g], dim);
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
                runners.emplace_back(dim, cur_mean[g].data(), rss_ptr[g], nonzeros[g].data());
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
                runners.emplace_back(dim, cur_mean[g].data(), rss_ptr[g]);
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

    if (nused == 1) {
        const auto& cur_count = all_partial_count[0];
        for (std::size_t g = 0; g < num_groups; ++g) {
            quickstats::rss_to_variance(dim, cur_count[g], output_variances[g]);
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
        auto global_mean = sanisizer::create<std::vector<std::vector<Output_> > >(num_groups);
        for (std::size_t g = 0; g < num_groups; ++g) {
            auto& cur_global = global_mean[g];
            tatami::resize_container_to_Index_size(cur_global, dim);
            for (int u = 0; u < nused; ++u) {
                const auto& cur_mean = all_partial_mean[u][g];
                const Output_ mult = static_cast<Output_>(all_partial_count[u][g]) / static_cast<Output_>(global_count[g]);
                for (Index_ d = 0; d < dim; ++d) {
                    cur_global[d] += cur_mean[d] * mult;
                }
            }
        }

        // Combining the RSS. We need to use the safe variant of recenter_rss(), just to protect against the
        // case where a group has no observations within a particular thread. 
        for (std::size_t g = 0; g < num_groups; ++g) {
            const auto& cur_global = global_mean[g];
            const auto cur_output = output_variances[g];
            for (int u = 0; u < nused; ++u) {
                const auto cur_count = all_partial_count[u][g];
                const auto& cur_mean = all_partial_mean[u][g];
                if (u == 0) {
                    for (Index_ d = 0; d < dim; ++d) {
                        cur_output[d] = quickstats::recenter_rss(cur_count, cur_output[d], cur_mean[d], cur_global[d]); 
                    }
                } else {
                    const auto& cur_rss = all_partial_rss[u][g];
                    for (Index_ d = 0; d < dim; ++d) {
                        cur_output[d] += quickstats::recenter_rss(cur_count, cur_rss[d], cur_mean[d], cur_global[d]); 
                    }
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
    Output_** const output_variances,
    const Options& vopt
) {
    const auto dim = (row ? mat.nrow() : mat.ncol());
    const auto otherdim = (row ? mat.ncol() : mat.nrow());
    const bool is_sparse = mat.is_sparse();

    auto all_partial_rss = sanisizer::create<std::vector<std::vector<std::vector<Output_> > > >(vopt.num_threads);
    auto all_partial_mean = sanisizer::create<std::vector<std::vector<std::vector<Output_> > > >(vopt.num_threads);
    auto all_partial_count = sanisizer::create<std::vector<std::vector<std::vector<Index_> > > >(vopt.num_threads);

    const int nused = tatami::parallelize([&](int thread, Index_ s, Index_ l) -> void {
        // Storing the RSS's for the first thread in the output vector to save ourselves an allocation.
        Output_** rss_ptr;
        std::optional<std::vector<Output_*> >rss_ptrs;
        if (thread == 0) {
            for (std::size_t g = 0; g < num_groups; ++g) {
                std::fill_n(output_variances[g], dim, 0);
            }
            rss_ptr = output_variances;
        } else {
            rss_ptrs.emplace(sanisizer::cast<I<decltype(rss_ptrs->size())> >(num_groups));
            auto& cur_rss = all_partial_rss[thread];
            sanisizer::resize(cur_rss, num_groups);
            for (std::size_t g = 0; g < num_groups; ++g) {
                tatami::resize_container_to_Index_size(cur_rss[g], dim);
                (*rss_ptrs)[g] = cur_rss[g].data();
            }
            rss_ptr = rss_ptrs->data();
        }

        auto& cur_mean = all_partial_mean[thread];
        sanisizer::resize(cur_mean, num_groups);
        auto& cur_count = all_partial_count[thread];
        sanisizer::resize(cur_count, num_groups);
        for (std::size_t g = 0; g < num_groups; ++g) {
            tatami::resize_container_to_Index_size(cur_mean[g], dim);
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
                runners.emplace_back(dim, cur_mean[g].data(), rss_ptr[g], nonzeros[g].data(), cur_count[g].data());
            }

            for (Index_ x = 0; x < l; ++x) {
                auto out = ext->fetch(vbuffer.data(), ibuffer.data());
                runners[group[x + s]].add(
                    out.number,
                    out.value,
                    out.index,
                    [](const std::size_t, const Value_ val) -> bool { return std::isnan(val); }
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
                runners.emplace_back(dim, cur_mean[g].data(), rss_ptr[g], cur_count[g].data());
            }

            for (Index_ x = 0; x < l; ++x) {
                auto out = ext->fetch(buffer.data());
                runners[group[x + s]].add(
                    out,
                    [](const std::size_t, const Value_ val) -> bool { return std::isnan(val); }
                );
            }

            for (std::size_t g = 0; g < num_groups; ++g) {
                runners[g].finish();
            }
        }
    }, otherdim, vopt.num_threads);

    if (nused == 1) {
        const auto& cur_count = all_partial_count[0];
        for (std::size_t g = 0; g < num_groups; ++g) {
            quickstats::rss_to_variance(dim, cur_count[g].data(), output_variances[g]);
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
        auto global_mean = sanisizer::create<std::vector<std::vector<Output_> > >(num_groups);
        for (std::size_t g = 0; g < num_groups; ++g) {
            const auto& cur_global_count = global_count[g];
            auto& cur_global_mean = global_mean[g];
            tatami::resize_container_to_Index_size(cur_global_mean, dim);

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
            const auto& cur_global_mean = global_mean[g];
            const auto cur_output = output_variances[g];

            for (int u = 0; u < nused; ++u) {
                const auto& cur_mean = all_partial_mean[u][g];
                const auto& cur_count = all_partial_count[u][g];
                if (u == 0) {
                    for (Index_ d = 0; d < dim; ++d) {
                        cur_output[d] = quickstats::recenter_rss(cur_count[d], cur_output[d], cur_mean[d], cur_global_mean[d]); 
                    }
                } else {
                    const auto& cur_rss = all_partial_rss[u][g];
                    for (Index_ d = 0; d < dim; ++d) {
                        cur_output[d] += quickstats::recenter_rss(cur_count[d], cur_rss[d], cur_mean[d], cur_global_mean[d]); 
                    }
                }
            }

            quickstats::rss_to_variance(dim, global_count[g].data(), output_variances[g]);
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
 * @tparam Group_ Integer type of the group assignments for each row.
 * @tparam Output_ Floating-point type of the output value.
 * This should be capable of storing NaNs.
 *
 * @param row Whether to compute variances for the rows.
 * @param mat Instance of a `tatami::Matrix`.
 * @param[in] group Pointer to an array of length equal to the number of columns (if `row = true`) or rows (otherwise).
 * Each value should be an integer that specifies the group assignment.
 * Values should lie in \f$[0, N)\f$ where \f$N\f$ is the number of unique groups.
 * @param num_groups Number of groups, i.e., \f$N\f$.
 * This can be determined by calling `tatami_stats::total_groups()` on `group`.
 * @param[in] group_size Pointer to an array of length `num_groups`, containing the size of each group.
 * @param[out] output Pointer to an array of pointers of length equal to the number of groups.
 * Each inner pointer should reference an array of length equal to the number of rows (if `row = true`) or columns (otherwise).
 * On output, this will contain the row/column variances for each group (indexed according to the assignment in `group`).
 * @param sopt Summation options.
 */
template<typename Value_, typename Index_, typename Group_, typename Output_>
void apply(
    bool row,
    const tatami::Matrix<Value_, Index_>& mat,
    const Group_* group,
    std::size_t num_groups,
    const Index_*,
    Output_** const output,
    const Options& sopt
) {
    internal::nanable_ifelse<Value_>(
        sopt.skip_nan,
        [&]() -> void {
            if (mat.prefer_rows() == row) {
                apply_direct_skip(row, mat, group, num_groups, output, sopt);
            } else {
                apply_running_skip(row, mat, group, num_groups, output, sopt);
            }
        },
        [&]() -> void {
            if (mat.prefer_rows() == row) {
                apply_direct_noskip(row, mat, group, num_groups, output, sopt);
            } else {
                apply_running_noskip(row, mat, group, num_groups, output, sopt);
            }
        }
    );
}

/**
 * @cond
 */
// Back-compatibility.
template<typename Value_, typename Index_, typename Group_, typename Output_>
void apply(bool row, const tatami::Matrix<Value_, Index_>* p, const Group_* group, std::size_t num_groups, const Index_* group_size, Output_** output, const Options& sopt) {
    apply(row, *p, group, num_groups, group_size, output, sopt);
}
/**
 * @endcond
 */

/**
 * Wrapper around `apply()` for row-wise grouped variances.
 *
 * @tparam Value_ Numeric type of the matrix value.
 * @tparam Index_ Integer type of the row/column indices.
 * @tparam Group_ Integer type of the group assignments for each row.
 * @tparam Output_ Floating-point type of the output value.
 * This should be capable of storing NaNs.
 *
 * @param mat Instance of a `tatami::Matrix`.
 * @param[in] group Pointer to an array of length equal to the number of columns.
 * Each value should be an integer that specifies the group assignment.
 * Values should lie in \f$[0, N)\f$ where \f$N\f$ is the number of unique groups.
 * @param sopt Summation options.
 *
 * @return Vector of length equal to the number of groups.
 * Each entry is a vector of length equal to the number of rows, containing the row-wise variances for the corresponding group.
 */
template<typename Output_ = double, typename Value_, typename Index_, typename Group_>
std::vector<std::vector<Output_> > by_row(const tatami::Matrix<Value_, Index_>& mat, const Group_* group, const Options& sopt) {
    auto mydim = mat.nrow();
    auto group_size = tabulate_groups(group, mat.ncol());
    auto ngroup = group_size.size();

    auto output = sanisizer::create<std::vector<std::vector<Output_> > >(ngroup);
    std::vector<Output_*> ptrs;
    ptrs.reserve(output.size());
    for (auto& o : output) {
        o.resize(mydim);
        ptrs.push_back(o.data());
    }

    apply(true, mat, group, ngroup, group_size.data(), ptrs.data(), sopt);
    return output;
}

/**
 * @cond
 */
// Back-compatibility.
template<typename Output_ = double, typename Value_, typename Index_, typename Group_>
std::vector<std::vector<Output_> > by_row(const tatami::Matrix<Value_, Index_>* p, const Group_* group, const Options& sopt) {
    return by_row<Output_>(*p, group, sopt);
}

template<typename Output_ = double, typename Value_, typename Index_, typename Group_>
std::vector<std::vector<Output_> > by_row(const tatami::Matrix<Value_, Index_>& mat, const Group_* group) {
    return by_row<Output_>(mat, group, Options());
}

template<typename Output_ = double, typename Value_, typename Index_, typename Group_>
std::vector<std::vector<Output_> > by_row(const tatami::Matrix<Value_, Index_>* p, const Group_* group) {
    return by_row<Output_>(*p, group);
}
/**
 * @endcond
 */

/**
 * Wrapper around `apply()` for column-wise grouped variances.
 *
 * @tparam Value_ Numeric type of the matrix value.
 * @tparam Index_ Integer type of the row/column indices.
 * @tparam Group_ Integer type of the group assignments for each row.
 * @tparam Output_ Floating-point type of the output value.
 * This should be capable of storing NaNs.
 *
 * @param mat Instance of a `tatami::Matrix`.
 * @param[in] group Pointer to an array of length equal to the number of rows.
 * Each value should be an integer that specifies the group assignment.
 * Values should lie in \f$[0, N)\f$ where \f$N\f$ is the number of unique groups.
 * @param sopt Summation options.
 *
 * @return Vector of length equal to the number of groups.
 * Each entry is a vector of length equal to the number of columns, containing the column-wise variances for the corresponding group.
 */
template<typename Output_ = double, typename Value_, typename Index_, typename Group_>
std::vector<std::vector<Output_> > by_column(const tatami::Matrix<Value_, Index_>& mat, const Group_* group, const Options& sopt) {
    auto mydim = mat.ncol();
    auto group_size = tabulate_groups(group, mat.nrow());
    auto ngroup = group_size.size();

    auto output = sanisizer::create<std::vector<std::vector<Output_> > >(ngroup);
    std::vector<Output_*> ptrs;
    ptrs.reserve(output.size());
    for (auto& o : output) {
        o.resize(mydim);
        ptrs.push_back(o.data());
    }

    apply(false, mat, group, ngroup, group_size.data(), ptrs.data(), sopt);
    return output;
}

/**
 * @cond
 */
// Back-compatibility.
template<typename Output_ = double, typename Value_, typename Index_, typename Group_>
std::vector<std::vector<Output_> > by_column(const tatami::Matrix<Value_, Index_>* p, const Group_* group, const Options& sopt) {
    return by_column<Output_>(*p, group, sopt);
}

template<typename Output_ = double, typename Value_, typename Index_, typename Group_>
std::vector<std::vector<Output_> > by_column(const tatami::Matrix<Value_, Index_>& mat, const Group_* group) {
    return by_column<Output_>(mat, group, Options());
}

template<typename Output_ = double, typename Value_, typename Index_, typename Group_>
std::vector<std::vector<Output_> > by_column(const tatami::Matrix<Value_, Index_>* p, const Group_* group) {
    return by_column<Output_>(*p, group);
}
/**
 * @endcond
 */

}

}

#endif
