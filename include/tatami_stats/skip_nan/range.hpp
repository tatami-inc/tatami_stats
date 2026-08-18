#ifndef TATAMI_STATS_SKIP_NAN_RANGE_HPP
#define TATAMI_STATS_SKIP_NAN_RANGE_HPP

#include "../utils.hpp"

#include <vector>
#include <algorithm>
#include <type_traits>
#include <limits>

#include "tatami/tatami.hpp"

/**
 * @file range.hpp
 *
 * @brief Compute row and column ranges after skipping NaNs.
 */

namespace tatami_stats {

namespace skip_nan { 

/**
 * @tparam Output_ Numeric type of the output of `skip_nan::range()`.
 * @return Default placeholder value for the minimum in the output of `skip_nan::range()`.
 * This is positive infinity if supported by `Output_`, otherwise it is the largest finite value.
 */
template<typename Output_>
constexpr Output_ default_minimum_placeholder() {
    if constexpr(std::numeric_limits<Output_>::has_infinity) {
        return std::numeric_limits<Output_>::infinity();
    } else {
        return std::numeric_limits<Output_>::max();
    }
}

/**
 * @tparam Output_ Numeric type of the output of `skip_nan::range()`.
 * @return Default placeholder value for the maximum in the output of `skip_nan::range()`.
 * This is negative infinity if supported by `Output_`, otherwise it is the smallest finite value.
 */
template<typename Output_>
constexpr Output_ default_maximum_placeholder() {
    if constexpr(std::numeric_limits<Output_>::has_infinity) {
        return -std::numeric_limits<Output_>::infinity();
    } else {
        return std::numeric_limits<Output_>::lowest();
    }
}

/**
 * @brief Options for `range()`.
 * @tparam Output_ Numeric type of the output data.
 */
template<typename Output_ = double>
struct RangeOptions {
    /**
     * Number of threads to use when computing ranges across a `tatami::Matrix`.
     * See `tatami::parallelize()` for more details on the parallelization mechanism.
     */
    int num_threads = 1;

    /**
     * Placeholder for the minimum value in `RangeBuffers::minimum` or `RangeResults::minimum`,
     * when there are zero columns (if `row == true`) or rows (otherwise).
     */
    Output_ minimum_placeholder = default_minimum_placeholder<Output_>();

    /**
     * Placeholder for the maximum value in `RangeBuffers::maximum` or `RangeResults::maximum`,
     * when there are zero columns (if `row == true`) or rows (otherwise).
     */
    Output_ maximum_placeholder = default_maximum_placeholder<Output_>();
};

/**
 * @cond
 */
template<typename Output_, typename Index_>
struct RangeDirectResult {
    Output_ minimum;
    Output_ maximum;
    Index_ count;
};

template<typename Value_, typename Index_, typename Output_>
RangeDirectResult<Output_, Index_> range_direct(const Value_* const ptr, const Index_ num, const RangeOptions<Output_>& opt) {
    RangeDirectResult<Output_, Index_> output;
    output.minimum = opt.minimum_placeholder;
    output.maximum = opt.maximum_placeholder;
    output.count = 0;

    // First loop to get to the first non-NA value.
    Index_ i = 0;
    for (; i < num; ++i) {
        auto val = ptr[i];
        if (!std::isnan(val)) {
            output.minimum = val;
            output.maximum = val;
            output.count = 1;
            ++i;
            break;
        }
    }

    // Second loop to actually get the minimum.
    for (; i < num; ++i) {
        auto val = ptr[i];
        if (!std::isnan(val)) {
            ++output.count;
            output.minimum = std::min(output.minimum, val);
            output.maximum = std::max(output.maximum, val);
        }
    }

    return output;
}

template<typename Value_, typename Index_, typename Output_>
RangeDirectResult<Output_, Index_> range_direct(const Value_* value, const Index_ num_nonzero, const Index_ num_all, const RangeOptions<Output_>& opt) {
    if (num_nonzero) {
        auto candidate = range_direct(value, num_nonzero, opt);
        if (num_nonzero < num_all) {
            candidate.minimum = std::min(candidate.minimum, static_cast<Output_>(0));
            candidate.maximum = std::max(candidate.maximum, static_cast<Output_>(0));
            candidate.count += num_all - num_nonzero;
        }
        return candidate;
    } else if (num_all) {
        RangeDirectResult<Output_, Index_> output;
        output.minimum = 0;
        output.maximum = 0;
        output.count = num_all;
        return output;
    } else {
        RangeDirectResult<Output_, Index_> output;
        output.minimum = opt.minimum_placeholder;
        output.maximum = opt.maximum_placeholder;
        output.count = 0;
        return output;
    }
}
/**
 * @endcond
 */

/**
 * @brief Result buffers for `skip_nan::range()`.
 *
 * @tparam Output_ Floating-point type of the output data.
 * @tparam Count_ Numeric type of the non-NaN counts.
 * This is typically an integer type.
 */
template<typename Output_, typename Count_>
struct RangeBuffers {
    /**
     * Pointer to an array of length equal to the number of rows/columns (depending on `row`).
     * After calling `skip_nan::range()`, this is filled with the minimum value for each row/column.
     */
    Output_* minimum;

