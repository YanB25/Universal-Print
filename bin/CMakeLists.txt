add_executable(main main.cpp)
target_link_libraries(main PRIVATE helloworld_obj)

add_executable(log_test log_test.cpp)
target_link_libraries(log_test PRIVATE helloworld_obj)