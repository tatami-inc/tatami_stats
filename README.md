# Matrix statistics for tatami

![Unit tests](https://github.com/tatami-inc/tatami_stats/actions/workflows/run-tests.yaml/badge.svg)
![Documentation](https://github.com/tatami-inc/tatami_stats/actions/workflows/doxygenate.yaml/badge.svg)
[![Codecov](https://codecov.io/gh/tatami-inc/tatami_stats/branch/master/graph/badge.svg?token=Z189ORCLLR)](https://codecov.io/gh/tatami-inc/tatami)

## Overview

This library contains helper functions to compute matrix statistics for [**tatami**](https://github.com/tatami-inc/tatami), 
loosely inspired by what the **matrixStats** R package does for R matrices.
It currently performs calculation of sums, variances, medians, ranges, etc. along either dimension.
Each function automatically chooses the most efficient algorithm based on the matrix properties (e.g., sparsity, preferred access).
Calculations can be parallelized across dimension elements via `tatami::parallelize()`.
If NaNs are present, they can be treated as missing and skipped.
Low-level utilities for each algorithm are also exported for developer convenience.

## Quick start

**tatami_stats** is a header-only library, so it can be easily used by just `#include`ing the relevant source files:

```cpp
#include "tatami_stats/tatami_stats.hpp"

std::shared_ptr<tatami::NumericMatrix> mat(
    new tatami::DenseRowMatrix<double, int>(nrows, ncols, vals)
);

// Compute row-wise medians.
auto row_medians = tatami_stats::medians::by_row(mat.get());

// Column-wise means and variances with NaN skipping and multiple threads.
tatami_stats::variances::Options vopt;
vopt.skip_nan = true;
vopt.num_threads = 5;
auto col_mean_and_var = tatami_stats::variances::by_column(mat.get(), vopt);
```

Check out the [API documentation](https://tatami-inc.github.io/tatami_stats) for more details.

## Lower-level functionality

The `apply()` function in each statistic's namespace offers more control over the calculation of each statistic.
For example, instead of creating a new vector, we can fill an existing array with the sums:

```cpp
std::vector<double> my_output(mat->nrow());
tatami_stats::sums::Options sopt;
tatami_stats::sums::apply(/* row = */ true, mat.get(), output.data(), sopt);
```

Some of the algorithms expose low-level functions for even more fine-grained control.
For example, we can manage the loop over the matrix rows ourselves, computing the mean and median for each row:

```cpp
auto ext = mat->dense_row();
std::vector<double> buffer(ncols);
std::vector<double> mean_output(nrows), med_output(nrows);
for (int r = 0; r < nrows; ++r) {
    auto ptr = ext->fetch(r, buffer.data());

    // Need to count the number of values in the denominator.
    auto sum = tatami_stats::sums::direct(ptr, ncols, /* skip_nan= */ true);
    size_t num_notna = 0;
    for (int c = 0; c < ncols; ++c) {
        num_notna += !std::isnan(ptr[c]);
    }
    mean_output[r] = sum / not_na;

    // Copying values into the buffer as median calculation resorts the values.
    tatami::copy_n(ptr, ncols, buffer.data());
    med_output[r] = tatami_stats::medians::direct(ptr, ncols, /* skip_nan= */ true);
}
```

These low-level functions allow developers to compute multiple statistics with a single pass through the matrix.
This is often superior to calling, e.g., `tatami_stats::sums::by_row` and then `tatami_stats::medians::by_row` separately;
doing so extracts data from the matrix twice, which may be expensive for file-backed matrices.

## Building projects 

### CMake with `FetchContent`

If you're using CMake, you just need to add something like this to your `CMakeLists.txt`:

```cmake
include(FetchContent)

FetchContent_Declare(
  tatami_stats
  GIT_REPOSITORY https://github.com/tatami-inc/tatami_stats
  GIT_TAG master # or any version of interest 
)

FetchContent_MakeAvailable(tatami_stats)
```

Then you can link to **tatami_stats** to make the headers available during compilation:

```cmake
# For executables:
target_link_libraries(myexe tatami_stats)

# For libaries
target_link_libraries(mylib INTERFACE tatami_stats)
```

### CMake with `find_package()`

You can install the library by cloning a suitable version of this repository and running the following commands:

```sh
mkdir build && cd build
cmake .. -DTATAMI_STATS_TESTS=OFF
cmake --build . --target install
```

Then you can use `find_package()` as usual:

```cmake
find_package(tatami_tatami_stats CONFIG REQUIRED)
target_link_libraries(mylib INTERFACE tatami::tatami_stats)
```

### Manual

If you're not using CMake, the simple approach is to just copy the files the `include/` subdirectory - 
either directly or with Git submodules - and include their path during compilation with, e.g., GCC's `-I`.
You'll need to include the transitive dependencies yourself,
check out [`extern/CMakeLists.txt`](extern/CMakeLists.txt) for a list.
