#ifndef CLIENT_H
#define CLIENT_H

#include <zmq.hpp>

#include <string>

int timeout = 0;
int limit = 5;

const char *const TCP_ADDRESS = "tcp://localhost:5555";

void alarm_handler(int sig);

int run(const std::string &input, zmq::socket_t &socket);

#endif