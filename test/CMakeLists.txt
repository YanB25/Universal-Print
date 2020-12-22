add_executable(naive-test naive-test.cpp)
target_link_libraries(naive-test gtest_main helloworld_obj)
add_test(NAME naive-test COMMAND naive-test)

enable_testing()