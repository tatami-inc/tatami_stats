#ifndef TATAMI_STATS_RANGES_HPP
#define TATAMI_STATS_RANGES_HPP

#include "tatami/tatami.hpp"

#include <vector>
#include <algorithm>
#include <type_traits>

/**
 * @file ranges.hpp
 *
 * @brief Compute row and column ranges from a `tatami::Matrix`.
 */

namespace tatami_stats {

namespace extreme {

/**
 * @cond
 */
namespace internal {

template<bool get_minimum_, typename Value_>
constexpr auto choose_placeholder() {
    if constexpr(get_minimum_) {
        // Placeholder value 'x' is such that 'x > y' is always true for any non-NaN 'y'.
        if constexpr(std::numeric_limits<Value_>::has_infinity) {
            return std::numeric_limits<Value_>::infinity();
        } else {
            return std::numeric_limits<Value_>::max();
        }
    } else {
        // Placeholder value 'x' is such that 'x < y' is always true for any non-NaN 'y'.
        if constexpr(std::numeric_limits<Value_>::has_infinity) {
            return -std::numeric_limits<Value_>::infinity();
        } else {
            return std::numeric_limits<Value_>::lowest();
        }
    }
}

template<bool get_minimum_, typename Output_, typename Value_>
bool is_better(Output_ best, Value_ alt) {
    if constexpr(get_minimum_) {
        return best > static_cast<Output_>(alt);
    } else {
        return best < static_cast<Output_>(alt);
    }
}

}
/**
 * @endcond
 */

/**
 * Compute the extremes of a dense array.
 *
 * @tparam get_minimum_ Whether to compute the minimum.
 * If false, the maximum is computed instead.
 * @tparam skip_nan_ Whether to check for (and skip) NaNs.
 * If false, NaNs are assumed to be absent, and the behavior of this function with NaNs is undefined.
 * @tparam Value_ Type of the input data.
 * @tparam Index_ Type of the row/column indices.
 *
 * @param[in] ptr Pointer to an array of values of length `num`.
 * @param num Size of the array.
 *
 * @return The minimum or maximum value, depending on `get_minimum_`.
 * If `num = 0` or (if `skip_nan_ = true`) there are no non-NaN values, a placeholder value is returned instead
 * that is never less than (if `get_minimum_ true`) or greater than (otherwise) any non-NaN value of type `Value_`.
 */
template<bool get_minimum_, bool skip_nan_ = false, typename Value_, typename Index_>
Value_ compute(const Value_* ptr, Index_ num) {
    if constexpr(skip_nan_) {
        auto current = internal::choose_placeholder<get_minimum_, Value_>(); 
        for (Index_ i = 0; i < num; ++i) {
            auto val = ptr[i];
            if (internal::is_better<get_minimum_>(current, val)) { // no need to explicitly handle NaNs, as any comparison with NaNs is always false.
                current = val;
            }
        }
        return current;

    } else if (num) {
        if constexpr(get_minimum_) {
            return *std::min_element(ptr, ptr + num);
        } else {
            return *std::max_element(ptr, ptr + num);
        }

    } else {
        return internal::choose_placeholder<get_minimum_, Value_>(); 
    }
}

/**
 * Compute the extremes of a sparse array.
 *
 * @tparam get_minimum_ Whether to compute the minimum.
 * If false, the maximum is computed instead.
 * @tparam skip_nan_ Whether to check for (and skip) NaNs.
 * If false, NaNs are assumed to be absent, and the behavior of this function with NaNs is undefined.
 * @tparam Value_ Type of the input data.
 * @tparam Index_ Type of the row/column indices.
 *
 * @param[in] value Pointer to an array of values of length `num`.
 * @param num_nonzero Length of the array pointed to by `value`.
 * @param num_all Total number of values in the dataset, including the zeros not in `value`.
 * This should be greater than or equal to `num_nonzero`.
 *
 * @return The minimum or maximum value, depending on `get_minimum_`.
 * If `num_all = 0` or (if `skip_nan_ = true`) there are no non-NaN values, a placeholder value is returned instead
 * that is never less than (if `get_minimum_ true`) or greater than (otherwise) any non-NaN value of type `Value_`.
 */
template<bool get_minimum_, bool skip_nan_ = false, typename Value_, typename Index_>
Value_ compute(const Value_* value, Index_ num_nonzero, Index_ num_all) {
    if (num_nonzero) {
        auto candidate = compute<get_minimum_, skip_nan_>(value, num_nonzero);
        if (num_nonzero < num_all && internal::is_better<get_minimum_>(candidate, static_cast<Value_>(0))) {
            candidate = 0;
        }
        return candidate;
    } else if (num_all) {
        return 0;
    } else {
        return internal::choose_placeholder<get_minimum_, Value_>();
    }
}

/**
 * @brief Running extremes from dense data.
 *
 * Compute running minimum/maximum from dense data. 
 * This considers a scenario with a set of equilength "target" vectors [V1, V2, V3, ..., Vn],
 * but data are only available for "observed" vectors [P1, P2, P3, ..., Pm],
 * where Pi[j] contains the i-th element of target vector Vj.
 * The idea is to repeatedly call `add()` for `ptr` corresponding to observed vectors from 0 to m - 1,
 * which computes the running minimum/maximum at each invocation.
 *
 * @tparam get_minimum_ Whether to compute the minimum.
 * @tparam skip_nan_ Whether to check for (and skip) NaNs.
 * If false, NaNs are assumed to be absent, and the behavior of this class in the presence of NaNs is undefined.
 * @tparam Value_ Type of the input data.
 * @tparam Index_ Type of the row/column indices.
 * @tparam Output_ Type of the output data.
 */
template<bool get_minimum_, bool skip_nan_ = false, typename Value_ = double, typename Index_ = int, typename Output_ = double>
struct RunningDense {
    /**
     * @param num Number of target vectors, i.e., n.
     * @param[out] store Pointer to an output array of length `num`.
     * After `finish()` is called, this will contain the minimum/maximum for each target vector.
     */
    RunningDense(Index_ num, Output_* store) : num(num), store(store) {}

