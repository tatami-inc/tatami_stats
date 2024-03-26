#ifndef TATAMI_STATS_VARS_HPP
#define TATAMI_STATS_VARS_HPP

#include "tatami/tatami.hpp"

#include <vector>
#include <cmath>
#include <numeric>
#include <limits>
#include <algorithm>

/**
 * @file variances.hpp
 *
 * @brief Compute row and column variances from a `tatami::Matrix`.
 */

namespace tatami_stats {

/**
 * @brief Functions for computing dimension-wise variances.
 * @namespace tatami_stats::variance
 */
namespace variance {

/**
 * Compute the mean and variance from an array of values.
 * This is done using Welford's method for numerical stability (and consistency with `RunningDense`).
 *
 * @tparam skip_nan_ Whether to check for (and skip) NaNs.
 * @tparam Output_ Type of the output data.
 * @tparam Value_ Type of the input data.
 *
 * @param[in] ptr Pointer to an array of values of length `num`.
 * @param num Size of the array.
 *
 * @return The sample mean and variance of values in `[ptr, ptr + num)`.
 * This may be NaN if there are not enough (non-NaN) values in `ptr`.
 */
template<bool skip_nan_ = false, typename Output_ = double, typename Value_, typename Index_ >
std::pair<Output_, Output_> compute(const Value_* ptr, Index_ num) {
    Output_ mean = 0;
    Output_ var = 0;
    Index_ count = 0;

    if constexpr(skip_nan_) {
        for (Index_ i = 0; i < num; ++i, ++ptr) {
            if (!std::isnan(*ptr)) {
                ++count;
                Output_ delta = *ptr - mean;
                mean += delta / count;
                var += delta * (*ptr - mean);
            }
        }
    } else {
        for (Index_ i = 1; i <= num; ++i, ++ptr) {
            Output_ delta = *ptr - mean;
            mean += delta / i;
            var += delta * (*ptr - mean);
        }
        count = num;
    }

    if (count == 0) {
        return std::make_pair(std::numeric_limits<Output_>::quiet_NaN(), std::numeric_limits<Output_>::quiet_NaN());
    } else if (count == 1) {
        return std::make_pair(mean, std::numeric_limits<Output_>::quiet_NaN());
    } else {
        return std::make_pair(mean, var / (count - 1));
    }
}

/**
 * Compute the mean and variance from sparse data.
 * This achieves faster processing by only performing summations over non-zero elements.
 * We use Welford's method for numerical stability (and consistency with `RunningSparse`).
 *
 * @tparam skip_nan_ Whether to check for (and skip) NaNs.
 * @tparam Output_ Type of the output data.
 * @tparam Value_ Type of the input data.
 * @tparam Index_ Type of the indices.
 *
 * @param[in] value Pointer to an array of values for structural non-zero elements.
 * @param num_nonzero Length of the array pointed to by `value`.
 * @param num_all Total number of values in the dataset, including the zeros not in `value`.
 * This should be greater than or equal to `num_nonzero`.
 *
 * @return The sample mean and variance of values in the vector.
 * This may be NaN if there are not enough (non-NaN) values in `value`.
 */
template<bool skip_nan_ = false, typename Output_ = double, typename Value_, typename Index_>
std::pair<Output_, Output_> compute(const Value_* value, Index_ num_nonzero, Index_ num_all) {
    Output_ mean = 0;
    Output_ var = 0;
    Index_ count = 0;

    if constexpr(skip_nan_) {
        for (Index_ i = 0; i < num_nonzero; ++i, ++value) {
            if (!std::isnan(*value)) {
                ++count;
                Output_ delta = *value - mean;
                mean += delta / count;
                var += delta * (*value - mean);
            }
        }

        if (num_nonzero < num_all) {
            auto num_zero = num_all - num_nonzero;
            auto ratio = static_cast<Output_>(count) / static_cast<Output_>(count + num_zero);
            var += mean * mean * ratio * num_zero;
            mean *= ratio;
            count += num_zero;
        }

    } else {
        for (Index_ i = 1; i <= num_nonzero; ++i, ++value) {
            Output_ delta = *value - mean;
            mean += delta / i;
            var += delta * (*value - mean);
        }

        if (num_nonzero < num_all) {
            auto ratio = static_cast<Output_>(num_nonzero) / static_cast<Output_>(num_all);
            var += mean * mean * ratio * (num_all - num_nonzero);
            mean *= ratio;
        }

        count = num_all;
    }

    if (count == 0) {
        return std::make_pair(std::numeric_limits<Output_>::quiet_NaN(), std::numeric_limits<Output_>::quiet_NaN());
    } else if (count == 1) {
        return std::make_pair(mean, std::numeric_limits<Output_>::quiet_NaN());
    } else {
        return std::make_pair(mean, var / (count - 1));
    }
}

/**
 * @brief Running variances from dense data.
 *
 * Compute running means and variances from dense data using Welford's method.
 * This considers a scenario with a set of equilength "target" vectors [V1, V2, V3, ..., Vn],
 * but data are only available for "observed" vectors [P1, P2, P3, ..., Pm],
 * where Pi[j] contains the i-th element of target vector Vj.
 * The idea is to repeatedly call `add()` for `ptr` corresponding to observed vectors from 0 to m - 1,
 * and then finally call `finish()` to obtain the mean and variance for each target vector.
 *
 * @tparam skip_nan_ Whether to check for (and skip) NaNs.
 * @tparam Output_ Type of the output data.
 * @tparam Index_ Type of the row/column indices.
 */
template<bool skip_nan_ = false, typename Output_ = double, typename Index_ = int>
struct RunningDense {
    /**
     * @param num Number of target vectors, i.e., n.
     * @param[out] mean Pointer to an output array of length `num`, containing the means for each target vector.
     * This should be zeroed.
     * @param[out] variance Pointer to an output array of length `num`, containing the variances for each target vector.
     * This should be zeroed.
     */
    RunningDense(Index_ num, Output_* mean, Output_* variance) : 
        num(num), mean(mean), variance(variance), count(skip_nan_ ? num : 0) {}

