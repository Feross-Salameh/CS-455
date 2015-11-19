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
SOCKET baseSocket;
char recvBuf[DEFAULT_BUFLEN];
char sendBuf[DEFAULT_BUFLEN];
int recvBufLen = DEFAULT_BUFLEN;

int readConfig(wstring foldername)
{
	wstring dir = L"..\\..\\proj2-skeleton\\";
	dir += foldername;

	cout << "Opening directory: ";
	wcout << dir << endl;
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
				cout << name << " - dest: " << distanceDataIter.first << " cost: " << table[distanceDataIter.first].routingDistance << " nexthop: " << neighborTable.first << endl;
			}
			else if (distanceDataIter.second + table[neighborTable.first].routingDistance == table[distanceDataIter.first].routingDistance)
			{
				shortestRouteTable[distanceDataIter.first] = TRUE; // Equivalent route is found. This may be the best link we can get.
				table[distanceDataIter.first].routingDistance = distanceDataIter.second + table[neighborTable.first].routingDistance; // update cost
				table[distanceDataIter.first].nextHop = neighborTable.first; // update new next hop.
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

	// Generate and send new, appropriate U message to each router.
	for (auto& i: neighbors)
	{
		generateUMessage(i.first);
		sendUpdateMessage(i.first);
	}
}

void routerUpdate(string message, char routerName) // "Host to Host" Router update message looks like: "U d1 cost1 d2 cost2 … dn costn"
{
	message.erase(0, 1);
	char * context;
	bool tableChanged = false;
	char *update = &message[0];
	char *tok = strtok_s(update, " ", &context);
	while(tok != NULL)
	{
		char rt = tok[0];
		int dt = atoi(strtok_s(NULL, " ", &context));
		int cost = table[rt].distance;
		int newCost = table[routerName].distance + dt;
		tok = strtok_s(NULL, " ", &context);
		if (newCost < cost)
		{
			tableChanged = true;
			table[rt].distance = newCost;
			table[rt].nextHop = routerName;
		}
	}
	if (tableChanged)
		sendRoutTableAll();
}

void generateUMessage(char target) // message is a char array of 256 chars
{
	
	string dest = "U";

	for (auto& myTable : table)
	{
		if (dest.length() > 256) // Error checking for insanity.
		{
			cout << "Something is wrong, your U message is huge. You should look into this.\n";
			return;
		}
		
		if (poisonReverse && target == myTable.second.nextHop) // Poison Reverse
		{
			dest += " ";
			dest += myTable.first;
			dest += " ";
			dest += to_string(INF);
		}
		else // Either Poison Reverse is disabled or it doesn't matter if it's enabled or not.
		{
			dest = dest + " " + myTable.first;
			dest = dest + " " + to_string(myTable.second.distance);
		}
	}
	strcpy_s(sendBuf, sizeof(sendBuf), dest.c_str());
}

void sendUpdateMessage(char target) // Will generate the update message to send out.
{
	int numBytes = send(table[target].sendSocket, sendBuf, strlen(sendBuf), 0);

	if (numBytes == -1)
		cout << "Error sending message to neighbor: " << target << ". You should do something about that.\n\n";
}

