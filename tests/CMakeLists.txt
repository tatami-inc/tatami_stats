include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/afd902e992b720d1b3e106bc5e425a5768872265.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Avoid installing GoogleTest when installing this project.
option(INSTALL_GTEST "Enable installation of googletest." OFF)

FetchContent_MakeAvailable(googletest)

enable_testing()

include(GoogleTest)

option(CODE_COVERAGE "Enable coverage testing" OFF)
set(DO_CODE_COVERAGE OFF)
if(CODE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(DO_CODE_COVERAGE ON)
endif()

macro(decorate_test target)
    target_link_libraries(
        ${target}
        gtest_main
        tatami_stats
    )

    target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic -Werror)

    if(DO_CODE_COVERAGE)
        target_compile_options(${target} PRIVATE -O0 -g --coverage)
        target_link_options(${target} PRIVATE --coverage)
    endif()

    gtest_discover_tests(${target})
endmacro()

add_executable(
    libtest
    src/sums.cpp
    src/variances.cpp
    src/medians.cpp
    src/ranges.cpp
    src/counts.cpp
    src/grouped_medians.cpp
    src/grouped_sums.cpp
    src/grouped_variances.cpp
    src/utils.cpp
)
decorate_test(libtest)

# Adding custom parallelization checks for all functions that make
# use of the thread ID in their tatami::parallelize() sections.
add_executable(
    cuspartest 
    src/sums.cpp
    src/variances.cpp
    src/ranges.cpp
    src/counts.cpp
    src/grouped_medians.cpp
    src/grouped_sums.cpp
    src/grouped_variances.cpp
)
decorate_test(cuspartest)
target_compile_definitions(cuspartest PRIVATE SUBPAR_CUSTOM_PARALLELIZE_RANGE=::subpar::test_parallelize_range)
