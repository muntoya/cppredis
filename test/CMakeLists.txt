
include_directories(.)
file(GLOB srcs *.cpp)
add_executable (redistest ${srcs})
set_target_properties(redistest PROPERTIES OUTPUT_NAME "redistest")
target_link_libraries(redistest redisutil hiredis boost_unit_test_framework boost_filesystem boost_system boost_regex)
add_dependencies(redistest redisutil boost_unit_test_framework)