    /**
     * Add the next observed vector to the running min/max calculation.
     * @param[in] ptr Pointer to an array of values of length `num`, corresponding to an observed vector.
     */
    void add(const Value_* ptr) {
        if (init) {
            init = false;
            for (Index_ i = 0; i < num; ++i, ++ptr) {
                auto val = *ptr;
                if constexpr(skip_nan_) {
                    if (std::isnan(val)) {
                        val = internal::choose_placeholder<get_minimum_, Value_>();
                    }
                }
                store[i] = val;
            }

        } else {
            for (Index_ i = 0; i < num; ++i, ++ptr) {
                auto val = *ptr;
                if (internal::is_better<get_minimum_>(store[i], val)) { // this should implicitly skip NaNs, any NaN comparison will be false.
                    store[i] = val;
                }
            }
        }
    }

    /**
     * Finish the running calculation once all observed vectors have been passed to `add()`. 
     */
    void finish() {
        if (init) {
            std::fill_n(store, num, internal::choose_placeholder<get_minimum_, Value_>());
        }
    }

private:
    bool init = true;
    Index_ num;
    Output_* store;
};

/**
 * @brief Running extremes from sparse data.
 *
 * Compute running minima and maximuma from sparse data. 
 * This does the same as `RunningDense` but for sparse observed vectors.
 *
 * @tparam get_minimum_ Whether to compute the minimum.
 * @tparam skip_nan_ Whether to check for (and skip) NaNs.
 * If false, NaNs are assumed to be absent; the behavior of this function in the presence of NaNs is undefined.
 * @tparam Value_ Type of the input value.
 * @tparam Index_ Type of the row/column indices.
 * @tparam Output_ Type of the output data.
 */
template<bool get_minimum_, bool skip_nan_ = false, typename Value_ = double, typename Index_ = int, typename Output_ = double>
struct RunningSparse {
    /**
     * @param num Number of target vectors.
     * @param[out] store Pointer to an output array of length `num`.
     * After `finish()` is called, this will contain the minimum/maximum for each target vector.
     * @param subtract Offset to subtract from each element of `index` before using it to index into `store`.
     * Only relevant if `store` holds statistics for a contiguous subset of target vectors,
     * e.g., during task allocation for parallelization.
     */
    RunningSparse(Index_ num, Output_* store, Index_ subtract = 0) : num(num), store(store), subtract(subtract) {}

