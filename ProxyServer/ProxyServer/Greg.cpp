#include "Greg.hpp"

int messageHandler(int clientSocketFd, char* target_port)
{
	char message[HTTP_MAX_HEADER_SIZE] = { 0 }, *header, *body, *start, *end, hostName[256] = { 0 };
	vector<char> bulkMessage;
	int i, j, err, hostLength, servSocket, messageLength = 0, headerLength, bodyLength, requestType;// if requestType == 0 it's a RESPONSE
	struct addrinfo hints, *targetInfo, *p;

	//****************************************
	//       Request Message Handling
	//****************************************

	headerLength = readInMessage(clientSocketFd, bulkMessage, messageLength, message); // Begin reading in the message, starting with the header.
	
	// Construct header array.
	header = new char[headerLength]; // It's easier to deal with a character array with system calls than char-vectors.
	cout << "Header Length is: " << headerLength << endl;
	for (i = 0; i < headerLength; i++) // Transfers daya from the bulkMessage to the new char array "bulk".
		header[i] = bulkMessage[i];
	
	// Clean up the header. Delete any proxy-connection headers and correct the connection field.
	i = proxyHeaderCleanup(header, headerLength);
	j = correctConnectionField(header, headerLength);

	if (i > 0 || j > 0) // Resize header to new modified size if any changes made.
	{
		cout << "Resized the header." << endl;
		cout << "Removing " << i << " characters for Proxy-Connection removal." << endl;
		cout << "Removing " << j << " characters for correcting the Connection type field." << endl;
		cout << "Old header size is: " << headerLength << endl;
		headerLength = headerLength - i - j;
		cout << "New header size is: " << headerLength << endl;
	}
		
	// Determine type of message received.
	requestType = isRequest(header);
	if (requestType == 1 || requestType == 2) // GET or HEAD (no body section)
		bodyLength = 0;
	else // POST
	{
		bodyLength = findContentLength(header);
		body = new char[bodyLength];
		for (i = 0; i < HTTP_MAX_HEADER_SIZE - (bulkMessage.size() - headerLength); i++) // Move what remains in the message buffer after the header over.
			body[i] = bulkMessage[(bulkMessage.size() - headerLength) + i]; // Utilizes what remains in message

		while (bodyLength > i) // Fill what remains of 
		{
			messageLength = recv(clientSocketFd, message, HTTP_MAX_HEADER_SIZE, 0);
			for (j = 0; j < messageLength, i < bodyLength; j++, i++)
				body[i] = message[j];
		}
	}

	// Extract the host name
	start = strstr(header, "Host: ") + sizeof("ost: ");
	end = strstr(start, "\r\n"); // Found where that line ends in header after "Host: ".
	hostLength = end - start;

	for (i = 0; i < hostLength; i++)
		hostName[i] = *(start + i);

	cout << "Host name: " << hostName << endl;

	// Get the target host struct created.
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	
	// Get IP of host.
	if ((err = getaddrinfo(hostName, "80", &hints, &targetInfo)) != 0) // Populates targetInfo with a list of information about the target URL.
	{
		cout << "Miss in DNS poll: " << err << endl;
		return -1;
	}
	else
		cout << "Successful hit in DNS poll." << endl;

	// Make connection to the host IP (targetInfo contatins a list of potential IPs. The loop attempts to make connection with each before quitting.
	for (p = targetInfo, i = 1; p != NULL; p = p->ai_next, i++) // Provided by: beej.us's tutorial on connection() and socket().
	{
		servSocket = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		
		if (servSocket == -1)
		{
			cout << "Socket() call failed. Error: " << WSAGetLastError() << endl;
			continue;
		}
		else
			cout << "Socket to connect to host: " << servSocket << endl;

		if (connect(servSocket, p->ai_addr, p->ai_addrlen) == SOCKET_ERROR) {
			
			cout << "Connect() call failed. WSA last error: " << WSAGetLastError() << endl;
			closesocket(servSocket);
			continue;
		}

		break; // if we get here, we must have connected successfully
	}

	if (p == NULL) // looped off the end of the list with no connection
	{
		cout << "Failed to connect" << endl;
		return -1;
	}
	else
	{
		struct sockaddr_in *addr;
		addr = (struct sockaddr_in *)p->ai_addr;
		cout << "Successfully connected." << endl;
		cout << "Host: " << hostName << " on IP: " << inet_ntoa(addr->sin_addr) << endl;
	}
	
	// Send the header and body
	sendData(headerLength, servSocket, header, "header");
	if(bodyLength != 0)
		sendData(bodyLength, servSocket, body, "body");
	
	// Clean up. Makes dynamically instantiated arrays "size" 0.
	delete [] header;  
	if (bodyLength != 0)
		delete [] body;
	bulkMessage.clear();

	//****************************************
	//       Response Message Handling
	//****************************************

	memset(message, '\0', HTTP_MAX_HEADER_SIZE);
	headerLength = readInMessage(servSocket, bulkMessage, messageLength, message); // Begin reading in the message, starting with the header.

	// Construct header array.
	header = new char[headerLength]; // It's easier to deal with a character array with system calls than char-vectors.
	cout << "Header Length is: " << headerLength << endl;
	for (i = 0; i < headerLength; i++) // Transfers daya from the bulkMessage to the new char array "bulk".
		header[i] = bulkMessage[i];
	header[i] = '\r'; // Appends terminators for end of 
	header[i + 1] = '\n';
	header[i + 2] = '\r';
	header[i + 3] = '\n';

	// Clean up the header. Delete any proxy-connection headers and correct the connection field.
	i = proxyHeaderCleanup(header, headerLength);
	j = correctConnectionField(header, headerLength);
	if (i > 0 || j > 0) // Resize header to new modified size if any changes made.
	{
		bulkMessage.clear();
		for (int k = 0; k < headerLength - i - j; k++)
			bulkMessage.push_back(header[k]);
		bulkMessage.shrink_to_fit(); // Temporary space holder.

		delete[] header;
		header = new char[headerLength - i - j]; // Reallocate

		for (int k = 0; k < headerLength - i - j; k++) //Re-populate
			header[k] = bulkMessage[k];
	}

	// Construct body array
	bodyLength = findContentLength(header);
	body = new char[bodyLength];
	for (i = 0; i < HTTP_MAX_HEADER_SIZE - (bulkMessage.size() - headerLength); i++) // Move what remains in the message buffer after the header over.
		body[i] = bulkMessage[(bulkMessage.size() - headerLength) + i]; // Utilizes what remains in message
	
	while (bodyLength > i) // Fill what remains of 
	{
		messageLength = recv(servSocket, message, HTTP_MAX_HEADER_SIZE, 0);
		for (j = 0; j < messageLength, i < bodyLength; j++, i++)
			body[i] = message[j];
	}

	// Send the header and body
	sendData(headerLength, clientSocketFd, header, "header");
	sendData(bodyLength, clientSocketFd, body, "body");

	// Clean up. Makes dynamically instantiated arrays "size" 0.
	delete[] header;
	delete[] body;
	bulkMessage.clear();

	return 0;
}

