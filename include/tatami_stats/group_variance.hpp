#ifndef TATAMI_STATS_GROUP_VARIANCE_HPP
#define TATAMI_STATS_GROUP_VARIANCE_HPP

#include <vector>
#include <algorithm>
#include <cstddef>
#include <optional>
#include <cassert>
#include <limits>
#include <cmath>

#include "tatami/tatami.hpp"
#include "sanisizer/sanisizer.hpp"

#include "group_rss.hpp"
#include "skip_nan/group_rss.hpp"
#include "utils.hpp"

/**
 * @file group_variance.hpp
 *
 * @brief Compute group-wise variances from a `tatami::Matrix`.
 */

namespace tatami_stats {

/**
 * @brief Options for `group_variance()`.
 */
struct GroupVarianceOptions {
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
 * @brief Result buffers for `group_variance()`.
 *
 * @tparam Output_ Floating-point type of the output data.
 * This should be capable of storing NaNs.
 */
template<typename Output_>
struct GroupVarianceBuffers {
    /**
     * Vector of length equal to the number of groups.
     * Each element is a pointer to an array of length equal to the appropriate dimension extent (rows for `row = true`, columns otherwise).
     * After `group_variance()`, this is filled with the sample mean of each row/column for the corresponding group.
     */
    std::vector<Output_*> mean;

    /**
     * Vector of length equal to the number of groups.
     * Each element is a pointer to an array of length equal to the appropriate dimension extent (rows for `row = true`, columns otherwise).
     * After `group_variance()`, this is filled with the sample variance of each row/column for the corresponding group.
     */
    std::vector<Output_*> variance;
};

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
 * @param[out] output Buffers in which to store the results.
 * On output, each array stores the means and variances of the corresponding group.
 * @param opt Further options.
 */
template<typename Value_, typename Index_, typename Group_, typename Output_>
void group_variance(
    bool row,
    const tatami::Matrix<Value_, Index_>& mat,
    const Group_* const group,
    const std::size_t num_groups,
    GroupVarianceBuffers<Output_>& output,
    const GroupVarianceOptions& opt
) {
    assert(sanisizer::is_equal(num_groups, output.mean.size()));
    assert(sanisizer::is_equal(num_groups, output.variance.size()));
    const auto dim = (row ? mat.nrow() : mat.ncol());

    nanable_ifelse<Value_>(
        opt.skip_nan,
        [&]() -> void {
            skip_nan::GroupRssBuffers<Output_, Index_> tmp;
            tmp.mean = output.mean;
            tmp.rss = output.variance;

            auto count = sanisizer::create<std::vector<std::vector<Index_> > >(num_groups);
            tmp.count.reserve(num_groups);
            for (std::size_t g = 0; g < num_groups; ++g) {
                tatami::resize_container_to_Index_size(count[g], dim);
                tmp.count.push_back(count[g].data());
            }

            skip_nan::GroupRssOptions ropt;
            ropt.num_threads = opt.num_threads;
            skip_nan::group_rss(row, mat, group, num_groups, tmp, ropt);
            for (std::size_t g = 0; g < num_groups; ++g) {
                quickstats::rss_to_variance(dim, count[g].data(), output.variance[g]);
            }
        },
        [&]() -> void {
            GroupRssBuffers<Output_, Index_> tmp;
            tmp.mean = output.mean;
            tmp.rss = output.variance;
            auto group_sizes = sanisizer::create<std::vector<Index_> >(num_groups);
            tmp.count = group_sizes.data();

            GroupRssOptions ropt;
            ropt.num_threads = opt.num_threads;
            group_rss(row, mat, group, num_groups, tmp, ropt);
            for (std::size_t g = 0; g < num_groups; ++g) {
                quickstats::rss_to_variance(dim, group_sizes[g], output.variance[g]);
            }
        }
    );
}

/**
 * @brief Results of `group_variance()`.
 *
 * @tparam Output_ Floating-point type of the output data.
 * This should be capable of storing NaNs.
 */
template<typename Output_>
struct GroupVarianceResult {
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
 * @param opt Further options.
 *
 * @return Variance and mean of each group for each row/column.
 */
template<typename Output_ = double, typename Value_, typename Index_, typename Group_> 
GroupVarianceResult<Output_> group_variance(
    bool row,
    const tatami::Matrix<Value_, Index_>& mat,
    const Group_* const group,
    const std::size_t num_groups,
    const GroupVarianceOptions& opt
) {
    GroupVarianceResult<Output_> output;
    sanisizer::resize(output.mean, num_groups);
    sanisizer::resize(output.variance, num_groups);

    GroupVarianceBuffers<Output_> buffers;
    sanisizer::resize(buffers.mean, num_groups);
    sanisizer::resize(buffers.variance, num_groups);
    const auto dim = (row ? mat.nrow() : mat.ncol());

    for (std::size_t g = 0; g < num_groups; ++g) {
        tatami::resize_container_to_Index_size(output.mean[g], dim
#ifdef TATAMI_STATS_TEST_DIRTY
            , -1
#endif
        );
        buffers.mean[g] = output.mean[g].data();
        tatami::resize_container_to_Index_size(output.variance[g], dim
#ifdef TATAMI_STATS_TEST_DIRTY
            , -1
#endif
        );
        buffers.variance[g] = output.variance[g].data();
    }

    group_variance(row, mat, group, num_groups, buffers, opt);
    return output;
}

}

#endif
