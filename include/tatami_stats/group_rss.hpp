#ifndef TATAMI_STATS_GROUP_RSS_HPP
#define TATAMI_STATS_GROUP_RSS_HPP

#include "utils.hpp"

#include <vector>
#include <algorithm>
#include <cstddef>
#include <optional>
#include <cassert>
#include <limits>
#include <cmath>

#include "tatami/tatami.hpp"
#include "sanisizer/sanisizer.hpp"
#include "quickstats/quickstats.hpp"

/**
 * @file group_rss.hpp
 *
 * @brief Compute group-wise residual sum of squares from a `tatami::Matrix`.
 */

namespace tatami_stats {

/**
 * @brief Options for `group_rss()`.
 */
struct GroupRssOptions {
    /**
     * Number of threads to use when computing RSS values across a `tatami::Matrix`.
     * See `tatami::parallelize()` for more details on the parallelization mechanism.
     */
    int num_threads = 1;
};

/**
 * @brief Result buffers for `group_rss()`.
 *
 * @tparam Output_ Floating-point type of the output data.
 * This should be capable of storing NaNs.
 * @tparam Count_ Numeric type of the group sizes, typically integer.
 */
template<typename Output_, typename Count_>
struct GroupRssBuffers {
    /**
     * Vector of length equal to the number of groups.
     * Each element is a pointer to an array of length equal to the appropriate dimension extent (rows for `row = true`, columns otherwise).
     * After `group_rss()`, this is filled with the sample mean of each row/column for the corresponding group.
     */
    std::vector<Output_*> mean;

    /**
     * Vector of length equal to the number of groups.
     * Each element is a pointer to an array of length equal to the appropriate dimension extent (rows for `row = true`, columns otherwise).
     * After `group_rss()`, this is filled with the residual sum of squares of each row/column for the corresponding group.
     */
    std::vector<Output_*> rss;

    /**
     * Pointer to an array of length equal to the number of groups.
     * After `group_rss()`, this is filled with the group sizes.
     */
    Count_* count;
};

/**
 * @cond
 */
template<typename Count_, typename Output_, typename Index_>
void group_rss_finish_means(
    const std::size_t num_groups,
    const Count_* const group_size,
    std::vector<Output_>& means,
    const Index_ i,
    std::vector<Output_*>& output_means
) {
    for (std::size_t b = 0; b < num_groups; ++b) {
        if (group_size[b]) {
            means[b] /= group_size[b];
        } else {
            means[b] = std::numeric_limits<Output_>::quiet_NaN();
        }
        output_means[b][i] = means[b];
    }
}

template<typename Value_, typename Index_, typename Group_, typename Output_, typename Count_>
void group_rss_direct(
    const bool row,
    const tatami::Matrix<Value_, Index_>& mat, 
    const Group_* const group, 
    const std::size_t num_groups, 
    GroupRssBuffers<Output_, Count_>& output,
    const GroupRssOptions& opt
) {
    const auto dim = (row ? mat.nrow() : mat.ncol());
    const auto otherdim = (row ? mat.ncol() : mat.nrow());

    std::fill_n(output.count, num_groups, 0);
    for (Index_ i = 0; i < otherdim; ++i) {
        output.count[group[i]] += 1;
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
                group_rss_finish_means(num_groups, output.count, cur_means, static_cast<Index_>(s + x), output.mean);

                // Now computing the RSS.
                for (Index_ i = 0; i < range.number; ++i) {
                    const auto g = group[range.index[i]];
                    const auto delta = range.value[i] - cur_means[g];
                    cur_rss[g] += delta * delta;
                }
                for (std::size_t g = 0; g < num_groups; ++g) {
                    if (output.count[g] > 0) { // preserve RSS = 0 if the group is empty, otherwise the NaN mean causes problems.
                        const Output_ my_rss = cur_rss[g] + cur_means[g] * cur_means[g] * (output.count[g] - cur_non_zeros[g]);
                        output.rss[g][s + x] = my_rss;
                    } else {
                        output.rss[g][s + x] = 0;
                    }
                }

                std::fill(cur_means.begin(), cur_means.end(), 0);
                std::fill(cur_rss.begin(), cur_rss.end(), 0);
                std::fill(cur_non_zeros.begin(), cur_non_zeros.end(), 0);
            }
        }, dim, opt.num_threads);

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
                group_rss_finish_means(num_groups, output.count, cur_means, static_cast<Index_>(s + x), output.mean);

                // Now computing the RSS.
                for (Index_ j = 0; j < otherdim; ++j) {
                    const auto g = group[j];
                    const auto delta = ptr[j] - cur_means[g];
                    cur_rss[g] += delta * delta;
                }
                for (std::size_t g = 0; g < num_groups; ++g) {
                    output.rss[g][s + x] = cur_rss[g];
                }

                std::fill(cur_means.begin(), cur_means.end(), 0);
                std::fill(cur_rss.begin(), cur_rss.end(), 0);
            }
        }, dim, opt.num_threads);
    }
}

