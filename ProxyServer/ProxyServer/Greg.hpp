#ifndef GREG_HPP
#define GREG_HPP

#pragma once
#include "stdHeaders.hpp"
#include "http-parser-master\http_parser.h"
#include <iostream>
#include <sstream>
#include <cstring>
#include <vector>
#include <string>
#include <ios>

#pragma comment(lib, "Ws2_32.lib")

int messageHandler(int clientSocketFd, char* target_port); // Handles message traffic from the browser to the internet or vice versa.
int isRequest(char* message); // Returns 0 for other, 1 for GET, 2 for HEAD, 3 for POST
int findContentLength(char* header); // Returns content-length when passed pointer to start of header.
int correctConnectionField(char* header, int headerLength); // Replaces "Connection: xxx" field with "Connection: close".
int requestLineCleanup(char* header, int headerLength); // Turns "GET www.cnn.com/blah HTTP/1.1" into "GET /blah HTTP/1.1"
int handleRequest(char* targetPort, int requestType, int messageLength, char* message);
int handleResponse(char* targetPort, int messageLength, char* message);
int sendData(int length, int socket, char arr[], string name);
int readInMessage(int clientSocketFd, vector<char> &bulkMessage, int &messageLength, char* message);
int proxyHeaderCleanup(char* header, int headerLength);

#endif // GREG_HPP
