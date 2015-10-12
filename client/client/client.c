// client.cpp : Defines the entry point for the console application.
//
//#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>


// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define DEFAULT_BUFLEN 512
SOCKET ConnectSocket = INVALID_SOCKET;
char recvbuf[DEFAULT_BUFLEN];
int recvbuflen = DEFAULT_BUFLEN;
int s_send(char *message);
int s_recv();
void shut();
char *filteredMsg = "  ";


int s_send(char * message)
{
	int iSendResult = send(ConnectSocket, message, strlen(message), 0);
	if (iSendResult == SOCKET_ERROR) {
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}
	return 0;
}
int s_recv()
{
	char* context = NULL;
	int result = recv(ConnectSocket, (char *)&recvbuf, recvbuflen, 0);
	if (result < 0)
	{
		shut();
		return -1;
	}

	filteredMsg = strtok_s(recvbuf, "\n", &context);
	return 1;
}
void shut()
{
	int iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return;
	}

	// cleanup
	closesocket(ConnectSocket);
	WSACleanup();

}

int main(int argc, char **argv)
{
	WSADATA wsaData;

	char *sendbuf = "this is a test";
	char recvbuf[DEFAULT_BUFLEN];
	int iResult;
	int recvbuflen = DEFAULT_BUFLEN;
	char IP[10] = "127.0.0.1";
	char input[128];
	int port = -1;
	struct sockaddr_in addr;
	u_short len;
	char lenMsg[3] = "12\n";

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}

	printf("Started client process\n");
	printf("Please enter IP for client process(localhost by default): ");
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

	printf("client process using address: tcp://%s:%d\n", IP, port);

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	if (inet_pton(AF_INET, IP, &addr.sin_addr) != 1)
		printf("Error in code\n");

	// Create a SOCKET for connecting to server
	ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ConnectSocket == INVALID_SOCKET) {
		printf("socket failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}
	printf("client: connecting to socket...");
	// Connect to server.
	iResult = connect(ConnectSocket, (struct sockaddr *)&addr, sizeof(addr));
	if (iResult == SOCKET_ERROR) {
		closesocket(ConnectSocket);
		ConnectSocket = INVALID_SOCKET;
	}

	if (ConnectSocket == INVALID_SOCKET) {
		printf("Unable to connect to server!\n");
		WSACleanup();
		return 1;
	}

	printf("Connected.\nClient: waiting for welcome message.\n");
	if (s_recv() < 0)
	{	
		printf("Client: error recieving message. Closing. \n");
		shut();
		return 0;
	}
	printf("Recieved: %s.\n", filteredMsg);
	if (strcmp(filteredMsg, "welcome") != 0)
	{
		printf("Recieved unknown message, Closing\n.");
		shut();
		return 0;
	}

	printf("Client, please enter ID number:");
	gets(input);

	if (s_send(input))
	{
		shut();
		return 0;
	}

	printf("please enter name: ");
	gets(input);
	if (s_send(input))
	{
		shut();
		return 0;
	}
	if (s_recv() < 0)
	{
		printf("Client: error recieving message. Closing. \n");
		shut();
		return 0;
	}
	if (!strcmp(filteredMsg, "failure"))
	{
		printf("Incorrect ID/name combination. Closing");
		shut();
		return 0;
	}
	else if (strcmp(filteredMsg, "success") != 0)
	{
		printf("Recieved unknown message, Closing\n.");
		shut();
		return 0;
	}

	printf("Client: found pair in server.\n");
	printf("Please enter password: ");
	gets(input);

	len = htons(strlen(input));
	int iSendResult = send(ConnectSocket, &len, sizeof(u_short), 0);
	if (iSendResult == SOCKET_ERROR) {
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	input[strlen(input)] = '\n';
	if (s_send(input))
	{
		shut();
		return 0;
	}
	if (s_recv() < 0)
	{
		printf("Client: error recieving message. Closing. \n");
		shut();
		return 0;
	}

	printf("%s\n", filteredMsg);

	//if (s_recv() < 0)
	//{
	//	printf("Client: error recieving message. Closing. \n");
	//	shut();
	//	return 0;
	//}
	//// cleanup
	gets(input);
	return 0;
}

