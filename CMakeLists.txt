cmake_minimum_required(VERSION 3.13)
project(Judge)

set(CMAKE_CXX_STANDARD 17)

add_executable(Judge src/main.cpp src/util.cpp src/util.h src/judge.cpp src/judge.h src/container.cpp src/container.h src/status.cpp src/status.h src/path.cpp src/path.h)
add_executable(Client tests/client/client.cpp tests/client/client.h)
add_executable(Jailed tests/jailed.c src/seccomp/rules.c src/seccomp/rules.h)

target_link_libraries(Judge pthread zmq stdc++fs)
target_link_libraries(Client zmq)
target_link_libraries(Jailed dl seccomp)