int isRequest(char* message)
{
	int position, type = 0;
	char* location;

	if (strstr(message, "GET"))
	{
		type = 1;
		location = strstr(message, "GET");
		position = location - message;
		cout << "Found GET message. \"GET\" found at position: " << position << endl;
	}
	if (strstr(message, "HEAD"))
	{
		type = 2;
		location = strstr(message, "HEAD");
		position = location - message;
		cout << "Found HEAD message. \"HEAD\" found at position: " << position << endl;
	}
	if (strstr(message, "POST"))
	{
		type = 3;
		location = strstr(message, "POST");
		position = location - message;
		cout << "Found POST message. \"POST\" found at position: " << position << endl;
	}

	return type;
}

int readInMessage(int clientSocketFd, vector<char> &bulkMessage, int &messageLength, char* message)
{
	int j = 0, i = 0, headerLength = 0;

	cout << "Reading in header from browser." << endl;

	while (strstr(message, "\r\n\r\n") == nullptr)
	{
		j = recv(clientSocketFd, message, HTTP_MAX_HEADER_SIZE, 0);
		cout << "Read in a chunk of: " << j << "B." << endl;

		if (strstr(message, "\r\n\r\n") != nullptr)
			headerLength = (messageLength + (strstr(message, "\r\n\r\n") - &message[0])) + 4;

		messageLength += j;

		for (i = 0; i < j; i++)
			bulkMessage.push_back(message[i]);
	}
	bulkMessage.shrink_to_fit();
	cout << "Read in a total of: " << messageLength << "B." << endl << "Bulk Message contains: " << bulkMessage.size() << " characters." << endl;

	return headerLength;
}

