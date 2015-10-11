// server.cpp : Defines the entry point for the console application.
//

//#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 512

// table of IDs/Names
char ID[4][10] = {	"1",
					"2",
					"3",
					"4"};


char name[4][10] = {	"test1",
						"test2",
						"test3",
						"test4" };


char passwords[4][10] = {"test1",
						"test2",
						"test3",
						"test4" };

char recvbuf[DEFAULT_BUFLEN];
int recvbuflen = DEFAULT_BUFLEN;
int s_send(char *message);
int s_recv(char *message);
void terminate();
SOCKET ListenSocket = INVALID_SOCKET;
SOCKET ClientSocket = INVALID_SOCKET;

int s_send(char * message)
{
	int iSendResult = send(ClientSocket, message, strlen(message), 0);
	if (iSendResult == SOCKET_ERROR) {
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(ClientSocket);
		WSACleanup();
		return 1;
	}
	return 0;
}
int s_recv()
{
	 return recv(ClientSocket, recvbuf, recvbuflen, 0);

}
void terminate()
{
	int iResult = shutdown(ClientSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ClientSocket);
		WSACleanup();
		return 1;
	}

	// cleanup
	closesocket(ClientSocket);
	WSACleanup();

}

int main()
{
	WSADATA wsaData;
	int iResult;
	char IP[10] = "127.0.0.1";
	char input[128];
	int port = -1;
	int i;
	UINT16 len = 0;
	char *pass;
	struct addrinfo *result = NULL;
	struct addrinfo hints;
	struct sockaddr_in addr; 
	int iSendResult;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}
	printf("Started server process\n");
	printf("Please enter IP for server process(localhost by default): ");
	gets(IP);
	if (!strcmp(IP, ""))
		strncpy_s(IP, 10, "127.0.0.1", 9);
	printf("Please enter port number: ");
	gets(input);
	port = atoi(input);
	if (port <= 0)
	{
		printf("Invalid port number entered, leaving process\n");
		return 1;
	}

	printf("server process using address: tcp://%s:%d\n", IP, port);

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	if (inet_pton(AF_INET, IP, &addr.sin_addr) != 1)
		printf("Error in code\n");
	// Create a SOCKET for connecting to server
	ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ListenSocket == INVALID_SOCKET) {
		printf("socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}
	
	// Setup the TCP listening socket
	iResult = bind(ListenSocket, (struct sockaddr *)&addr, sizeof(addr));
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	freeaddrinfo(result);

	
	iResult = listen(ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}
	printf("Server: now accepting connections...");
	// Accept a client socket
	ClientSocket = accept(ListenSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET) {
		printf("accept failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}
	printf("accepted. closing socket.\n");
	// No longer need server socket
	closesocket(ListenSocket);

	printf("Sending initial message: welcome to client...\n");
	if (s_send("welcome\n"))
	{
		terminate();
		return 0;
	}

	printf("Waiting to recv messages from client...");
	if (s_recv() < 0)
	{
		terminate();
		return 0;
	}
	printf("got message.\nNow checking for matches...");
	for (i = 0; i < 4; i++)
	{
		if (!strncmp(ID[i], recvbuf, strlen(ID[i])))
			break; // found match
		else if (i == 4)
			i = 15; // error number, beyound range.
	}
	if (i == 15)
	{
		printf("server: no matchinges found, closing connection.\n");
		terminate();
		return 0;
	}
	printf("Match ID found.\n");
	if (s_recv() < 0)
	{
		terminate();
		return 0;
	}
	if (strncmp(name[i], recvbuf, strlen(name[i])))
	{
		printf("server: no matchinges found, closing connection.\n");
		terminate();
		return 0;
	}
	printf("server: both matches found, sedning succes.\n");
	if (s_send("success\n"))
	{
		terminate();
		return 0;
	}

	if (s_recv() < 0) // length of password. 
	{
		terminate();
		return 0;
	}
	len = ntohs(recvbuf);
	if (s_recv() < 0) // actual password. 
	{
		terminate();
		return 0;
	}
	pass = (char *)malloc(sizeof(char) * len);
	strncpy_s(pass, len, recvbuf, len);

	if (strncmp(passwords[i], pass, len))
	{
		printf("server: password is incorrect.\n");
		s_send("Password incorrect\n");
	}

	s_send(htons(strlen("Congradulations %s; you've just revealed the password for %s to the world!\n", name[i], ID[i])));

	s_send("Congradulations %s; you've just revealed the password for %s to the world!\n", name[i], ID[i]);

	// shutdown the connection since we're done
	terminate();

    return 0;
}

