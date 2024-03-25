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

#endif