int findContentLength(char* header)
{
	string temp;
	int length = 0;
	char *target, *targetStart, *targetEnd;	

	target = strstr(header, "Content-Length: ");
	targetStart = strstr(target, " "); // Points to the space in "Content-Length: "
	targetEnd = strstr(targetStart, "\r\n");
	temp.copy(targetStart + 1, (targetEnd - targetStart) - 1); // Copy starting location of the length to the end of the line.
	length = stoi(temp);
	cout << "Copied body length: " << temp << "Length of body is: " << length << endl;

	return length;
}

int proxyHeaderCleanup(char* header, int headerLength)
{
	int length = 0, charToMove;
	char *targetStart, *targetEnd;

	if ((targetStart = strstr(header, "Proxy-Connection: ")) != nullptr) // If true, then the header DOES contain a "Proxy-Connection" header.
	{
		targetEnd = strstr(targetStart, "\r\n");
		length = targetEnd - targetStart;
		charToMove = ((int)header + headerLength) - ((int)targetEnd + 2);

		for (int i = 0; i < charToMove; i++)
			*(targetStart + i) = *(targetEnd + i);

		cout << "Deleted the Proxy-Connection line which was " << length <<" bytes." << endl;
	}

	return length;
}

int correctConnectionField(char* header, int headerLength)
{
	int charToMove = 0, copyStartIndex, copyStopIndex;
	char *target, *targetStart, *targetEnd;
	vector<char> temp;

	target = strstr(header, "Connection:");
	if (target == nullptr)
	{
		cout << "This header does not have a Connection-type field." << endl;
		return 0;
	}

	if (strstr(header, "Connection: close") == nullptr)
	{
		cout << "Correcting the Connection-type field." << endl;
		targetStart = strstr(target, " ") + 1; // targetStart is the character after the space in "Connection: ".
		targetEnd = strstr(target, "\r\n") + 2;
		copyStartIndex = (strstr(target, "\r\n") + 2) - header; // Everything after this point needs to be copied to the end of the new connection type.
		copyStopIndex = headerLength;

		for (int i = 0; i < copyStopIndex - copyStartIndex; i++)
			temp.push_back(*(targetEnd + i)); // Build up temporary buffer.

		charToMove = strstr(target, "\r\n") - header;
		
		strcpy(targetStart, " close\r\n"); // Refill with "Connection: close\r\n\0" Don't forget the null-terminator.
		charToMove = charToMove - (strstr(target, "\r\n") - header) + 1;

		targetStart = strstr(target, "\r\n") + 2; // This is where the remaining message is being appended after deleting the old connection.
		for (int i = 0; i < copyStopIndex - copyStartIndex; i++)
			*(targetStart + i) = temp[i]; // Append temp array to the end of "close\r\n".

	}
	else
	{
		cout << "Connection-type requires no correction." << endl;
		return 0;
	}

	return charToMove;
}

int sendData(int length, int socket, char arr[], string name)
{
	int i = 0; // counter
	int j = 0; // bytes sent

	while (length > j)
	{
		cout << "Need to send " << length << "B total." << "Sending " << length - j << "B." << endl;
		i = send(socket, &arr[0 + j], length - j, 0);
		if (i < 0)
		{
			cout << "Error sending data from the " << name <<"." << endl;
			return -1;
		}
		else
		{
			length -= i;
			j += i;
			cout << "Sent " << i << "B. Need to send " << length << "B more." << endl;
		}
	}

	return 0;
}