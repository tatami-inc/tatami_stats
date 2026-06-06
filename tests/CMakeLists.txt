include(FetchContent)
FetchContent_Declare(
  tatami_test
  GIT_REPOSITORY https://github.com/tatami-inc/tatami_test
  GIT_TAG master
)
FetchContent_MakeAvailable(tatami_test)

include(GoogleTest)

option(CODE_COVERAGE "Enable coverage testing" OFF)
set(DO_CODE_COVERAGE OFF)
if(CODE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(DO_CODE_COVERAGE ON)
endif()

macro(make_test target)
    add_executable(
        ${target}
        src/sum.cpp
        src/variance.cpp
        src/median.cpp
        src/quantile.cpp
        src/range.cpp
        src/count.cpp
        src/group_median.cpp
        src/group_sum.cpp
        src/group_variance.cpp
    )

    target_link_libraries(
        ${target}
        tatami_stats
        tatami_test
    )

    target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic -Werror)

    if(DO_CODE_COVERAGE)
        target_compile_options(${target} PRIVATE -O0 -g --coverage)
        target_link_options(${target} PRIVATE --coverage)
    endif()

    gtest_discover_tests(${target})
endmacro()

make_test(libtest)

make_test(dirtytest)
target_compile_definitions(dirtytest PUBLIC TATAMI_STATS_TEST_DIRTY=1)
