cmake_minimum_required(VERSION 3.14)
project(Judgement)

set(CMAKE_CXX_STANDARD 17)

add_executable(Judge main.cpp judgement.cpp judgement.h judge.cpp judge.h  container.cpp container.h)
add_executable(Client client.cpp)

target_link_libraries(Judge pthread zmq)
target_link_libraries(Client zmq)
