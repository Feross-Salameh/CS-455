#ifndef GREG_HPP
#define GREG_HPP

#pragma once
#include "stdHeaders.hpp"
#include "http-parser-master\http_parser.h"
#include <winsock2.h>
#include <iostream>
#include <WS2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

int fromBrowser(int clientSocketFd); // Handles message traffic from the browser to the internet.

#endif // GREG_HPP