void linkCostChange() // "User to Host" Link cost message looks like: "L n cost"
{
	char* strptr;
	char tokstr[3][5];

	strptr = strtok(recvBuf, " ");
	cout << 0 << ": strptr = " << *strptr << endl;
	strcpy(tokstr[0], strptr);
	cout << 0 << ": tokstr[" << 0 << "]= " << tokstr[0] << endl;

	if (tokstr[0][0] != 'L') // Basic error checking, this should never be true.
	{
		cout << "Something is wrong. The link cost change function was called.\nBut the message does not contain a link cost change message header." << endl;
		return;
	}

	for (int i = 1; i < 3; i++) // Populate string array.
	{
		strptr = strtok(NULL, " ");
		cout << i << ": strptr = " << *strptr << endl;
		strcpy(tokstr[i], strptr);
		cout << i << ": tokstr[" << i << "]= " << tokstr[i] << endl;

		if (i == 1 && name == tokstr[i][0])
		{
			cout << "Invalid Link message content. You are attempting to create an entry for this router in it's own table. " << endl;
			return;
		}
		if ((i == 1 || i == 2) && tokstr[i][0] == 0) // Basic error checking, this should never be true.
		{
			cout << "Something is wrong. The link cost change function was called.\nBut the message is not a complete link cost change message." << endl;
			return;
		}
	}
	memset(recvBuf, '\0', sizeof(recvBuf));

	if (table[tokstr[1][0]].distance == INF)
	{
		cout << "The node to change a link with is not a neighbor, updating the routing distance with the new value." << endl;
		cout << "You may not have intended to do this." << endl;
		table[tokstr[1][0]].distance = stoi(tokstr[2]); // update distance cost with new value from user.
	}
	else
	{
		cout << "The node to change a link with is a neighbor, updating the adjacent distance with the new value." << endl;
		updateDistanceVectorTable(); // Update distance vector table to see if a better route exists after new link cost update.
	}
}

	

void printRoutingTable() // "User to Host" Print message looks like: "P d" or "P" 
{
	char* strptr;
	char tokstr[2][5] = { 0 };

	// Populate string array.
	strptr = strtok(recvBuf, " ");
	strcpy(tokstr[0],strptr);
	strptr = strtok(NULL, " ");
	if (strptr != 0)
		strcpy(tokstr[1], strptr);

	// recvBuf has been modified by strtok. Erase it for GP.
	memset(recvBuf, '\0', sizeof(recvBuf));

	if (table.find(tokstr[1][0]) == table.end() && tokstr[1][0] != 0) // The table entry to print does not exist.
	{
		cout << "The entry you desire to have printed does not exist in the table." << endl;
		return;
	}

	// Option 1: print P d
	if (tokstr[1][0] != 0) // Two parameter passed in with message.
		cout << tokstr[1][0] << " - cost: " << table[tokstr[1][0]].distance << " nexthop: " << table[tokstr[1][0]].nextHop << endl;

	// Option 2: print whole table
	else // Only one parameter passed in with message, print the entire table.
		for (auto& x : table)
			cout << x.first << " - cost: " << x.second.distance << " nexthop: " << x.second.nextHop << endl;

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
	//initLisSok(table[name].basePort, name);
	//BASEPORT
	u_long nonblock = 1;
	struct addrinfo *result = NULL;
	struct sockaddr_in addr;
	string IP = "127.0.0.1";
	addr.sin_family = AF_INET;
	addr.sin_port = htons(table[name].basePort);
	if (inet_pton(AF_INET, IP.c_str(), &addr.sin_addr) != 1)
		return -1;

	baseSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (baseSocket == INVALID_SOCKET)
		return -1;

	iResult = ioctlsocket(baseSocket, FIONBIO, &nonblock);
	if (iResult == SOCKET_ERROR)
		return -1;

	iResult = bind(baseSocket, (PSOCKADDR)&addr, sizeof(addr));
	if (iResult == SOCKET_ERROR)
		return -1;
	listen(baseSocket, 5);
	//accept(baseSocket, NULL, NULL);
	FD_SET(baseSocket, &masterRead);
	cout << "Bound to port " << table[name].basePort << endl;
	//END BASEPORT
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
			else
				cout << "Bound to port " << writePort << endl;
			if (initConSok(readPort, iter->first) == -1)
				cout << "failed to connect to port: " << readPort << " error: "<< WSAGetLastError() << endl; // this setup will need to be repeated until all sockets are connected. 
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

	table[router].listenSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (table[router].listenSocket == INVALID_SOCKET)
		return -1;

	iResult = ioctlsocket(table[router].listenSocket, FIONBIO, &nonblock);
	if (iResult == SOCKET_ERROR)
		return -1;

	iResult = bind(table[router].listenSocket, (PSOCKADDR)&addr, sizeof(addr));
	if (iResult == SOCKET_ERROR)
		return -1;
	listen(table[router].listenSocket, 5);
	//accept(table[router].listenSocket, NULL, NULL);
	FD_SET(table[router].listenSocket, &masterRead);

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

	table[router].sendSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (table[router].sendSocket == INVALID_SOCKET)
		return -1;

	iResult = ioctlsocket(table[router].sendSocket, FIONBIO, &nonblock);
	if (iResult == SOCKET_ERROR)
		return -1;

	iResult = connect(table[router].sendSocket, (PSOCKADDR)&addr, sizeof(addr));
	//if (iResult == SOCKET_ERROR)
		//return -1;
	FD_SET(table[router].sendSocket, &masterWrite);
	return 1;
}


