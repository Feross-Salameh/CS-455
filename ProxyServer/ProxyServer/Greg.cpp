#include "Greg.hpp"

//From Web Browser (Chrome) to the Proxy Server
int fromBrowser(int clientSocketFd)
{
	char buffer[HTTP_MAX_HEADER_SIZE] = { 0 };
	string header, body, hostName, postContent, message;
	SSIZE_T numBytesRcvd, numBytesSent;
	int find1, find2, contentLength, hostIP, proxyServSocket;

	while ((numBytesRcvd = recv(clientSocketFd, buffer, HTTP_MAX_HEADER_SIZE, 0)) > 0)
		message += buffer; // Generate the message from buffer contents.

	// Isolate the header from the body in the message.
	header.clear(); // empty out header initially.
	header = message; // Shallow copies the entire message into the header.
	find1 = header.find("\r\n\r\n"); // Looks for blank line between body and header.
	body.copy(&header[0], header.size() - find1); // Copy the body over (includes the \r\n\r\n).
	header.erase(find1); // Erase everything after and including the first \r\n pair.
	cout << "Original header from browser: " << endl << header << endl << endl;
	
	// Correct the connection type for the proxy server.
	find1 = header.find("Connection: "); // Finds where "Connection: " is, we don't know what follows.
	find2 = header.find("\r\n", find1); // Found where that line ends in header after "Connection: ".
	header.erase(find1, find2 - 1); // Erase "Connection:" and whatever follows through the end of the line.
	header.insert(find1, "Connection: close"); // Refill with "Connection: close\r\n"
	cout << "Modified header from browser: " << endl << header << endl << endl;

	// Extract the host name
	find1 = header.find("Host: "); // Finds where "Host: " is, we don't know what follows.
	find2 = header.find("\r\n", find1); // Found where that line ends in header after "Host: ".
	hostName.copy(&header[find1], find2 - find1); // Copy the Host of the header to hostName.
	cout << "Host name: " << hostName << endl;

	// Check for a POST header
	if ((find1 = header.find("POST")) != string::npos) // POST exists
	{
		cout << "POST message." << endl;
	}
	else if ((find1 = header.find("GET")) != string::npos) // GET exists
	{
		cout << "GET message." << endl;
	}
	else if ((find1 = header.find("HEAD")) != string::npos) // HEAD exists
	{
		cout << "GET message." << endl;
	}
	else
	{
		cout << "Unknown message type." << endl;
		return -1;
	}
	
	// Get the target host struct created.
	struct addrinfo hints, *targetInfo;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	hostIP = getaddrinfo(&hostName[0], "http", &hints, &targetInfo); // Populates targetInfo with information about the target URL.
	
	proxyServSocket = socket(targetInfo->ai_family, targetInfo->ai_socktype, targetInfo->ai_protocol);
	if (proxyServSocket < 0) // Socket() failed.
	{
		cout << "Socket() call failed." << endl;
		return -1;
	}

	if (connect(proxyServSocket, targetInfo->ai_addr, targetInfo->ai_addrlen) < 0)
	{
		cout << "Connect() failed." << endl;
		return -1;
	}
	
	//Create and send the message
	message.clear();
	message = header;
	message += body;
	
	numBytesSent = send(proxyServSocket, &message[0], message.size(), 0);
	if (numBytesSent < 0)
	{
		cout << "Send() failed." << endl;
	}
	else // Sent SOME data.
	{
		while (numBytesSent < message.size()) // Maybe didn't send all of the data. Keep sending until it is all sent.
			numBytesSent += send(proxyServSocket, &message[numBytesSent], message.size() - numBytesSent, 0);
	}
}