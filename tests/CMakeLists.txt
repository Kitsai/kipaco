include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.5.2
)
FetchContent_MakeAvailable(Catch2)

file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS "*.cpp")

add_executable(tests ${TEST_SOURCES})

target_link_libraries(tests PRIVATE Catch2::Catch2WithMain kipaco)

include(CTest)
include(Catch)
catch_discover_tests(tests)