    /**
     * Add the next observed vector to the variance calculation.
     * @tparam Value_ Type of the input data.
     * @param[in] ptr Pointer to an array of values of length `num`, corresponding to an observed vector.
     */
    template<typename Value_>
    void add(const Value_* ptr) {
        if constexpr(skip_nan_) {
            for (Index_ i = 0; i < num; ++i, ++ptr) {
                if (!std::isnan(*ptr)) {
                    auto& ct = count[i];
                    ++ct;
                    auto& curm = mean[i];
                    Output_ delta = *ptr - curm;
                    curm += delta / ct;
                    variance[i] += delta * (*ptr - curm);
                }
            }

        } else {
            ++count;
            for (Index_ i = 0; i < num; ++i, ++ptr) {
                auto& curm = mean[i];
                Output_ delta = *ptr - curm;
                curm += delta / count;
                variance[i] += delta * (*ptr - curm);
            }
        }
    }

    /**
     * Finish the variance calculation once all observed vectors have been passed to `add()`. 
     */
    void finish() {
        if constexpr(skip_nan_) {
            for (Index_ i = 0; i < num; ++i) {
                auto ct = count[i];
                if (ct < 2) {
                    variance[i] = std::numeric_limits<Output_>::quiet_NaN();
                    if (ct == 0) {
                        mean[i] = std::numeric_limits<Output_>::quiet_NaN();
                    }
                } else {
                    variance[i] /= ct - 1;
                }
            }
        } else {
            if (count < 2) {
                std::fill_n(variance, num, std::numeric_limits<Output_>::quiet_NaN());
                if (count == 0) {
                    std::fill_n(mean, num, std::numeric_limits<Output_>::quiet_NaN());
                }
            } else {
                for (Index_ i = 0; i < num; ++i) {
                    variance[i] /= count - 1;
                }
            }
        }
    }

private:
    Index_ num;
    Output_* mean;
    Output_* variance;
    typename std::conditional<skip_nan_, std::vector<Index_>, Index_>::type count;
};

/**
 * @brief Running variances from sparse data.
 *
 * Compute running means and variances from sparse data using Welford's method.
 * This does the same as its dense overload for sparse observed vectors.
 *
 * @tparam skip_nan_ Whether to check for (and skip) NaNs.
 * @tparam skip_zeros Whether non-structural zeros in `range.value` should be skipped.
 * If `false`, the output `nonzeros` instead contains the number of _structural_ non-zero values in each target vector,
 * which may be useful for informing further operations on the compressed sparse matrix structure.
 * Note that this choice has no effect on the computed means or variances, besides some differences due to numeric imprecision.
 * @tparam Output_ Type of the output data.
 * @tparam Index_ Type of the row/column indices.
 * @tparam Nonzero_ Type of the non-zero counts.
 */
template<bool skip_nan_ = false, bool skip_zeros_ = true, typename Output_ = double, typename Index_ = int, typename Nonzero_ = int>
struct RunningSparse {
    /**
     * @param num Number of target vectors.
     * @param[out] mean Pointer to an output array of length `num`, containing the means for each target vector.
     * This should be zeroed.
     * @param[out] variance Pointer to an output array of length `num`, containing the variances for each target vector.
     * This should be zeroed.
     * @param[out] nonzero Pointer to an output array of length `num` containing the number of non-zeros for each target vector.
     * This should be zeroed.
     * @param subtract Offset to subtract from each element of `index` before using it to index into `mean` and friends.
     * Only relevant if `mean` and friends hold statistics for a contiguous subset of target vectors,
     * e.g., during task allocation for parallelization.
     */
    RunningSparse(Index_ num, Output_* mean, Output_* variance, Nonzero_* nonzero, Index_ subtract = 0) : 
        num(num), mean(mean), variance(variance), nonzero(nonzero), subtract(subtract), nan(skip_nan_ ? num : 0) {}

