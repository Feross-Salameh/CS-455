// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once
// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")

#ifndef ROUTER_HPP
#define ROUTER_HPP

// includes
#include "targetver.h"
#include <stdio.h>
#include <tchar.h>
#include <WinSock2.h>
#include <Windows.h>
#include <iostream>
#include <map>
#include <fstream>
#include <string>
#include <locale>
#include <codecvt>
#include <ws2tcpip.h>

// definitions 
#define INF 64 // used for calculating distances 
#define BUFSIZE MAX_PATH // needed for changing directories. 

// namespace
using namespace std;

// this structy will be used when setting up the forwarding table for the router. 
// Each router used will have it's own entry, including the router itself.
typedef struct myStruct 
{
	int distance = INF; //distance to spesific router
	char nextHop = 0; // the Router to tavel to next. 
	int portTo = -1; // first number after cost
	int portFrom = -1; // second number after cost
	int basePort = -1; 
	SOCKET listenSocket;
	SOCKET sendSocket;

}routingEntry; 

typedef struct otherRoutersDistanceTable
{
	char name = 0; // Name of the "other" router.
	map<char, int> routingTable; // distance (int) to another router (char) from "other" router.
}thisRouterDistanceTableEntry;

//typedef struct thisRouterDistanceTableEntry;

// functions prototypes

/*
	readConfig() will read in the configuration files for the router that has the spesific name. If any error
	occurs, will return a -1 for failure, otherwise will return 1 for success.
	NOTE: This function changes the current working directory to the spesific test folder by using SetCurrentDirectory.
	if it's called from another directory besides router\router, will throw error.
*/
int readConfig(wstring foldername);

/*
	setup and configure all of the sokcets required. This will store all of the sockets to the fd_set master.
	Will return 1 if creating of all sockets successful. Otherwise return -1;
*/

int updateDistanceVectorTable(void);// returns 1 if need to send update messages to neighbors.
/*
	Handler for the distance vector table algorithm. This is called after 'L' or 'U' messages.
*/

void routerUpdate(string message, char routerName); 
/*
	Handler for U-messages. "Host to Host" Router update message looks like: "U d1 cost1 d2 cost2 … dn costn"
*/

void sendUpdateMessage(char* message);
/*
	This function will create a message to be sent of from the router's distance vector table.
*/

void linkCostChange(string message);
/*
	Handler for L-messages. "User to Host" Link cost message looks like: "L n cost"
*/

void printRoutingTable(string message);
/*
	Handler for P-messages. "User to Host" Print message looks like: "P d" or "P" 
*/

int setupSockets();

/*
	Will initilize and setup a socket to bind to a certain port, then will add the socket to the masterWrite fd_set. will also return the socket
	NOTE: this function was designed to only be called from the setupSockets() function. Since setupSockets will initilize the WSUDATA
*/
int initLisSok(int port, char router);

/*
	Will initilize and setup a socket to bind to a certain port, then will add the socket to the masterRead fd_set. will also return the socket
	NOTE: this function was designed to only be called from the setupSockets() function. Since setupSockets will initilize the WSUDATA
*/
int initConSok(int port, char router);

#endif // ROUTER_HPP