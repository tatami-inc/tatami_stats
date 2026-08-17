#ifndef SKIP_NAN_UTILS_H
#define SKIP_NAN_UTILS_H

#include <random>
#include <cstddef>
#include <vector>

enum SkipNanSimulationType { NONE, RANDOM, BLOCK, GROUP };

inline void inject_nans_by_row(std::vector<double>& dump, std::size_t NR, std::size_t NC, SkipNanSimulationType nan_type, unsigned long long seed) {
    // Dump is assumed to be a row-major matrix.
    std::mt19937_64 rng(seed);

    // Either randomly inserting NaNs within each row, or creating a block of NaNs in one half of the row.
    // The latter tests that we handle situations where one thread contains all-NaNs while other threads have valid results.
    if (nan_type == RANDOM) {
        std::uniform_real_distribution runif;
        for (size_t r = 0; r < NR; ++r) {
            for (size_t c = 0; c < NC; ++c) {
                if (runif(rng) < 0.5) {
                    dump[r * NC + c] = std::numeric_limits<double>::quiet_NaN();
                }
            }
        }

    } else if (nan_type == BLOCK) {
        for (size_t r = 0; r < NR; ++r) {
            const bool left = rng() % 2;
            size_t start = (left ? 0 : NC / 2);
            size_t end = (left ? NC / 2 : NC);
            for (size_t c = start; c < end; ++c) {
                dump[r * NC + c] = std::numeric_limits<double>::quiet_NaN();
            }
        }
    }
}

inline void inject_nans_by_column(std::vector<double>& dump, std::size_t NR, std::size_t NC, SkipNanSimulationType nan_type, unsigned long long seed) {
    // Dump is assumed to be a row-major matrix.
    std::mt19937_64 rng(seed);

    // Either randomly inserting NaNs within each row, or creating a block of NaNs in one half of the row.
    // The latter tests that we handle situations where one thread contains all-NaNs while other threads have valid results.
    if (nan_type == RANDOM) {
        std::uniform_real_distribution runif;
        for (size_t c = 0; c < NC; ++c) {
            for (size_t r = 0; r < NR; ++r) {
                if (runif(rng) < 0.5) {
                    dump[r * NC + c] = std::numeric_limits<double>::quiet_NaN();
                }
            }
        }

    } else if (nan_type == BLOCK) {
        for (size_t c = 0; c < NC; ++c) {
            const bool left = rng() % 2;
            size_t start = (left ? 0 : NR / 2);
            size_t end = (left ? NR / 2 : NR);
            for (size_t r = start; r < end; ++r) {
                dump[r * NC + c] = std::numeric_limits<double>::quiet_NaN();
            }
        }
    }
}

inline void inject_nans_by_row(
    std::vector<double>& dump,
    std::size_t NR,
    std::size_t NC,
    SkipNanSimulationType nan_type,
    const std::vector<std::vector<int> >& groups,
    unsigned long long seed
) {
    if (nan_type == GROUP) {
        std::mt19937_64 rng(seed);
        for (std::size_t r = 0; r < NR; ++r) {
            auto chosen = rng() % groups.size();
            for (auto c : groups[chosen]) {
                dump[r * NC + c] = std::numeric_limits<double>::quiet_NaN();
            }
        }
    } else {
        inject_nans_by_row(dump, NR, NC, nan_type, seed);
    }
}

inline void inject_nans_by_column(
    std::vector<double>& dump,
    std::size_t NR,
    std::size_t NC,
    SkipNanSimulationType nan_type,
    const std::vector<std::vector<int> >& groups,
    unsigned long long seed
) {
    if (nan_type == GROUP) {
        std::mt19937_64 rng(seed);
        for (std::size_t c = 0; c < NC; ++c) {
            auto chosen = rng() % groups.size();
            for (auto r : groups[chosen]) {
                dump[r * NC + c] = std::numeric_limits<double>::quiet_NaN();
            }
        }
    } else {
        inject_nans_by_column(dump, NR, NC, nan_type, seed);
    }
}

#endif
