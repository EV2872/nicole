# cmake/FindCatch2.cmake
# —————————————————————————————————————————————————————————————
# Install Catch2 for tests
# —————————————————————————————————————————————————————————————

include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.7.0
)

FetchContent_MakeAvailable(Catch2)

get_target_property(_catch2_incs Catch2::Catch2WithMain INTERFACE_INCLUDE_DIRECTORIES)