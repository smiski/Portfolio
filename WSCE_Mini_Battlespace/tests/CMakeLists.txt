add_executable(test_math test_math.cpp)
target_link_libraries(test_math PRIVATE wsce)
add_test(NAME test_math COMMAND test_math)

add_executable(test_sim test_sim.cpp)
target_link_libraries(test_sim PRIVATE wsce)
add_test(NAME test_sim COMMAND test_sim)