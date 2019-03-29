cmake_minimum_required(VERSION 3.14)
project(Judgement)

set(CMAKE_CXX_STANDARD 17)

add_executable(Judge src/main.cpp src/judgement.cpp src/judgement.h src/judge.cpp src/judge.h  src/container.cpp src/container.h)
add_executable(Client src/client/client.cpp src/client/client.h)

target_link_libraries(Judge pthread zmq)
target_link_libraries(Client zmq)
