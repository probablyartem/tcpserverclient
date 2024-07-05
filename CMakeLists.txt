CMAKE_MINIMUM_REQUIRED(VERSION 3.21)
PROJECT(ClienServerEcn)

FIND_PACKAGE(Boost 1.40 COMPONENTS system REQUIRED)
FIND_PACKAGE(Threads REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

SET(CMAKE_CXX_STANDARD 17)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Объединяем определение Server
ADD_EXECUTABLE(Server Server.cpp core.cpp Common.hpp json.hpp)
TARGET_LINK_LIBRARIES(Server PRIVATE Threads::Threads ${Boost_LIBRARIES})

ADD_EXECUTABLE(Client Client.cpp Common.hpp json.hpp)
TARGET_LINK_LIBRARIES(Client PRIVATE Threads::Threads ${Boost_LIBRARIES})

enable_testing()

add_executable(
    test_core
    test/test_core.cpp
    core.cpp  # Добавляем core.cpp в тесты
)
target_link_libraries(
    test_core
    GTest::gtest_main 
    ${Boost_LIBRARIES}  
)

include(GoogleTest)
gtest_discover_tests(test_core)