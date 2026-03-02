#ifndef UTILS_H
#define UTILS_H

#include <cmath>

template<class L_, class R_>
void compare_double_vectors(const L_& left, const R_& right) {
    ASSERT_EQ(left.size(), right.size());
    for (size_t i = 0; i < left.size(); ++i) {
        EXPECT_FLOAT_EQ(left[i], right[i]);
    }
    return;
}

template<class V_>
bool is_all_nan(const V_& v) {
    for (auto x : v) {
        if (!std::isnan(x)) {
            return false;
        }
    }
    return true;
}

template<typename Rng_>
void inject_variable_zeros(std::size_t primary, std::size_t secondary, std::vector<double>& vec, Rng_& rng) {
    // Turning elements to zero based on its primary dimension index; we get more and more zeros as we get to later dimension elements.
    // The aim is to check that certain metrics (e.g., medians, quantiles) are computed correctly from sparse data of varying density.
    for (std::size_t p = 0; p < primary; ++p) {
        auto vStart = vec.begin() + p * secondary;
        auto vEnd = vStart + secondary;
        const std::size_t keep = std::ceil((static_cast<double>(p) / (primary - 1)) * static_cast<double>(secondary));
        std::fill(vStart, vStart + keep, 0);
        std::shuffle(vStart, vEnd, rng);
    }
}

#endif
