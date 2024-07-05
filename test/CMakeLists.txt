enable_testing()

# Build test executables
add_executable(
    hello_test
    hello_test.cc
    test_client.cpp
)
target_link_libraries(
    hello_test
    GTest::gtest_main 
    ${Boost_LIBRARIES}  
)

include(GoogleTest)
gtest_discover_tests(hello_test)