template<typename Value_, typename Index_, typename Group_, typename Output_, typename Count_>
void group_rss_running(
    const bool row,
    const tatami::Matrix<Value_, Index_>& mat,
    const Group_* const group, 
    const std::size_t num_groups, 
    GroupRssBuffers<Output_, Count_>& output,
    const GroupRssOptions& opt
) {
    const auto dim = (row ? mat.nrow() : mat.ncol());
    const auto otherdim = (row ? mat.ncol() : mat.nrow());
    const bool is_sparse = mat.is_sparse();

    if (otherdim == 0) {
        std::fill_n(output.count, num_groups, 0);
        for (std::size_t g = 0; g < num_groups; ++g) {
            std::fill_n(output.mean[g], dim, std::numeric_limits<Output_>::quiet_NaN());
            std::fill_n(output.rss[g], dim, 0);
        }
        return; 
    }

    const bool do_parallel = opt.num_threads > 1;
    std::optional<std::vector<std::optional<std::vector<std::vector<Output_> > > > > all_partial_mean, all_partial_rss;
    std::optional<std::vector<std::optional<std::vector<Count_> > > > all_partial_count;
    if (do_parallel) {
        // -1, as we'll repurpose the RSS output buffer to store the partial RSS of the first thread.
        all_partial_rss.emplace(sanisizer::cast<I<decltype(all_partial_rss->size())> >(opt.num_threads - 1));
        all_partial_mean.emplace(sanisizer::cast<I<decltype(all_partial_mean->size())> >(opt.num_threads));
        all_partial_count.emplace(sanisizer::cast<I<decltype(all_partial_count->size())> >(opt.num_threads));
    }

    for (std::size_t g = 0; g < num_groups; ++g) {
        std::fill_n(output.mean[g], dim, 0);
        std::fill_n(output.rss[g], dim, 0);
    }

    const int nused = tatami::parallelize([&](int thread, Index_ s, Index_ l) -> void {
        Output_** mean_ptrs;
        Output_** rss_ptrs;
        std::optional<std::vector<Output_*> > tmp_mean_ptrs, tmp_rss_ptrs;
        std::optional<std::vector<std::vector<Output_> > > cur_mean, cur_rss;
        Count_* count_ptr;
        std::optional<std::vector<Count_> > cur_count;

        if (!do_parallel) {
            // Storing mean and RSS directly in the output vector to cut down two allocations if we're not working in parallel.
            mean_ptrs = output.mean.data();
            rss_ptrs = output.rss.data();
            count_ptr = output.count;
        } else {
            // Storing the partial RSS directly in the output vectors to save ourselves an allocation if we're in the first thread.
            // We can't do the same for the mean, though, as we need to keep the partial mean and the global mean separate for the reduction.
            cur_mean.emplace(sanisizer::cast<I<decltype(cur_mean->size())> >(num_groups));
            tmp_mean_ptrs.emplace(sanisizer::cast<I<decltype(tmp_mean_ptrs->size())> >(num_groups));
            for (std::size_t g = 0; g < num_groups; ++g) {
                tatami::resize_container_to_Index_size((*cur_mean)[g], dim);
                (*tmp_mean_ptrs)[g] = (*cur_mean)[g].data();
            }
            mean_ptrs = tmp_mean_ptrs->data();

            cur_count.emplace(sanisizer::cast<I<decltype(cur_count->size())> >(num_groups));
            count_ptr = cur_count->data();

            if (thread == 0) {
                rss_ptrs = output.rss.data();
            } else {
                cur_rss.emplace(sanisizer::cast<I<decltype(cur_rss->size())> >(num_groups));
                tmp_rss_ptrs.emplace(sanisizer::cast<I<decltype(tmp_rss_ptrs->size())> >(num_groups));
                for (std::size_t g = 0; g < num_groups; ++g) {
                    tatami::resize_container_to_Index_size((*cur_rss)[g], dim);
                    (*tmp_rss_ptrs)[g] = (*cur_rss)[g].data();
                }
                rss_ptrs = tmp_rss_ptrs->data();
            }
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
                count_ptr[g] = runners[g].num_obs();
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
                count_ptr[g] = runners[g].num_obs();
                runners[g].finish();
            }
        }

        if (do_parallel) {
            (*all_partial_count)[thread] = std::move(cur_count);
            (*all_partial_mean)[thread] = std::move(cur_mean);
            if (thread > 0) {
                (*all_partial_rss)[thread - 1] = std::move(cur_rss);
            }
        }
    }, otherdim, opt.num_threads);
    assert(nused > 0);

    if (do_parallel) {
        const auto& ap_mean = *all_partial_mean;
        const auto& ap_rss = *all_partial_rss;
        const auto& ap_count = *all_partial_count;

        std::fill_n(output.count, num_groups, 0);
        for (int u = 0; u < nused; ++u) {
            const auto& cur_count = *(ap_count[u]);
            for (std::size_t g = 0; g < num_groups; ++g) {
                output.count[g] += cur_count[g];
            }
        }

        // Computing the global mean.
        for (std::size_t g = 0; g < num_groups; ++g) {
            const auto cur_output = output.mean[g];
            const auto cur_global_count = output.count[g];
            if (cur_global_count == 0) {
                std::fill_n(cur_output, dim, std::numeric_limits<Output_>::quiet_NaN());
                continue;
            }

            for (int u = 0; u < nused; ++u) {
                const auto cur_count = (*((*all_partial_count)[u]))[g];
                if (cur_count == 0) {
                    continue;
                }

                const auto& cur_mean = (*(ap_mean[u]))[g];
                const Output_ mult = static_cast<Output_>(cur_count) / static_cast<Output_>(cur_global_count);
                for (Index_ d = 0; d < dim; ++d) {
                    cur_output[d] += cur_mean[d] * mult;
                }
            }
        }

        // Combining the RSS. 
        for (std::size_t g = 0; g < num_groups; ++g) {
            const auto& cur_global = output.mean[g];
            const auto cur_output = output.rss[g];

            for (int u = 0; u < nused; ++u) {
                const auto cur_count = (*((*all_partial_count)[u]))[g];
                if (cur_count == 0) { // This check allows us to use the unsafe RSS centering below.
                    continue;
                }

                const auto& cur_mean = (*(ap_mean[u]))[g];
                if (u == 0) {
                    for (Index_ d = 0; d < dim; ++d) {
                        cur_output[d] = quickstats::recenter_rss_unsafe(cur_count, cur_output[d], cur_mean[d], cur_global[d]); 
                    }
                } else {
                    const auto& cur_rss = (*(ap_rss[u - 1]))[g];
                    for (Index_ d = 0; d < dim; ++d) {
                        cur_output[d] += quickstats::recenter_rss_unsafe(cur_count, cur_rss[d], cur_mean[d], cur_global[d]); 
                    }
                }
            }
        }
    }
}
/**
 * @endcond
 */

