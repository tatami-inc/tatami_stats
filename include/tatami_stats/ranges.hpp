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

template<bool get_minimum_, typename Value_>
bool is_better(Value_ best, Value_ alt) {
    if constexpr(get_minimum_) {
        return best > alt;
    } else {
        return best < alt;
    }
}

}
/**
 * @endcond
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

template<bool get_minimum_, bool skip_nan_ = false, typename Output_ = double, typename Index_ = int>
struct RunningDense {
    RunningDense(Index_ num, Output_* store) : num(num), store(store) {}

    template<typename Value_>
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

private:
    bool init = true;
    Index_ num;
    Output_* store;
};

template<bool get_minimum_, bool skip_nan_ = false, typename Output_ = double, typename Index_ = int>
struct RunningSparse {
    RunningSparse(Index_ num, Output_* store, Index_ subtract = 0) : num(num), store(store), subtract(subtract) {}

    template<typename Value_>
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

    void finish() {
        for (Index_ i = 0; i < num; ++i) {
            if (count > nonzero[i]) {
                auto& current = store[i];
                if (internal::is_better<get_minimum_>(current, static_cast<Output_>(0))) {
                    current = 0;
                }
            }
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

                extreme::RunningSparse<true, skip_nan_, Output_, Index_> runmin(l, (store_min ? min_out + s : NULL), s);
                extreme::RunningSparse<false, skip_nan_, Output_, Index_> runmax(l, (store_max ? max_out + s : NULL), s);

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

                extreme::RunningDense<true, skip_nan_, Output_, Index_> runmin(l, (store_min ? min_out + s : NULL));
                extreme::RunningDense<false, skip_nan_, Output_, Index_> runmax(l, (store_max ? max_out + s : NULL));

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
    extremes<skip_nan_, Output_>(true, p, threads, static_cast<Output_*>(NULL), output);
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