    /**
     * Add the next observed vector to the min/max calculation.
     * @param[in] value Value of structural non-zero elements.
     * @param[in] index Index of structural non-zero elements.
     * @param number Number of non-zero elements in `value` and `index`.
     */
    void add(const Value_* value, const Index_* index, Index_ number) {
        if (count == 0) {
            nonzero.resize(num);
            std::fill_n(store, num, internal::choose_placeholder<get_minimum_, Value_>());
        }

        for (Index_ i = 0; i < number; ++i, ++value, ++index) {
            auto val = *value;
            auto idx = *index - subtract;
            auto& current = store[idx];
            if (internal::is_better<get_minimum_>(current, val)) { // this should implicitly skip NaNs, any NaN comparison will be false.
                current = val;
            }
            ++nonzero[idx];
        }

        ++count;
    }

    /**
     * Finish the min/max calculation once all observed vectors have been passed to `add()`. 
     */
    void finish() {
        if (count) {
            for (Index_ i = 0; i < num; ++i) {
                if (count > nonzero[i]) {
                    auto& current = store[i];
                    if (internal::is_better<get_minimum_>(current, static_cast<Output_>(0))) {
                        current = 0;
                    }
                }
            }
        } else {
            std::fill_n(store, num, internal::choose_placeholder<get_minimum_, Value_>());
        }
    }

private:
    Index_ num;
    Output_* store;
    Index_ subtract;
    Index_ count = 0;
    std::vector<Index_> nonzero;
};

}

/**
 * Compute extremes for each element of a chosen dimension of a `tatami::Matrix`.
 *
 * @tparam skip_nan_ Whether to check for (and skip) NaNs.
 * If false, NaNs are assumed to be absent; the behavior of this function in the presence of NaNs is undefined.
 * @tparam Value_ Type of the matrix value, should be numeric.
 * @tparam Index_ Type of the row/column indices.
 * @tparam Output_ Type of the output value.
 *
 * @param row Whether to compute variances for the rows.
 * @param p Pointer to a `tatami::Matrix`.
 * @param threads Number of threads to use.
 * @param[out] min_out Pointer to an array of length equal to the number of rows (if `row = true`) or columns (otherwise).
 * On output, this will contain the minimum of each row/column.
 * Alternatively, this may be NULL, in which case the minima are not computed.
 * @param[out] max_out Pointer to an array of length equal to the number of rows (if `row = true`) or columns (otherwise).
 * On output, this will contain the maximum of each row/column.
 * Alternatively, this may be NULL, in which case the maxima are not computed.
 */
template<bool skip_nan_, typename Value_, typename Index_, typename Output_>
void extremes(bool row, const tatami::Matrix<Value_, Index_>* p, int threads, Output_* min_out, Output_* max_out) {
    auto dim = (row ? p->nrow() : p->ncol());
    auto otherdim = (row ? p->ncol() : p->nrow());
    const bool direct = p->prefer_rows() == row;

    bool store_min = min_out != NULL;
    bool store_max = max_out != NULL;

    if (p->sparse()) {
        tatami::Options opt;
        opt.sparse_ordered_index = false;

        if (direct) {
            opt.sparse_extract_index = false;
            tatami::parallelize([&](size_t, Index_ s, Index_ l) {
                auto ext = tatami::consecutive_extractor<true>(p, row, s, l, opt);
                std::vector<Value_> vbuffer(otherdim);
                for (Index_ x = 0; x < l; ++x) {
                    auto out = ext->fetch(vbuffer.data(), NULL);
                    if (store_min) {
                        min_out[x + s] = extreme::compute<true, skip_nan_>(out.value, out.number, otherdim);
                    }
                    if (store_max) {
                        max_out[x + s] = extreme::compute<false, skip_nan_>(out.value, out.number, otherdim);
                    }
                }
            }, dim, threads);

        } else {
            tatami::parallelize([&](size_t, Index_ s, Index_ l) {
                auto ext = tatami::consecutive_extractor<true>(p, !row, 0, otherdim, s, l, opt);
                std::vector<Value_> vbuffer(l);
                std::vector<Index_> ibuffer(l);
                extreme::RunningSparse<true, skip_nan_, Value_, Index_, Output_> runmin(l, (store_min ? min_out + s : NULL), s);
                extreme::RunningSparse<false, skip_nan_, Value_, Index_, Output_> runmax(l, (store_max ? max_out + s : NULL), s);

                for (Index_ x = 0; x < otherdim; ++x) {
                    auto out = ext->fetch(vbuffer.data(), ibuffer.data());
                    if (store_min) {
                        runmin.add(out.value, out.index, out.number);
                    }
                    if (store_max) {
                        runmax.add(out.value, out.index, out.number);
                    }
                }

                if (store_min) {
                    runmin.finish();
                }
                if (store_max) {
                    runmax.finish();
                }
            }, dim, threads);
        }

    } else {
        if (direct) {
            tatami::parallelize([&](size_t, Index_ s, Index_ l) {
                auto ext = tatami::consecutive_extractor<false>(p, row, s, l);
                std::vector<Value_> buffer(otherdim);
                for (Index_ x = 0; x < l; ++x) {
                    auto ptr = ext->fetch(buffer.data());
                    if (store_min) {
                        min_out[x + s] = extreme::compute<true, skip_nan_>(ptr, otherdim);
                    }
                    if (store_max) {
                        max_out[x + s] = extreme::compute<false, skip_nan_>(ptr, otherdim);
                    }
                }
            }, dim, threads);

        } else {
            tatami::parallelize([&](size_t, Index_ s, Index_ l) {
                auto ext = tatami::consecutive_extractor<false>(p, !row, 0, otherdim, s, l);
                std::vector<Value_> buffer(l);
                extreme::RunningDense<true, skip_nan_, Value_, Index_, Output_> runmin(l, (store_min ? min_out + s : NULL));
                extreme::RunningDense<false, skip_nan_, Value_, Index_, Output_> runmax(l, (store_max ? max_out + s : NULL));

                for (Index_ x = 0; x < otherdim; ++x) {
                    auto ptr = ext->fetch(buffer.data());
                    if (store_min) {
                        runmin.add(ptr);
                    }
                    if (store_max) {
                        runmax.add(ptr);
                    }
                }
            }, dim, threads);
        }
    }

    return;
}

/**
 * @tparam Output Type of the output value.
 * @tparam Value_ Type of the matrix value.
 * @tparam Index_ Type of the row/column indices.
 *
 * @param p Pointer to a `tatami::Matrix`.
 * @param[out] output Pointer to an array of length equal to the number of columns.
 * On output, this contains the maximum value in each column.
 * @param threads Number of threads to use.
 */
template<bool skip_nan_, typename Value_, typename Index_, typename Output_>
void column_maxs(const tatami::Matrix<Value_, Index_>* p, Output_* output, int threads = 1) {
    extremes<skip_nan_>(false, p, threads, static_cast<Output_*>(NULL), output);
    return;
}

/**
 * @tparam Output Type of the output value.
 * @tparam Value_ Type of the matrix value.
 * @tparam Index_ Type of the row/column indices.
 *
 * @param p Pointer to a `tatami::Matrix`.
 * @param threads Number of threads to use.
 *
 * @return A vector of length equal to the number of columns, containing the maximum value in each column.
 */
template<bool skip_nan_ = false, typename Output_ = double, typename Value_, typename Index_>
std::vector<Output_> column_maxs(const tatami::Matrix<Value_, Index_>* p, int threads = 1) {
    std::vector<Output_> output(p->ncol());
    column_maxs<skip_nan_>(p, output.data(), threads);
    return output;
}

/**
 * @tparam Value_ Type of the matrix value.
 * @tparam Index_ Type of the row/column indices.
 * @tparam Output Type of the output value.
 *
 * @param p Pointer to a `tatami::Matrix`.
 * @param[out] output Pointer to an array of length equal to the number of rows.
 * On output, this contains the maximum value in each row.
 * @param threads Number of threads to use.
 */
template<bool skip_nan_ = false, typename Value_, typename Index_, typename Output_>
void row_maxs(const tatami::Matrix<Value_, Index_>* p, Output_* output, int threads = 1) {
    extremes<skip_nan_>(true, p, threads, static_cast<Output_*>(NULL), output);
    return;
}

/**
 * @tparam Output Type of the output value.
 * @tparam Value_ Type of the matrix value.
 * @tparam Index_ Type of the row/column indices.
 *
 * @param p Pointer to a `tatami::Matrix`.
 * @param threads Number of threads to use.
 *
 * @return A vector of length equal to the number of rows, containing the maximum value in each row.
 */
template<bool skip_nan_ = false, typename Output_ = double, typename Value_, typename Index_>
std::vector<Output_> row_maxs(const tatami::Matrix<Value_, Index_>* p, int threads = 1) {
    std::vector<Output_> output(p->nrow());
    row_maxs<skip_nan_>(p, output.data(), threads);
    return output;
}

/**
 * @tparam Value_ Type of the matrix value.
 * @tparam Index_ Type of the row/column indices.
 * @tparam Output Type of the output value.
 *
 * @param p Pointer to a `tatami::Matrix`.
 * @param[out] output Pointer to an array of length equal to the number of columns.
 * On output, this contains the minimum value in each column.
 * @param threads Number of threads to use.
 */
template<bool skip_nan_ = false, typename Value_, typename Index_, typename Output_>
void column_mins(const tatami::Matrix<Value_, Index_>* p, Output_* output, int threads = 1) {
    extremes<skip_nan_>(false, p, threads, output, static_cast<Output_*>(NULL));
    return; 
}

/**
 * @tparam Output Type of the output value.
 * @tparam Value_ Type of the matrix value.
 * @tparam Index_ Type of the row/column indices.
 *
 * @param p Pointer to a `tatami::Matrix`.
 * @param threads Number of threads to use.
 *
 * @return A vector of length equal to the number of columns, containing the minimum value in each column.
 */
template<bool skip_nan_ = false, typename Output_ = double, typename Value_, typename Index_>
std::vector<Output_> column_mins(const tatami::Matrix<Value_, Index_>* p, int threads = 1) {
    std::vector<Output_> output(p->ncol());
    column_mins<skip_nan_>(p, output.data(), threads);
    return output;
}

/**
 * @tparam Value_ Type of the matrix value.
 * @tparam Index_ Type of the row/column indices.
 * @tparam Output Type of the output value.
 *
 * @param p Pointer to a `tatami::Matrix`.
 * @param[out] output Pointer to an array of length equal to the number of rows.
 * On output, this is filled with the minimum value in each row.
 * @param threads Number of threads to use.
 */
template<bool skip_nan_ = false, typename Value_, typename Index_, typename Output_>
void row_mins(const tatami::Matrix<Value_, Index_>* p, Output_* output, int threads = 1) {
    extremes<skip_nan_>(true, p, threads, output, static_cast<Output_*>(NULL));
    return;
}

/**
 * @tparam Output Type of the output value.
 * @tparam Value_ Type of the matrix value.
 * @tparam Index_ Type of the row/column indices.
 *
 * @param p Pointer to a `tatami::Matrix`.
 * @param threads Number of threads to use.
 *
 * @return A vector of length equal to the number of rows, containing the minimum value in each row.
 */
template<bool skip_nan_ = false, typename Output_ = double, typename Value_, typename Index_>
std::vector<Output_> row_mins(const tatami::Matrix<Value_, Index_>* p, int threads = 1) {
    std::vector<Output_> output(p->nrow());
    row_mins<skip_nan_>(p, output.data(), threads);
    return output;
}

/**
 * @tparam Value_ Type of the matrix value.
 * @tparam Index_ Type of the row/column indices.
 * @tparam Output Type of the output value.
 *
 * @param p Pointer to a `tatami::Matrix`.
 * @param[out] min_output Pointer to an array of length equal to the number of rows.
 * On output, this contains the minimum value per row.
 * @param[out] max_output Pointer to an array of length equal to the number of rows.
 * On output, this contains the maximum value per row.
 * @param threads Number of threads to use.
 */
template<bool skip_nan_ = false, typename Value_, typename Index_, typename Output_>
void column_ranges(const tatami::Matrix<Value_, Index_>* p, Output_* min_output, Output_* max_output, int threads = 1) {
    extremes<skip_nan_>(false, p, threads, min_output, max_output);
    return;
}

/**
 * @tparam Output Type of the output value.
 * @tparam Value_ Type of the matrix value.
 * @tparam Index_ Type of the row/column indices.
 *
 * @param p Pointer to a `tatami::Matrix`.
 * @param threads Number of threads to use.
 *
 * @return A pair of vectors, each of length equal to the number of columns.
 * The first and second vector contains the minimum and maximum value per column, respectively.
 */
template<bool skip_nan_ = false, typename Output_ = double, typename Value_, typename Index_>
std::pair<std::vector<Output_>, std::vector<Output_> > column_ranges(const tatami::Matrix<Value_, Index_>* p, int threads = 1) {
    std::vector<Output_> mins(p->ncol()), maxs(p->ncol());
    column_ranges<skip_nan_>(p, mins.data(), maxs.data(), threads);
    return std::make_pair(std::move(mins), std::move(maxs));
}

/**
 * @tparam Value_ Type of the matrix value.
 * @tparam Index_ Type of the row/column indices.
 * @tparam Output Type of the output value.
 *
 * @param p Pointer to a `tatami::Matrix`.
 * @param[out] min_output Pointer to an array of length equal to the number of rows.
 * On output, this contains the minimum value per row.
 * @param[out] max_output Pointer to an array of length equal to the number of rows.
 * On output, this contains the maximum value per row.
 * @param threads Number of threads to use.
 */
template<bool skip_nan_ = false, typename Value_, typename Index_, typename Output_>
void row_ranges(const tatami::Matrix<Value_, Index_>* p, Output_* min_output, Output_* max_output, int threads = 1) {
    extremes<skip_nan_>(true, p, threads, min_output, max_output);
    return;
}

/**
 * @tparam Output Type of the output value.
 * @tparam Value_ Type of the matrix value.
 * @tparam Index_ Type of the row/column indices.
 *
 * @param p Pointer to a `tatami::Matrix`.
 * @param threads Number of threads to use.
 *
 * @return A pair of vectors, each of length equal to the number of rows.
 * The first and second vector contains the minimum and maximum value per row, respectively.
 */
template<bool skip_nan_ = false, typename Output_ = double, typename Value_, typename Index_>
std::pair<std::vector<Output_>, std::vector<Output_> > row_ranges(const tatami::Matrix<Value_, Index_>* p, int threads = 1) {
    std::vector<Output_> mins(p->nrow()), maxs(p->nrow());
    row_ranges<skip_nan_>(p, mins.data(), maxs.data(), threads);
    return std::make_pair(std::move(mins), std::move(maxs));
}

}

#endif