/**
 * Compute per-group residual sums of squares (RSS) for each element of a chosen dimension of a `tatami::Matrix`.
 *
 * @tparam Value_ Numeric type of the matrix value.
 * @tparam Index_ Integer type of the row/column indices.
 * @tparam Group_ Integer type of the group assignments for each row/column.
 * @tparam Output_ Floating-point type of the output value.
 * This should be capable of storing NaNs.
 * @tparam Count_ Numeric type of the group sizes, typically integer.
 *
 * @param row Whether to compute RSS values for the rows.
 * @param mat Instance of a `tatami::Matrix`.
 * @param[in] group Pointer to an array of length equal to the number of columns (if `row = true`) or rows (otherwise).
 * Each value should be an integer that specifies the group assignment.
 * Values should lie in \f$[0, N)\f$ where \f$N\f$ is the number of unique groups.
 * @param num_groups Number of groups, i.e., \f$N\f$.
 * @param[out] output Buffers in which to store the results.
 * On output, each array stores the means and RSS values of the corresponding group.
 * @param opt Further options.
 */
template<typename Value_, typename Index_, typename Group_, typename Output_, typename Count_>
void group_rss(
    bool row,
    const tatami::Matrix<Value_, Index_>& mat,
    const Group_* const group,
    const std::size_t num_groups,
    GroupRssBuffers<Output_, Count_>& output,
    const GroupRssOptions& opt
) {
    assert(sanisizer::is_equal(num_groups, output.mean.size()));
    assert(sanisizer::is_equal(num_groups, output.rss.size()));
    if (mat.prefer_rows() == row) {
        group_rss_direct(row, mat, group, num_groups, output, opt);
    } else {
        group_rss_running(row, mat, group, num_groups, output, opt);
    }
}

