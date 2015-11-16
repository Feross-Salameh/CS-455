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
map<char, routingEntry> table; // This will contain the distance vector routing table for "this" router.
map<char, otherRoutersDistanceTable> distanceData; // All neighboring routers will have a table of distance data and be in this map.
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

int updateDistanceVectorTable(void) // returns 1 if need to send update messages to neighbors.
{
	// Generate "this" nodes distance vector table following new data from L-message or U-message.
	// Generate string message to send out to neighbors
	// sendUpdateMessage(updateMessage);
	return 0;
}

void routerUpdate(string message, char routerName) // "Host to Host" Router update message looks like: "U d1 cost1 d2 cost2 � dn costn"
{
	char* strptr = &message[0];
	string tokstr[127]; // Assume an upper limit of 63 routers, 63 distance costs, and the letter U = 127 strings max
	char router_name = '/0';

	for (int i = 0; i < 127; i++) // Populate string array.
	{
		strptr = strtok_s(&message[0], " ", NULL);
		tokstr[i] = strptr;

		if (i == 0 && tokstr[i][0] != 'U') // Basic error checking, this should never be true.
		{
			cout << "Something is wrong. The router update function was called.\nBut the message does not contain a router update message header." << endl;
			return;
		}
	}

	//map<char, otherRoutersDistanceTable> distanceData;
	distanceData[routerName].routingTable.clear(); // Clear out old data to be repopulated.
	distanceData[routerName].name = routerName;

	for (int i = 1; i < 127; i += 2) // Ignore the first tokenized string which is "U", increment through the pairs of tokenized strings.
	{
		if (tokstr[i].empty() == 1) // An empty token means you have no more data to consider.
			return;

		distanceData[routerName].routingTable[tokstr[i][0]] = stoi(tokstr[i + 1]);
	}

	updateDistanceVectorTable(); // Update distance vector table to see if a better route exists after new link cost update.
}

void sendUpdateMessage(char* message) // Will generate the update message to send out.
{
	// Loop through neighbors and send message.
	for (auto& x : table)
	{
		if (x.second.nextHop == 0) // nextHop == 0 means it is a neighbor.
		{
			SSIZE_T numBytes = send(x.second.sendSocket, message, strlen(message), 0);

			if (numBytes == -1)
				cout << "Error sending message to neighbor: " << x.first << ".\n You should do something about that.\n\n";
		}
	}
}

void linkCostChange(string message) // "User to Host" Link cost message looks like: "L n cost"
{
	char* strptr = &message[0];
	char *context = NULL;
	string tokstr[3];

	for (int i = 0; i < 3; i++) // Populate string array.
	{
		strptr = strtok_s(&message[0], " ", &context);
		tokstr[i] = strptr;

		if (i == 0 && tokstr[i][0] != 'L') // Basic error checking, this should never be true.
		{
			cout << "Something is wrong. The link cost change function was called.\nBut the message does not contain a link cost change message header." << endl;
			return;
		}

		if ((i == 1 || i == 2) && tokstr[i].empty() == 1) // Basic error checking, this should never be true.
		{
			cout << "Something is wrong. The link cost change function was called.\nBut the message is not a complete link cost change message." << endl;
			return;
		}
	}

	table[tokstr[1].front()].distance = stoi(tokstr[2]); // update distance cost with new value from user.

	updateDistanceVectorTable(); // Update distance vector table to see if a better route exists after new link cost update.
}

void printRoutingTable(string message) // "User to Host" Print message looks like: "P d" or "P" 
{
	char* strptr = &message[0];
	char *context = NULL;
	string tokstr[2];
	for (int i = 0; i < 3; i++) // Populate string array.
	{
		strptr = strtok_s(&message[0], " ", &context);
		tokstr[i] = strptr;

		if (i == 0 && tokstr[i][0] != 'P') // Basic error checking, this should never be true.
		{
			cout << "Something is wrong. The print routing table function was called.\nBut the message does not contain a print table message header." << endl;
			return;
		}

		if (i == 1 && tokstr[i].empty() == 1) // Basic error checking, this should never be true.
		{
			cout << "Something is wrong. The print routing table function was called.\nBut the message is not a complete print table message." << endl;
			return;
		}
	}

	// Option 1: print P d
	if (tokstr[1].empty() == 0) // Two parameter passed in with message.
		cout << tokstr[1].front() << ": " << table[tokstr[1].front()].distance << ' ' << table[tokstr[1].front()].nextHop << endl << endl;

	// Option 2: print whole table
	else // Only one parameter passed in with message, print the entire table.
	{
		for (auto& x : table)
			cout << x.first << ":" << x.second.distance << " " << x.second.nextHop << endl;
		cout << endl;
	}
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
	initLisSok(table[name].basePort, name);
	for (auto iter = table.begin(); iter != table.end(); iter++)
	{
		routingEntry temp = iter->second;
		//setting up all the sockets needed for reading/writing
		if ((table[name].basePort != temp.basePort) && (temp.portFrom != -1))  // no need to calculate own ports
		{ 
			int readPort, writePort;
			writePort = table[name].basePort + temp.portTo;
			readPort = temp.basePort + temp.portFrom;
			if (initLisSok(writePort, iter->first) == -1)
				cout << "failed to bind to port: " << writePort << endl;
			if (initConSok(readPort, iter->first) == -1)
				cout << "failed to connect to port: " << readPort << endl; // this setup will need to be repeated until all sockets are connected. 
		}	

	}

	return 1;
}

int initLisSok(int port, char router)
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

	table[router].sendSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (table[router].sendSocket == INVALID_SOCKET)
		return -1;

	iResult = ioctlsocket(table[router].sendSocket, FIONBIO, &nonblock);
	if (iResult == SOCKET_ERROR)
		return -1;

	iResult = bind(table[router].sendSocket, (PSOCKADDR)&addr, sizeof(addr));
	if (iResult == SOCKET_ERROR)
		return -1;
	
	listen(table[router].sendSocket, 5);
	FD_SET(table[router].sendSocket, &masterWrite);

	return 1;
}

int initConSok(int port, char router)
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

	table[router].listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (table[router].listenSocket == INVALID_SOCKET)
		return -1;

	iResult = ioctlsocket(table[router].listenSocket, FIONBIO, &nonblock);
	if (iResult == SOCKET_ERROR)
		return -1;

	iResult = connect(table[router].listenSocket, (PSOCKADDR)&addr, sizeof(addr));
	FD_SET(table[router].listenSocket, &masterRead);
	if (iResult == SOCKET_ERROR)
		return -1;
	
	return 1;
}

