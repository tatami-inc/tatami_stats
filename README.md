# Matrix statistics for tatami

![Unit tests](https://github.com/tatami-inc/tatami_stats/actions/workflows/run-tests.yaml/badge.svg)
![Documentation](https://github.com/tatami-inc/tatami_stats/actions/workflows/doxygenate.yaml/badge.svg)
[![Codecov](https://codecov.io/gh/tatami-inc/tatami_stats/branch/master/graph/badge.svg?token=wt1JXHOpEk)](https://codecov.io/gh/tatami-inc/tatami_stats)

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
auto row_medians = tatami_stats::median(true, *mat, {});

// Column-wise variances with NaN skipping and multiple threads.
tatami_stats::VarianceOptions vopt;
vopt.skip_nan = true;
vopt.num_threads = 5;
auto vres = tatami_stats::variance(false, *mat, vopt);
vres.mean; // we get the means as a side-effect.
vres.variance;

// We can also fill up an existing buffer.
tatami_stats::SumOptions sopt;
sopt.num_threads = 2;
std::vector<double> sums(nrows);
tatami_stats::sum(true, *mat, sums.data(), sopt);
```

Check out the [API documentation](https://tatami-inc.github.io/tatami_stats) for more details.

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

By default, this will use `FetchContent` to fetch all external dependencies.
Applications are advised to pin the versions of these dependencies themselves - see [`extern/CMakeLists.txt`](extern/CMakeLists.txt) for suggested (minimum) versions.
If you want to install them manually, use `-DTATAMI_STATS_FETCH_EXTERN=OFF`.

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

Again, this will use `FetchContent` to retrieve dependencies, see advice above.

### Manual

If you're not using CMake, the simple approach is to just copy the files the `include/` subdirectory - 
either directly or with Git submodules - and include their path during compilation with, e.g., GCC's `-I`.
This also requires the dependencies listed in [`extern/CMakeLists.txt`](extern/CMakeLists.txt). 
