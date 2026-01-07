file(GLOB_RECURSE kipaco_src CONFIGURE_DEPENDS "src/*.cpp")

add_library(cxx_setup INTERFACE)

add_library(kipaco ${kipaco_src})

target_include_directories(kipaco PUBLIC include)
