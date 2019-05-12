# Judge

## Introduction

This is a prototype for an Online Judge's judger mainly built with C++.

In examples/ , there are some example code and IO files for a problem.
In tests/ , there is a TCP client and a simple web page built with PHP.

## Dependencies

- ZeroMQ (C++ binding)
- libseccomp
- php-zeromq (for web page test)

## Usage

```bash
$ cmake .
$ make
```
There will be three executable binary files, Jailed, Client and Judge.

Jailed tests if seccomp works, which ensures safety.

The message from Client is like "id:IO_path:source_code_path:source_code_extension_name:source_code_type".

The result from Judge is like "id:status_message:compilation_time_usage:execution_time_usage:execution_memory_usage".