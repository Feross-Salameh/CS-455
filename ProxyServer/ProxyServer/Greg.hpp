#ifndef GREG_HPP
#define GREG_HPP

#pragma once
#include "stdHeaders.hpp"
#include "http-parser-master\http_parser.h"
#include <iostream>
#include <sstream>

#pragma comment(lib, "Ws2_32.lib")

int messageHandler(int clientSocketFd, char* target_port); // Handles message traffic from the browser to the internet or vice versa.

#endif // GREG_HPP