/**
 * @brief Results of `group_rss()`.
 *
 * @tparam Output_ Floating-point type of the output data.
 * This should be capable of storing NaNs.
 * @tparam Count_ Numeric type of the group sizes, typically integer.
 */
template<typename Output_, typename Count_>
struct GroupRssResult {
    /**
     * Vector of length equal to the number of groups.
     * Each element is a vector of length equal to the appropriate dimension extent (rows for `row = true`, columns otherwise),
     * containing the sample mean of each row/column.
     */
    std::vector<std::vector<Output_> > mean;

    /**
     * Vector of length equal to the number of groups.
     * Each element is a vector of length equal to the appropriate dimension extent (rows for `row = true`, columns otherwise),
     * containing the residual sum of squares of each row/column.
     */
    std::vector<std::vector<Output_> > rss;

    /**
     * Vector of length equal to the number of groups, containing the number of observations in each group.
     */
    std::vector<Count_> count;
};

/**
 * Compute per-group residual sums of squares (RSS) for each element of a chosen dimension of a `tatami::Matrix`.
 *
 * @tparam Output_ Floating-point type of the output value.
 * This should be capable of storing NaNs.
 * @tparam Count_ Numeric type of the group sizes, typically integer.
 * @tparam Value_ Numeric type of the matrix value.
 * @tparam Index_ Integer type of the row/column indices.
 * @tparam Group_ Integer type of the group assignments for each row/column.
 *
 * @param row Whether to compute RSS values for the rows.
 * @param mat Instance of a `tatami::Matrix`.
 * @param[in] group Pointer to an array of length equal to the number of columns (if `row = true`) or rows (otherwise).
 * Each value should be an integer that specifies the group assignment.
 * Values should lie in \f$[0, N)\f$ where \f$N\f$ is the number of unique groups.
 * @param num_groups Number of groups, i.e., \f$N\f$.
 * @param opt Further options.
 *
 * @return RSS and mean of each group for each row/column.
 */
template<typename Output_, typename Count_, typename Value_, typename Index_, typename Group_> 
GroupRssResult<Output_, Count_> group_rss(
    bool row,
    const tatami::Matrix<Value_, Index_>& mat,
    const Group_* const group,
    const std::size_t num_groups,
    const GroupRssOptions& opt
) {
    GroupRssResult<Output_, Count_> output;
    sanisizer::resize(output.mean, num_groups);
    sanisizer::resize(output.rss, num_groups);
    sanisizer::resize(output.count, num_groups);

    GroupRssBuffers<Output_, Count_> buffers;
    sanisizer::resize(buffers.mean, num_groups);
    sanisizer::resize(buffers.rss, num_groups);
    buffers.count = output.count.data();

    const auto dim = (row ? mat.nrow() : mat.ncol());
    for (std::size_t g = 0; g < num_groups; ++g) {
        tatami::resize_container_to_Index_size(output.mean[g], dim
#ifdef TATAMI_STATS_TEST_DIRTY
            , -1
#endif
        );
        buffers.mean[g] = output.mean[g].data();
        tatami::resize_container_to_Index_size(output.rss[g], dim
#ifdef TATAMI_STATS_TEST_DIRTY
            , -1
#endif
        );
        buffers.rss[g] = output.rss[g].data();
    }

    group_rss(row, mat, group, num_groups, buffers, opt);
    return output;
}

}

#endif
