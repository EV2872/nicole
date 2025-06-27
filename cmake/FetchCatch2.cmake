# cmake/FindCatch2.cmake
# —————————————————————————————————————————————————————————————
# Install Catch2 for tests
# —————————————————————————————————————————————————————————————

include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY ${CATCH2_URL}
  GIT_TAG        ${CATCH2_VERSION}
)

FetchContent_MakeAvailable(Catch2)

get_target_property(_catch2_incs Catch2::Catch2WithMain INTERFACE_INCLUDE_DIRECTORIES)