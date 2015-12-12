#include "Greg.hpp"

int messageHandler(int clientSocketFd, char* target_port)
{
	char message[HTTP_MAX_HEADER_SIZE] = { 0 }, *header, *body, *start, *end, *bulk, hostName[256] = { 0 };
	vector<char> bulkMessage;
	int i, j, hostIP, hostLength, proxyServSocket, messageLength = 0, headerLength, bodyLength, requestType;// if requestType == 0 it's a RESPONSE
	struct addrinfo hints, *targetInfo;

	cout << "Reading in header from browser." << endl;
	
	while (strstr(message, "\r\n\r\n") == nullptr)
	{
		j = recv(clientSocketFd, message, HTTP_MAX_HEADER_SIZE, 0);
		cout << "Read in a chunk of: " << j << "B." << endl;
		messageLength += j;

		for (i = 0; i < messageLength; i++)
			bulkMessage.push_back(message[i]);
	}
	bulkMessage.shrink_to_fit();
	cout << "Read in a total of: " << messageLength << "B." << endl << "Bulk Message contains: "<< bulkMessage.size() << " characters." <<  endl;
	bulk = new char[bulkMessage.size()];

	for (i = 0; i < bulkMessage.size(); i++)
		bulk[i] = bulkMessage[i];

	end = strstr(bulk, "\r\n\r\n");
	headerLength = end - &bulk[0];


	// Construct header array.
	header = new char[headerLength + 4];
	cout << "Header Length is: " << headerLength << endl;
	for (i = 0; i < headerLength; i++)
	{
		header[i] = bulkMessage[i];
		cout << i << ". " << header[i] << endl;
	}

	header[i] = '\r';
	header[i + 1] = '\n';
	header[i + 2] = '\r';
	header[i + 3] = '\n';
	correctConnectionField(header);
	
	// Determine type of message received.
	requestType = isRequest(header);
	if (requestType == 1 || requestType == 2) // GET or HEAD
		bodyLength = 0;
	else // POST or OTHER (response)
	{
		bodyLength = findContentLength(header);
		body = new char[bodyLength];

		// Construct body array
		start = end + 4; // body starts 4B after the header ends. "end" points inside message buffer.
		for (i = 0; i < HTTP_MAX_HEADER_SIZE - (start - &message[0]); i++) // Move what remains in the message buffer after the header over.
			body[0] = *(start + i); // Utilizes what remains in message
		while (i < bodyLength)
		{
			messageLength = recv(clientSocketFd, message, HTTP_MAX_HEADER_SIZE, 0);
			for (j = 0; j < messageLength, i < bodyLength; j++, i++)
				body[i] = message[j];
		}
	}

	// Extract the host name
	if (requestType) // A request type message
	{
		start = strstr(header, "Host: ") + sizeof("ost: ");
		end = strstr(start, "\r\n"); // Found where that line ends in header after "Host: ".
		hostLength = end - start;
		//cout << "Copying: " << hostLength << " bytes. Copying: \"" << (char*)start << "\"." << endl;
		for (i = 0; i < hostLength; i++)
			hostName[i] = *(start + i);
		
		cout << "Host name: " << hostName << endl;
		
		// Get the target host struct created.
		memset(&hints, 0, sizeof(hints));
		
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		
		hostIP = getaddrinfo(hostName, target_port, &hints, &targetInfo); // Populates targetInfo with information about the target URL.
		cout << "Host IP address: " << hostIP << endl << "Port: " << target_port << endl;
	}
	else // response type message
	{

	}
	
	// Error checking and creation of network interface.
	proxyServSocket = socket(targetInfo->ai_family, targetInfo->ai_socktype, targetInfo->ai_protocol);
	cout << "TEST0 " << endl;
	if (proxyServSocket < 0) // Socket() failed.
	{
		cout << "Socket() call failed." << endl;
		return -1;
	}
	cout << "TEST1 " << endl;
	if (connect(proxyServSocket, targetInfo->ai_addr, (int)(targetInfo->ai_addrlen)) < 0)
	{
		cout << "Connect() failed." << endl;
		return -1;
	}
	cout << "TEST2 " << endl;
	//Create and send the message
	cout << "Header size: " << headerLength << endl;
	cout << "Body size: " << bodyLength << endl;
	cout << "Message size: " << headerLength + bodyLength << endl;
	cout << "TEST3 " << endl;
	i = 0; // counter
	j = 0; // bytes sent
	while (headerLength > j)
	{
		cout << "Need to send " << headerLength << "B total." << "Sending " << headerLength - j << "B." << endl;
		i = send(proxyServSocket, &header[0 + j], headerLength - j, 0);
		if (i < 0)
			cout << "Error sending data from the header." << endl;
		else
		{
			headerLength -= i;
			j += i;
			cout << "Sent " << i << "B. Need to send " << headerLength << "B more." << endl;
		}
	}
	i = 0; // counter
	j = 0; // bytes sent
	while (bodyLength > j)
	{
		cout << "Need to send " << bodyLength << "B total." << "Sending " << bodyLength - j << "B." << endl;
		i = send(proxyServSocket, &body[0 + j], bodyLength - j, 0);
		if (i < 0)
			cout << "Error sending data from the header." << endl;
		else
		{
			bodyLength -= i;
			j += i;
			cout << "Sent " << i << "B. Need to send " << bodyLength << "B more." << endl;
		}
	}

	delete [] header;
	delete [] body;

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

int findContentLength(char* header)
{
	string temp;
	int bodyLength;
	char *target, *targetStart, *targetEnd;

	target = strstr(header, "Content-Length: ");
	targetStart = target + sizeof("Content-Length: ");
	targetEnd = strstr(targetStart, "\r\n");
	temp.copy(targetStart, targetEnd - targetStart); // Copy starting location of the length to the end of the line.
	bodyLength = stoi(temp);
	cout << "Copied body length: " << temp << "Length of body is: " << bodyLength << endl;

	return bodyLength;
}

void correctConnectionField(char* header)
{
	char *target, *targetStart, *targetEnd;

	if ((target = strstr(header, "Connection: ")))
	{
		target = strstr(header, "Connection: ");
		targetStart = target + sizeof("Connection: ") - 1;
		targetEnd = strstr(targetStart, "\r\n"); // Found where that line ends in header after "Connection: ".
		strcpy(targetStart, "close"); // Refill with "Connection: close\r\n"
		*(targetStart + 6) = ' '; // Replace \0 terminator from strcpy() with a space.
		cout << "Deleted a connection of: " << targetEnd - targetStart << "B." << endl << "New connection type is: \"";
		for (int k = 0; k < sizeof("Connection: close"); k++)
			cout << *(target + k);
		cout << "\"." << endl;
	}
}