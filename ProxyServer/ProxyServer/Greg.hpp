#ifndef GREG_HPP
#define GREG_HPP

#pragma once
#include "stdHeaders.hpp"
#include "http-parser-master\http_parser.h"
#include <iostream>
#include <sstream>
#include <cstring>
#include <vector>

#pragma comment(lib, "Ws2_32.lib")

int messageHandler(int clientSocketFd, char* target_port); // Handles message traffic from the browser to the internet or vice versa.
int isRequest(char* message); // Returns 0 for other, 1 for GET, 2 for HEAD, 3 for POST
int findContentLength(char* header); // Returns content-length when passed pointer to start of header.
void correctConnectionField(char* header); // Replaces "Connection: xxx" field with "Connection: close".
int handleRequest(char* targetPort, int requestType, int messageLength, char* message);
int handleResponse(char* targetPort, int messageLength, char* message);

#endif // GREG_HPP
