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
	cout << "Header Length is: " << headerLength + 4 << endl;
	for (i = 0; i < headerLength; i++) // Transfers daya from the bulkMessage to the new char array "bulk".
		header[i] = bulkMessage[i];
	header[i] = '\r'; // Appends terminators for end of header section
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

		delete [] header;
		header = new char[headerLength - i - j]; // Reallocate

		for (int k = 0; k < headerLength - i - j; k++) //Re-populate
			header[k] = bulkMessage[k];
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
	if ((err = getaddrinfo(hostName, NULL, &hints, &targetInfo)) != 0) // Populates targetInfo with a list of information about the target URL.
	{
		cout << "Miss in DNS poll: " << err << endl;
		return -1;
	}
	else
		cout << "Successful hit in DNS poll: " << err << endl;

	// Make connection to the host IP (targetInfo contatins a list of potential IPs. The loop attempts to make connection with each before quitting.
	for (p = targetInfo, i = 1; p != NULL; p = p->ai_next, i++) // Provided by: beej.us's tutorial on connection() and socket().
	{
		cout << "Connection attempt # " << i << end;

		if ((servSocket = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
		{
			cout << "Socket() call failed." << endl;
			continue;
		}
		if (connect(servSocket, p->ai_addr, p->ai_addrlen) != 0) {
			closesocket(servSocket);
			cout << "Connect() call failed." << endl;
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
		cout << "Successfully connected" << endl;
	
	// Send the header and body
	sendData(headerLength, servSocket, header, "header");
	sendData(bodyLength, servSocket, body, "body");
	
	// Clean up. Makes dynamically instantiated arrays "size" 0.
	delete [] header;  
	delete [] body;
	bulkMessage.clear();

	//****************************************
	//       Response Message Handling
	//****************************************

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

	cout << "Checking if a request-type message." << endl;


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

	if (type)
		cout << "A request-type message has been received." << endl;
	else
		cout << "A response-type message has been received." << endl;

	return type;
}

int readInMessage(int clientSocketFd, vector<char> &bulkMessage, int &messageLength, char* message)
{
	int j = 0, i = 0, headerLength;

	cout << "Reading in header from browser." << endl;

	while (strstr(message, "\r\n\r\n") == nullptr)
	{
		j = recv(clientSocketFd, message, HTTP_MAX_HEADER_SIZE, 0);
		cout << "Read in a chunk of: " << j << "B." << endl;
		
		if (strstr(message, "\r\n\r\n") != nullptr)
			headerLength = (messageLength + (strstr(message, "\r\n\r\n") - &message[0])) + 3;

		messageLength += j;

		for (i = 0; i < messageLength; i++)
			bulkMessage.push_back(message[i]);
	}
	bulkMessage.shrink_to_fit();
	cout << "Read in a total of: " << messageLength << "B." << endl << "Bulk Message contains: " << bulkMessage.size() << " characters." << endl;

	return headerLength;
}

int findContentLength(char* header)
{
	string temp;
	int length;
	char *target, *targetStart, *targetEnd;

	target = strstr(header, "Content-Length: ");
	targetStart = target + sizeof("Content-Length: ");
	targetEnd = strstr(targetStart, "\r\n");
	temp.copy(targetStart, targetEnd - targetStart); // Copy starting location of the length to the end of the line.
	length = stoi(temp);
	cout << "Copied body length: " << temp << "Length of body is: " << length << endl;

	return length;
}

int proxyHeaderCleanup(char* header, int headerLength)
{
	int length, charToMove;
	char *targetStart, *targetEnd;

	if ((targetStart = strstr(header, "Proxy-Connection: ")) != nullptr) // If true, then the header DOES contain a "Proxy-Connection" header.
	{
		targetEnd = strstr(targetStart, "\r\n");
		length = targetEnd - targetStart;
		charToMove = ((int)header + headerLength) - ((int)targetEnd + 2);
		
		cout << "Header contains the following proxy header and the subsequent line: " << endl;
		fwrite(targetStart, 1, length, stdout);
		cout << (char*)(targetEnd + 2) << endl;
		cout << "Deleting: " << length << " characters." << endl;
		
		for (int i = 0; i < charToMove; i++)
			*(targetStart + i) = *(targetEnd + i);

		cout << "Deleted the Proxy-Connection line. Line address now states: " << endl;
		cout << (char*)(targetStart) << endl;
	}

	return length;
}

int correctConnectionField(char* header, int headerLength)
{
	int length = 0, charToMove = 0;
	char *target, *targetStart, *targetEnd;

	if ((target = strstr(header, "Connection: ")) != nullptr)
	{
		targetStart = target + sizeof("Connection: ") - 1;
		targetEnd = strstr(targetStart, "\r\n"); // Found where that line ends in header after "Connection: ".
		strcpy(targetStart, "close\r\n"); // Refill with "Connection: close\r\n\0"
		charToMove = targetEnd - strstr(targetStart, "\r\n");
		cout << "Deleted a connection of: " << targetEnd - targetStart << "B." << endl << "New connection type is: \"" << (char*)(target) << "\"." << endl;

		targetEnd = strstr(targetStart, "\r\n"); // Update the new targetEnd.
		for (int i = 0; i < charToMove; i++)
			*(targetEnd + 2 + i) = *(targetEnd + charToMove + i);
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