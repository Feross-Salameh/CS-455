// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

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

// definitions 
#define INF 64 // used for calculating distances 
#define BUFSIZE MAX_PATH // needed for changing directories. 

// namespace
using namespace std;


typedef struct myStruct 
{
	int distance;
	char nextHop;
	int portTo;
	int portFrom;
	int basePort;

}routingEntry; 
static map<char, routingEntry> table; //this will contain the distance vector routing table.

// functions prototypes

/*
	readConfig() will read in the configuration files for the router that has the spesific name. If any error
	occurs, will return a -1 for failure, otherwise will return 1 for success.
	NOTE: This function changes the current working directory to the spesific test folder by using SetCurrentDirectory.
	if it's called from another directory besides router\router, will throw error.
*/
int readConfig(wstring foldername);

#endif // ROUTER_HPP