cmake_minimum_required(VERSION 3.24)

project(tatami_stats
    VERSION 1.1.1
    DESCRIPTION "Matrix statistics for tatami"
    LANGUAGES CXX)

add_library(tatami_stats INTERFACE)
add_library(tatami::tatami_stats ALIAS tatami_stats)

target_compile_features(tatami_stats INTERFACE cxx_std_17)

option(TATAMI_STATS_FETCH_EXTERN "Automatically fetch tatami_stats's external dependencies." ON)
if(TATAMI_STATS_FETCH_EXTERN)
    add_subdirectory(extern)
else()
    find_package(tatami_tatami 3.0.0 CONFIG REQUIRED)
    find_package(ltla_subpar 0.3.0 CONFIG REQUIRED)
endif()

target_link_libraries(tatami_stats INTERFACE tatami::tatami ltla::subpar)

include(GNUInstallDirs)
target_include_directories(tatami_stats INTERFACE 
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/tatami_tatami_stats>"
)

# Building the test-related machinery, if we are compiling this library directly.
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    option(TATAMI_STATS_TESTS "Build tatami_stats's test suite." ON)
else()
    option(TATAMI_STATS_TESTS "Build tatami_stats's test suite." OFF)
endif()

if(TATAMI_STATS_TESTS)
    include(CTest)
    if(BUILD_TESTING)
        add_subdirectory(tests)
    endif() 
endif()

# Installing for find_package.
include(CMakePackageConfigHelpers)

install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/tatami_tatami_stats)

install(TARGETS tatami_stats
    EXPORT tatami_statsTargets)

install(EXPORT tatami_statsTargets
    FILE tatami_tatami_statsTargets.cmake
    NAMESPACE tatami::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/tatami_tatami_stats)

configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Config.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/tatami_tatami_statsConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/tatami_tatami_stats)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/tatami_tatami_statsConfigVersion.cmake
    COMPATIBILITY SameMajorVersion)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/tatami_tatami_statsConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/tatami_tatami_statsConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/tatami_tatami_stats)
