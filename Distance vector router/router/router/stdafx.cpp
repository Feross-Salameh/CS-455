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


/*void routerUpdate(string message) // "Host to Host" Router update message looks like: "U d1 cost1 d2 cost2 … dn costn"
{
	char* strptr = &message[0];
	string tokenized[127]; // Assume an upper limit of 63 routers, 63 distance costs, and the letter U = 127 strings max
	char router_name = '/0';

	for (int i = 0; i < 127; i++) // Populate string array.
	{
		strptr = strtok(&message[0], " ");
		tokenized[i] = strptr;
	}
	for (int i = 1; i < 127; i += 2) // Ignore the first tokenized string which is "U", increment through the pairs of tokenized strings.
	{
		if (tokenized[i].empty() == 1) // An empty token means you have no more data to consider.
			return;

		for (auto& x : table) // find the name of the router table to consider.
		{
			if (x.first == tokenized[i][0]) // found it
			{
				router_name = x.first;
				break;
			}
		}

		if (router_name == '/0') // The router info is not in the table, it is a new router and cost.
		{
			routingEntry newEntry;
			newEntry.distance = stoi(tokenized[i + 1]);
			newEntry.nextHop = tokenized[i][0];
			table.emplace(tokenized[i][0], newEntry);
			//Still to address is the basePort, portTo, and portFrom of a new router where appropriate.
		}
		else // The router exists in the table. It is node = router_name.
		{
			if (table[router_name].distance < )
		}
			

	}

}; */

void linkCostChange(string message) // "User to Host" Link cost message looks like: "L n cost"
{
	char* strptr = &message[0];
	char *context = NULL;
	string tokenized[3];

	for (int i = 0; i < 3; i++) // Populate string array.
	{
		strptr = strtok_s(&message[0], " ", &context);
		tokenized[i] = strptr;
	}

	table[tokenized[1].front()].distance = stoi(tokenized[2]); // update distance cost with new value from user.
};

void printRoutingTable(string message) // "User to Host" Print message looks like: "P d" or "P" 
{
	char* strptr = &message[0];
	char *context = NULL;
	string tokenized[2];
	for (int i = 0; i < 3; i++) // Populate string array.
	{
		strptr = strtok_s(&message[0], " ", &context);
		tokenized[i] = strptr;
	}

	// Option 1: print P d
	if (tokenized[1].empty() == 0) // Two parameter passed in with message.
		cout << tokenized[1].front() << ": " << table[tokenized[1].front()].distance << ' ' << table[tokenized[1].front()].nextHop << endl;

	// Option 2: print whole table
	else // Only one parameter passed in with message, print the entire table.
		for (auto& x : table)
		{
			cout << x.first << ": " << x.second.distance << ' ' << x.second.nextHop << endl; 
		}
	
	cout << endl;
};

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
		if (table[name].basePort != temp.basePort)  // no need to calculate own ports
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

