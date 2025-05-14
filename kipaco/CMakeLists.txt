file(GLOB_RECURSE kipaco_src CONFIGURE_DEPENDS "src/*.cpp")


add_library(cxx_setup INTERFACE)
target_compile_options(cxx_setup INTERFACE -Wall -Wpedantic -Wextra)

add_library(kipaco ${kipaco_src})

target_include_directories(kipaco PUBLIC include)
target_link_libraries(kipaco PUBLIC cxx_setup)
