// Use this header to include all of the standard headers used. This way we can pull from one place.
#ifndef STDHEADERS_HPP
#define STDHEADERS_HPP

#pragma once
#define WIN32_LEAN_AND_MEAN 1
#include <Windows.h>
#include <ostream>
#include <iostream>
#include <string>
#include <cmath>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <tchar.h>
#include <strsafe.h>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")

// these will be used to for the web browser to send message to the proxy server
#define DEFAULT_ADDR "127.0.0.1"
#define DEFAULT_PORT 1990

using namespace std;

#endif