    /**
     * Pointer to an array of length equal to the number of rows/columns (depending on `row`).
     * After calling `skip_nan::range()`, this is filled with the maximum value for each row/column.
     */
    Output_* maximum;

    /**
     * Pointer to an array of length equal to the appropriate dimension extent (rows for `row = true`, columns otherwise).
     * After `skip_nan::range()`, this is filled with the number of unskipped observations in each row/column.
     */
    Count_* count;
};

/**
 * @cond
 */
template<typename Value_, typename Index_, typename Output_, typename Count_>
void range_direct(bool row, const tatami::Matrix<Value_, Index_>& mat, RangeBuffers<Output_, Count_>& output, const RangeOptions<Output_>& opt) {
    const auto dim = (row ? mat.nrow() : mat.ncol());
    const auto otherdim = (row ? mat.ncol() : mat.nrow());

    if (mat.is_sparse()) {
        tatami::Options topt;
        topt.sparse_extract_index = false;
        tatami::parallelize([&](int, Index_ s, Index_ l) -> void {
            auto ext = tatami::consecutive_extractor<true>(mat, row, s, l, topt);
            auto vbuffer = tatami::create_container_of_Index_size<std::vector<Value_> >(otherdim);
            for (Index_ x = 0; x < l; ++x) {
                auto out = ext->fetch(vbuffer.data(), NULL);
                auto res = range_direct(out.value, out.number, otherdim, opt);
                output.minimum[x + s] = res.minimum;
                output.maximum[x + s] = res.maximum;
                output.count[x + s] = res.count;
            }
        }, dim, opt.num_threads);

    } else {
        tatami::parallelize([&](int, Index_ s, Index_ l) -> void {
            auto ext = tatami::consecutive_extractor<false>(mat, row, s, l);
            auto buffer = tatami::create_container_of_Index_size<std::vector<Value_> >(otherdim);
            for (Index_ x = 0; x < l; ++x) {
                auto ptr = ext->fetch(buffer.data());
                auto res = range_direct(ptr, otherdim, opt);
                output.minimum[x + s] = res.minimum;
                output.maximum[x + s] = res.maximum;
                output.count[x + s] = res.count;
            }
        }, dim, opt.num_threads);
    }
}

template<typename Value_, typename Index_, typename Output_, typename Count_>
void range_running(bool row, const tatami::Matrix<Value_, Index_>& mat, RangeBuffers<Output_, Count_>& output, const RangeOptions<Output_>& opt) {
    const auto dim = (row ? mat.nrow() : mat.ncol());
    const auto otherdim = (row ? mat.ncol() : mat.nrow());
    const bool is_sparse = mat.is_sparse();

    const bool do_parallel = opt.num_threads > 1; 
    std::optional<std::vector<std::optional<std::vector<Output_> > > > all_partial_min, all_partial_max;
    std::optional<std::vector<std::optional<std::vector<Count_> > > > all_partial_count;
    if (do_parallel) {
        all_partial_min.emplace(sanisizer::cast<I<decltype(all_partial_min->size())> >(opt.num_threads - 1));
        all_partial_max.emplace(sanisizer::cast<I<decltype(all_partial_max->size())> >(opt.num_threads - 1));
        all_partial_count.emplace(sanisizer::cast<I<decltype(all_partial_count->size())> >(opt.num_threads - 1));
    }

    std::fill_n(output.count, dim, 0);

    // If we're not skipping NaNs and we have at least one dimension element,
    // the output arrays will be fully populated when thread 0 processes the first dimension element.
    if (otherdim == 0) {
        std::fill_n(output.minimum, dim, opt.minimum_placeholder);
        std::fill_n(output.maximum, dim, opt.maximum_placeholder);
        return;
    }

    // No need to wipe dirty output buffers in the dense case, as we already set each entry of the output buffers.
    if (is_sparse) {
        std::fill_n(output.minimum, dim, 0);
        std::fill_n(output.maximum, dim, 0);
    }

    const auto nused = tatami::parallelize([&](int thread, Index_ s, Index_ l) -> void {
        Output_* min_ptr;
        Output_* max_ptr;
        Count_* count_ptr;
        std::optional<std::vector<Output_> > cur_min, cur_max;
        std::optional<std::vector<Count_> > cur_count;
        if (!do_parallel) {
            min_ptr = output.minimum;
            max_ptr = output.maximum;
            count_ptr = output.count;
        } else {
            if (thread == 0) {
                min_ptr = output.minimum;
                max_ptr = output.maximum;
                count_ptr = output.count;
            } else {
                cur_min.emplace(tatami::cast_Index_to_container_size<std::vector<Output_> >(dim));
                cur_max.emplace(tatami::cast_Index_to_container_size<std::vector<Output_> >(dim));
                cur_count.emplace(tatami::cast_Index_to_container_size<std::vector<Count_> >(dim));
                min_ptr = cur_min->data();
                max_ptr = cur_max->data();
                count_ptr = cur_count->data();
            }
        }

        if (is_sparse) {
            tatami::Options topt;
            topt.sparse_ordered_index = false;
            auto ext = tatami::consecutive_extractor<true>(mat, !row, s, l, topt);
            auto vbuffer = tatami::create_container_of_Index_size<std::vector<Value_> >(dim);
            auto ibuffer = tatami::create_container_of_Index_size<std::vector<Index_> >(dim);
            auto nonzeros = tatami::create_container_of_Index_size<std::vector<Index_> >(dim);

            for (Index_ x = 0; x < l; ++x) {
                auto out = ext->fetch(vbuffer.data(), ibuffer.data());

                // For the first observed vector in each thread, we can optimize it a little as we don't need to read existing min/max.
                if (x == 0) {
                    AUVEH_NODEP
                    for (Index_ i = 0; i < out.number; ++i) {
                        const auto val = out.value[i];
                        const auto idx = out.index[i];
                        if (!std::isnan(val)) {
                            min_ptr[idx] = val;
                            max_ptr[idx] = val;
                            ++count_ptr[idx];
                        } else {
                            min_ptr[idx] = opt.minimum_placeholder;
                            max_ptr[idx] = opt.maximum_placeholder;
                        }
                        ++nonzeros[idx];
                    }
                } else {
                    AUVEH_NODEP
                    for (Index_ i = 0; i < out.number; ++i) {
                        const auto val = out.value[i];
                        const auto idx = out.index[i];
                        if (!std::isnan(val)) {
                            auto& min_current = min_ptr[idx];
                            auto& max_current = max_ptr[idx];
                            if (count_ptr[idx] == 0) {
                                min_current = val;
                                max_current = val;
                            } else {
                                min_current = std::min(min_current, val);
                                max_current = std::max(max_current, val);
                            }
                            ++count_ptr[idx];
                        }
                        ++nonzeros[idx];
                    }
                }
            }

            AUVEH_NODEP
            for (Index_ d = 0; d < dim; ++d) {
                if (l > nonzeros[d]) {
                    count_ptr[d] += l - nonzeros[d];
                    auto& min_current = min_ptr[d];
                    min_current = std::min(min_current, static_cast<Output_>(0));
                    auto& max_current = max_ptr[d];
                    max_current = std::max(max_current, static_cast<Output_>(0));
                }
            }

        } else {
            auto ext = tatami::consecutive_extractor<false>(mat, !row, s, l);
            auto buffer = tatami::create_container_of_Index_size<std::vector<Value_> >(dim);

            for (Index_ x = 0; x < l; ++x) {
                auto ptr = ext->fetch(buffer.data());

                if (x == 0) {
                    // For the first observed vector in each thread,
                    // we can optimize it a little as we don't need to read existing min/max.
                    AUVEH_NODEP
                    for (Index_ i = 0; i < dim; ++i) {
                        const auto val = ptr[i];
                        if (!std::isnan(val)) {
                            min_ptr[i] = val;
                            max_ptr[i] = val;
                            ++count_ptr[i];
                        } else {
                            min_ptr[i] = opt.minimum_placeholder;
                            max_ptr[i] = opt.maximum_placeholder;
                        }
                    }
                } else {
                    AUVEH_NODEP
                    for (Index_ i = 0; i < dim; ++i) {
                        const auto val = ptr[i];
                        if (!std::isnan(val)) {
                            auto& min_current = min_ptr[i];
                            auto& max_current = max_ptr[i];
                            if (count_ptr[i] == 0) {
                                min_current = val;
                                max_current = val;
                            } else {
                                min_current = std::min(min_current, val);
                                max_current = std::max(max_current, val);
                            }
                            ++count_ptr[i];
                        }
                    }
                }
            }
        }

        if (do_parallel) {
            if (thread > 0) {
                (*all_partial_min)[thread - 1] = std::move(cur_min);
                (*all_partial_max)[thread - 1] = std::move(cur_max);
                (*all_partial_count)[thread - 1] = std::move(cur_count);
            }
        }
    }, otherdim, opt.num_threads);

    if (do_parallel) {
        for (int u = 1; u < nused; ++u) {
            const auto& cur_min = *((*all_partial_min)[u - 1]);
            const auto& cur_max = *((*all_partial_max)[u - 1]);
            const auto& cur_count = *((*all_partial_count)[u - 1]);
            AUVEH_NODEP
            for (Index_ d = 0; d < dim; ++d) {
                if (!cur_count[d]) {
                    continue;
                }
                if (output.count[d]) {
                    output.minimum[d] = std::min(cur_min[d], output.minimum[d]);
                    output.maximum[d] = std::max(cur_max[d], output.maximum[d]);
                } else {
                    output.minimum[d] = cur_min[d];
                    output.maximum[d] = cur_max[d];
                }
                output.count[d] += cur_count[d];
            }
        }
    }
}
/**
 * @endcond
 */

/**
 * Compute ranges for each element of a chosen dimension of a `tatami::Matrix`, after skipping any NaNs.
 *
 * @tparam Value_ Numeric type of the input data.
 * @tparam Index_ Integer type of the row/column indices.
 * @tparam Output_ Numeric type of the output data.
 * It is assumed that this is large enough to store the maxima/minima. 
 * @tparam Count_ Numeric type of the non-NaN counts.
 * This is typically an integer type.
 *
 * @param row Whether to compute the range for each row.
 * If false, the range is computed for each column instead.
 * @param mat Instance of a `tatami::Matrix`.
 * @param[out] output Buffers to output arrays.
 * On output, this will contain the row/column variances.
 * @param opt Further options.
 */
template<typename Value_, typename Index_, typename Output_, typename Count_>
void range(bool row, const tatami::Matrix<Value_, Index_>& mat, RangeBuffers<Output_, Count_>& output, const RangeOptions<Output_>& opt) {
    if (mat.prefer_rows() == row) {
        range_direct(row, mat, output, opt);
    } else {
        range_running(row, mat, output, opt);
    }
}

/**
 * @brief Results of `skip_nan::range()`.
 *
 * @tparam Output_ Floating-point type of the output data.
 * @tparam Count_ Numeric type of the non-NaN counts.
 * This is typically an integer type.
 */
template<typename Output_, typename Count_>
struct RangeResult {
    /**
     * Vector of length equal to the number of rows/columns (depending on `row`),
     * containing the minimum value for each row/column.
     */
    std::vector<Output_> minimum;

