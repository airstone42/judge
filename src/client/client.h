#ifndef CLIENT_H
#define CLIENT_H

#include <zmq.hpp>

#include <string>

extern const char *const TCP_ADDRESS;

void run(const std::string &input, zmq::socket_t &socket);

#endif