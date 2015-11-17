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
map<char, map<char, int>> neighbors; // All neighboring routers will have a table (map) of routable nodes and distance data.
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

void updateDistanceVectorTable(void)
{
	map <char, bool> shortestRouteTable; // Table of log detection shorter or same distance routes.
	for (map<char, routingEntry>::iterator i = table.begin(); i == table.end(); i++) // Setup table and set to false
		shortestRouteTable[i->first] = FALSE; // This fills the table with the same number of entries as table.size() and sets each to false.

	// Find the shortest routes possible.
	for (auto& neighborTable: neighbors)
	{
		for (auto& distanceDataIter : neighborTable.second) 
		{
			if (distanceDataIter.second + table[neighborTable.first].routingDistance < table[distanceDataIter.first].routingDistance) // This will create new nodes if discovered with routingDistances of INF upon creation.
			{
				shortestRouteTable[distanceDataIter.first] = TRUE; // Faster route found!
				
				table[distanceDataIter.first].routingDistance = distanceDataIter.second + table[neighborTable.first].routingDistance; // update cost
				table[distanceDataIter.first].nextHop = neighborTable.first; // update new next hop.
			}
			else if (distanceDataIter.second + table[neighborTable.first].routingDistance == table[distanceDataIter.first].routingDistance)
			{
				shortestRouteTable[distanceDataIter.first] = TRUE; // Equivalent route is found. This may be the best link we can get.
			}
		}
	}

	// Not all routes may have decreased or stayed the same. Some may have gone up since last update. We need to loop through and find the shortest route possible.
	// First let's check if we have any routes that have increased in cost.
	for (map<char, routingEntry>::iterator i = table.begin(); i == table.end(); i++) // Setup table and set to false
		if (shortestRouteTable[i->first] == FALSE) // A cost has increased since last time.
			table[i->first].routingDistance = INF;

	//Now search for the shortest route distance available again.
	for (auto& neighborTable : neighbors)
	{
		for (auto& distanceDataIter : neighborTable.second)
		{
			if (distanceDataIter.second + table[neighborTable.first].routingDistance < table[distanceDataIter.first].routingDistance) // This will create new nodes if discovered with routingDistances of INF upon creation.
			{
				shortestRouteTable[distanceDataIter.first] = TRUE; // Faster route found!

				table[distanceDataIter.first].routingDistance = distanceDataIter.second + table[neighborTable.first].routingDistance; // update cost
				table[distanceDataIter.first].nextHop = neighborTable.first; // update new next hop.
			}
		}
	}

	// Check to ensure all shortest distance are found again.
	for (map<char, routingEntry>::iterator i = table.begin(); i == table.end(); i++) // Setup table and set to false
		if (shortestRouteTable[i->first] == FALSE) // If this "if"-check is true, that means the remaining table entry is a neighbor and it's distance increased. Set the routingDistance = new distance.
			table[i->first].routingDistance = table[i->first].distance;

	char updateMessage[253] = { 0 }; // 'U' + 63*4char groups of " dn costn" =  253
	generateUMessage(updateMessage);
	sendUpdateMessage(updateMessage);
}

void routerUpdate(string message, char routerName) // "Host to Host" Router update message looks like: "U d1 cost1 d2 cost2 … dn costn"
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

	neighbors[routerName].cost = 0; // Clear out old data to be repopulated.
	neighbors[routerName].name = routerName;

	for (int i = 1; i < 127; i += 2) // Ignore the first tokenized string which is "U", increment through the pairs of tokenized strings.
	{
		if (tokstr[i].empty() == 1) // An empty token means you have no more data to consider.
			return;

		neighbors[routerName].cost = stoi(tokstr[i + 1]);
	}

	updateDistanceVectorTable(); // Update distance vector table to see if a better route exists after new link cost update.
}

void generateUMessage(char* message)
{
	message[0] = 'U';
	char c = 'A', *j;

	for (int i = 1; i < 253; i += 4)
	{
		while (table.count(c) == 0) // scans table for this index element until 'c' value is found. Stop when that routing index exists.
		{
			if (table.size < c - 65) // 'A' == 65. If router A's table size is 5 (B,C,D,E, & F) and you hit c == 'G' == 71, c - 65 = 6, out of bounds.
				break;
			else
				c++;
		}
		if (c != -1) // end of table, blank out rest of message.
		{
			message[i] = ' ';
			message[i + 1] = ' ';
			message[i + 2] = ' ';
			message[i + 3] = ' ';
		}
		else // not at end of table, keep populating message with pertinent information.
		{
			message[i] = ' ';
			message[i + 1] = c;
			message[i + 2] = ' ';
			message[i + 3] = *itoa(table[c].distance, j, 10);
		}
	}
}

void sendUpdateMessage(char* message) // Will generate the update message to send out.
{
	// Loop through neighbors and send message.
	for (auto& x : table)
	{
		if (x.second.nextHop == x.first) // nextHop == send-to router means it is a neighbor.
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
		if (i == 1 && name == tokstr[i][0])
		{
			cout << "Invalid Link message content. You are attempting to create an entry for this router in it's own table. " << endl;
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

	if (table.find(tokstr[1][0]) == table.end()) // The table entry to print does not exist.
	{
		cout << "The entry you desire to have printed does not exist in the table." << endl;
		return;
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

