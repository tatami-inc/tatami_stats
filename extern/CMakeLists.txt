include(FetchContent)

FetchContent_Declare(
  tatami
  GIT_REPOSITORY https://github.com/tatami-inc/tatami
  GIT_TAG master # ^3.0.0
)

FetchContent_Declare(
  subpar
  GIT_REPOSITORY https://github.com/LTLA/subpar
  GIT_TAG master # ^0.3.0
)

FetchContent_MakeAvailable(tatami)
FetchContent_MakeAvailable(subpar)
