// stdafx.cpp : source file that includes just the standard includes
// router.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.hpp"
// variables
char name;
bool poisonReverse = false;
fd_set masterRead; // contains all sockets for reading
fd_set masterWrite; // contains all sockets for writing
fd_set read; // used when calling select
fd_set write; // used when calling select
map<char, routingEntry> table; //this will contain the distance vector routing table.
WSADATA wsaData;
int readConfig(wstring foldername)
{
	wstring dir = L"..\\..\\proj2-skeleton\\";
	dir += foldername;

	if (!SetCurrentDirectory((LPCWSTR)dir.c_str()))
	{
		MessageBox(NULL, (LPCWSTR)L"Unable to open directory", (LPCWSTR)name, MB_OK);
		return -1;
	}
	string line;
	line =".cfg";
	line.insert(0, &name);
	ifstream config(line);
	if (!config.is_open())
	{
		MessageBox(NULL, (LPCWSTR)L"Unable to open config file", (LPCWSTR)name, MB_OK);
		return -1;
	}
	while (getline(config, line))
	{
		char *context = NULL;
		char *tok = strtok_s(&line[0], " ", &context);
		// first tok name of other router.
		char newName = *tok;
		routingEntry newEntry;
		// next is cost.
		tok = strtok_s(NULL, " ", &context);
		newEntry.distance = atoi(tok);
		// next is port to
		tok = strtok_s(NULL, " ", &context);
		newEntry.portTo = atoi(tok);
		// finally is port from
		tok = strtok_s(NULL, " ", &context);
		newEntry.portFrom = atoi(tok);

		//ADDED: need to give next hop, since there are direclty connected to router, next hop should be the new router itself. 
		newEntry.nextHop = newName;

		// lastly, insert new entry into table
		table[newName] = newEntry;
		
	}
	config.close();
	// reading router file now.
	ifstream rtr("routers");
	if (!rtr.is_open())
	{
		MessageBox(NULL, (LPCWSTR)L"Unable to open config file", (LPCWSTR)name, MB_OK);
		return -1;
	}
	while (getline(rtr, line))
	{
		char *context = NULL;
		char *tok = strtok_s(&line[0], " ", &context);
		// first tok name of other router.
		char newName = *tok;
		tok = strtok_s(NULL, " ", &context);
		tok = strtok_s(NULL, " ", &context);

		table[newName].basePort = atoi(tok);
		if (newName == name)
		{
			table[name].distance = 0;
			table[name].nextHop = 0;
			table[name].portFrom = 0;
			table[name].portTo = 0;
		}
	}

	return 1;
}

int setupSockets()
{
	//clear all fd sets
	FD_ZERO(&masterRead);
	FD_ZERO(&masterWrite);
	FD_ZERO(&read);
	FD_ZERO(&write);
	int iResult;
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		cout << "WSAStartup failed with error: " << iResult << endl;
		return -1;
	}
	initLisSok(table[name].basePort);
	for (auto iter = table.begin(); iter != table.end(); iter++)
	{
		routingEntry temp = iter->second;
		//setting up all the sockets needed for reading/writing
		if (table[name].basePort != temp.basePort)  // no need to calculate own ports
		{ 
			int readPort, writePort;
			writePort = table[name].basePort + temp.portTo;
			readPort = temp.basePort + temp.portFrom;
			if (initLisSok(writePort) == -1)
				cout << "failed to bind to port: " << writePort << endl;
			if (initConSok(readPort) == -1)
				cout << "failed to connect to port: " << readPort << endl; // this setup will need to be repeated until all sockets are connected. 
		}	

	}

	return 1;
}

int initLisSok(int port)
{
	int iResult;
	u_long nonblock = 1;
	struct addrinfo *result = NULL;
	struct sockaddr_in addr;
	string IP = "127.0.0.1";
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	if (inet_pton(AF_INET, IP.c_str(), &addr.sin_addr) != 1)
		return -1;

	SOCKET sok = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sok == INVALID_SOCKET)
		return -1;

	iResult = ioctlsocket(sok, FIONBIO, &nonblock);
	if (iResult == SOCKET_ERROR)
		return -1;

	iResult = bind(sok, (PSOCKADDR)&addr, sizeof(addr));
	if (iResult == SOCKET_ERROR)
		return -1;
	
	listen(sok, 5);
	FD_SET(sok, &masterWrite);

	return 1;
}

int initConSok(int port)
{
	int iResult;
	u_long nonblock = 1;
	struct addrinfo *result = NULL;
	struct sockaddr_in addr;
	string IP = "127.0.0.1";
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	if (inet_pton(AF_INET, IP.c_str(), &addr.sin_addr) != 1)
		return -1;

	SOCKET sok = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sok == INVALID_SOCKET)
		return -1;

	iResult = ioctlsocket(sok, FIONBIO, &nonblock);
	if (iResult == SOCKET_ERROR)
		return -1;

	iResult = connect(sok, (PSOCKADDR)&addr, sizeof(addr));
	if (iResult == SOCKET_ERROR)
		return -1;
	FD_SET(sok, &masterRead);
	return 1;
}
