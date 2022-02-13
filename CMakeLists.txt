cmake_minimum_required(VERSION 3.21)
project(ClienServerEcn)

FIND_PACKAGE( Boost 1.40 COMPONENTS program_options REQUIRED )
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )

set(CMAKE_CXX_STANDARD 17)

add_executable(Server Server.cpp Common.hpp json.hpp)
TARGET_LINK_LIBRARIES(Server LINK_PUBLIC ${Boost_LIBRARIES} )


add_executable(Client Client.cpp Common.hpp json.hpp)