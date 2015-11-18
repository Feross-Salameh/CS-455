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
#include <iterator>

// definitions 
#define INF 64 // used for calculating distances 
#define BUFSIZE MAX_PATH // needed for changing directories. 
#define DEFAULT_BUFLEN 512
// namespace
using namespace std;

// this structy will be used when setting up the forwarding table for the router. 
// Each router used will have it's own entry, including the router itself.
typedef struct myStruct 
{
	int distance = INF; //distance to specific router
	int routingDistance = INF; // distance to route messages. The default value for this will be "distance" unless a quicker route is found.
	char nextHop = 0; // the Router to travel to next. 
	int portTo = -1; // first number after cost
	int portFrom = -1; // second number after cost
	int basePort = -1; 
	SOCKET listenSocket = 0;
	SOCKET sendSocket = 0;

}routingEntry; 

// functions prototypes

/*
	readConfig() will read in the configuration files for the router that has the spesific name. If any error
	occurs, will return a -1 for failure, otherwise will return 1 for success.
	NOTE: This function changes the current working directory to the spesific test folder by using SetCurrentDirectory.
	if it's called from another directory besides router\router, will throw error.
*/
int readConfig(wstring foldername);

/*
	Handler for the distance vector table algorithm. This is called after 'L' or 'U' messages.
*/
void updateDistanceVectorTable(void);// returns 1 if need to send update messages to neighbors.

/*
	Handler for U-messages. "Host to Host" Router update message looks like: "U d1 cost1 d2 cost2 … dn costn"
*/
void routerUpdate(string message, char routerName); 

/*
	Creates a message at pointer passed that meets U-message format based on this routers vector distance format.
*/
void generateUMessage(char target);

/*
	This function will create a message to be sent of from the router's distance vector table.
*/
void sendUpdateMessage(char target);

/*
	Handler for L-messages. "User to Host" Link cost message looks like: "L n cost"
*/
void linkCostChange(string message);

/*
	Handler for P-messages. "User to Host" Print message looks like: "P d" or "P" 
*/
void printRoutingTable(string message);

/*
setup and configure all of the sokcets required. This will store all of the sockets to the fd_set master.
Will return 1 if creating of all sockets successful. Otherwise return -1;
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


/*
	This function will deal with the ready sockets that have returned from select. Will return 0 if 
	anything went wrong in the process.
*/
int processSelect(int socs);

/*
	Resets the FD_sets, this will make sure select timeout is called correctly
*/
void resetFD();

/*
	resends it's routing table to all 
*/
void sendRoutTableAll();
#endif // ROUTER_HPP