    /**
     * Add the next observed vector to the variance calculation.
     * @tparam Value_ Type of the input value.
     * @param[in] value Value of structural non-zero elements.
     * @param[in] index Index of structural non-zero elements.
     * @param number Number of non-zero elements in `value` and `index`.
     */
    template<typename Value_>
    void add(const Value_* value, const Index_* index, Index_ number) {
        ++count;

        if constexpr(skip_nan_) {
            for (Index_ i = 0; i < number; ++i) {
                auto val = value[i];

                if (std::isnan(val)) {
                    auto ri = index[i] - subtract;
                    ++nan[ri];
                } else {
                    if ([&]() {
                        if constexpr(skip_zeros_) {
                            return val != 0;
                        } else {
                            return true;
                        }
                    }()) {
                        auto ri = index[i] - subtract;
                        auto& curM = mean[ri];
                        auto& curS = variance[ri];
                        auto& curNZ = nonzero[ri];
                        ++curNZ;

                        Output_ delta = val - curM;
                        curM += delta / curNZ;
                        curS += delta * (val - curM);
                    }
                }
            }

        } else {
            for (Index_ i = 0; i < number; ++i) {
                auto val = value[i];
                if ([&]() {
                    if constexpr(skip_zeros_) {
                        return val != 0;
                    } else {
                        return true;
                    }
                }()) {
                    auto ri = index[i] - subtract;
                    auto& curM = mean[ri];
                    auto& curS = variance[ri];
                    auto& curNZ = nonzero[ri];
                    ++curNZ;

                    Output_ delta = val - curM;
                    curM += delta / curNZ;
                    curS += delta * (val - curM);
                }
            }
        }
    }