    /**
     * Vector of length equal to the number of rows/columns (depending on `row`),
     * containing the maximum value for each row/column.
     */
    std::vector<Output_> maximum;

    /**
     * Vector of length equal to the appropriate dimension extent (rows for `row = true`, columns otherwise),
     * containing the number of unskipped observations in each row/column.
     */
    std::vector<Count_> count;
};

/**
 * Overload of `skip_nan::range()` that allocates memory for the minimum/maximum.
 *
 * @tparam Value_ Numeric type of the input data.
 * @tparam Index_ Integer type of the row/column indices.
 * @tparam Output_ Numeric type of the output data.
 * It is assumed that this is large enough to store the maxima/minima. 
 *
 * @param row Whether to compute the range for each row.
 * If false, the range is computed for each column instead.
 * @param mat Instance of a `tatami::Matrix`.
 * @param opt Further options.
 *
 * @return Minimum and maximum for each row/column.
 */
template<typename Value_, typename Index_, typename Output_ = Value_, typename Count_ = Index_>
RangeResult<Output_, Count_> range(bool row, const tatami::Matrix<Value_, Index_>& mat, const RangeOptions<Output_>& opt) {
    RangeResult<Output_, Count_> output;
    const auto dim = (row ? mat.nrow() : mat.ncol());
    tatami::resize_container_to_Index_size(output.minimum, dim
#ifdef TATAMI_STATS_TEST_DIRTY
        , -1
#endif
    );
    tatami::resize_container_to_Index_size(output.maximum, dim
#ifdef TATAMI_STATS_TEST_DIRTY
        , -1
#endif
    );
    tatami::resize_container_to_Index_size(output.count, dim
#ifdef TATAMI_STATS_TEST_DIRTY
        , -1
#endif
    );

    RangeBuffers<Output_, Count_> buffers;
    buffers.minimum = output.minimum.data();
    buffers.maximum = output.maximum.data();
    buffers.count = output.count.data();
    range(row, mat, buffers, opt);

    return output;
}

}

}

#endif
