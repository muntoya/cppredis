
include_directories(.)
file(GLOB srcs *.cpp)
add_library (redisutil STATIC ${srcs})
add_subdirectory(test)
