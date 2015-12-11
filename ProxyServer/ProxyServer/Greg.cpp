#include "Greg.hpp"

//From Web Browser (Chrome) to the Proxy Server
int messageHandler(int clientSocketFd, char* target_port)
{
	char buffer[HTTP_MAX_HEADER_SIZE] = { 0 };
	string header, body, hostName, postContent, message, buf_str;
	SSIZE_T numBytesRcvd = 0, numBytesSent = 0;
	int find1, find2, contentLength, hostIP, proxyServSocket;
	bool loop = true;

	while (loop) // Start reading in the message header. Unknown when to stop.
	{
		buf_str.clear();
		cout << "Reading in header from browser." << endl;
		numBytesRcvd = recv(clientSocketFd, buffer, HTTP_MAX_HEADER_SIZE, 0);
		message += buffer;
		SSIZE_T endOfHeader = message.find("\r\n\r\n"); // position is at the beginning of the "\r\n" "\r\n" pairs.
		if (endOfHeader != string::npos) // End of header found. Populate the header string.
		{
			// Populate the header
			//header.copy(&message[0], endOfHeader + 2, 0); // Copy from the beginning of the message to the end of the header.
			header = message.substr(0, endOfHeader + 2);
			string convertBuf;
			SSIZE_T contentLengthPos = header.find("Content-Length: ");
			if (contentLengthPos < 1) // couldn't find it
			{
				// TODO: something should happen here, maybe a break?
			}
			convertBuf.copy(&header[contentLengthPos + strlen("Content-Length: ")], header.find("\r\n", contentLengthPos) - contentLengthPos); // Copy starting location of the length to the end of the line.
			contentLength = stoi(convertBuf);
			cout << "Length of header is: " << header.capacity() << endl << "Length of content is: " << contentLength << endl;

			// Populate the body
			body.copy(&message[endOfHeader + 2], message.capacity() - (endOfHeader + 3));
			while ((numBytesRcvd = recv(clientSocketFd, buffer, HTTP_MAX_HEADER_SIZE, 0)) > 0)
			{
				buf_str.clear();
				buf_str.copy(buffer, numBytesRcvd);
				body.append(buf_str, buf_str.capacity());
			}
			cout << "Length of body is: " << body.capacity() << endl << endl;

			loop = false;
		}
	}
	
	// Correct the connection type for the proxy server.
	find1 = header.find("Connection: "); // Finds where "Connection: " is, we don't know what follows.
	find2 = header.find("\r\n", find1); // Found where that line ends in header after "Connection: ".
	cout << *(&header + find1) << endl;
	header.erase(find1, find2 - 1); // Erase "Connection:" and whatever follows through the end of the line.
	header.insert(find1, "Connection: close"); // Refill with "Connection: close\r\n"
	cout << *(&header + find1) << endl << endl;

	// Extract the host name
	find1 = header.find("Host: "); // Finds where "Host: " is, we don't know what follows.
	find2 = header.find("\r\n", find1); // Found where that line ends in header after "Host: ".
	hostName.copy(&header[find1], find2 - find1); // Copy the Host of the header to hostName.
	cout << "Host name: " << hostName << endl;
	
	// Get the target host struct created.
	struct addrinfo hints, *targetInfo;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	hostIP = getaddrinfo(&hostName[0], target_port, &hints, &targetInfo); // Populates targetInfo with information about the target URL.
	
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
	message.copy(&header[0], header.capacity());
	message.append(body, body.capacity());

	cout << "Header size: " << header.capacity() << endl;
	cout << "Body size: " << body.capacity() << endl;
	cout << "Message size: " << message.capacity() << endl;

	numBytesSent = send(proxyServSocket, &message[0], message.capacity(), 0);
	if (numBytesSent < 0)
	{
		cout << "Send() failed." << endl;
	}
	else // Sent SOME data. Send the rest.
	{
		while (numBytesSent < message.capacity()) // Maybe didn't send all of the data. Keep sending until it is all sent.
			numBytesSent += send(proxyServSocket, &message[numBytesSent], message.capacity() - numBytesSent, 0);
	}
	
	cout << "Sent " << numBytesSent <<" bytes." << endl;
	return 0;
}