    /**
     * Finish the variance calculation once all observed vectors have been passed to `add()`. 
     */
    void finish() {
        if constexpr(skip_nan_) {
            for (Index_ i = 0; i < num; ++i) {
                auto& curM = mean[i];
                auto& curV = variance[i];
                auto ct = count - nan[i];

                if (ct < 2) {
                    curV = std::numeric_limits<Output_>::quiet_NaN();
                    if (ct == 0) {
                        curM = std::numeric_limits<Output_>::quiet_NaN();
                    }
                } else {
                    auto curNZ = nonzero[i];
                    auto ratio = static_cast<Output_>(curNZ) / static_cast<Output_>(ct);
                    curV += curM * curM * ratio * (ct - curNZ);
                    curV /= ct - 1;
                    curM *= ratio;
                }
            }

        } else {
            if (count < 2) {
                std::fill_n(variance, num, std::numeric_limits<Output_>::quiet_NaN());
                if (count == 0) {
                    std::fill_n(mean, num, std::numeric_limits<Output_>::quiet_NaN());
                }
            } else {
                for (Index_ i = 0; i < num; ++i) {
                    auto curNZ = nonzero[i];
                    auto ratio = static_cast<Output_>(curNZ) / static_cast<Output_>(count);
                    auto& curM = mean[i];
                    auto& curV = variance[i];
                    curV += curM * curM * ratio * (count - curNZ);
                    curV /= count - 1;
                    curM *= ratio;
                }
            }
        }
    }

private:
    Index_ num;
    Output_* mean;
    Output_* variance;
    Nonzero_* nonzero;
    Index_ subtract;
    Index_ count = 0;
    typename std::conditional<skip_nan_, std::vector<Index_>, bool>::type nan;
};

}

/**
 * Compute variances for each element of a chosen dimension of a `tatami::Matrix`.
 *
 * @tparam skip_nan_ Whether to check for (and skip) NaNs.
 * @tparam Value_ Type of the matrix value, should be numeric.
 * @tparam Index_ Type of the row/column indices.
 * @tparam Output_ Type of the output value.
 *
 * @param row Whether to compute variances for the rows.
 * @param p Pointer to a `tatami::Matrix`.
 * @param[out] output Pointer to an array of length equal to the number of rows (if `row = true`) or columns (otherwise).
 * On output, this will contain the row/column variances.
 * @param threads Number of threads to use.
 */
template<bool skip_nan_ = false, typename Value_, typename Index_, typename Output_>
void variances(bool row, const tatami::Matrix<Value_, Index_>* p, Output_* output, int threads) {
    auto dim = (row ? p->nrow() : p->ncol());
    auto otherdim = (row ? p->ncol() : p->nrow());
    const bool direct = p->prefer_rows() == row;

    if (p->sparse()) {
        if (direct) {
            tatami::Options opt;
            opt.sparse_extract_index = false;
            tatami::parallelize([&](size_t, Index_ s, Index_ l) {
                auto ext = tatami::consecutive_extractor<true>(p, row, s, l);
                std::vector<Value_> vbuffer(otherdim);
                for (Index_ x = 0; x < l; ++x) {
                    auto out = ext->fetch(vbuffer.data(), NULL);
                    output[x + s] = variance::compute<skip_nan_, Output_>(out.value, out.number, otherdim).second;
                }
            }, dim, threads);

        } else {
            std::fill(output, output + dim, static_cast<Output_>(0));
            tatami::parallelize([&](size_t, Index_ s, Index_ l) {
                auto ext = tatami::consecutive_extractor<true>(p, !row, 0, otherdim, s, l);
                std::vector<Value_> vbuffer(l);
                std::vector<Index_> ibuffer(l);
                std::vector<Output_> running_means(l);
                std::vector<Index_> running_nzeros(l);
                variance::RunningSparse<skip_nan_, true, Output_, Index_, Index_> runner(l, running_means.data(), output + s, running_nzeros.data(), s);
                for (Index_ x = 0; x < otherdim; ++x) {
                    auto out = ext->fetch(vbuffer.data(), ibuffer.data());
                    runner.add(out.value, out.index, out.number);
                }
                runner.finish();
            }, dim, threads);
        }

    } else {
        if (direct) {
            tatami::parallelize([&](size_t, Index_ s, Index_ l) {
                auto ext = tatami::consecutive_extractor<false>(p, row, s, l);
                std::vector<Value_> buffer(otherdim);
                for (Index_ x = 0; x < l; ++x) {
                    auto out = ext->fetch(buffer.data());
                    output[x + s] = variance::compute<skip_nan_, Output_>(out, otherdim).second;
                }
            }, dim, threads);

        } else {
            std::fill(output, output + dim, static_cast<Output_>(0));
            tatami::parallelize([&](size_t, Index_ s, Index_ l) {
                auto ext = tatami::consecutive_extractor<false>(p, !row, 0, otherdim, s, l);
                std::vector<Value_> buffer(l);
                std::vector<Output_> running_means(l);
                variance::RunningDense<skip_nan_, Output_, Index_> runner(l, running_means.data(), output + s);
                for (Index_ x = 0; x < otherdim; ++x) {
                    runner.add(ext->fetch(buffer.data()));
                }
                runner.finish();
            }, dim, threads);
        }
    }
}

/**
 * @tparam skip_nan_ Whether to check for (and skip) NaNs.
 * @tparam Value_ Type of the matrix value, should be numeric.
 * @tparam Index_ Type of the row/column indices.
 * @tparam Output_ Type of the output value.
 *
 * @param p Pointer to a `tatami::Matrix`.
 * @param[out] output Pointer to an array of length equal to the number of columns.
 * On output, this will contain the column variances.
 * @param threads Number of threads to use.
 */
template<bool skip_nan_ = false, typename Value_, typename Index_, typename Output_>
void column_variances(const tatami::Matrix<Value_, Index_>* p, Output_* output, int threads = 1) {
    variances<skip_nan_>(false, p, output, threads);
    return;
}

/**
 * @tparam skip_nan_ Whether to check for (and skip) NaNs.
 * @tparam Output_ Type of the output value.
 * @tparam Value_ Type of the matrix value, should be numeric.
 * @tparam Index_ Type of the row/column indices.
 *
 * @param p Pointer to a `tatami::Matrix`.
 * @param threads Number of threads to use.
 *
 * @return A vector of length equal to the number of columns, containing the column variances.
 */
template<bool skip_nan_ = false, typename Output_ = double, typename Value_, typename Index_>
std::vector<Output_> column_variances(const tatami::Matrix<Value_, Index_>* p, int threads = 1) {
    std::vector<Output_> output(p->ncol());
    column_variances<skip_nan_>(p, output.data(), threads);
    return output;
}

/**
 * @tparam skip_nan_ Whether to check for (and skip) NaNs.
 * @tparam Value_ Type of the matrix value, should be numeric.
 * @tparam Index_ Type of the row/column indices.
 * @tparam Output_ Type of the output value.
 *
 * @param p Pointer to a `tatami::Matrix`.
 * @param[out] output Pointer to an array of length equal to the number of rows.
 * On output, this will be filled with the row variances.
 * @param threads Number of threads to use.
 */
template<bool skip_nan_ = false, typename Value_, typename Index_, typename Output_>
void row_variances(const tatami::Matrix<Value_, Index_>* p, Output_* output, int threads = 1) {
    variances<skip_nan_>(true, p, output, threads);
    return;
}

/**
 * @tparam skip_nan_ Whether to check for (and skip) NaNs.
 * @tparam Output_ Type of the output value.
 * @tparam Value_ Type of the matrix value, should be numeric.
 * @tparam Index_ Type of the row/column indices.
 *
 * @param p Pointer to a `tatami::Matrix`.
 * @param threads Number of threads to use.
 *
 * @return A vector of length equal to the number of rows, containing the row variances.
 */
template<bool skip_nan_ = false, typename Output_ = double, typename Value_, typename Index_>
std::vector<Output_> row_variances(const tatami::Matrix<Value_, Index_>* p, int threads = 1) {
    std::vector<Output_> output(p->nrow());
    row_variances<skip_nan_>(p, output.data(), threads);
    return output;
}

}

#endif