int processSelect(int socs)
{
	for (int i = 0; i < socs; i++)
	{
		if (FD_ISSET(baseSocket, &read))
		{
			int res;
			//incoming message....
			memset(recvBuf, '\0', sizeof(recvBuf));
			res = recv(baseSocket, recvBuf, recvBufLen, 0);
			if (res > 0)
			{
				cout << "recieved: " << recvBuf << endl;
				switch (recvBuf[0])
				{
				case 'P':
					printRoutingTable();
					break;
				case 'L':
					linkCostChange();
					break;
				default:
					cout << "Unkown command recieved on base." << endl;
				}

			}
		}
		for (auto iter = table.begin(); iter != table.end(); iter++)
		{
			routingEntry entry = iter->second;
			if (FD_ISSET(entry.listenSocket, &read) != 0)
			{
				int res;
				//incoming message....
				memset(recvBuf, '\0', sizeof(recvBuf));
				res = recv(entry.listenSocket, recvBuf, recvBufLen, 0);
				if (res > 0)
				{
					cout << "received: " << recvBuf << endl;
					switch (recvBuf[0])
					{
					case 'P':
						//printRoutingTable();
						break;
					case 'U':
						routerUpdate((string)recvBuf, iter->first);
						break;
					case 'L':
						//linkCostChange();
						break;
					}

				}
				else if(WSAGetLastError() != WSAEWOULDBLOCK)
				{
					cout << "error on recv(): " << WSAGetLastError() << endl;
				}
			}
			//else if (FD_ISSET(entry.sendSocket, &write) != 0)
			//{
			//	 /*connected to new socket should send routing table....*/
			//	generateUMessage(iter->first);
			//	//cout << "sending table to " << iter->first << endl;
			//	int numBytes = send(entry.sendSocket, sendBuf, strlen(sendBuf), 0);
			//	if (numBytes == -1)
			//		cout << "Error sending message to neighbor: "<< WSAGetLastError() << endl;
			//	Sleep(1000);
			//}
		}
	}

	return 1;
}

void resetFD()
{
	FD_ZERO(&masterRead);
	FD_ZERO(&masterWrite);
	FD_ZERO(&read);
	FD_ZERO(&write);
	FD_SET(baseSocket, &masterRead);
	for (auto iter = table.begin(); iter != table.end(); iter++)
	{
		routingEntry entry = iter->second;
		if (entry.portTo > 0)
		{
			FD_SET(entry.listenSocket, &masterRead);
			FD_SET(entry.sendSocket, &masterWrite);
		}
	}

}


void sendRoutTableAll()
{
	for (auto iter = table.begin(); iter != table.end(); iter++)
	{
		generateUMessage(iter->first);
		if (iter->second.sendSocket != 0)
		{
			int result = send(iter->second.sendSocket, sendBuf, strlen(sendBuf), 0);
			if (result < 0)
			{
				cout << "Error while trying to send updated table to router " << iter->first <<" : " << WSAGetLastError() << endl;
			}
		}

	}
}
