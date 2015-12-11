#include "Feross.hpp"

SOCKET listenSocket = INVALID_SOCKET; //socket used to recieve inc connects. 
using namespace std;
int setupLisSok()
{
	WSADATA wsaData;
	struct addrinfo *result = NULL;
	struct sockaddr_in addr;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) 
	{
		printf("WSAStartup failed with error: %d\n", iResult);
		return -1;
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons(DEFAULT_PORT);

	if (inet_pton(AF_INET, DEFAULT_ADDR, &addr.sin_addr) != 1)
	{
		printf("Error in code, bad address.\n");
		return -1;
	}

	listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listenSocket == INVALID_SOCKET) {
		printf("socket() failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return -1;
	}
	freeaddrinfo(result);
	iResult = listen(listenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		printf("listen() failed with error: %d\n", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();
		return -1;
	}

	return 1;
}


int selecLoop()
{
	FD_SET lis;
	FD_ZERO(&lis);
	timeval timeout = timeval();
	timeout.tv_sec = 10;
	DWORD timeout2 = 100;
	while (true)
	{
		FD_SET(listenSocket, &lis);
		int selectReturn = select(0, &lis, NULL, NULL, &timeout);

		if (selectReturn < 0)
			cout << "Error on select call: " << WSAGetLastError() << endl;
		else if (selectReturn == 0)
		{
			// wait for child processes to die
			WaitForMultipleObjects(5, NULL, FALSE, timeout2);
		}
		else // selectReturn > 0
		{

		}
	}
}